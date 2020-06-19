#include "counter.h"

Counter::Counter(QObject *parent) : QObject(parent)
{
    config = new CounterConfig();
    cmd = new Commands();
}

void Counter::setModuleWindow(WindowCounter *cntWin)
{
    cntWindow = cntWin;
}

void Counter::setDockWidgetWindow(ModuleDockWidget *dockWidget)
{
    cntDockWidgetWindow = dockWidget;

  //  connect(cntDockWidgetWindow, &ModuleDockWidget::moduleWindowClosing,this, &Counter::closeModule);

}
