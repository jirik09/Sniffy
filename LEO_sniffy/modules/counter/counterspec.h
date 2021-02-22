#ifndef COUNTERSPEC_H
#define COUNTERSPEC_H

#include <QObject>
#include <QDataStream>
#include "../abstractspecification.h"

class CounterSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit CounterSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int hf_max;
    int hf_min_Tg100ms;
    int hf_min_Tg500ms;
    int hf_min_Tg1s;
    int hf_min_Tg5s;
    int hf_min_Tg10s;

    int lf_max;
    float lf_min;

    int rat_max_ref;
    int rat_max_chan;

    struct pins{
        QString hf_ch1;
        QString lf_ch1, lf_ch2;
        QString rat_ref, rat_ch3;
        QString int_ch1, int_ch2;
    }pins;

    QList<QString> pinsList = { pins.hf_ch1,
                                pins.lf_ch1, pins.lf_ch2,
                                pins.rat_ref, pins.rat_ch3,
                                pins.int_ch1, pins.int_ch2,};

signals:

};

#endif // COUNTERSPEC_H
