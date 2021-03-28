#ifndef SYNCPWMPAINTER_H
#define SYNCPWMPAINTER_H

#include <QObject>

class SyncPwmPainter : public QObject
{
    Q_OBJECT
public:
    explicit SyncPwmPainter(QObject *parent = nullptr);

signals:

};

#endif // SYNCPWMPAINTER_H
