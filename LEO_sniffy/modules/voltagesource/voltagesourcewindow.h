#ifndef VOLTAGESOURCEWINDOW_H
#define VOLTAGESOURCEWINDOW_H

#include <QWidget>
#include <QScrollArea>
#include <QDebug>
#include <QVBoxLayout>

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetchart.h"
#include "../../GUI/widgettab.h"
#include "../../GUI/widgetdisplay.h"

#include "voltagesourceconfig.h"
#include "voltagesourcedefs.h"

namespace Ui {
class VoltageSourceWindow;
}

class VoltageSourceWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VoltageSourceWindow(VoltageSourceConfig *config, QWidget *parent = nullptr);
    ~VoltageSourceWindow();

    void restoreGUIAfterStartup();
    void setNumberOfChannels(int numChannels);
    void setDisplayValue(qreal value, int channelIndex);

private:
    Ui::VoltageSourceWindow *ui;
    VoltageSourceConfig *config;

    QList<WidgetDisplay *> displays;
    QList<WidgetDialRange *> dials;

    WidgetLabel *labelVDDA;

private slots:
    void dialChangedCallback(qreal value, int channel);

signals:
    void voltagechanged(qreal value, int channel);
};

#endif // VOLTAGESOURCEWINDOW_H
