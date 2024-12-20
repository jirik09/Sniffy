#ifndef WIDGETLOGININFO_H
#define WIDGETLOGININFO_H

#include <QWidget>

#include "../graphics/graphics.h"

#include "customsettings.h"

namespace Ui {
class WidgetLoginInfo;
}

class WidgetLoginInfo : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetLoginInfo(QWidget *parent = nullptr);
    ~WidgetLoginInfo();

signals:
    void loginDialogClicked();

private:
    Ui::WidgetLoginInfo *ui;

public slots:
    void updateInfo();

private slots:
    void loginCallback();

};

#endif // WIDGETLOGININFO_H
