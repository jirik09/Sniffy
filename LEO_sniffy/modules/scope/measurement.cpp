#include "measurement.h"

Measurement::Measurement(MeasurementType type, qint32 channel, QObject *parent) : QObject(parent)
{
    this->channel = channel;
    this->type = type;
}

Measurement::Measurement (const Measurement& m){
    this->label = m.getLabel();
    this->value = m.getValue();
    this->type = m.getType();
    this->channel = m.getChannel();
    return;
}

Measurement::~Measurement(){
}

QString Measurement::getLabel() const
{
    return label;
}

void Measurement::setLabel(const QString &label)
{
    this->label = label;
}

QString Measurement::getValue() const
{
    return value;
}

void Measurement::setValue(const QString &value)
{
    this->value = value;
}

MeasurementType Measurement::getType() const
{
    return type;
}

qint32 Measurement::getChannel() const
{
    return channel;
}

