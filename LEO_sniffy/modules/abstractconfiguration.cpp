#include "abstractconfiguration.h"

AbstractConfiguration::AbstractConfiguration(QObject *parent) : QObject(parent)
{

}

bool AbstractConfiguration::isLoaded() const
{
    return isConfigurationLoaded;
}
