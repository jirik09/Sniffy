#ifndef STLINKFLASHER_H
#define STLINKFLASHER_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>

#include "stlinkconnector.h"
#include "stlinkwriter.h"
#include "stlinkeraser.h"
#include "stlinkinspector.h"

class StLinkFlasher : public QObject
{
    Q_OBJECT
public:
    explicit StLinkFlasher(QObject *parent = nullptr);
    ~StLinkFlasher();

    bool isConnected() const;

public slots:
    void connectDevice();
    void flashFirmware(const QString &filePath);
    void disconnectDevice();
    void stopAndCleanup(); // New slot for thread-safe cleanup
    void readDeviceUID();  // Read 96-bit UID and emit signal
    void performMassErase(); // Perform mass erase (with RDP regression if needed)
    QString getDetectedMcu(); // Get detected MCU string (e.g. F303RE)

signals:
    void logMessage(const QString &msg);
    void progressChanged(int value, int total);
    void operationStarted(const QString &operation);
    void operationFinished(bool success, const QString &message);
    void deviceConnected(const QString &info);
    void deviceDisconnected();
    void deviceUIDAvailable(const QString &uidHex, const QString &mcu);
    void deviceUIDError(const QString &message);

private:
    StLinkConnector m_connector;
    StLinkWriter m_writer;
    StLinkEraser m_eraser;
    StLinkInspector m_inspector;

    bool m_flashing; // true while a flash operation is in progress
    QMutex m_mutex;

    // Non-copyable
    StLinkFlasher(const StLinkFlasher&) = delete;
    StLinkFlasher& operator=(const StLinkFlasher&) = delete;
};

#endif // STLINKFLASHER_H
