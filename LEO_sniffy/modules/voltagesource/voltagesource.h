#ifndef VOLTAGESOURCE_H
#define VOLTAGESOURCE_H

#include <QObject>

#include "../abstractmodule.h"
#include "VoltageSourcespec.h"
#include "VoltageSourceconfig.h"
#include "VoltageSourcewindow.h"

class VoltageSource : public AbstractModule
{
    Q_OBJECT
public:
    explicit VoltageSource(QObject *parent = nullptr);
    QWidget* getWidget();

signals:


public slots:
    void parseData(QByteArray data);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();
    void startModule();
    void stopModule();

private slots:
    //In case hold is needed

    void showHoldButtonCallback();
    void holdButtonCallback(bool held);


private:
    VoltageSourceConfig *config;
    VoltageSourceWindow *tempWindow;
   // scpWindow = new ScopeWindow();

};

#endif // VOLTAGESOURCE_H