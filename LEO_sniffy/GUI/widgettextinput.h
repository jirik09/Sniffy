#ifndef WIDGETTEXTINPUT_H
#define WIDGETTEXTINPUT_H

#include <QWidget>
#include <QDebug>
#include <QKeyEvent>

#include "../graphics/colors.h"


namespace Ui {
class WidgetTextInput;
}

class WidgetTextInput : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetTextInput(QWidget *parent = nullptr, QString name = "", QString value = "");
    ~WidgetTextInput();
    QString getText();

signals:
    void textChanged(QString text);

private:
    Ui::WidgetTextInput *ui;

private slots:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // WIDGETTEXTINPUT_H
