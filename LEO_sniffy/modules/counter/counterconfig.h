#ifndef COUNTERCONFIG_H
#define COUNTERCONFIG_H

#include <QObject>

class CounterConfig : public QObject
{
    Q_OBJECT
public:
    explicit CounterConfig(QObject *parent = nullptr);

    /* Direct Frequency measurement (High Frequency) */
    int gateTime = 100; // ms
    int averaging = 2;
    int autoInputPsc = 1;

    /* Reciprocal Frequency measurement (Low Frequency) */


signals:

};

#endif // COUNTERCONFIG_H
