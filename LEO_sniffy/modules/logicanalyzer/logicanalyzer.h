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

    QWidget *getWidget() override;

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
    void updateTriggerMode(int mode);

    void startCapture();
    void stopCapture();

private:
    void buildModuleDescription();
    LogicAnalyzerWindow *window;
    LogicAnalyzerConfig *config;
    LogicAnalyzerSpec *spec;
    bool isRunning = false;
    quint32 lastSequence = 0;
    bool continuousMode = false;
    bool currentSegmentBitPacked = false;
    quint32 pendingDataLengthBytes = 0;
};
#endif // LOGICANALYSER_H
