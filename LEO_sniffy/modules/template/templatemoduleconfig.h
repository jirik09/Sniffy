#ifndef TEMPLATEMODULECONFIG_H
#define TEMPLATEMODULECONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"

class TemplateModuleConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit TemplateModuleConfig(QObject *parent = nullptr);
    void parse (QByteArray config);
    QByteArray serialize ();
};

#endif // TEMPLATEMODULECONFIG_H
