/*
Class for custom dial graphics.
Extends standard dial and just override the paint function
*/

#include "customdial.h"

#include <QPainter>
#include <QColor>
#include <QMouseEvent>
#include <QDebug>

#include <cmath>

CustomDial::CustomDial(QWidget* parent,
                       double knobRadius,
                       double knobMargin)
    : QDial(parent),
      knobRadius_(knobRadius),
      knobMargin_(knobMargin)
{
    // Default range
    QDial::setRange(0,100);
    QDial::setPageStep(5);
    QDial::setSingleStep(2);
    //   int size = parent->size().height();
    // QDial::setMinimumSize(size,size);
    //  QDial::setMaximumSize(size,size);
}

void CustomDial::setKnobRadius(double radius)
{
    knobRadius_ = radius;
}

double CustomDial::getKnobRadius() const
{
    return knobRadius_;
}

void CustomDial::setKnobMargin(double margin)
{
    knobMargin_ = margin;
}

double CustomDial::getKnobMargin() const
{
    return knobMargin_;
}

void CustomDial::drawMarker(bool draw)
{
    drawMark = draw;
}

void CustomDial::mouseMoveEvent(QMouseEvent *me) {
    double relativeX = me->x() - mousePressX+0.01;
    double relativeY = me->y() - mousePressY+0.01;

    //double distance = sqrt(relativeX*relativeX + relativeY*relativeY); //distance from initial point
    double distance = abs(relativeX - relativeY) /sqrt(2); //distance from -xy axis

    double phi = atan(relativeX/relativeY)*180/3.14159;

    double angle = 0;
    if (relativeY>0){
        angle = 90-phi;
    }else {
        angle = 270-phi;
    }

    //qDebug() << "dist"<<distance <<"angle" <<angle<<"(X" << relativeX << " Y" << relativeY << ")";
    if (angle>225 || angle<45){
        QDial::setValue(initialDialValue + distance/QDial::size().width()*QDial::maximum()*MOUSE_DRAG_DISTANCE);
    }else {
        QDial::setValue(initialDialValue - distance/QDial::size().width()*QDial::maximum()*MOUSE_DRAG_DISTANCE);
    }
}

void CustomDial::mousePressEvent(QMouseEvent *me) {
    emit dialPressed(me);
    mousePressX = me->x();
    mousePressY = me->y();
    initialDialValue = QDial::value();
}

void CustomDial::mouseReleaseEvent(QMouseEvent *me){
    emit dialReleased(me);
    if(initialDialValue==QDial::value()){
        QDial::mouseReleaseEvent(me);
    }
    mousePressX = 0;
    mousePressY = 0;
    QDial::update();
}

void CustomDial::paintEvent(QPaintEvent*)
{
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


    int size = (QDial::width()<QDial::height())?QDial::width():QDial::height();
    int diffH = (QDial::width()>QDial::height())?QDial::width()-QDial::height():0;
    int diffV = (QDial::width()<QDial::height())?QDial::height()-QDial::width():0;

    margin = margin * size / 100;
    int marginHalf = margin/2;
    const QRectF rect(marginHalf+diffH/2,marginHalf+diffV/2,size-margin,size-margin);

    Qt::BrushStyle bs;

    //draw background arc
    if(drawMark){
        bs = (Qt::BrushStyle)(size/30+3); //5 for static size
        painter.setPen(QPen(QBrush(QColor(48,48,48)),bs));
    }else {
        bs = (Qt::BrushStyle)2;
        painter.setPen(QPen(QBrush(QColor(68,68,68)),bs));
    }
    painter.drawArc(rect,0,360*16);

    //draw actual value
    painter.setPen(QPen(QBrush(pointColor),bs));
    painter.drawArc(rect,225*16,-ratio*16*270-5*16);

    if(drawMark){
        //draw marker
        painter.setPen(QPen(QBrush(QColor(214,214,214)),size/15+4)); //8 for static size
        painter.drawArc(rect,225*16-ratio*16*270-5*16,10*16);
        //draw click pointer
        if(mousePressX!=0){
            painter.setPen(QPen(QBrush(QColor(128,128,128)),4));
            painter.drawArc(mousePressX-2,mousePressY-2,4,4,0,360*16);
        }
    }
}

