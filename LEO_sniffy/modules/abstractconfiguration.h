#ifndef ABSTRACTCONFIGURATION_H
#define ABSTRACTCONFIGURATION_H

#include <QObject>
#include <QDebug>

class AbstractConfiguration : public QObject
{
    Q_OBJECT
public:
    explicit AbstractConfiguration(QObject *parent = nullptr);
    virtual void parse (QByteArray config) = 0;
    virtual QByteArray serialize () = 0;

    bool isLoaded() const;

protected:
    bool isConfigurationLoaded = false;

};

#endif // ABSTRACTCONFIGURATION_H
