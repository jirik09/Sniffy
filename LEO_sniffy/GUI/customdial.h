#ifndef CUSTOMDIAL_H
#define CUSTOMDIAL_H

#include <QDial>
#include <QTimer>
#include <functional>
#include "math.h"

#include "../graphics/graphics.h"

#define MOUSE_DRAG_DISTANCE 0.6 // higher number more sensitive (short distance big change)

class CustomDial : public QDial
{
    Q_OBJECT

public:
    CustomDial(QWidget *parent = nullptr);
    ~CustomDial();

    void setCustomGraphics(bool enable);
    void drawMarker(bool draw);

    // Override setValue to capture first value as default
    void setValue(int value);

    // Set default value for double-click reset
    void setDefaultValue(int value);

    // Callback for double-click
    void setDoubleClickCallback(std::function<void()> callback);

private:
    std::function<void()> doubleClickCallback;

    void mouseMoveEvent(QMouseEvent *me) override;
    void mousePressEvent(QMouseEvent *me) override;
    void mouseReleaseEvent(QMouseEvent *me) override;
    void mouseDoubleClickEvent(QMouseEvent *me) override;

    virtual void paintEvent(QPaintEvent *) override;

    int mousePressX;
    int mousePressY;
    int initialDialValue;
    int defaultValue = 0;                 // Store default value for double-click reset
    bool defaultValueInitialized = false; // Track if default was ever set

    QTimer *mouseMoveDebounceTimer = nullptr;
    int pendingDialValue = 0;
    bool pendingDialValueValid = false;

    bool customGraphics = false;
    bool drawMark = true;
    bool doubleClickInProgress = false;

signals:
    void dialPressed(QMouseEvent *me);
    void dialReleased(QMouseEvent *me);
};

#endif // CUSTOMDIAL_H
