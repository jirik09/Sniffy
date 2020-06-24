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
    explicit Measurement(MeasurementType type, qint32 channel, QObject *parent = nullptr);
    explicit Measurement (const Measurement& meas);
    ~Measurement();


    QString getLabel() const;
    void setLabel(const QString &value);

    QString getValue() const;
    void setValue(const QString &value);

    MeasurementType getType() const;

    qint32 getChannel() const;

private:
    MeasurementType type;
    QString label;
    QString value;
    qint32 channel;
};

#endif // MEASTYPE_H
