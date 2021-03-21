#ifndef PATTERNGENERATORSPEC_H
#define PATTERNGENERATORSPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"

class PatternGeneratorSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit PatternGeneratorSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int specificationVariable;

};

#endif // PATTERNGENERATORSPEC_H
