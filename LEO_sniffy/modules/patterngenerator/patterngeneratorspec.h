#ifndef PATTERNGENERATORSPEC_H
#define PATTERNGENERATORSPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"
#include "patterngeneratordefs.h"

class PatternGeneratorSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit PatternGeneratorSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    QString chanPins[MAX_PATT_CHANNELS_NUM];
};

#endif // PATTERNGENERATORSPEC_H
