#ifndef LOGICANALYSERCONFIG_H
#define LOGICANALYSERCONFIG_H

#include <QObject>
#include "../abstractconfiguration.h"

class LogicAnalyzerConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit LogicAnalyzerConfig(QObject *parent = nullptr);

    void parse(QByteArray config) override;
    QByteArray serialize() override;

    // Configuration properties
    int sampleRate = 1000000; // Default 1 MHz for immediate responsiveness
    int preTrigger = 0;
    int triggerChannel = 1;
    int triggerEdge = 0; // 0: Rising, 1: Falling
    int dataLength = 20000; // Match LA default buffer size

    // Channel settings
    struct Channel
    {
        QString name;
        bool enabled;
        int color;
    };
    QVector<Channel> channels;

signals:
    void configurationChanged();
};

#endif // LOGICANALYSERCONFIG_H
