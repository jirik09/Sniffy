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

class SyncPwmConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit SyncPwmConfig(QObject *parent = nullptr);

    void parse (QByteArray config);
    QByteArray serialize ();

    SyncPwmLayout layout;

signals:

};

#endif // SYNCPWMCONFIG_H
