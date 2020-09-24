#ifndef TIMING_H
#define TIMING_H

#include <QObject>
#include <QTimer>
#include <QEventLoop>

class Timing
{
public:
    Timing();

    void nonBlockSleep(int msec);

private:

};

#endif // TIMING_H
