#ifndef CHANNELDATA_H
#define CHANNELDATA_H

#include <QObject>

class ChannelData : public QObject
{
    Q_OBJECT
public:
    explicit ChannelData(QObject *parent = nullptr);

    qreal voltage = 0;
    qreal ripple = 0;
    qreal frequency = 0;
    qreal max = -10000;
    qreal min = 10000;
    qreal percent = 0;

};

#endif // CHANNELDATA_H
