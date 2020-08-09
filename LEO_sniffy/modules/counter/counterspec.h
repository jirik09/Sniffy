#ifndef COUNTERSPEC_H
#define COUNTERSPEC_H

#include <QObject>
#include <QDataStream>

class CounterSpec : public QObject
{
    Q_OBJECT
public:
    explicit CounterSpec(QByteArray data, QObject *parent = nullptr);

    int hf_uppLmt;        // = 300000000;
    int hf_lowLmt_Tg100ms;  // = 38000;
    int hf_lowLmt_Tg500ms;  // = 16970;
    int hf_lowLmt_Tg1s;   // = 12000;
    int hf_lowLmt_Tg5s;   // = 5366;
    int hf_lowLmt_Tg10s;  // = 3800;

    int lf_uppLmt;        // = hfLowLmt_Tg10s;
    float lf_lowLmt;      // = 0.033528;

    struct pins{
        QString hf_pin;
        QString lf_pin_ch1;
        QString lf_pin_ch2;
        QString ref_pin_in1;
        QString ref_pin_in2;
        QString int_pin_ch1;
        QString int_pin_ch2;
    }pins;

    QList<QString> pinsList = {pins.hf_pin, pins.lf_pin_ch1, pins.lf_pin_ch2,
                               pins.ref_pin_in1, pins.ref_pin_in2,
                               pins.int_pin_ch1, pins.int_pin_ch2 };

signals:

};

#endif // COUNTERSPEC_H
