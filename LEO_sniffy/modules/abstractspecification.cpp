#include "abstractspecification.h"

AbstractSpecification::AbstractSpecification(QObject *parent) : QObject(parent)
{

}

int AbstractSpecification::getResources()
{
    return resources;
}

bool AbstractSpecification::isLoaded() const
{
    return isSpecificationLoaded;
}
