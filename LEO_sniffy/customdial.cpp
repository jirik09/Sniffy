/*
Class for custom dial graphics.
Extends standard dial and just override the paint function
*/

#include "customdial.h"

#include <QPainter>
#include <QColor>

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
    int size = parent->size().height();
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

void CustomDial::paintEvent(QPaintEvent*)
{
  //  static const double degree270 = 1.5 * M_PI;

  //  static const double degree225 = 1.25 * M_PI;

    static const int margin = 15;

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

    // The maximum amount of degrees is 270, offset by 225
  //  double angle = ratio * degree270 - degree225;

    // Radius of background circle
  //  double r = QDial::height() / 2.0;

    // Add r to have (0,0) in center of dial
 //   double y = sin(angle) * (r - knobRadius_ - knobMargin_) + r-5;

  //  double x = cos(angle) * (r - knobRadius_ - knobMargin_) + r-5;

    // Draw the ellipse
   // painter.drawEllipse(QPointF(x,y),knobRadius_, knobRadius_);

    //painter.setBrush(QBrush(QColor(10,200,150)));

    int marginHalf = margin/2;
    int size = (QDial::width()<QDial::height())?QDial::width():QDial::height();
    int diffH = (QDial::width()>QDial::height())?QDial::width()-QDial::height():0;
    int diffV = (QDial::width()<QDial::height())?QDial::height()-QDial::width():0;


    const QRectF rect(marginHalf+diffH/2,marginHalf+diffV/2,size-margin,size-margin);


    //draw background arc
    painter.setPen(QPen(QBrush(QColor(48,48,48)),5));
    painter.drawArc(rect,0,360*16);

    //draw actual value
    painter.setPen(QPen(QBrush(pointColor),5));
    painter.drawArc(rect,225*16,-ratio*16*270-5*16);

    //draw marker
    painter.setPen(QPen(QBrush(QColor(214,214,214)),8));
    painter.drawArc(rect,225*16-ratio*16*270-5*16,10*16);



}
