#include "measurement.h"

Measurement::Measurement(MeasurementType type, qint32 channelIndex, QObject *parent) : QObject(parent)
{
    this->channelIndex = channelIndex;
    this->type = type;
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

QString Measurement::getValueString() const
{
    return valueString;
}

void Measurement::setValueString(const QString &value)
{
    this->valueString = value;
}

MeasurementType Measurement::getType() const
{
    return type;
}

qint32 Measurement::getChannelIndex() const
{
    return channelIndex;
}

qreal Measurement::getValue() const
{
    return value;
}

void Measurement::setValue(const qreal &value)
{
    this->value = value;
}

