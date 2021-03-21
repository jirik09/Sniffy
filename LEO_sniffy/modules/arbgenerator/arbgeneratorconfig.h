#ifndef ARBGENERATORCONFIG_H
#define ARBGENERATORCONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"

class ArbGeneratorConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit ArbGeneratorConfig(QObject *parent = nullptr);
    void parse (QByteArray config);
    QByteArray serialize ();
};

#endif // ARBGENERATORCONFIG_H
