#ifndef PATTERNGENERATORCONFIG_H
#define PATTERNGENERATORCONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"

class PatternGeneratorConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit PatternGeneratorConfig(QObject *parent = nullptr);
    void parse (QByteArray config);
    QByteArray serialize ();
};

#endif // PATTERNGENERATORCONFIG_H
