#ifndef PWMGENERATORSPEC_H
#define PWMGENERATORSPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"

class PWMGeneratorSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit PWMGeneratorSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int specificationVariable;

};

#endif // PWMGENERATORSPEC_H
