#include "counterspec.h"

CounterSpec::CounterSpec(QByteArray data, QObject *parent) : QObject(parent)
{
    QDataStream stream(data);
    stream >> hf_uppLmt >> hf_lowLmt_Tg100ms >> hf_lowLmt_Tg500ms >> hf_lowLmt_Tg1s >> hf_lowLmt_Tg5s >> hf_lowLmt_Tg10s;
    stream >> lf_uppLmt >> lf_lowLmt;

    char chars[4] = "";
    for(int i = 0; i < pinsList.size(); i++){
        stream.readRawData(chars, 4);
        pinsList[i] = QString(chars);
        pinsList[i].remove('_');
    }

    pins.hf_pin = pinsList.at(0);
    pins.lf_pin_ch1 = pinsList.at(1);
    pins.lf_pin_ch2 = pinsList.at(2);
    pins.ref_pin_in1 = pinsList.at(3);
    pins.ref_pin_in2 = pinsList.at(4);
    pins.int_pin_ch1 = pinsList.at(5);
    pins.int_pin_ch2 = pinsList.at(6);
}
