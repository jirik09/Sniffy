#ifndef VOLTMETERCONFIG_H
#define VOLTMETERCONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"

class VoltmeterConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit VoltmeterConfig(QObject *parent = nullptr);
    void parse (QByteArray config);
    QByteArray serialize ();
};

#endif // VOLTMETERCONFIG_H
