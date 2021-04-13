#ifndef CUSTOMDIAL_H
#define CUSTOMDIAL_H

#include <QDial>
#include "math.h"

#include "../graphics/graphics.h"

#define MOUSE_DRAG_DISTANCE 0.6 //higher number more sensitive (short distance big change)

class CustomDial : public QDial
{   
    Q_OBJECT

public:
    CustomDial(QWidget * parent = nullptr);

    void setCustomGraphics(bool enable);
    void drawMarker(bool draw);

private:
    void mouseMoveEvent(QMouseEvent *me) override;
    void mousePressEvent(QMouseEvent *me) override;
    void mouseReleaseEvent(QMouseEvent *me) override;

    virtual void paintEvent(QPaintEvent*) override;

    int mousePressX;
    int mousePressY;
    int initialDialValue;

    bool customGraphics = false;
    bool drawMark = true;

signals:
    void dialPressed(QMouseEvent *me);
    void dialReleased(QMouseEvent *me);

};

#endif // CUSTOMDIAL_H
