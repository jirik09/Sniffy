#ifndef VOLTAGESOURCECONFIG_H
#define VOLTAGESOURCECONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"

class VoltageSourceConfig : public AbstractConfiguration
{
public:
    explicit VoltageSourceConfig(QObject *parent = nullptr);
    void parse (QByteArray config);
    QByteArray serialize ();
};

#endif // VOLTAGESOURCECONFIG_H
