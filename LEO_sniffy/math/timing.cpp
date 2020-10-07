#include "timing.h"

Timing::Timing()
{

}

void Timing::sleep(int msec){
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, &QEventLoop::quit);
    loop.exec();
}
