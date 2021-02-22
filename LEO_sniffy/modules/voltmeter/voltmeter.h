#ifndef VOLTMETER_H
#define VOLTMETER_H

#include <QObject>

#include "../abstractmodule.h"
#include "voltmeterspec.h"
#include "voltmeterconfig.h"
#include "voltmeterwindow.h"

class Voltmeter : public AbstractModule
{
    Q_OBJECT
public:
    explicit Voltmeter(QObject *parent = nullptr);
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
    VoltmeterConfig *config;
    VoltmeterWindow *voltWindow;
   // scpWindow = new ScopeWindow();

};

#endif // VOLTMETER_H
