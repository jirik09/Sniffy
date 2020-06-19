#ifndef COUNTERSPEC_H
#define COUNTERSPEC_H

#include <QObject>

class CounterSpec : public QObject
{
    Q_OBJECT
public:
    explicit CounterSpec(QObject *parent = nullptr);

signals:

};

#endif // COUNTERSPEC_H
