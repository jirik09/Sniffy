#ifndef WIDGETFEATURE_H
#define WIDGETFEATURE_H

#include <QWidget>
#include <QtWidgets/QPushButton>

namespace Ui {
class WidgetFeature;
}

enum class FeatureStatus
  {
    STOP,
    PAUSE,
    PLAY
  };

enum class FeatureIcon
  {
    NONE,
    SCOPE,
    GENERATOR,
    COUNTER
  };

class WidgetFeature : public QWidget
{
    Q_OBJECT

public:

    explicit WidgetFeature(QWidget *parent = nullptr, FeatureIcon icon=FeatureIcon::NONE, QString name="N/A");
    ~WidgetFeature();

    void setStatus(FeatureStatus status);
    QPushButton * getPushButton();

private:
    Ui::WidgetFeature *ui;
};





#endif // WIDGETFEATURE_H
