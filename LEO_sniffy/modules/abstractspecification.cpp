#include "abstractspecification.h"

AbstractSpecification::AbstractSpecification(QObject *parent) : QObject(parent)
{

}

bool AbstractSpecification::isLoaded() const
{
    return isSpecificationLoaded;
}
