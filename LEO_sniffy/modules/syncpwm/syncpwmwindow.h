#ifndef SYNCPWMWINDOW_H
#define SYNCPWMWINDOW_H

#include <QWidget>

#include "syncpwmspec.h"
#include "syncpwmconfig.h"
#include "syncpwmdefs.h"

namespace Ui {
class SyncPWMWindow;
}

class SyncPWMWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SyncPWMWindow(QWidget *parent = nullptr);
    ~SyncPWMWindow();

private slots:

private:
    Ui::SyncPWMWindow *ui;
};

#endif // SYNCPWMWINDOW_H
