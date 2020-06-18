#ifndef WIDGETFEATURE_H
#define WIDGETFEATURE_H

#include <QWidget>
#include <QtWidgets/QPushButton>

namespace Ui {
class WidgetFeature;
}

enum class ModuleStatus
  {
    STOP,
    PAUSE,
    PLAY
  };

class WidgetModule : public QWidget
{
    Q_OBJECT

public:

    explicit WidgetModule(QWidget *parent = nullptr,  QString name="N/A");
    ~WidgetModule();

    void setStatus(ModuleStatus stat);
    ModuleStatus getStatus(void);
    void setIcon (QString ImageURI);

signals:
    void clicked(ModuleStatus);

private slots:

    void clickedInternal();

private:
    Ui::WidgetFeature *ui;
    ModuleStatus status;
};





#endif // WIDGETFEATURE_H
