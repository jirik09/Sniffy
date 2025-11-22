#ifndef LOGICANALYSER_H
#define LOGICANALYSER_H

#include <QObject>
#include "../abstractmodule.h"
#include "logicanalyzerwindow.h"
#include "logicanalyzerconfig.h"
#include "logicanalyzerspec.h"

class LogicAnalyzer : public AbstractModule
{
    Q_OBJECT
public:
    explicit LogicAnalyzer(QObject *parent = nullptr);
    ~LogicAnalyzer() override;

    QWidget* getWidget() override;

public slots:
    void parseData(QByteArray data) override;
    void writeConfiguration() override;
    void parseConfiguration(QByteArray config) override;
    QByteArray getConfiguration() override;

    void startModule() override;
    void stopModule() override;

    void updateSampleRate(int rate);
    void updateTriggerChannel(int channel);
    void updateTriggerEdge(int edge);

private:
    LogicAnalyzerWindow *window;
    LogicAnalyzerConfig *config;
    LogicAnalyzerSpec *spec;
};
#endif // LOGICANALYSER_H
