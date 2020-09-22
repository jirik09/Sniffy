#ifndef CUSTOMDIAL_H
#define CUSTOMDIAL_H

#include <QDial>
#include "math.h"

class CustomDial : public QDial
{
    Q_OBJECT

    Q_PROPERTY(double knobRadius READ getKnobRadius WRITE setKnobRadius)

    Q_PROPERTY(double knobMargin READ getKnobMargin WRITE setKnobMargin)

public:

    CustomDial(QWidget * parent = nullptr,
               double knobRadius = 5,
               double knobMargin = 5);

    void setKnobRadius(double radius);

    double getKnobRadius() const;

    void setKnobMargin(double margin);

    double getKnobMargin() const;

    void drawMarker(bool draw);

private:

    void mouseMoveEvent(QMouseEvent *me) override;
    void mousePressEvent(QMouseEvent *me) override;
    void mouseReleaseEvent(QMouseEvent *me) override;

    virtual void paintEvent(QPaintEvent*) override;

    int mousePressX;
    int mousePressY;
    int initialDialValue;

    double knobRadius_;

    double knobMargin_;

    bool drawMark = true;

};

#endif // CUSTOMDIAL_H
