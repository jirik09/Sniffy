#ifndef STLINKERASER_H
#define STLINKERASER_H

#include <QObject>
#include <QString>

typedef struct _stlink stlink_t;

class StLinkEraser : public QObject
{
    Q_OBJECT
public:
    explicit StLinkEraser(QObject *parent = nullptr);

    void massErase(stlink_t* stlink);

signals:
    void logMessage(const QString &msg);
    void operationStarted(const QString &operation);
    void operationFinished(bool success, const QString &message);
};

#endif // STLINKERASER_H
