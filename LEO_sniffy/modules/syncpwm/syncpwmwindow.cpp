#include "syncpwmwindow.h"
#include "ui_syncpwmwindow.h"

SyncPWMWindow::SyncPWMWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyncPWMWindow)
{
    ui->setupUi(this);
}

SyncPWMWindow::~SyncPWMWindow()
{
    delete ui;
}
