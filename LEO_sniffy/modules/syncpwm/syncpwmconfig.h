#ifndef SYNCPWMCONFIG_H
#define SYNCPWMCONFIG_H

#include <QObject>
#include <QDataStream>

#include "syncpwmdefs.h"

#include "../abstractconfiguration.h"

class SyncPWMConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit SyncPWMConfig(QObject *parent = nullptr);

    void parse (QByteArray config);
    QByteArray serialize ();

signals:

};

#endif // SYNCPWMCONFIG_H
