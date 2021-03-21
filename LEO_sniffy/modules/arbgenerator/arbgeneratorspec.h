#ifndef ARBGENERATORSPEC_H
#define ARBGENERATORSPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"

class ArbGeneratorSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit ArbGeneratorSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int specificationVariable;

};

#endif // ARBGENERATORSPEC_H
