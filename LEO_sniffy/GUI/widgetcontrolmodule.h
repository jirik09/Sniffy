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
    PLAY,
    HIDDEN_PAUSE,
    HIDDEN_PLAY

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

signals:
    void clicked(ModuleStatus);

private slots:

    void clickedInternal();

private:
    Ui::WidgetFeature *ui;
    ModuleStatus status;
};





#endif // WIDGETFEATURE_H
