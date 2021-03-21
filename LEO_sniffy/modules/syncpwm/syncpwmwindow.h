#ifndef SYNCPWMWINDOW_H
#define SYNCPWMWINDOW_H

#include <QWidget>
#include <QSplitter>

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetdisplay.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"

#include "syncpwmspec.h"
#include "syncpwmconfig.h"
#include "syncpwmsettings.h"
#include "syncpwmdefs.h"

namespace Ui {
class SyncPwmWindow;
}

class SyncPwmWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SyncPwmWindow(SyncPwmConfig *conf, QWidget *parent = nullptr);
    ~SyncPwmWindow();

    SyncPwmSettings *settings;

private slots:

private:
    Ui::SyncPwmWindow *ui;
    SyncPwmConfig *config;    

    widgetChart *chart;

    void updateChartTimeScale(float timeBase);
    //void paintTraces();
};

#endif // SYNCPWMWINDOW_H