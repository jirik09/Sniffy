#include "pinoutwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFontMetricsF>
#include <QtMath>
#include <QPen>

#include "../../graphics/graphics.h"

// --------------------------------------------------------------------------
PinoutWidget::PinoutWidget(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(360, 260);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

// --------------------------------------------------------------------------
void PinoutWidget::setBoard(const QString &boardId)
{
    m_connectors.clear();
    m_pins.clear();
    clearPinFunctions();

    // -- Load board config ---------------------------------------------------
    const QString boardPath = ":/config/" + boardId + ".json";
    QFile bf(boardPath);
    if(!bf.open(QIODevice::ReadOnly)){
        qWarning("PinoutWidget: board config not found: %s", qPrintable(boardPath));
        update();
        return;
    }
    const QJsonObject boardObj = QJsonDocument::fromJson(bf.readAll()).object();
    const QString templateId = boardObj.value("template").toString("NUCLEO-64");

    // -- Load template -------------------------------------------------------
    const QString tmplPath = templatePathFor(templateId);
    QFile tf(tmplPath);
    if(!tf.open(QIODevice::ReadOnly)){
        qWarning("PinoutWidget: template not found: %s", qPrintable(tmplPath));
        update();
        return;
    }
    const QJsonObject tmplObj = QJsonDocument::fromJson(tf.readAll()).object();

    parseTemplate(tmplObj);
    parsePins(boardObj.value("pins").toArray());
    buildPinPositions();
    QList<PinoutBoardPin> boardPins;
    boardPins.reserve(m_pins.size());
    for(const PinDesc &pin : m_pins)
        boardPins.append({pin.port, pin.arduino});
    m_functionMap.setBoardPins(boardPins);
    update();
}

// --------------------------------------------------------------------------
void PinoutWidget::parseTemplate(const QJsonObject &tmpl)
{
    m_canvasW = tmpl.value("canvas_w").toInt(1000);
    m_canvasH = tmpl.value("canvas_h").toInt(700);

    const QJsonArray conns = tmpl.value("connectors").toArray();
    m_connectors.reserve(conns.size());
    for(const QJsonValue &v : conns){
        const QJsonObject o = v.toObject();
        ConnectorDesc c;
        c.id         = o.value("id").toString();
        c.label      = o.value("label").toString(c.id);
        c.anchorX    = (float)o.value("anchor_x").toDouble();
        c.anchorY    = (float)o.value("anchor_y").toDouble();
        c.rows       = o.value("rows").toInt(1);
        c.pinCount   = o.value("pin_count").toInt(1);
        c.pitch      = (float)o.value("pitch").toDouble(30.0);
        c.rowSpacing = (float)o.value("row_spacing").toDouble(20.0);
        c.row0X      = (float)o.value("row0_x").toDouble(c.anchorX + 14);
        c.row1X      = (float)o.value("row1_x").toDouble(c.row0X + c.rowSpacing);
        c.pin0Y      = (float)o.value("pin0_y").toDouble(c.anchorY + 16);
        m_connectors.append(c);
    }
}

// --------------------------------------------------------------------------
void PinoutWidget::parsePins(const QJsonArray &pinsArr)
{
    m_pins.reserve(pinsArr.size());
    for(const QJsonValue &v : pinsArr){
        const QJsonObject o = v.toObject();
        PinDesc p;
        p.connectorId = o.value("connector").toString();
        p.row         = o.value("row").toInt(0);
        p.index       = o.value("index").toInt(0);
        p.port        = o.value("morpho").toString();
        p.arduino     = o.value("arduino").toString();
        p.cx = p.cy = 0.f;  // filled by buildPinPositions
        m_pins.append(p);
    }
}

// --------------------------------------------------------------------------
QPointF PinoutWidget::pinPos(const ConnectorDesc &c, int row, int index) const
{
    float x = (row == 0) ? c.row0X : c.row1X;
    float y = c.pin0Y + index * c.pitch;
    return {x, y};
}

// --------------------------------------------------------------------------
QString PinoutWidget::templatePathFor(const QString &templateId) const
{
    QString templateKey = templateId.toLower();
    templateKey.remove('-');
    templateKey.remove('_');
    return ":/config/" + templateKey + "_template.json";
}

// --------------------------------------------------------------------------
void PinoutWidget::buildPinPositions()
{
    const QHash<QString, int> connIdx = buildConnectorIndex();

    for(PinDesc &pin : m_pins){
        const int ci = connIdx.value(pin.connectorId, -1);
        if(ci < 0) continue;
        const QPointF pos = pinPos(m_connectors[ci], pin.row, pin.index);
        pin.cx = (float)pos.x();
        pin.cy = (float)pos.y();
    }
}

// --------------------------------------------------------------------------
QHash<QString, int> PinoutWidget::buildConnectorIndex() const
{
    QHash<QString, int> connectorIndexById;
    connectorIndexById.reserve(m_connectors.size());
    for(int i = 0; i < m_connectors.size(); ++i)
        connectorIndexById.insert(m_connectors[i].id, i);
    return connectorIndexById;
}

// --------------------------------------------------------------------------
const PinoutWidget::ConnectorDesc *PinoutWidget::connectorById(
    const QString &connectorId, const QHash<QString, int> &connectorIndexById) const
{
    const int idx = connectorIndexById.value(connectorId, -1);
    return (idx >= 0) ? &m_connectors[idx] : nullptr;
}

// --------------------------------------------------------------------------
float PinoutWidget::padSideFor(const ConnectorDesc &connector, float invScale) const
{
    const float grid = (connector.rows == 2)
                       ? qMin(connector.pitch, connector.rowSpacing)
                       : connector.pitch;
    return grid + 1.0f * invScale;
}

// --------------------------------------------------------------------------
QRectF PinoutWidget::bodyRectFor(const ConnectorDesc &connector, float invScale) const
{
    const float padW = padSideFor(connector, invScale);
    const float padH = padSideFor(connector, invScale);
    const float bodyX = connector.row0X - padW / 2.0f;
    const float bodyY = connector.pin0Y - padH / 2.0f;
    const float bodyW = (connector.rows == 2)
                        ? (connector.row1X - connector.row0X + padW)
                        : padW;
    const float bodyH = (connector.pinCount - 1) * connector.pitch + padH;
    return QRectF(bodyX, bodyY, bodyW, bodyH);
}

// --------------------------------------------------------------------------
QRectF PinoutWidget::combinedBody(const QHash<QString, QRectF> &connectorBodies,
                                  const QString &first,
                                  const QString &second) const
{
    if(connectorBodies.contains(first) && connectorBodies.contains(second))
        return connectorBodies.value(first).united(connectorBodies.value(second));
    if(connectorBodies.contains(first))
        return connectorBodies.value(first);
    if(connectorBodies.contains(second))
        return connectorBodies.value(second);
    return QRectF();
}

// --------------------------------------------------------------------------
QList<const PinoutWidget::PinDesc*> PinoutWidget::collectPins(
    const std::function<bool(const PinDesc &)> &predicate) const
{
    QList<const PinDesc*> pins;
    pins.reserve(m_pins.size());
    for(const PinDesc &candidate : m_pins){
        if(predicate(candidate))
            pins.append(&candidate);
    }
    return pins;
}

// --------------------------------------------------------------------------
PinoutWidget::PinLinkMap PinoutWidget::buildMorphoLinks(
    const QList<const PinDesc*> &morphoPins,
    const QList<const PinDesc*> &arduinoPins) const
{
    PinLinkMap links;
    QList<const PinDesc*> usedArduinoPins;
    for(const PinDesc *morphoPin : morphoPins){
        const PinDesc *bestArduinoPin = nullptr;
        qreal bestDistance = 1.0e9;
        for(const PinDesc *arduinoPin : arduinoPins){
            if(usedArduinoPins.contains(arduinoPin))
                continue;
            if(morphoPin->port.compare(arduinoPin->port, Qt::CaseInsensitive) != 0)
                continue;

            const qreal distance = qAbs(morphoPin->cy - arduinoPin->cy);
            if(distance < bestDistance){
                bestDistance = distance;
                bestArduinoPin = arduinoPin;
            }
        }

        if(bestArduinoPin && bestDistance <= 20.0){
            links.insert(morphoPin, bestArduinoPin);
            usedArduinoPins.append(bestArduinoPin);
        }
    }
    return links;
}

// --------------------------------------------------------------------------
qreal PinoutWidget::averageLinkDeltaY(const PinLinkMap &links)
{
    if(links.isEmpty())
        return 0.0;

    qreal totalDelta = 0.0;
    for(auto it = links.constBegin(); it != links.constEnd(); ++it)
        totalDelta += (it.value()->cy - it.key()->cy);
    return totalDelta / links.size();
}

// --------------------------------------------------------------------------
QTransform PinoutWidget::fitTransform(const QRectF &contentBounds, qreal marginPx) const
{
    const qreal availableW = qMax<qreal>(1.0, width() - 2.0 * marginPx);
    const qreal availableH = qMax<qreal>(1.0, height() - 2.0 * marginPx);
    const qreal fitScale = qMin(availableW / contentBounds.width(),
                                availableH / contentBounds.height());

    QTransform transform;
    transform.translate(marginPx + (availableW - fitScale * contentBounds.width()) / 2.0 - fitScale * contentBounds.left(),
                        marginPx + (availableH - fitScale * contentBounds.height()) / 2.0 - fitScale * contentBounds.top());
    transform.scale(fitScale, fitScale);
    return transform;
}

// --------------------------------------------------------------------------
QString PinoutWidget::overlayLabelText(const QString &label) const
{
    static const QRegularExpression channelSuffix(QStringLiteral("^(?:[A-Za-z]+\\s*)+(\\d+)\\s*$"),
                                                  QRegularExpression::CaseInsensitiveOption);
    const QRegularExpressionMatch match = channelSuffix.match(label.trimmed());
    if(match.hasMatch())
        return match.captured(1);

    return label;
}

// --------------------------------------------------------------------------
void PinoutWidget::setPinFunctions(const QList<PinFunctionInfo> &functions)
{
    m_functionMap.setFunctions(functions);
    m_iconCache.clear();
    update();
}

// --------------------------------------------------------------------------
void PinoutWidget::clearPinFunctions()
{
    m_functionMap.clear();
    m_iconCache.clear();
    update();
}

// --------------------------------------------------------------------------
void PinoutWidget::onThemeChanged()
{
    m_iconCache.clear();
    update();
}

// --------------------------------------------------------------------------
PinoutWidget::PaintColors PinoutWidget::buildPaintColors() const
{
    const ThemePalette &pal = Graphics::palette();
    const QColor bgColor(pal.windowWidget);

    PaintColors colors;
    colors.bgColor = bgColor;
    colors.portLabelColor = QColor(pal.textAll);
    colors.arduinoLabelColor = QColor(235, 0, 220);
    colors.activeFunctionColor = QColor(50, 220, 50);
    colors.boxOutlineColor = QColor(245, 245, 245);
    colors.stubColor = bgColor.lightnessF() < 0.45
                       ? QColor(196, 202, 214)
                       : QColor(110, 116, 126);
    return colors;
}

// --------------------------------------------------------------------------
PinoutWidget::PaintMetrics PinoutWidget::buildPaintMetrics(const QFont &baseFont) const
{
    PaintMetrics metrics;
    metrics.canvasScale = (float)qMin(width() / (float)m_canvasW,
                                      height() / (float)m_canvasH);
    if(metrics.canvasScale <= 0.0f)
        return metrics;

    metrics.uiScale = qBound(0.45f, metrics.canvasScale / 0.95f, 1.0f);
    metrics.invS = 1.f / metrics.canvasScale;
    metrics.iconSize = 22.0f * metrics.uiScale * metrics.invS;
    metrics.outerStubLen = 18.0f * metrics.uiScale * metrics.invS;
    metrics.stubStartGap = 0.9f * metrics.invS;
    metrics.groupGap = 10.0f * metrics.uiScale * metrics.invS;
    metrics.columnGap = 10.0f * metrics.uiScale * metrics.invS;
    metrics.overlayGap = 8.0f * metrics.uiScale * metrics.invS;
    metrics.outerTextGap = 7.0f * metrics.uiScale * metrics.invS;
    metrics.innerTextGap = 7.0f * metrics.uiScale * metrics.invS;
    metrics.showFunctionLabels = DISPLAY_PINOUT_CHANNEL && (metrics.uiScale >= 0.60f);
    metrics.showFunctionIcons = (metrics.uiScale >= 0.52f);

    metrics.labelFont = baseFont;
    metrics.labelFont.setPixelSize(qMax(7, (int)qRound(22.0f * metrics.uiScale * metrics.invS)));
    metrics.labelFm = QFontMetricsF(metrics.labelFont);

    metrics.connectorFont = metrics.labelFont;
    metrics.connectorFont.setWeight(QFont::Normal);
    metrics.connectorFont.setPixelSize(qMax(8, (int)qRound(28.0f * metrics.uiScale * metrics.invS)));
    metrics.connectorFm = QFontMetricsF(metrics.connectorFont);

    metrics.pinNumberFont = metrics.labelFont;
    metrics.pinNumberFont.setBold(true);
    metrics.pinNumberFont.setPixelSize(qMax(6, (int)qRound(18.0f * metrics.uiScale * metrics.invS)));
    metrics.pinNumberFm = QFontMetricsF(metrics.pinNumberFont);

    metrics.overlayFont = metrics.labelFont;
    metrics.overlayFont.setPixelSize(qMax(7, (int)qRound(OVERLAY_LABEL_FONT_PX * metrics.uiScale * metrics.invS)));
    metrics.overlayFm = QFontMetricsF(metrics.overlayFont);
    return metrics;
}

// --------------------------------------------------------------------------
QHash<QString, QRectF> PinoutWidget::buildConnectorBodies(float invScale) const
{
    QHash<QString, QRectF> connectorBodies;
    connectorBodies.reserve(m_connectors.size());
    for(const ConnectorDesc &connector : m_connectors)
        connectorBodies.insert(connector.id, bodyRectFor(connector, invScale));
    return connectorBodies;
}

// --------------------------------------------------------------------------
PinoutWidget::ColumnWidths PinoutWidget::computeColumnWidths(const QFontMetricsF &fontMetrics) const
{
    ColumnWidths widths;
    for(const PinDesc &pin : m_pins){
        if(pin.connectorId == "CN7" && pin.row == 0)
            widths.leftMorphoWidth = qMax(widths.leftMorphoWidth, fontMetrics.horizontalAdvance(pin.port));
        else if(pin.connectorId == "CN10" && pin.row == 1)
            widths.rightMorphoWidth = qMax(widths.rightMorphoWidth, fontMetrics.horizontalAdvance(pin.port));
        else if(pin.connectorId == "CN6" || pin.connectorId == "CN8"){
            widths.leftPortWidth = qMax(widths.leftPortWidth, fontMetrics.horizontalAdvance(pin.port));
            widths.leftArduinoWidth = qMax(widths.leftArduinoWidth, fontMetrics.horizontalAdvance(pin.arduino));
        } else if(pin.connectorId == "CN5" || pin.connectorId == "CN9"){
            widths.rightPortWidth = qMax(widths.rightPortWidth, fontMetrics.horizontalAdvance(pin.port));
            widths.rightArduinoWidth = qMax(widths.rightArduinoWidth, fontMetrics.horizontalAdvance(pin.arduino));
        }
    }
    return widths;
}

// --------------------------------------------------------------------------
PinoutWidget::LayoutState PinoutWidget::buildLayoutState(
    const PaintMetrics &metrics,
    const QHash<QString, int> &connectorIndexById) const
{
    LayoutState layout;
    layout.connectorBodies = buildConnectorBodies(metrics.invS);
    layout.columnWidths = computeColumnWidths(metrics.labelFm);

    layout.leftMorphoBody = layout.connectorBodies.value("CN7");
    layout.rightMorphoBody = layout.connectorBodies.value("CN10");
    layout.leftArduinoBody = combinedBody(layout.connectorBodies, "CN6", "CN8");
    layout.rightArduinoBody = combinedBody(layout.connectorBodies, "CN5", "CN9");

    layout.leftMorphoTextRight = layout.leftMorphoBody.left() - metrics.stubStartGap - metrics.outerStubLen - metrics.outerTextGap;
    layout.leftPortTextX = layout.leftArduinoBody.right() + metrics.groupGap;
    layout.leftArduinoTextX = layout.leftPortTextX + layout.columnWidths.leftPortWidth + metrics.columnGap;
    layout.rightPortTextRight = layout.rightArduinoBody.left() - metrics.groupGap;
    layout.rightArduinoTextRight = layout.rightPortTextRight - layout.columnWidths.rightPortWidth - metrics.columnGap;
    layout.rightMorphoTextX = layout.rightMorphoBody.right() + metrics.stubStartGap + metrics.outerStubLen + metrics.outerTextGap;

    layout.leftOuterOverlayX = layout.leftMorphoTextRight - layout.columnWidths.leftMorphoWidth - metrics.overlayGap - metrics.iconSize;
    layout.leftInnerOverlayX = layout.leftArduinoTextX + layout.columnWidths.leftArduinoWidth + metrics.overlayGap;
    layout.rightInnerOverlayX = layout.rightArduinoTextRight - layout.columnWidths.rightArduinoWidth - metrics.overlayGap - metrics.iconSize;
    layout.rightOuterOverlayX = layout.rightMorphoTextX + layout.columnWidths.rightMorphoWidth + metrics.overlayGap;
    layout.leftInnerMorphoTextX = layout.leftPortTextX;
    layout.rightInnerMorphoTextRight = layout.rightPortTextRight;

    const qreal labelOffsetY = 24.0f * metrics.uiScale * metrics.invS;
    layout.topLabelY = qMin(layout.leftMorphoBody.top(), layout.rightMorphoBody.top()) - labelOffsetY;
    layout.bottomLabelY = qMax(layout.leftArduinoBody.bottom(), layout.rightArduinoBody.bottom())
                          + metrics.connectorFm.ascent() + labelOffsetY;

    layout.leftInnerMorphoPins = collectPins([&](const PinDesc &candidate) {
        return candidate.connectorId == "CN7" && candidate.row == 1;
    });
    layout.rightInnerMorphoPins = collectPins([&](const PinDesc &candidate) {
        return candidate.connectorId == "CN10" && candidate.row == 0;
    });
    layout.leftArduinoPins = collectPins([&](const PinDesc &candidate) {
        return candidate.connectorId == "CN6" || candidate.connectorId == "CN8";
    });
    layout.rightArduinoPins = collectPins([&](const PinDesc &candidate) {
        return candidate.connectorId == "CN5" || candidate.connectorId == "CN9";
    });

    layout.leftInnerMorphoLinks = buildMorphoLinks(layout.leftInnerMorphoPins, layout.leftArduinoPins);
    layout.rightInnerMorphoLinks = buildMorphoLinks(layout.rightInnerMorphoPins, layout.rightArduinoPins);
    layout.leftInnerLineDeltaY = averageLinkDeltaY(layout.leftInnerMorphoLinks);
    layout.rightInnerLineDeltaY = averageLinkDeltaY(layout.rightInnerMorphoLinks);

    for(const PinDesc &candidate : m_pins){
        if(candidate.connectorId != "CN10" || candidate.row != 0)
            continue;
        if(layout.rightInnerMorphoLinks.contains(&candidate))
            continue;
        if(!layout.topRightUnlinkedPin || candidate.cy < layout.topRightUnlinkedPin->cy)
            layout.topRightUnlinkedPin = &candidate;
    }

    for(const PinDesc *candidate : layout.rightArduinoPins){
        if(candidate->connectorId != "CN5")
            continue;
        if(!layout.topRightArduinoPin || candidate->cy < layout.topRightArduinoPin->cy)
            layout.topRightArduinoPin = candidate;
    }

    if(layout.topRightUnlinkedPin && layout.topRightArduinoPin){
        const ConnectorDesc *arduinoConnector = connectorById(layout.topRightArduinoPin->connectorId,
                                                              connectorIndexById);
        if(arduinoConnector){
            layout.hasTopRightVirtualY = true;
            layout.topRightVirtualY = (qreal)layout.topRightArduinoPin->cy - arduinoConnector->pitch;
        }
    }

    qreal maxOverlayLabelWidth = 0.0;
    if(DISPLAY_PINOUT_CHANNEL){
        for(const PinFunctionInfo &function : m_functionMap.functions()){
            maxOverlayLabelWidth = qMax(maxOverlayLabelWidth,
                                        metrics.overlayFm.horizontalAdvance(overlayLabelText(function.label)));
        }
    }

    const qreal minOverlayLabelWidth = metrics.overlayFm.horizontalAdvance(QStringLiteral("Ref"));
    layout.maxOverlayIconWidth = metrics.iconSize * 2.0f;
    layout.overlayTextGap = qMax<qreal>(OVERLAY_TEXT_GAP_MIN_PX * metrics.uiScale * metrics.invS,
                                        metrics.overlayFm.horizontalAdvance(QString(OVERLAY_TEXT_GAP_SPACES,
                                                                                     QLatin1Char(' '))));
    layout.reservedOverlayLabelWidth = DISPLAY_PINOUT_CHANNEL
                                       ? qMax(maxOverlayLabelWidth, minOverlayLabelWidth)
                                       : 0.0;
    layout.reservedOverlayTextGap = DISPLAY_PINOUT_CHANNEL ? layout.overlayTextGap : 0.0;

    layout.contentBounds = layout.leftMorphoBody.united(layout.rightMorphoBody)
                                            .united(layout.leftArduinoBody)
                                            .united(layout.rightArduinoBody);
    layout.contentBounds = layout.contentBounds.united(QRectF(layout.leftMorphoTextRight - layout.columnWidths.leftMorphoWidth,
                                                              layout.leftMorphoBody.top() - metrics.labelFm.ascent(),
                                                              layout.columnWidths.leftMorphoWidth,
                                                              layout.leftMorphoBody.height() + metrics.labelFm.height()));
    layout.contentBounds = layout.contentBounds.united(QRectF(layout.leftPortTextX,
                                                              layout.leftArduinoBody.top() - metrics.labelFm.ascent(),
                                                              layout.columnWidths.leftPortWidth,
                                                              layout.leftArduinoBody.height() + metrics.labelFm.height()));
    layout.contentBounds = layout.contentBounds.united(QRectF(layout.leftArduinoTextX,
                                                              layout.leftArduinoBody.top() - metrics.labelFm.ascent(),
                                                              layout.columnWidths.leftArduinoWidth,
                                                              layout.leftArduinoBody.height() + metrics.labelFm.height()));
    layout.contentBounds = layout.contentBounds.united(QRectF(layout.rightMorphoTextX,
                                                              layout.rightMorphoBody.top() - metrics.labelFm.ascent(),
                                                              layout.columnWidths.rightMorphoWidth,
                                                              layout.rightMorphoBody.height() + metrics.labelFm.height()));
    layout.contentBounds = layout.contentBounds.united(QRectF(layout.rightArduinoTextRight - layout.columnWidths.rightArduinoWidth,
                                                              layout.rightArduinoBody.top() - metrics.labelFm.ascent(),
                                                              layout.columnWidths.rightArduinoWidth,
                                                              layout.rightArduinoBody.height() + metrics.labelFm.height()));
    layout.contentBounds = layout.contentBounds.united(QRectF(layout.rightPortTextRight - layout.columnWidths.rightPortWidth,
                                                              layout.rightArduinoBody.top() - metrics.labelFm.ascent(),
                                                              layout.columnWidths.rightPortWidth,
                                                              layout.rightArduinoBody.height() + metrics.labelFm.height()));
    layout.contentBounds = layout.contentBounds.united(QRectF(layout.leftOuterOverlayX - layout.reservedOverlayLabelWidth - layout.reservedOverlayTextGap,
                                                              layout.leftMorphoBody.top() - metrics.overlayFm.ascent(),
                                                              layout.reservedOverlayLabelWidth + layout.reservedOverlayTextGap + layout.maxOverlayIconWidth,
                                                              layout.leftMorphoBody.height() + metrics.overlayFm.height()));
    layout.contentBounds = layout.contentBounds.united(QRectF(layout.leftInnerOverlayX,
                                                              layout.leftArduinoBody.top() - metrics.overlayFm.ascent(),
                                                              layout.maxOverlayIconWidth + layout.reservedOverlayTextGap + layout.reservedOverlayLabelWidth,
                                                              layout.leftArduinoBody.height() + metrics.overlayFm.height()));
    layout.contentBounds = layout.contentBounds.united(QRectF(layout.rightInnerOverlayX - layout.reservedOverlayLabelWidth - layout.reservedOverlayTextGap,
                                                              layout.rightArduinoBody.top() - metrics.overlayFm.ascent(),
                                                              layout.reservedOverlayLabelWidth + layout.reservedOverlayTextGap + layout.maxOverlayIconWidth,
                                                              layout.rightArduinoBody.height() + metrics.overlayFm.height()));
    layout.contentBounds = layout.contentBounds.united(QRectF(layout.rightOuterOverlayX,
                                                              layout.rightMorphoBody.top() - metrics.overlayFm.ascent(),
                                                              layout.maxOverlayIconWidth + layout.reservedOverlayTextGap + layout.reservedOverlayLabelWidth,
                                                              layout.rightMorphoBody.height() + metrics.overlayFm.height()));

    for(const ConnectorDesc &connector : m_connectors){
        const QRectF body = layout.connectorBodies.value(connector.id);
        const qreal textWidth = metrics.connectorFm.horizontalAdvance(connector.label);
        const qreal textX = body.center().x() - textWidth / 2.0;
        const qreal textY = (connector.id == "CN8" || connector.id == "CN9")
                            ? layout.bottomLabelY
                            : layout.topLabelY;
        layout.contentBounds = layout.contentBounds.united(QRectF(textX,
                                                                  textY - metrics.connectorFm.ascent(),
                                                                  textWidth,
                                                                  metrics.connectorFm.height()));
    }

    if(layout.topRightUnlinkedPin){
        const qreal topRightTextWidth = metrics.labelFm.horizontalAdvance(layout.topRightUnlinkedPin->port);
        const qreal topRightBaseline = rightInnerUnlinkedBaseline(layout,
                                                                  metrics.labelFm,
                                                                  *layout.topRightUnlinkedPin);
        layout.contentBounds = layout.contentBounds.united(QRectF(layout.rightInnerMorphoTextRight - topRightTextWidth,
                                                                  topRightBaseline - metrics.labelFm.ascent(),
                                                                  topRightTextWidth,
                                                                  metrics.labelFm.height()));
    }

    if(layout.hasTopRightVirtualY){
        layout.contentBounds = layout.contentBounds.united(QRectF(layout.rightArduinoBody.left(),
                                                                  layout.topRightVirtualY,
                                                                  layout.rightArduinoBody.width(),
                                                                  layout.rightArduinoBody.top() - layout.topRightVirtualY));
    }

    layout.contentBounds.adjust(0.0, 0.0, metrics.outerTextGap, 0.0);
    return layout;
}

// --------------------------------------------------------------------------
QColor PinoutWidget::connectorColor(const QString &connectorId) const
{
    if(connectorId == "CN7" || connectorId == "CN10")
        return QColor(24, 32, 230);
    return QColor(235, 0, 220);
}

// --------------------------------------------------------------------------
qreal PinoutWidget::pinLabelBaseline(const QFontMetricsF &fontMetrics, qreal centerY) const
{
    return centerY + (fontMetrics.ascent() - fontMetrics.descent()) / 2.0;
}

// --------------------------------------------------------------------------
qreal PinoutWidget::rightInnerUnlinkedCenterY(const LayoutState &layout, const PinDesc &pin) const
{
    if(&pin == layout.topRightUnlinkedPin && layout.hasTopRightVirtualY)
        return layout.topRightVirtualY;
    return (qreal)pin.cy + layout.rightInnerLineDeltaY;
}

// --------------------------------------------------------------------------
qreal PinoutWidget::rightInnerUnlinkedBaseline(const LayoutState &layout,
                                               const QFontMetricsF &fontMetrics,
                                               const PinDesc &pin) const
{
    return pinLabelBaseline(fontMetrics, rightInnerUnlinkedCenterY(layout, pin));
}

// --------------------------------------------------------------------------
void PinoutWidget::drawFunctionOverlay(QPainter &p,
                                       const PaintColors &colors,
                                       const PaintMetrics &metrics,
                                       const LayoutState &layout,
                                       const PinDesc &pin,
                                       const PinFunctionInfo &function) const
{
    const bool isLeftMorpho = (pin.connectorId == "CN7");
    const bool isRightMorpho = (pin.connectorId == "CN10");
    const bool isLeftArduino = (pin.connectorId == "CN6" || pin.connectorId == "CN8");

    const QString iconKey = function.moduleName;
    const QString overlayLabel = DISPLAY_PINOUT_CHANNEL ? overlayLabelText(function.label)
                                                        : QString();
    if(!m_iconCache.contains(iconKey)){
        const QString iconPath = Graphics::getCommonPath() + "icon_" + iconKey + ".png";
        QPixmap src = Graphics::tintedPixmap(iconPath, colors.activeFunctionColor);
        m_iconCache.insert(iconKey, src);
    }

    const QPixmap &icon = m_iconCache.value(iconKey);
    qreal overlayCenterY = pin.cy;
    if(isLeftMorpho && pin.row == 1 && !layout.leftInnerMorphoLinks.contains(&pin))
        overlayCenterY += layout.leftInnerLineDeltaY;
    else if(isRightMorpho && pin.row == 0 && !layout.rightInnerMorphoLinks.contains(&pin))
        overlayCenterY = rightInnerUnlinkedCenterY(layout, pin);

    bool overlayRight = true;
    qreal iconX = 0.0;
    if(isLeftMorpho){
        if(pin.row == 0){
            overlayRight = false;
            iconX = layout.leftOuterOverlayX;
        } else {
            iconX = layout.leftInnerOverlayX;
        }
    } else if(isRightMorpho){
        if(pin.row == 1){
            iconX = layout.rightOuterOverlayX;
        } else {
            overlayRight = false;
            iconX = layout.rightInnerOverlayX;
        }
    } else if(isLeftArduino){
        iconX = layout.leftInnerOverlayX;
    } else {
        overlayRight = false;
        iconX = layout.rightInnerOverlayX;
    }

    const qreal iconY = overlayCenterY - metrics.iconSize / 2.0f;
    const qreal iconWidth = icon.isNull()
                            ? metrics.iconSize
                            : metrics.iconSize * ((qreal)icon.width() / icon.height());
    if(metrics.showFunctionIcons && !icon.isNull()){
        p.drawPixmap(QRectF(iconX, iconY, iconWidth, metrics.iconSize),
                     icon,
                     QRectF(0.0, 0.0, icon.width(), icon.height()));
    }

    if(metrics.showFunctionLabels){
        p.setFont(metrics.overlayFont);
        p.setPen(colors.activeFunctionColor);
        const qreal textBaseY = pinLabelBaseline(metrics.overlayFm, overlayCenterY);
        if(overlayRight){
            p.drawText(QPointF(iconX + iconWidth + layout.overlayTextGap, textBaseY), overlayLabel);
        } else {
            const qreal labelWidth = metrics.overlayFm.horizontalAdvance(overlayLabel);
            p.drawText(QPointF(iconX - layout.overlayTextGap - labelWidth, textBaseY), overlayLabel);
        }
        p.setFont(metrics.labelFont);
    }
}

// --------------------------------------------------------------------------
void PinoutWidget::drawConnectorLabels(QPainter &p,
                                       const PaintColors &colors,
                                       const PaintMetrics &metrics,
                                       const LayoutState &layout) const
{
    p.setFont(metrics.connectorFont);
    for(const ConnectorDesc &connector : m_connectors){
        if(!layout.connectorBodies.contains(connector.id))
            continue;

        const QRectF body = layout.connectorBodies.value(connector.id);
        const qreal textWidth = metrics.connectorFm.horizontalAdvance(connector.label);
        const qreal textX = body.center().x() - textWidth / 2.0;
        const bool isArduino = (connector.id == "CN6" || connector.id == "CN8"
                                || connector.id == "CN5" || connector.id == "CN9");
        const qreal textY = (connector.id == "CN8" || connector.id == "CN9")
                            ? layout.bottomLabelY
                            : layout.topLabelY;

        p.setPen(isArduino ? colors.arduinoLabelColor : colors.portLabelColor.darker(120));
        p.drawText(QPointF(textX, textY), connector.label);
    }
}

void PinoutWidget::paintEvent(QPaintEvent *)
{
    const PaintColors colors = buildPaintColors();

    QPainter p(this);
    if(m_connectors.isEmpty()){
        p.fillRect(rect(), colors.bgColor);
        return;
    }

    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    p.fillRect(rect(), colors.bgColor);

    const PaintMetrics metrics = buildPaintMetrics(p.font());
    if(metrics.canvasScale <= 0.0f)
        return;

    p.setFont(metrics.labelFont);
    const QFontMetricsF &fm = metrics.labelFm;
    const QFontMetricsF &pinNoFm = metrics.pinNumberFm;

    const QHash<QString, int> connectorIndexById = buildConnectorIndex();

    const LayoutState layout = buildLayoutState(metrics, connectorIndexById);
    p.setTransform(fitTransform(layout.contentBounds, 8.0));

    auto drawLeftColumnText = [&](const QString &txt, qreal x, qreal baselineY, const QColor &col) {
        if(txt.isEmpty())
            return;
        p.setPen(col);
        p.drawText(QPointF(x, baselineY), txt);
    };

    auto drawRightColumnText = [&](const QString &txt, qreal rightX, qreal baselineY, const QColor &col) {
        if(txt.isEmpty())
            return;
        p.setPen(col);
        const qreal tw = fm.horizontalAdvance(txt);
        p.drawText(QPointF(rightX - tw, baselineY), txt);
    };

    // Keep clean visual style close to the official image: no cross-board net spaghetti.

    // ------ Draw pins -------------------------------------------------------
    for(const PinDesc &pin : m_pins){
        if(pin.cx == 0.f && pin.cy == 0.f) continue;

        const ConnectorDesc *connector = connectorById(pin.connectorId, connectorIndexById);
        if(!connector)
            continue;

        const float padW = padSideFor(*connector, metrics.invS);
        const float padH = padW;
        const bool isSingleRow = (connector->rows == 1);
        const bool isLeftArduino = (pin.connectorId == "CN6" || pin.connectorId == "CN8");
        const bool isRightArduino = (pin.connectorId == "CN5" || pin.connectorId == "CN9");
        const bool isLeftMorpho = (pin.connectorId == "CN7");
        const bool isRightMorpho = (pin.connectorId == "CN10");
        const bool isLinkedMorphoDuplicate =
            (isLeftMorpho && pin.row == 1 && layout.leftInnerMorphoLinks.contains(&pin)) ||
            (isRightMorpho && pin.row == 0 && layout.rightInnerMorphoLinks.contains(&pin));

        const PinFunctionInfo *func = m_functionMap.pinFunction(pin.port, pin.arduino);
        const bool hasActiveFunction = (func != nullptr);

        // Draw the official-style connector lines first so the pads can sit on top.
        p.setPen(QPen(colors.stubColor, 3.0f * metrics.uiScale * metrics.invS, Qt::SolidLine, Qt::RoundCap));

        if(!isSingleRow){
            if(isLeftMorpho && pin.row == 0){
                p.drawLine(QPointF(pin.cx - padW / 2.0f - metrics.stubStartGap, pin.cy),
                           QPointF(pin.cx - padW / 2.0f - metrics.stubStartGap - metrics.outerStubLen, pin.cy));
            } else if(isRightMorpho && pin.row == 1){
                p.drawLine(QPointF(pin.cx + padW / 2.0f + metrics.stubStartGap, pin.cy),
                           QPointF(pin.cx + padW / 2.0f + metrics.stubStartGap + metrics.outerStubLen, pin.cy));
            } else if(isLeftMorpho){
                const PinDesc *arduinoPin = layout.leftInnerMorphoLinks.value(&pin, nullptr);
                if(arduinoPin){
                    const ConnectorDesc *arduinoConnector = connectorById(arduinoPin->connectorId, connectorIndexById);
                    const float arduinoPadW = arduinoConnector ? padSideFor(*arduinoConnector, metrics.invS) : 24.0f * metrics.invS;
                    p.drawLine(QPointF(pin.cx + padW / 2.0f + metrics.stubStartGap, pin.cy),
                               QPointF(arduinoPin->cx - arduinoPadW / 2.0f - metrics.stubStartGap, arduinoPin->cy));
                } else {
                    p.drawLine(QPointF(pin.cx + padW / 2.0f + metrics.stubStartGap, pin.cy),
                               QPointF(layout.leftPortTextX - metrics.innerTextGap,
                                       pin.cy + layout.leftInnerLineDeltaY));
                }
            } else if(isRightMorpho){
                const PinDesc *arduinoPin = layout.rightInnerMorphoLinks.value(&pin, nullptr);
                if(arduinoPin){
                    const ConnectorDesc *arduinoConnector = connectorById(arduinoPin->connectorId, connectorIndexById);
                    const float arduinoPadW = arduinoConnector ? padSideFor(*arduinoConnector, metrics.invS) : 24.0f * metrics.invS;
                    p.drawLine(QPointF(arduinoPin->cx + arduinoPadW / 2.0f + metrics.stubStartGap, arduinoPin->cy),
                               QPointF(pin.cx - padW / 2.0f - metrics.stubStartGap, pin.cy));
                } else {
                    const QPointF pinPoint(pin.cx - padW / 2.0f - metrics.stubStartGap, pin.cy);
                    const qreal targetY = rightInnerUnlinkedCenterY(layout, pin);
                    const qreal lineEndX = layout.rightInnerMorphoTextRight + metrics.outerTextGap;
                    if(&pin == layout.topRightUnlinkedPin && layout.topRightArduinoPin && layout.hasTopRightVirtualY){
                        const ConnectorDesc *arduinoConnector = connectorById(layout.topRightArduinoPin->connectorId,
                                                                              connectorIndexById);
                        if(arduinoConnector){
                            const float arduinoPadW = padSideFor(*arduinoConnector, metrics.invS);
                            const QPointF bendPoint(layout.topRightArduinoPin->cx + arduinoPadW / 2.0f + metrics.stubStartGap,
                                                    targetY);
                            const QPointF textPoint(lineEndX, targetY);
                            p.drawLine(pinPoint, bendPoint);
                            p.drawLine(bendPoint, textPoint);
                        }
                    } else {
                        const qreal angledRun = qMin((pinPoint.x() - lineEndX) * 0.42,
                                                     qAbs(targetY - pin.cy) / 1.75);
                        const qreal bendX = pinPoint.x() - angledRun;

                        const QPointF bendPoint(bendX, targetY);
                        const QPointF textPoint(lineEndX, targetY);
                        p.drawLine(pinPoint, bendPoint);
                        p.drawLine(bendPoint, textPoint);
                    }
                }
            }
        }

        // Draw numbered rectangular pin pad.
        const QColor padFill = hasActiveFunction ? colors.activeFunctionColor : connectorColor(pin.connectorId);
        const QRectF pinRect(pin.cx - padW / 2.0f, pin.cy - padH / 2.0f, padW, padH);
        p.setBrush(padFill);
        p.setPen(QPen(colors.boxOutlineColor, 1.3f * metrics.invS));
        p.drawRect(pinRect);

        int pinNo = 0;
        if(!isSingleRow)
            pinNo = pin.index * 2 + pin.row + 1;
        else if(isRightArduino)
            pinNo = connector->pinCount - pin.index;
        else
            pinNo = pin.index + 1;

        p.setFont(metrics.pinNumberFont);
        p.setPen(QColor(245, 245, 245));
        const QString pinNoTxt = QString::number(pinNo);
        const qreal noW = pinNoFm.horizontalAdvance(pinNoTxt);
        const qreal noX = pin.cx - noW / 2.0;
        const qreal noY = pinLabelBaseline(pinNoFm, pin.cy);
        p.drawText(QPointF(noX, noY), pinNoTxt);
        p.setFont(metrics.labelFont);

        // --- Labels ---------------------------------------------------------
        const qreal centerBaseline = pinLabelBaseline(fm, pin.cy);

        if(!isSingleRow){
            if(isLeftMorpho && pin.row == 0)
                drawRightColumnText(pin.port, layout.leftMorphoTextRight, centerBaseline, colors.portLabelColor);
            else if(isLeftMorpho && !layout.leftInnerMorphoLinks.contains(&pin))
                drawLeftColumnText(pin.port,
                                   layout.leftInnerMorphoTextX,
                                   centerBaseline + layout.leftInnerLineDeltaY,
                                   colors.portLabelColor);
            else if(isRightMorpho && pin.row == 1)
                drawLeftColumnText(pin.port, layout.rightMorphoTextX, centerBaseline, colors.portLabelColor);
            else if(isRightMorpho && !layout.rightInnerMorphoLinks.contains(&pin))
                drawRightColumnText(pin.port,
                                    layout.rightInnerMorphoTextRight,
                                    rightInnerUnlinkedBaseline(layout, fm, pin),
                                    colors.portLabelColor);
        } else if(isLeftArduino){
            drawLeftColumnText(pin.port, layout.leftPortTextX, centerBaseline, colors.portLabelColor);
            drawLeftColumnText(pin.arduino, layout.leftArduinoTextX, centerBaseline, colors.arduinoLabelColor);
        } else if(isRightArduino){
            drawRightColumnText(pin.arduino, layout.rightArduinoTextRight, centerBaseline, colors.arduinoLabelColor);
            drawRightColumnText(pin.port, layout.rightPortTextRight, centerBaseline, colors.portLabelColor);
        }

        // --- Function overlay icon + channel label --------------------------
        if(func && !isLinkedMorphoDuplicate)
            drawFunctionOverlay(p, colors, metrics, layout, pin, *func);
    }

    drawConnectorLabels(p, colors, metrics, layout);
}
