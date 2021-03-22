#ifndef SYNCPWMCONFIG_H
#define SYNCPWMCONFIG_H

#include <QObject>
#include <QDataStream>

#include "syncpwmdefs.h"

#include "../abstractconfiguration.h"

enum class SyncPwmLayout
{
    HORIZONTAL = 0,
    VERTICAL = 1
};

enum class SyncPwmState
{
    STOPPED = 0,
    RUNNING = 1
};

enum class SyncPwmModeEqui
{
    EQUI_DISABLED = 0,
    EQUI_ENABLED = 1
};

class SyncPwmConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit SyncPwmConfig(QObject *parent = nullptr);

    void parse (QByteArray config);
    QByteArray serialize ();

    SyncPwmLayout layout;
    SyncPwmState state;
    SyncPwmModeEqui equiMode;

signals:

};

#endif // SYNCPWMCONFIG_H
