#ifndef COUNTERCONFIG_H
#define COUNTERCONFIG_H

#include <QObject>
#include "counterdefs.h"

class CounterConfig : public QObject
{
    Q_OBJECT
public:
    explicit CounterConfig(QObject *parent = nullptr);

    CounterQuantity quantity = CounterQuantity::FREQUENCY;
    ErrorType error = ErrorType::SIMPLE;

    /* Direct Frequency measurement (High Frequency) */
    uint gateTime = 100; // ms
    //int autoInputPsc = 1;

    /* Reciprocal Frequency measurement (Low Frequency) */


signals:

};

#endif // COUNTERCONFIG_H
