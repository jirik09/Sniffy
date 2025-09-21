#ifndef WIDGETTEXTINPUT_H
#define WIDGETTEXTINPUT_H

#include <QWidget>
#include <QDebug>
#include <QKeyEvent>

#include "../graphics/graphics.h"
#include "../modules/numberparser.h"


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
    qreal getValue();
    void processInput();
    void setAsPassword();
    void setText(QString txt);

signals:
    void textChanged(QString text);
    void numberChanged(qreal number);

private:

    Ui::WidgetTextInput *ui;
    InputTextType inputType;
    QString lastParsed = "";
    qreal number = 0;
    bool graphicsShown = false;


private slots:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // WIDGETTEXTINPUT_H
