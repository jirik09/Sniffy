#ifndef TIMING_H
#define TIMING_H

#include <QObject>
#include <QTimer>
#include <QEventLoop>

/*
 * Timing
 * Provides a blocking sleep that still processes queued Qt events (via QEventLoop::exec()).
 * Use sparingly on the GUI thread; for long operations prefer asynchronous design.
 */
class Timing {
public:
    Timing();
    void sleep(int msec); // Blocks approximately msec while allowing event processing
};

#endif // TIMING_H
