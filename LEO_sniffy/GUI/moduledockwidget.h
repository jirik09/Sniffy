#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QMainWindow>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class ModuleDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ModuleDockWidget(QWidget *parent = nullptr, QString title = nullptr);
    ~ModuleDockWidget();

signals:
    void moduleWindowClosing();

private slots:
    void unDockOrMaximize();
    void dockOrMinimize();
    void close();
private:
    QRect winGeometry;
    QString winStyleSheet;
    Qt::WindowFlags winFlags;
    bool fullScreen = false;
};


#endif // DOCKWIDGET_H
