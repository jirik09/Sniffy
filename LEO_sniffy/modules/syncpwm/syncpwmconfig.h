#ifndef SYNCPWMCONFIG_H
#define SYNCPWMCONFIG_H

#include <QObject>
#include <QDataStream>

#include "syncpwmdefs.h"

#include "../abstractconfiguration.h"

enum class Layout
{
    HORIZONTAL = 0,
    VERTICAL = 1
};

enum class State
{
    STOPPED = 0,
    RUNNING = 1
};

struct Common
{
    bool stepMode;
    bool equiMode;
};

struct Channel
{
    double frequency;
    float dutyCycle;
    float phase;
    bool enabled;
    bool inverted;
};

class SyncPwmConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit SyncPwmConfig(QObject *parent = nullptr);

    void parse (QByteArray config);
    QByteArray serialize ();    

    Layout layout;
    State state;
    Common common;
    Channel chan[CHANNELS_NUM];

signals:

};

#endif // SYNCPWMCONFIG_H
