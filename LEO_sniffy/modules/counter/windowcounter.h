#ifndef WINDOWSCOUNTER_H
#define WINDOWSCOUNTER_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/widgettop.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgettab.h"

namespace Ui {
class WindowCounter;
}

class WindowCounter : public QWidget
{
    Q_OBJECT

public:
    explicit WindowCounter(QWidget *parent = nullptr);
    ~WindowCounter();


private:
    Ui::WindowCounter *ui;

};

#endif // WINDOWSCOUNTER_H
