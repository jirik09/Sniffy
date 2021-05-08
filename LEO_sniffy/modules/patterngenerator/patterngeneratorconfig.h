#ifndef PATTERNGENERATORCONFIG_H
#define PATTERNGENERATORCONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"
#include "patterngeneratordefs.h"

class PatternGeneratorConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit PatternGeneratorConfig(QObject *parent = nullptr);

    void parse (QByteArray config);
    QByteArray serialize ();

    enum class State { STOPPED = 0,
                       RUNNING = 1,
                       UPLOADING = 2
                     } state = State::STOPPED;

    int pattIndex = 0;

    qreal freq[PATTERNS_NUM];
};

#endif // PATTERNGENERATORCONFIG_H
