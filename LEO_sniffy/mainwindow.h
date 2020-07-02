#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//#include <QDebug>
//#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>
//#include <QMessageBox>
//#include <QGraphicsDropShadowEffect>

#include "device.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();    

private:
  //  Comms *communication;    
    Ui::MainWindow *ui;

    Device *device;
    QSpacerItem *verticalSpacer;

signals:
    void setVerticalLayoutSpec(QVBoxLayout *boxLayout);

private slots:
   void setMenuSize();
};
#endif // MAINWINDOW_H
