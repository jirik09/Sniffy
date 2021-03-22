#ifndef ARBGENERATOR_H
#define ARBGENERATOR_H

#include <QObject>

#include "../abstractmodule.h"
#include "arbgeneratorspec.h"
#include "arbgeneratorconfig.h"
#include "arbgeneratorwindow.h"

class ArbGenerator : public AbstractModule
{
    Q_OBJECT
public:
    explicit ArbGenerator(QObject *parent = nullptr);
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


private:
    ArbGeneratorConfig *config;
    ArbGeneratorWindow *arbGenWindow;
   // scpWindow = new ScopeWindow();

};

#endif // ARBGENERATOR_H
