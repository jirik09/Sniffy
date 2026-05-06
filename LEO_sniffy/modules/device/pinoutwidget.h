#ifndef PINOUTWIDGET_H
#define PINOUTWIDGET_H

#include <QWidget>
#include <QList>
#include <QHash>
#include <QColor>
#include <QFont>
#include <QFontMetricsF>
#include <QPixmap>
#include <QJsonObject>
#include <QJsonArray>
#include <QRectF>
#include <QRegularExpression>
#include <QTransform>
#include <functional>

#include "pinoutfunctionmap.h"
#include "../pinfunctioninfo.h"

class QPainter;

// A data-driven vector pinout widget for STM32 NUCLEO boards.
// Loads connector geometry from a template JSON and pin labels from a board JSON
// (both embedded as Qt resources). Module pin-function overlays are injected at
// runtime via setPinFunctions() without touching any data file.
class PinoutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PinoutWidget(QWidget *parent = nullptr);

    // Pinout overlay tuning.
    static constexpr bool DISPLAY_PINOUT_CHANNEL = true;
    static constexpr int OVERLAY_TEXT_GAP_SPACES = 2;
    static constexpr float OVERLAY_TEXT_GAP_MIN_PX = 3.0f;
    static constexpr float OVERLAY_LABEL_FONT_PX = 16.0f;

    // Load layout template + board pin data from QRC (e.g. "Nucleo-G474RE")
    void setBoard(const QString &boardId);
    // Update the module function overlay and repaint
    void setPinFunctions(const QList<PinFunctionInfo> &functions);
    // Remove all function overlays and repaint
    void clearPinFunctions();

public slots:
    void onThemeChanged();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // ---- Internal pin descriptor (fully resolved at load time) ----
    struct PinDesc {
        QString connectorId;
        int     row;        // 0 = inner/only row, 1 = outer row (Morpho)
        int     index;      // position within row (0-based top-to-bottom)
        QString port;       // MCU port label, e.g. "PA5", "GND", "5V"
        QString arduino;    // Arduino label, e.g. "A0", "D13" (empty if none)

        // Computed screen position (virtual canvas coordinates)
        float   cx;         // centre x of the pin dot
        float   cy;         // centre y of the pin dot
    };

    // ---- Connector descriptor (from template) ----
    struct ConnectorDesc {
        QString id;
        QString label;
        float   anchorX, anchorY;          // connector body top-left
        int     rows;                       // 1 (Arduino) or 2 (Morpho)
        int     pinCount;                   // pins per row
        float   pitch;                      // y-spacing between pins
        float   rowSpacing;                 // x-spacing between the two rows (Morpho)
        float   row0X;                      // x for row 0 (or only row)
        float   row1X;                      // x for row 1 (Morpho only)
        float   pin0Y;                      // y of first pin
    };

    using PinLinkMap = QHash<const PinDesc*, const PinDesc*>;

    struct PaintColors {
        QColor bgColor;
        QColor portLabelColor;
        QColor arduinoLabelColor;
        QColor activeFunctionColor;
        QColor boxOutlineColor;
        QColor stubColor;
    };

    struct PaintMetrics {
        PaintMetrics()
            : labelFm(QFont())
            , connectorFm(QFont())
            , pinNumberFm(QFont())
            , overlayFm(QFont())
        {
        }

        float canvasScale = 0.0f;
        float uiScale = 1.0f;
        float invS = 1.0f;
        float iconSize = 0.0f;
        float outerStubLen = 0.0f;
        float stubStartGap = 0.0f;
        float groupGap = 0.0f;
        float columnGap = 0.0f;
        float overlayGap = 0.0f;
        float outerTextGap = 0.0f;
        float innerTextGap = 0.0f;
        bool showFunctionLabels = false;
        bool showFunctionIcons = false;
        QFont labelFont;
        QFont connectorFont;
        QFont pinNumberFont;
        QFont overlayFont;
        QFontMetricsF labelFm;
        QFontMetricsF connectorFm;
        QFontMetricsF pinNumberFm;
        QFontMetricsF overlayFm;
    };

    struct ColumnWidths {
        qreal leftMorphoWidth = 0.0;
        qreal rightMorphoWidth = 0.0;
        qreal leftPortWidth = 0.0;
        qreal leftArduinoWidth = 0.0;
        qreal rightPortWidth = 0.0;
        qreal rightArduinoWidth = 0.0;
    };

    struct LayoutState {
        QHash<QString, QRectF> connectorBodies;
        ColumnWidths columnWidths;
        QRectF leftMorphoBody;
        QRectF rightMorphoBody;
        QRectF leftArduinoBody;
        QRectF rightArduinoBody;
        qreal leftMorphoTextRight = 0.0;
        qreal leftPortTextX = 0.0;
        qreal leftArduinoTextX = 0.0;
        qreal rightPortTextRight = 0.0;
        qreal rightArduinoTextRight = 0.0;
        qreal rightMorphoTextX = 0.0;
        qreal leftOuterOverlayX = 0.0;
        qreal leftInnerOverlayX = 0.0;
        qreal rightInnerOverlayX = 0.0;
        qreal rightOuterOverlayX = 0.0;
        qreal leftInnerMorphoTextX = 0.0;
        qreal rightInnerMorphoTextRight = 0.0;
        qreal topLabelY = 0.0;
        qreal bottomLabelY = 0.0;
        QList<const PinDesc*> leftInnerMorphoPins;
        QList<const PinDesc*> rightInnerMorphoPins;
        QList<const PinDesc*> leftArduinoPins;
        QList<const PinDesc*> rightArduinoPins;
        PinLinkMap leftInnerMorphoLinks;
        PinLinkMap rightInnerMorphoLinks;
        qreal leftInnerLineDeltaY = 0.0;
        qreal rightInnerLineDeltaY = 0.0;
        const PinDesc *topRightUnlinkedPin = nullptr;
        const PinDesc *topRightArduinoPin = nullptr;
        bool hasTopRightVirtualY = false;
        qreal topRightVirtualY = 0.0;
        qreal maxOverlayIconWidth = 0.0;
        qreal overlayTextGap = 0.0;
        qreal reservedOverlayLabelWidth = 0.0;
        qreal reservedOverlayTextGap = 0.0;
        QRectF contentBounds;
    };

    void parseTemplate(const QJsonObject &tmpl);
    void parsePins(const QJsonArray &pinsArr);
    void buildPinPositions();
    QPointF pinPos(const ConnectorDesc &c, int row, int index) const;
    QString templatePathFor(const QString &templateId) const;

    QHash<QString, int> buildConnectorIndex() const;
    const ConnectorDesc *connectorById(const QString &connectorId,
                                       const QHash<QString, int> &connectorIndexById) const;
    float padSideFor(const ConnectorDesc &connector, float invScale) const;
    QRectF bodyRectFor(const ConnectorDesc &connector, float invScale) const;
    QRectF combinedBody(const QHash<QString, QRectF> &connectorBodies,
                        const QString &first, const QString &second) const;
    QList<const PinDesc*> collectPins(const std::function<bool(const PinDesc &)> &predicate) const;
    PinLinkMap buildMorphoLinks(const QList<const PinDesc*> &morphoPins,
                                const QList<const PinDesc*> &arduinoPins) const;
    static qreal averageLinkDeltaY(const PinLinkMap &links);
    QTransform fitTransform(const QRectF &contentBounds, qreal marginPx) const;
    QString overlayLabelText(const QString &label) const;
    PaintColors buildPaintColors() const;
    PaintMetrics buildPaintMetrics(const QFont &baseFont) const;
    QHash<QString, QRectF> buildConnectorBodies(float invScale) const;
    ColumnWidths computeColumnWidths(const QFontMetricsF &fontMetrics) const;
    LayoutState buildLayoutState(const PaintMetrics &metrics,
                                 const QHash<QString, int> &connectorIndexById) const;
    QColor connectorColor(const QString &connectorId) const;
    qreal pinLabelBaseline(const QFontMetricsF &fontMetrics, qreal centerY) const;
    qreal rightInnerUnlinkedCenterY(const LayoutState &layout, const PinDesc &pin) const;
    qreal rightInnerUnlinkedBaseline(const LayoutState &layout,
                                     const QFontMetricsF &fontMetrics,
                                     const PinDesc &pin) const;
    void drawFunctionOverlay(QPainter &p,
                             const PaintColors &colors,
                             const PaintMetrics &metrics,
                             const LayoutState &layout,
                             const PinDesc &pin,
                             const PinFunctionInfo &function) const;
    void drawConnectorLabels(QPainter &p,
                             const PaintColors &colors,
                             const PaintMetrics &metrics,
                             const LayoutState &layout) const;

    // Data
    int                        m_canvasW = 1000;
    int                        m_canvasH = 700;
    QList<ConnectorDesc>       m_connectors;
    QList<PinDesc>             m_pins;
    PinoutFunctionMap          m_functionMap;

    // Icon cache: moduleName → tinted pixmap (scaled to icon size)
    mutable QHash<QString,QPixmap> m_iconCache;
};

#endif // PINOUTWIDGET_H
