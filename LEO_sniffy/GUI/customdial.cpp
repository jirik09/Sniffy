/*
Class for custom dial graphics.
Extends standard dial and just override the paint function
*/

#include "customdial.h"

#include <QPainter>
#include <QColor>
#include <QMouseEvent>
#include <QTimer>

#include <cmath>

CustomDial::CustomDial(QWidget *parent)
    : QDial(parent)
{
    // Default range
    QDial::setRange(0, 100);
    QDial::setPageStep(5);
    QDial::setSingleStep(2);
    // Don't set defaultValue here - it will be set later when the dial gets its actual value

    mouseMoveDebounceTimer = new QTimer(this);
    mouseMoveDebounceTimer->setInterval(20);
    mouseMoveDebounceTimer->setSingleShot(true);
    connect(mouseMoveDebounceTimer, &QTimer::timeout, this, [this]()
            {
        if (doubleClickInProgress) {
            pendingDialValueValid = false;
            return;
        }
        if (!pendingDialValueValid) {
            return;
        }
        QDial::setValue(pendingDialValue);
        pendingDialValueValid = false; });
}

CustomDial::~CustomDial()
{
}

void CustomDial::setCustomGraphics(bool enable)
{
    customGraphics = enable;
}

void CustomDial::drawMarker(bool draw)
{
    drawMark = draw;
}

void CustomDial::setValue(int value)
{
    // If this is the first time setValue is called, store as default
    if (!defaultValueInitialized)
    {
        defaultValue = value;
        defaultValueInitialized = true;
    }
    // Call parent implementation
    QDial::setValue(value);
}

void CustomDial::setDefaultValue(int value)
{
    defaultValue = value;
    defaultValueInitialized = true; // Mark as explicitly set
}

void CustomDial::setDoubleClickCallback(std::function<void()> callback)
{
    doubleClickCallback = callback;
}

void CustomDial::mouseMoveEvent(QMouseEvent *me)
{
    // Ignore mouse movement if we're in the middle of processing a double-click
    if (doubleClickInProgress)
    {
        return;
    }

    // Qt6: x()/y() deprecated -> use position()
    const QPointF pos = me->position();
    double relativeX = pos.x() - mousePressX + 0.01;
    double relativeY = pos.y() - mousePressY + 0.01;

    // Calculate distance from -xy axis
    double distance = abs(relativeX - relativeY) / sqrt(2);

    double phi = atan(relativeX / relativeY) * 180 / 3.14159;

    double angle = 0;
    if (relativeY > 0)
    {
        angle = 90 - phi;
    }
    else
    {
        angle = 270 - phi;
    }

    int newValue = 0;
    if (angle > 225 || angle < 45)
    {
        newValue = initialDialValue + distance / QDial::size().width() * QDial::maximum() * MOUSE_DRAG_DISTANCE;
    }
    else
    {
        newValue = initialDialValue - distance / QDial::size().width() * QDial::maximum() * MOUSE_DRAG_DISTANCE;
    }

    pendingDialValue = newValue;
    pendingDialValueValid = true;

    // Throttle updates to avoid flooding valueChanged() while dragging.
    // If the timer is already running, it will apply the latest pending value when it fires.
    if (mouseMoveDebounceTimer && !mouseMoveDebounceTimer->isActive())
    {
        mouseMoveDebounceTimer->start();
    }
}

void CustomDial::mousePressEvent(QMouseEvent *me)
{
    // Ignore mouse press if we're in the middle of processing a double-click
    if (doubleClickInProgress)
    {
        me->accept();
        return;
    }

    if (mouseMoveDebounceTimer)
    {
        mouseMoveDebounceTimer->stop();
    }
    pendingDialValueValid = false;

    emit dialPressed(me);
    const QPointF pos = me->position();
    mousePressX = pos.x();
    mousePressY = pos.y();
    initialDialValue = QDial::value();
}

void CustomDial::mouseReleaseEvent(QMouseEvent *me)
{
    emit dialReleased(me);

    // Never call QDial::mouseReleaseEvent - it causes problems with value changes
    // Our custom drag implementation handles value changes in mouseMoveEvent

    mousePressX = 0;
    mousePressY = 0;

    // Flush any pending drag update immediately on release.
    if (mouseMoveDebounceTimer)
    {
        mouseMoveDebounceTimer->stop();
    }
    if (pendingDialValueValid)
    {
        QDial::setValue(pendingDialValue);
        pendingDialValueValid = false;
    }

    // Reset flag after a delay if it's set
    // This allows double-click event to be processed first
    if (doubleClickInProgress)
    {
        QTimer::singleShot(100, this, [this]()
                           { doubleClickInProgress = false; });
    }

    QDial::update();
}

void CustomDial::mouseDoubleClickEvent(QMouseEvent *me)
{
    // Set flag to prevent subsequent mouse events from changing value
    doubleClickInProgress = true;

    if (mouseMoveDebounceTimer)
    {
        mouseMoveDebounceTimer->stop();
    }
    pendingDialValueValid = false;

    // Reset to default value directly
    QDial::setValue(defaultValue);

    // Call callback if set (for WidgetDial to update its display)
    if (doubleClickCallback)
    {
        doubleClickCallback();
    }

    // Accept the event to prevent further propagation
    me->accept();
}

void CustomDial::paintEvent(QPaintEvent *evt)
{
    if (!customGraphics)
    {
        QDial::paintEvent(evt);
        return;
    }
    int margin = 16;

    QPainter painter(this);

    // So that we can use the background color
    painter.setBackgroundMode(Qt::OpaqueMode);

    // Smooth out the circle
    painter.setRenderHint(QPainter::Antialiasing);

    // Use background color
    painter.setBrush(painter.background());

    // Store color from stylesheet, pen will be overriden
    QColor pointColor(painter.pen().color());

    // No border
    painter.setPen(QPen(Qt::NoPen));

    // Draw first circle
    //  painter.drawEllipse(0, 0, QDial::height(), QDial::height());

    // Reset color to pointColor from stylesheet
    painter.setBrush(QBrush(pointColor));

    // Get ratio between current value and maximum to calculate angle
    double ratio = static_cast<double>(QDial::value()) / QDial::maximum();

    int size = (QDial::width() < QDial::height()) ? QDial::width() : QDial::height();
    int diffH = (QDial::width() > QDial::height()) ? QDial::width() - QDial::height() : 0;
    int diffV = (QDial::width() < QDial::height()) ? QDial::height() - QDial::width() : 0;

    margin = margin * size / 100;
    int marginHalf = margin / 2;
    const QRectF rect(marginHalf + diffH / 2, marginHalf + diffV / 2, size - margin, size - margin);

    Qt::BrushStyle bs;

    // draw background arc
    if (drawMark)
    {
        bs = (Qt::BrushStyle)(size / 30 + 3); // 5 for static size
        painter.setPen(QPen(QBrush(QColor(Graphics::palette().dataInputArea)), bs));
    }
    else
    {
        bs = (Qt::BrushStyle)2;
        painter.setPen(QPen(QBrush(QColor(Graphics::palette().textLabel)), bs));
    }

    painter.drawArc(rect, 0, 360 * 16);

    // draw actual value
    painter.setPen(QPen(QBrush(pointColor), bs));
    painter.drawArc(rect, 225 * 16, -ratio * 16 * 270 - 5 * 16);

    if (drawMark)
    {
        // draw marker
        painter.setPen(QPen(QBrush(QColor(Graphics::palette().textAll)), size / 15 + 4)); // 8 for static size
        painter.drawArc(rect, 225 * 16 - ratio * 16 * 270 - 5 * 16, 10 * 16);
        // draw click pointer
        if (mousePressX != 0)
        {
            painter.setPen(QPen(QBrush(QColor(Graphics::palette().textLabel)), 4));
            painter.drawArc(mousePressX - 2, mousePressY - 2, 4, 4, 0, 360 * 16);
        }
    }
}
