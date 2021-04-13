#ifndef WIDGETFOOTER_H
#define WIDGETFOOTER_H

#include <QWidget>
#include <QtWidgets/QPushButton>
#include <QGraphicsScene>
#include <QMovie>
#include <QSize>

#include "GUI/clickablelabel.h"
#include "../graphics/graphics.h"

namespace Ui {
class WidgetFooter;
}

class WidgetFooter : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetFooter(QWidget *parent = nullptr);
    ~WidgetFooter();
    QPushButton * getPushButtonSize();

signals:
    void settingsClicked();
    void sizeClicked(bool isWide);


private:
    Ui::WidgetFooter *ui;
    bool isWide = true;
    QMovie *animationForward;
    QMovie *animationBackward;

    void setAppearance(bool isWide);

private slots:
    void settingsCallback();
    void sizeCallback();

};

#endif // WIDGETFOOTER_H
