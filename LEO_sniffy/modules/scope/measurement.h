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

    QString getValue() const;
    void setValue(const QString &value);

    MeasurementType getType() const;

    qint32 getChannelIndex() const;

private:
    MeasurementType type;
    QString label;
    QString value;
    qint32 channelIndex;
};

#endif // MEASTYPE_H
