#ifndef WIDGETFEATURE_H
#define WIDGETFEATURE_H

#include <QWidget>
#include <QMouseEvent>
#include <QtWidgets/QPushButton>
#include <QBitmap>
#include <QTimer>
#include <QPainter>
#include <QConicalGradient>

#include "../graphics/graphics.h"

class SpinnerOverlay : public QWidget {
    Q_OBJECT
public:
    explicit SpinnerOverlay(QWidget *parent = nullptr);
    void start();
    void stop();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QTimer timer;
    int angle = 0;
};

namespace Ui {
class WidgetFeature;
}

enum class ModuleStatus
  {
    STOP = 0,
    PAUSE = 1,
    PLAY = 2,
    HIDDEN_PAUSE = 3,
    HIDDEN_PLAY = 4,
    LOCKED = 5,
    WAIT_EVENT = 6,
    HIDDEN_WAIT_EVENT = 7
  };

class WidgetControlModule : public QWidget
{
    Q_OBJECT

public:

    explicit WidgetControlModule(QWidget *parent = nullptr,  QString name="N/A");
    ~WidgetControlModule();

    void setStatus(ModuleStatus stat);
    ModuleStatus getStatus(void);
    void hideStatus();
    void setIcon (QString ImageURI);
    void setName (QString name);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void mouseWheelPressEvent(QMouseEvent *event);
    void clicked(ModuleStatus);

private slots:    
    void clickedInternal();    

private:
    Ui::WidgetFeature *ui;
    ModuleStatus status;
    QIcon icon;
    QString iconURI;
    SpinnerOverlay *spinner = nullptr;

    void updateIcon();
};





#endif // WIDGETFEATURE_H
