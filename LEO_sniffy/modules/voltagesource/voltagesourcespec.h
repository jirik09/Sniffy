#ifndef VOLTAGESOURCESPEC_H
#define VOLTAGESOURCESPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"

class VoltageSourceSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit VoltageSourceSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int specificationVariable;

};

#endif // VOLTAGESOURCESPEC_H
