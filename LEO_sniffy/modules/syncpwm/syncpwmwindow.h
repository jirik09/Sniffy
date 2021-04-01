#ifndef SYNCPWMWINDOW_H
#define SYNCPWMWINDOW_H

#include <QWidget>
#include <QSplitter>

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetchart.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"

#include "syncpwmspec.h"
#include "syncpwmconfig.h"
#include "syncpwmsettings.h"
#include "syncpwmdefs.h"
#include "syncpwmpainter.h"

namespace Ui {
class SyncPwmWindow;
}

class SyncPwmWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SyncPwmWindow(SyncPwmConfig *config, QWidget *parent = nullptr);
    ~SyncPwmWindow();

    SyncPwmSettings *settings;
    SyncPwmPainter *painter;

    void setSpecification(SyncPwmSpec *spec);
    void restoreGUIAfterStartup();
    void setStartTxt();
    void setStopTxt();
    void uncheckStartButton();

    void setFreqDial(float val, int chanIndex);
    void setPhaseDial(float val, int chanIndex);
    void setDutyDial(float val, int chanIndex);

    void enableChannel(bool enable, int chanIndex);
    void repaint();

private slots:
    void dialFreqCallback(float val, int chanIndex);

private:
    Ui::SyncPwmWindow *ui;
    SyncPwmConfig *config;
    SyncPwmSpec *spec;

    widgetChart *chart;

    void updateChartTimeScale(float timeBase);
    //void paintTraces();
};

#endif // SYNCPWMWINDOW_H
