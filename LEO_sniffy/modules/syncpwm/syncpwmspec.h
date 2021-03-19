#ifndef SYNCPWMSPEC_H
#define SYNCPWMSPEC_H

#include <QObject>
#include <QDataStream>
#include "../abstractspecification.h"

class SyncPWMSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit SyncPWMSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int max_freq;
    int chan_num;

    struct pins{
        QString chan1, chan2, chan3, chan4;
    }pins;

    QList<QString> pinsList = { pins.chan1,
                                pins.chan2,
                                pins.chan3,
                                pins.chan4 };

signals:

};

#endif // SYNCPWMSPEC_H
