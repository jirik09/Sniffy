#ifndef MEASTYPE_H
#define MEASTYPE_H

#include <QObject>

enum class MeasurementType
{
FREQUENCY, PERIOD, PHASE, DUTY, LOW, HIGH, RMS, MEAN, PKPK, MAX, MIN
};

class Measurement : public QObject
{
    Q_OBJECT
public:
    explicit Measurement(MeasurementType type, qint32 channelIndex, QObject *parent = nullptr);
    ~Measurement();


    QString getLabel() const;
    void setLabel(const QString &value);

    QString getValueString() const;
    void setValueString(const QString &value);

    MeasurementType getType() const;

    qint32 getChannelIndex() const;

    qreal getValue() const;
    void setValue(const qreal &value);

private:
    MeasurementType type;
    QString label;
    QString valueString;
    qreal value;
    qint32 channelIndex;
};

#endif // MEASTYPE_H
