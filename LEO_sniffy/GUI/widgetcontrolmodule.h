#ifndef WIDGETFEATURE_H
#define WIDGETFEATURE_H

#include <QWidget>
#include <QMouseEvent>
#include <QtWidgets/QPushButton>
#include <QBitmap>

#include "../graphics/graphics.h"

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
    LOCKED = 5
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
    QIcon *icon;
};





#endif // WIDGETFEATURE_H
