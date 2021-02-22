#ifndef VOLTMETERSPEC_H
#define VOLTMETERSPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"

class VoltmeterSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit VoltmeterSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int specificationVariable;

};

#endif // VOLTMETERSPEC_H
