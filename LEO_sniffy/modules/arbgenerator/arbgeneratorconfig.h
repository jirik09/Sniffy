#ifndef ARBGENERATORCONFIG_H
#define ARBGENERATORCONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"


enum class ArbGenLayout
{
    HORIZONTAL = 0,
    VERTICAL = 1
};

class ArbGeneratorConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit ArbGeneratorConfig(QObject *parent = nullptr);
    void parse (QByteArray config);
    QByteArray serialize ();

    ArbGenLayout layout;
};

#endif // ARBGENERATORCONFIG_H
