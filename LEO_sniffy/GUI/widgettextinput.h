#ifndef WIDGETTEXTINPUT_H
#define WIDGETTEXTINPUT_H

#include <QWidget>
#include <QDebug>
#include <QKeyEvent>

#include "../graphics/colors.h"


namespace Ui {
class WidgetTextInput;
}

enum class InputTextType
  {
    STRING,
    NUMBER
  };


class WidgetTextInput : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetTextInput(QWidget *parent = nullptr, QString name = "", QString value = "", InputTextType type = InputTextType::STRING);
    ~WidgetTextInput();
    QByteArray saveGeometry();
    void restoreGeometry(QByteArray geom);
    QString getText();

signals:
    void textChanged(QString text);

private:
    Ui::WidgetTextInput *ui;
    InputTextType inputType;

private slots:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // WIDGETTEXTINPUT_H
