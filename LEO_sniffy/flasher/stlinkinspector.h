#ifndef STLINKINSPECTOR_H
#define STLINKINSPECTOR_H

#include <QObject>
#include <QString>

typedef struct _stlink stlink_t;

class StLinkInspector : public QObject
{
    Q_OBJECT
public:
    explicit StLinkInspector(QObject *parent = nullptr);

    void readUID(stlink_t* stlink);
    QString getDetectedMcu(stlink_t* stlink);

signals:
    void logMessage(const QString &msg);
    void deviceUIDAvailable(const QString &uidHex, const QString &mcu);
    void deviceUIDError(const QString &message);
};

#endif // STLINKINSPECTOR_H
