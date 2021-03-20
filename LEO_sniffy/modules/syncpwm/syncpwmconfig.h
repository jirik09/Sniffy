#ifndef SYNCPWMCONFIG_H
#define SYNCPWMCONFIG_H

#include <QObject>
#include <QDataStream>

#include "syncpwmdefs.h"

#include "../abstractconfiguration.h"

class SyncPwmConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit SyncPwmConfig(QObject *parent = nullptr);

    void parse (QByteArray config);
    QByteArray serialize ();

signals:

};

#endif // SYNCPWMCONFIG_H
