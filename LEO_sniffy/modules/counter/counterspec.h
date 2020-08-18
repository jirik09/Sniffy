#ifndef COUNTERSPEC_H
#define COUNTERSPEC_H

#include <QObject>
#include <QDataStream>

class CounterSpec : public QObject
{
    Q_OBJECT
public:
    explicit CounterSpec(QByteArray data, QObject *parent = nullptr);

    int hf_uppLmt;
    int hf_lowLmt_Tg100ms;
    int hf_lowLmt_Tg500ms;
    int hf_lowLmt_Tg1s;
    int hf_lowLmt_Tg5s;
    int hf_lowLmt_Tg10s;

    int lf_uppLmt;
    float lf_lowLmt;

    struct pins{
        QString hf_pin;
        QString lf_pin_ch1, lf_pin_ch2;
        QString ref_pin_in1, ref_pin_in2;
        QString int_pin_ch1, int_pin_ch2;
    }pins;

    QList<QString> pinsList = { pins.hf_pin,
                                pins.lf_pin_ch1, pins.lf_pin_ch2,
                                pins.ref_pin_in1, pins.ref_pin_in2,
                                pins.int_pin_ch1, pins.int_pin_ch2 };

signals:

};

#endif // COUNTERSPEC_H
