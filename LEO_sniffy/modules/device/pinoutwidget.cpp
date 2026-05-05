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
    QString templateKey = templateId.toLower();
    // Template files are named without separators, e.g. "nucleo64_template.json".
    templateKey.remove('-');
    templateKey.remove('_');
    const QString tmplPath = ":/config/" + templateKey + "_template.json";
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
        p.port        = o.value("port").toString();
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
void PinoutWidget::buildPinPositions()
{
    // Build a quick lookup: connector id → index in m_connectors
    QHash<QString,int> connIdx;
    for(int i = 0; i < m_connectors.size(); ++i)
        connIdx.insert(m_connectors[i].id, i);

    for(PinDesc &pin : m_pins){
        const int ci = connIdx.value(pin.connectorId, -1);
        if(ci < 0) continue;
        const QPointF pos = pinPos(m_connectors[ci], pin.row, pin.index);
        pin.cx = (float)pos.x();
        pin.cy = (float)pos.y();
    }
}

// --------------------------------------------------------------------------
void PinoutWidget::setPinFunctions(const QList<PinFunctionInfo> &functions)
{
    m_functions = functions;
    m_funcByPort.clear();
    m_funcByArduino.clear();
    m_iconCache.clear();
    for(const PinFunctionInfo &f : m_functions){
        if(!f.pin.isEmpty()){
            m_funcByPort.insert(f.pin, &f);
            m_funcByArduino.insert(f.pin, &f);
        }
    }
    update();
}

// --------------------------------------------------------------------------
void PinoutWidget::clearPinFunctions()
{
    m_functions.clear();
    m_funcByPort.clear();
    m_funcByArduino.clear();
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
const PinFunctionInfo *PinoutWidget::pinFunction(const QString &port, const QString &arduino) const
{
    // Check by arduino label first (firmware sends arduino-style names like "A5", "D13")
    if(!arduino.isEmpty()){
        auto it = m_funcByArduino.find(arduino);
        if(it != m_funcByArduino.end()) return it.value();
    }
    // Then by MCU port name
    if(!port.isEmpty()){
        auto it = m_funcByPort.find(port);
        if(it != m_funcByPort.end()) return it.value();
    }
    return nullptr;
}

// --------------------------------------------------------------------------
bool PinoutWidget::isPowerPin(const QString &port)
{
    static const QStringList powerPins = {
        "GND","AGND","5V","+5V","E5V","U5V","3.3V","+3V3","VDD","VBAT",
        "AVDD","AREF","VIN","IOREF","NRST","RESET","NC","BOOT0"
    };
    return powerPins.contains(port, Qt::CaseInsensitive);
}

// --------------------------------------------------------------------------
QTransform PinoutWidget::canvasTransform() const
{
    const float sw = width()  / (float)m_canvasW;
    const float sh = height() / (float)m_canvasH;
    const float s  = qMin(sw, sh);
    // Centre the scaled canvas
    const float ox = (width()  - s * m_canvasW) / 2.f;
    const float oy = (height() - s * m_canvasH) / 2.f;
    QTransform t;
    t.translate(ox, oy);
    t.scale(s, s);
    return t;
}

// --------------------------------------------------------------------------
void PinoutWidget::paintEvent(QPaintEvent *)
{
    if(m_connectors.isEmpty()){
        QPainter p(this);
        p.fillRect(rect(), QColor(Graphics::palette().windowWidget));
        return;
    }

    const ThemePalette &pal = Graphics::palette();
    const QColor bgColor(pal.windowWidget);
    const QColor connBodyColor(pal.display);
    const QColor portLabelColor(pal.textLabel);
    const QColor arduinoLabelColor(235, 0, 220); // Pink color for all Arduino-associated text
    const QColor pinFreeColor(pal.controls);
    const QColor pinPowerColor(pal.componentDisabled);
    const bool darkBg = bgColor.lightnessF() < 0.45;
    const QColor boxOutlineColor(245, 245, 245);
    const QColor stubColor = darkBg ? QColor(196, 202, 214) : QColor(110, 116, 126);
    const QColor overlayTextColor = darkBg ? QColor(226, 230, 238) : QColor(92, 96, 104);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    // Fill background
    p.fillRect(rect(), bgColor);

    const float canvasScale = (float)qMin(width() / (float)m_canvasW,
                                          height() / (float)m_canvasH);
    if(canvasScale <= 0.0f)
        return;

    // Keep key visual elements readable in actual screen pixels.
    const float uiScale = qBound(0.45f, canvasScale / 0.95f, 1.0f);
    const float invS = 1.f / canvasScale;
    const float ICON_SIZE_V = 22.0f * uiScale * invS;
    const float OUTER_STUB_LEN = 18.0f * uiScale * invS;
    const float STUB_START_GAP = 0.9f * invS;
    const float GROUP_GAP = 10.0f * uiScale * invS;
    const float COLUMN_GAP = 10.0f * uiScale * invS;
    const float OVERLAY_GAP = 8.0f * uiScale * invS;
    const float CONNECTOR_LABEL_GAP = 4.0f * uiScale * invS;
    const float OUTER_TEXT_GAP = 7.0f * uiScale * invS;
    const float INNER_TEXT_GAP = 7.0f * uiScale * invS;
    const bool showFunctionLabels = (uiScale >= 0.60f);
    const bool showFunctionIcons = (uiScale >= 0.52f);

    // Convert desired screen px size into canvas coords (because painter is transformed).
    QFont labelFont = p.font();
    labelFont.setPixelSize(qMax(7, (int)qRound(22.0f * uiScale * invS)));
    p.setFont(labelFont);
    QFontMetricsF fm(labelFont);

    QFont connFont = labelFont;
    connFont.setWeight(QFont::Normal);
    connFont.setPixelSize(qMax(8, (int)qRound(28.0f * uiScale * invS)));
    QFontMetricsF connFm(connFont);

    QFont pinNumberFont = labelFont;
    pinNumberFont.setBold(true);
    pinNumberFont.setPixelSize(qMax(6, (int)qRound(18.0f * uiScale * invS)));
    QFontMetricsF pinNoFm(pinNumberFont);

    QHash<QString, int> connectorIndexById;
    for(int i = 0; i < m_connectors.size(); ++i)
        connectorIndexById.insert(m_connectors[i].id, i);

    auto connectorById = [&](const QString &connectorId) -> const ConnectorDesc * {
        const int idx = connectorIndexById.value(connectorId, -1);
        return (idx >= 0) ? &m_connectors[idx] : nullptr;
    };

    auto padSideFor = [&](const ConnectorDesc &connector) {
        const float grid = (connector.rows == 2)
                           ? qMin(connector.pitch, connector.rowSpacing)
                           : connector.pitch;
        return grid + 1.0f * invS;
    };

    auto padWidthFor = [&](const ConnectorDesc &connector) {
        return padSideFor(connector);
    };

    auto padHeightFor = [&](const ConnectorDesc &connector) {
        return padSideFor(connector);
    };

    auto bodyRectFor = [&](const ConnectorDesc &connector) {
        const float padW = padWidthFor(connector);
        const float padH = padHeightFor(connector);
        const float bodyX = connector.row0X - padW / 2.0f;
        const float bodyY = connector.pin0Y - padH / 2.0f;
        const float bodyW = (connector.rows == 2)
                            ? (connector.row1X - connector.row0X + padW)
                            : padW;
        const float bodyH = (connector.pinCount - 1) * connector.pitch + padH;
        return QRectF(bodyX, bodyY, bodyW, bodyH);
    };

    auto connectorColor = [&](const QString &connId) {
        // Match official style: Morpho headers blue, Arduino headers magenta.
        if(connId == "CN7" || connId == "CN10")
            return QColor(24, 32, 230);
        return QColor(235, 0, 220);
    };

    QHash<QString, QRectF> connectorBodies;

    // ------ Draw connector bodies ------------------------------------------
    for(const ConnectorDesc &c : m_connectors){
        const QRectF body = bodyRectFor(c);
        connectorBodies.insert(c.id, body);
    }

    auto combinedBody = [&](const QString &first, const QString &second) {
        if(connectorBodies.contains(first) && connectorBodies.contains(second))
            return connectorBodies.value(first).united(connectorBodies.value(second));
        if(connectorBodies.contains(first))
            return connectorBodies.value(first);
        if(connectorBodies.contains(second))
            return connectorBodies.value(second);
        return QRectF();
    };

    qreal leftMorphoWidth = 0.0;
    qreal rightMorphoWidth = 0.0;
    qreal leftPortWidth = 0.0;
    qreal leftArduinoWidth = 0.0;
    qreal rightPortWidth = 0.0;
    qreal rightArduinoWidth = 0.0;
    for(const PinDesc &pin : m_pins){
        if(pin.connectorId == "CN7" && pin.row == 0)
            leftMorphoWidth = qMax(leftMorphoWidth, fm.horizontalAdvance(pin.port));
        else if(pin.connectorId == "CN10" && pin.row == 1)
            rightMorphoWidth = qMax(rightMorphoWidth, fm.horizontalAdvance(pin.port));
        else if(pin.connectorId == "CN6" || pin.connectorId == "CN8"){
            leftPortWidth = qMax(leftPortWidth, fm.horizontalAdvance(pin.port));
            leftArduinoWidth = qMax(leftArduinoWidth, fm.horizontalAdvance(pin.arduino));
        } else if(pin.connectorId == "CN5" || pin.connectorId == "CN9"){
            rightPortWidth = qMax(rightPortWidth, fm.horizontalAdvance(pin.port));
            rightArduinoWidth = qMax(rightArduinoWidth, fm.horizontalAdvance(pin.arduino));
        }
    }

    const QRectF leftMorphoBody = connectorBodies.value("CN7");
    const QRectF rightMorphoBody = connectorBodies.value("CN10");
    const QRectF leftArduinoBody = combinedBody("CN6", "CN8");
    const QRectF rightArduinoBody = combinedBody("CN5", "CN9");

    const qreal leftMorphoTextRight = leftMorphoBody.left() - STUB_START_GAP - OUTER_STUB_LEN - OUTER_TEXT_GAP;
    const qreal leftPortTextX = leftArduinoBody.right() + GROUP_GAP;
    const qreal leftArduinoTextX = leftPortTextX + leftPortWidth + COLUMN_GAP;
    const qreal rightPortTextRight = rightArduinoBody.left() - GROUP_GAP;
    const qreal rightArduinoTextRight = rightPortTextRight - rightPortWidth - COLUMN_GAP;
    const qreal rightMorphoTextX = rightMorphoBody.right() + STUB_START_GAP + OUTER_STUB_LEN + OUTER_TEXT_GAP;

    const qreal leftOuterOverlayX = leftMorphoTextRight - leftMorphoWidth - OVERLAY_GAP - ICON_SIZE_V;
    const qreal leftInnerOverlayX = leftArduinoTextX + leftArduinoWidth + OVERLAY_GAP;
    const qreal rightInnerOverlayX = rightArduinoTextRight - rightArduinoWidth - OVERLAY_GAP - ICON_SIZE_V;
    const qreal rightOuterOverlayX = rightMorphoTextX + rightMorphoWidth + OVERLAY_GAP;
    const qreal rightInnerLineStartX = rightPortTextRight + INNER_TEXT_GAP;
    const qreal leftInnerMorphoTextX = leftPortTextX;
    const qreal rightInnerMorphoTextRight = rightPortTextRight;

    // Use unified offset for connector labels from their own bounds
    const qreal LABEL_OFFSET_Y = 24.0f * uiScale * invS;
    const qreal topLabelY = qMin(leftMorphoBody.top(), rightMorphoBody.top()) - LABEL_OFFSET_Y;
    const qreal bottomLabelY = qMax(leftArduinoBody.bottom(), rightArduinoBody.bottom()) + connFm.ascent() + LABEL_OFFSET_Y;



    auto collectPins = [&](auto predicate) {
        QList<const PinDesc*> pins;
        pins.reserve(m_pins.size());
        for(const PinDesc &candidate : m_pins){
            if(predicate(candidate))
                pins.append(&candidate);
        }
        return pins;
    };

    auto buildMorphoLinks = [&](const QList<const PinDesc*> &morphoPins,
                                const QList<const PinDesc*> &arduinoPins) {
        QHash<const PinDesc*, const PinDesc*> links;
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
    };

    const QList<const PinDesc*> leftInnerMorphoPins = collectPins([&](const PinDesc &candidate) {
        return candidate.connectorId == "CN7" && candidate.row == 1;
    });
    const QList<const PinDesc*> rightInnerMorphoPins = collectPins([&](const PinDesc &candidate) {
        return candidate.connectorId == "CN10" && candidate.row == 0;
    });
    const QList<const PinDesc*> leftArduinoPins = collectPins([&](const PinDesc &candidate) {
        return candidate.connectorId == "CN6" || candidate.connectorId == "CN8";
    });
    const QList<const PinDesc*> rightArduinoPins = collectPins([&](const PinDesc &candidate) {
        return candidate.connectorId == "CN5" || candidate.connectorId == "CN9";
    });
    const QHash<const PinDesc*, const PinDesc*> leftInnerMorphoLinks =
        buildMorphoLinks(leftInnerMorphoPins, leftArduinoPins);
    const QHash<const PinDesc*, const PinDesc*> rightInnerMorphoLinks =
        buildMorphoLinks(rightInnerMorphoPins, rightArduinoPins);

    auto averageLinkDeltaY = [&](const QHash<const PinDesc*, const PinDesc*> &links) {
        if(links.isEmpty())
            return 0.0;

        qreal totalDelta = 0.0;
        for(auto it = links.constBegin(); it != links.constEnd(); ++it)
            totalDelta += (it.value()->cy - it.key()->cy);
        return totalDelta / links.size();
    };

    const qreal leftInnerLineDeltaY = averageLinkDeltaY(leftInnerMorphoLinks);
    const qreal rightInnerLineDeltaY = averageLinkDeltaY(rightInnerMorphoLinks);

    const PinDesc *topRightUnlinkedPin = nullptr;
    for(const PinDesc &candidate : m_pins){
        if(candidate.connectorId != "CN10" || candidate.row != 0)
            continue;
        if(rightInnerMorphoLinks.contains(&candidate))
            continue;
        if(!topRightUnlinkedPin || candidate.cy < topRightUnlinkedPin->cy)
            topRightUnlinkedPin = &candidate;
    }

    const PinDesc *topRightArduinoPin = nullptr;
    for(const PinDesc *candidate : rightArduinoPins){
        if(candidate->connectorId != "CN5")
            continue;
        if(!topRightArduinoPin || candidate->cy < topRightArduinoPin->cy)
            topRightArduinoPin = candidate;
    }

    auto rightInnerUnlinkedCenterY = [&](const PinDesc &pin) {
        if(&pin == topRightUnlinkedPin && topRightArduinoPin){
            const ConnectorDesc *arduinoConnector = connectorById(topRightArduinoPin->connectorId);
            if(arduinoConnector)
                return (qreal)topRightArduinoPin->cy - arduinoConnector->pitch;
        }
        return (qreal)pin.cy + rightInnerLineDeltaY;
    };

    auto rightInnerUnlinkedBaseline = [&](const PinDesc &pin) {
        return rightInnerUnlinkedCenterY(pin) + (fm.ascent() - fm.descent()) / 2.0;
    };

    QFont overlayFont = labelFont;
    overlayFont.setPixelSize(qMax(8, (int)qRound(19.0f * uiScale * invS)));
    QFontMetricsF overlayFm(overlayFont);
    qreal maxOverlayLabelWidth = 0.0;
    for(const PinFunctionInfo &func : m_functions)
        maxOverlayLabelWidth = qMax(maxOverlayLabelWidth, overlayFm.horizontalAdvance(func.label));
    const qreal maxOverlayIconWidth = ICON_SIZE_V * 2.0f;
    const qreal overlayTextGap = 3.0f * uiScale * invS;

    QRectF contentBounds = leftMorphoBody.united(rightMorphoBody)
                                       .united(leftArduinoBody)
                                       .united(rightArduinoBody);
    contentBounds = contentBounds.united(QRectF(leftMorphoTextRight - leftMorphoWidth,
                                                leftMorphoBody.top() - fm.ascent(),
                                                leftMorphoWidth,
                                                leftMorphoBody.height() + fm.height()));
    contentBounds = contentBounds.united(QRectF(leftPortTextX,
                                                leftArduinoBody.top() - fm.ascent(),
                                                leftPortWidth,
                                                leftArduinoBody.height() + fm.height()));
    contentBounds = contentBounds.united(QRectF(leftArduinoTextX,
                                                leftArduinoBody.top() - fm.ascent(),
                                                leftArduinoWidth,
                                                leftArduinoBody.height() + fm.height()));
    contentBounds = contentBounds.united(QRectF(rightMorphoTextX,
                                                rightMorphoBody.top() - fm.ascent(),
                                                rightMorphoWidth,
                                                rightMorphoBody.height() + fm.height()));
    contentBounds = contentBounds.united(QRectF(rightArduinoTextRight - rightArduinoWidth,
                                                rightArduinoBody.top() - fm.ascent(),
                                                rightArduinoWidth,
                                                rightArduinoBody.height() + fm.height()));
    contentBounds = contentBounds.united(QRectF(rightPortTextRight - rightPortWidth,
                                                rightArduinoBody.top() - fm.ascent(),
                                                rightPortWidth,
                                                rightArduinoBody.height() + fm.height()));
    contentBounds = contentBounds.united(QRectF(leftOuterOverlayX - maxOverlayLabelWidth - overlayTextGap,
                                                leftMorphoBody.top() - overlayFm.ascent(),
                                                maxOverlayLabelWidth + overlayTextGap + maxOverlayIconWidth,
                                                leftMorphoBody.height() + overlayFm.height()));
    contentBounds = contentBounds.united(QRectF(leftInnerOverlayX,
                                                leftArduinoBody.top() - overlayFm.ascent(),
                                                maxOverlayIconWidth + overlayTextGap + maxOverlayLabelWidth,
                                                leftArduinoBody.height() + overlayFm.height()));
    contentBounds = contentBounds.united(QRectF(rightInnerOverlayX - maxOverlayLabelWidth - overlayTextGap,
                                                rightArduinoBody.top() - overlayFm.ascent(),
                                                maxOverlayLabelWidth + overlayTextGap + maxOverlayIconWidth,
                                                rightArduinoBody.height() + overlayFm.height()));
    contentBounds = contentBounds.united(QRectF(rightOuterOverlayX,
                                                rightMorphoBody.top() - overlayFm.ascent(),
                                                maxOverlayIconWidth + overlayTextGap + maxOverlayLabelWidth,
                                                rightMorphoBody.height() + overlayFm.height()));
    for(const ConnectorDesc &c : m_connectors){
        const QRectF body = connectorBodies.value(c.id);
        const qreal tw = connFm.horizontalAdvance(c.label);
        const qreal tx = body.center().x() - tw / 2.0;
        const qreal ty = (c.id == "CN8" || c.id == "CN9") ? bottomLabelY : topLabelY;
        contentBounds = contentBounds.united(QRectF(tx,
                                                    ty - connFm.ascent(),
                                                    tw,
                                                    connFm.height()));
    }
    if(topRightUnlinkedPin){
        const qreal topRightTextWidth = fm.horizontalAdvance(topRightUnlinkedPin->port);
        const qreal topRightBaseline = rightInnerUnlinkedBaseline(*topRightUnlinkedPin);
        contentBounds = contentBounds.united(QRectF(rightInnerMorphoTextRight - topRightTextWidth,
                                                    topRightBaseline - fm.ascent(),
                                                    topRightTextWidth,
                                                    fm.height()));
    }
    if(topRightArduinoPin){
        const ConnectorDesc *arduinoConnector = connectorById(topRightArduinoPin->connectorId);
        if(arduinoConnector){
            const qreal virtualY = (qreal)topRightArduinoPin->cy - arduinoConnector->pitch;
            contentBounds = contentBounds.united(QRectF(rightArduinoBody.left(),
                                                        virtualY,
                                                        rightArduinoBody.width(),
                                                        rightArduinoBody.top() - virtualY));
        }
    }
    contentBounds.adjust(0.0, 0.0, OUTER_TEXT_GAP, 0.0);

    const qreal fitMarginPx = 8.0;
    const qreal availableW = qMax<qreal>(1.0, width() - 2.0 * fitMarginPx);
    const qreal availableH = qMax<qreal>(1.0, height() - 2.0 * fitMarginPx);
    const qreal fitScale = qMin(availableW / contentBounds.width(),
                                availableH / contentBounds.height());
    QTransform fitTransform;
    fitTransform.translate(fitMarginPx + (availableW - fitScale * contentBounds.width()) / 2.0 - fitScale * contentBounds.left(),
                           fitMarginPx + (availableH - fitScale * contentBounds.height()) / 2.0 - fitScale * contentBounds.top());
    fitTransform.scale(fitScale, fitScale);
    p.setTransform(fitTransform);

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

        const ConnectorDesc *connector = connectorById(pin.connectorId);
        if(!connector)
            continue;

        const float padW = padWidthFor(*connector);
        const float padH = padHeightFor(*connector);
        const bool isSingleRow = (connector->rows == 1);
        const bool isLeftArduino = (pin.connectorId == "CN6" || pin.connectorId == "CN8");
        const bool isRightArduino = (pin.connectorId == "CN5" || pin.connectorId == "CN9");
        const bool isLeftMorpho = (pin.connectorId == "CN7");
        const bool isRightMorpho = (pin.connectorId == "CN10");

        const PinFunctionInfo *func = pinFunction(pin.port, pin.arduino);
        const bool isPwr = isPowerPin(pin.port) || isPowerPin(pin.arduino);

        // Choose dot colour
        QColor dotColor;
        if(func){
            dotColor = QColor(Graphics::getChannelColor(0));  // will refine per-module below
        } else if(isPwr){
            dotColor = pinPowerColor;
        } else {
            dotColor = pinFreeColor;
        }

        // Per-module channel colour
        if(func){
            // Map module name to a channel index for colour variety
            const QString &mn = func->moduleName;
            int chIdx = 0;
            if(mn == "scope")             chIdx = 0;
            else if(mn == "counter")      chIdx = 1;
            else if(mn == "signal_generator" || mn == "pwm_gen") chIdx = 2;
            else if(mn == "pattern_generator") chIdx = 3;
            else if(mn == "syncpwm")      chIdx = 4;
            dotColor = QColor(Graphics::getChannelColor(chIdx));
        }

        // Draw the official-style connector lines first so the pads can sit on top.
        p.setPen(QPen(stubColor, 3.0f * uiScale * invS, Qt::SolidLine, Qt::RoundCap));

        if(!isSingleRow){
            if(isLeftMorpho && pin.row == 0){
                p.drawLine(QPointF(pin.cx - padW / 2.0f - STUB_START_GAP, pin.cy),
                           QPointF(pin.cx - padW / 2.0f - STUB_START_GAP - OUTER_STUB_LEN, pin.cy));
            } else if(isRightMorpho && pin.row == 1){
                p.drawLine(QPointF(pin.cx + padW / 2.0f + STUB_START_GAP, pin.cy),
                           QPointF(pin.cx + padW / 2.0f + STUB_START_GAP + OUTER_STUB_LEN, pin.cy));
            } else if(isLeftMorpho){
                const PinDesc *arduinoPin = leftInnerMorphoLinks.value(&pin, nullptr);
                if(arduinoPin){
                    const ConnectorDesc *arduinoConnector = connectorById(arduinoPin->connectorId);
                    const float arduinoPadW = arduinoConnector ? padWidthFor(*arduinoConnector) : 24.0f * invS;
                    p.drawLine(QPointF(pin.cx + padW / 2.0f + STUB_START_GAP, pin.cy),
                               QPointF(arduinoPin->cx - arduinoPadW / 2.0f - STUB_START_GAP, arduinoPin->cy));
                } else {
                    p.drawLine(QPointF(pin.cx + padW / 2.0f + STUB_START_GAP, pin.cy),
                               QPointF(leftPortTextX - INNER_TEXT_GAP, pin.cy + leftInnerLineDeltaY));
                }
            } else if(isRightMorpho){
                const PinDesc *arduinoPin = rightInnerMorphoLinks.value(&pin, nullptr);
                if(arduinoPin){
                    const ConnectorDesc *arduinoConnector = connectorById(arduinoPin->connectorId);
                    const float arduinoPadW = arduinoConnector ? padWidthFor(*arduinoConnector) : 24.0f * invS;
                    p.drawLine(QPointF(arduinoPin->cx + arduinoPadW / 2.0f + STUB_START_GAP, arduinoPin->cy),
                               QPointF(pin.cx - padW / 2.0f - STUB_START_GAP, pin.cy));
                } else {
                    const QPointF pinPoint(pin.cx - padW / 2.0f - STUB_START_GAP, pin.cy);
                    const qreal targetY = rightInnerUnlinkedCenterY(pin);
                    const qreal lineEndX = rightInnerMorphoTextRight + OUTER_TEXT_GAP;
                    if(&pin == topRightUnlinkedPin && topRightArduinoPin){
                        const ConnectorDesc *arduinoConnector = connectorById(topRightArduinoPin->connectorId);
                        if(arduinoConnector){
                            const float arduinoPadW = padWidthFor(*arduinoConnector);
                            const QPointF bendPoint(topRightArduinoPin->cx + arduinoPadW / 2.0f + STUB_START_GAP,
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
        const QColor padFill = connectorColor(pin.connectorId);
        const QRectF pinRect(pin.cx - padW / 2.0f, pin.cy - padH / 2.0f, padW, padH);
        p.setBrush(padFill);
        p.setPen(QPen(boxOutlineColor, 1.3f * invS));
        p.drawRect(pinRect);

        int pinNo = 0;
        if(!isSingleRow)
            pinNo = pin.index * 2 + pin.row + 1;
        else if(isRightArduino)
            pinNo = connector->pinCount - pin.index;
        else
            pinNo = pin.index + 1;

        p.setFont(pinNumberFont);
        p.setPen(QColor(245, 245, 245));
        const QString pinNoTxt = QString::number(pinNo);
        const qreal noW = pinNoFm.horizontalAdvance(pinNoTxt);
        const qreal noX = pin.cx - noW / 2.0;
        const qreal noY = pin.cy + (pinNoFm.ascent() - pinNoFm.descent()) / 2.0;
        p.drawText(QPointF(noX, noY), pinNoTxt);
        p.setFont(labelFont);

        // --- Labels ---------------------------------------------------------
        const float centerBaseline = pin.cy + (fm.ascent() - fm.descent()) / 2.0f;

        if(!isSingleRow){
            if(isLeftMorpho && pin.row == 0)
                drawRightColumnText(pin.port, leftMorphoTextRight, centerBaseline, portLabelColor);
            else if(isLeftMorpho && !leftInnerMorphoLinks.contains(&pin))
                drawLeftColumnText(pin.port, leftInnerMorphoTextX, centerBaseline + leftInnerLineDeltaY, portLabelColor);
            else if(isRightMorpho && pin.row == 1)
                drawLeftColumnText(pin.port, rightMorphoTextX, centerBaseline, portLabelColor);
            else if(isRightMorpho && !rightInnerMorphoLinks.contains(&pin))
                drawRightColumnText(pin.port, rightInnerMorphoTextRight, rightInnerUnlinkedBaseline(pin), portLabelColor);
        } else if(isLeftArduino){
            drawLeftColumnText(pin.port, leftPortTextX, centerBaseline, portLabelColor);
            drawLeftColumnText(pin.arduino, leftArduinoTextX, centerBaseline, arduinoLabelColor);
        } else if(isRightArduino){
            drawRightColumnText(pin.arduino, rightArduinoTextRight, centerBaseline, arduinoLabelColor);
            drawRightColumnText(pin.port, rightPortTextRight, centerBaseline, portLabelColor);
        }

        // --- Function overlay icon + channel label --------------------------
        if(func){
            const QString iconKey = func->moduleName;
            if(!m_iconCache.contains(iconKey)){
                const QString iconPath = Graphics::getCommonPath() + "icon_" + iconKey + ".png";
                QPixmap src = Graphics::tintedPixmap(iconPath, QColor(50, 220, 50));
                if(!src.isNull()){
                    m_iconCache.insert(iconKey, src);
                } else {
                    m_iconCache.insert(iconKey, QPixmap());
                }
            }
            const QPixmap &ico = m_iconCache.value(iconKey);

            // Keep overlays on the same side as the visible label columns.
            bool overlayRight = true;
            float iconX = 0.0f;
            if(isLeftMorpho){
                overlayRight = false;
                iconX = leftOuterOverlayX;
            } else if(isRightMorpho){
                overlayRight = true;
                iconX = rightOuterOverlayX;
            } else if(isLeftArduino){
                overlayRight = true;
                iconX = leftInnerOverlayX;
            } else {
                overlayRight = false;
                iconX = rightInnerOverlayX;
            }

            const float iconY = pin.cy - ICON_SIZE_V / 2.0f;
            if(showFunctionIcons && !ico.isNull()){
                // Draw icon maintaining ratio
                float iconW = ICON_SIZE_V * ((float)ico.width() / ico.height());
                p.drawPixmap(QRectF(iconX, iconY, iconW, ICON_SIZE_V),
                             ico,
                             QRectF(0.0, 0.0, ico.width(), ico.height()));
            }

            // Channel label next to icon.
            if(showFunctionLabels){
                p.setFont(overlayFont);
                p.setPen(QColor(50, 220, 50)); // Distinctly green
                QFontMetricsF smFm(overlayFont);
                const float textBaseY = pin.cy + smFm.ascent() / 2.0f;
                float iconW = ico.isNull() ? ICON_SIZE_V : ICON_SIZE_V * ((float)ico.width() / ico.height());
                if(overlayRight){
                    const float textX = iconX + iconW + 3.0f * uiScale * invS;
                    p.drawText(QPointF(textX, textBaseY), func->label);
                } else {
                    const qreal lw = smFm.horizontalAdvance(func->label);
                    const float textX = iconX - 3.0f * uiScale * invS - lw;
                    p.drawText(QPointF(textX, textBaseY), func->label);
                }
            }
            p.setFont(labelFont);
        }
    }

    // Draw connector labels as the top-most layer so they stay clear on resize.
    p.setFont(connFont);
    for(const ConnectorDesc &c : m_connectors){
        if(!connectorBodies.contains(c.id))
            continue;
        const QRectF body = connectorBodies.value(c.id);
        const qreal tw = connFm.horizontalAdvance(c.label);
        const qreal tx = body.center().x() - tw / 2.0;

        const bool isArduino = (c.id == "CN6" || c.id == "CN8" || c.id == "CN5" || c.id == "CN9");
        p.setPen(isArduino ? arduinoLabelColor : portLabelColor.darker(120));
        
        qreal ty;
        if(c.id == "CN8" || c.id == "CN9"){
            ty = bottomLabelY;
        } else {
            ty = topLabelY;
        }
        
        p.drawText(QPointF(tx, ty), c.label);
    }
}
