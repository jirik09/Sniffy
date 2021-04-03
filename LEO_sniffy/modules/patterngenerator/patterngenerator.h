#ifndef PATTERNGENERATOR_H
#define PATTERNGENERATOR_H

#include <QObject>

#include "../abstractmodule.h"
#include "PatternGeneratorspec.h"
#include "PatternGeneratorconfig.h"
#include "PatternGeneratorwindow.h"

class PatternGenerator : public AbstractModule
{
    Q_OBJECT
public:
    explicit PatternGenerator(QObject *parent = nullptr, AbstractColors *theme = nullptr);
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
    PatternGeneratorConfig *config;
    PatternGeneratorWindow *tempWindow;
   // scpWindow = new ScopeWindow();

};

#endif // PATTERNGENERATOR_H
