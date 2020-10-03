#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QMainWindow>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "../graphics/colors.h"

class ModuleDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ModuleDockWidget(QWidget *parent = nullptr, QString title = nullptr);
    ~ModuleDockWidget();

    void showHoldButton();

signals:
    void moduleWindowClosing();
    void holdClicked(bool holdClicked);

private slots:
    void unDockOrMaximize();
    void dockOrMinimize();
    void close();    
    void hold();

private:
    QRect winGeometry;
    QString winStyleSheet;
    Qt::WindowFlags winFlags;
    bool fullScreen = false;
    QPushButton *btnHold;
};


#endif // DOCKWIDGET_H
