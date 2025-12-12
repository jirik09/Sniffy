#ifndef STLINKWRITER_H
#define STLINKWRITER_H

#include <QObject>
#include <QString>

typedef struct _stlink stlink_t;

class StLinkWriter : public QObject
{
    Q_OBJECT
public:
    explicit StLinkWriter(QObject *parent = nullptr);
    
    void flash(stlink_t* stlink, const QString &filePath);

signals:
    void logMessage(const QString &msg);
    void progressChanged(int value, int total);
    void operationStarted(const QString &operation);
    void operationFinished(bool success, const QString &message);
};

#endif // STLINKWRITER_H
