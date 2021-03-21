#ifndef PWMGENERATOR_H
#define PWMGENERATOR_H

#include <QObject>

#include "../abstractmodule.h"
#include "pwmgeneratorspec.h"
#include "pwmgeneratorconfig.h"
#include "pwmgeneratorwindow.h"

class PWMGenerator : public AbstractModule
{
    Q_OBJECT
public:
    explicit PWMGenerator(QObject *parent = nullptr);
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
    PWMGeneratorConfig *config;
    PWMGeneratorWindow *tempWindow;
   // scpWindow = new ScopeWindow();

};

#endif // PWMGENERATOR_H
