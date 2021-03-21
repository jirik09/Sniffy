#ifndef PWMGENERATORCONFIG_H
#define PWMGENERATORCONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"

class PWMGeneratorConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit PWMGeneratorConfig(QObject *parent = nullptr);
    void parse (QByteArray config);
    QByteArray serialize ();
};

#endif // PWMGENERATORCONFIG_H
