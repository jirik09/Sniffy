#include "counterspec.h"
#include "modules/specutils.h"

CounterSpec::CounterSpec(QObject *parent)
{
    Q_UNUSED(parent);
}
void CounterSpec::parseSpecification(QByteArray spec){
    QDataStream stream(spec);
    stream >> resources >> hf_max >> hf_min_Tg100ms >> hf_min_Tg500ms >> hf_min_Tg1s;
    stream >> hf_min_Tg5s >> hf_min_Tg10s >> lf_max >> lf_min;
    stream >> rat_max_ref >> rat_max_chan;

    SpecParsing::readPins4(stream, pinsList.size(), [&](int i, const QString &pin){ pinsList[i] = pin; });

    pins.hf_ch1 = pinsList.at(0);
    pins.lf_ch1 = pinsList.at(1);
    pins.lf_ch2 = pinsList.at(2);
    pins.rat_ref = pinsList.at(3);
    pins.rat_ch3 = pinsList.at(4);
    pins.int_ch1 = pinsList.at(5);
    pins.int_ch2 = pinsList.at(6);
}
