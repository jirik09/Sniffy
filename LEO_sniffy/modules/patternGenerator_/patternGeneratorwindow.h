#ifndef TEMPLATEMODULEWINDOW_H
#define TEMPLATEMODULEWINDOW_H

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
#include "../../GUI/widgetlabelarea.h"

#include "templatemoduleconfig.h"

namespace Ui {
class TemplateModuleWindow;
}

class TemplateModuleWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TemplateModuleWindow(TemplateModuleConfig *config, QWidget *parent = nullptr);
    ~TemplateModuleWindow();

    void restoreGUIAfterStartup();

private:
    Ui::TemplateModuleWindow *ui;
    TemplateModuleConfig *config;
};

#endif // TEMPLATEMODULEWINDOW_H
