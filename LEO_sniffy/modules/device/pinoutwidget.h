#ifndef PINOUTWIDGET_H
#define PINOUTWIDGET_H

#include <QWidget>
#include <QList>
#include <QHash>
#include <QPixmap>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <functional>

#include "../pinfunctioninfo.h"

// A data-driven vector pinout widget for STM32 NUCLEO boards.
// Loads connector geometry from a template JSON and pin labels from a board JSON
// (both embedded as Qt resources). Module pin-function overlays are injected at
// runtime via setPinFunctions() without touching any data file.
class PinoutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PinoutWidget(QWidget *parent = nullptr);

    // Load layout template + board pin data from QRC (e.g. "Nucleo-G474RE")
    void setBoard(const QString &boardId);
    // Update the module function overlay and repaint
    void setPinFunctions(const QList<PinFunctionInfo> &functions);
    void setActiveModules(const QSet<QString> &activeModules);
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

    // Helpers
    const PinFunctionInfo *pinFunction(const QString &port, const QString &arduino) const;

    // Canvas virtualisation
    QTransform canvasTransform() const;
    static constexpr float CANVAS_W = 1000.f;
    static constexpr float CANVAS_H = 700.f;

    // Data
    int                        m_canvasW = 1000;
    int                        m_canvasH = 700;
    QList<ConnectorDesc>       m_connectors;
    QList<PinDesc>             m_pins;

    // Pin-function overlay: keyed by port name and Arduino name
    QList<PinFunctionInfo>     m_functions;
    QHash<QString,const PinFunctionInfo*> m_funcByPort;
    QHash<QString,const PinFunctionInfo*> m_funcByArduino;
    QSet<QString>              m_activeModules;

    // Icon cache: moduleName → tinted pixmap (scaled to icon size)
    mutable QHash<QString,QPixmap> m_iconCache;
    static constexpr int ICON_SIZE = 14;

    // Special colours for power/GND pins
    static bool isPowerPin(const QString &port);
};

#endif // PINOUTWIDGET_H
