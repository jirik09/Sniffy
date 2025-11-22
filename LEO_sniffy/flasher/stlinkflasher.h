#ifndef STLINKFLASHER_H
#define STLINKFLASHER_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>

// Forward declaration of stlink structure to avoid including stlink headers in header file
typedef struct _stlink stlink_t;

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

signals:
    void logMessage(const QString &msg);
    void progressChanged(int value, int total);
    void operationStarted(const QString &operation);
    void operationFinished(bool success, const QString &message);
    void deviceConnected(const QString &info);
    void deviceDisconnected();

private:
    stlink_t *m_stlink;
    bool m_connected;
    bool m_flashing; // true while a flash operation is in progress
    QMutex m_mutex;

    bool initStLink();
    void cleanupStLink();
    bool loadDeviceParamsFallback();
    // Non-copyable
    StLinkFlasher(const StLinkFlasher&) = delete;
    StLinkFlasher& operator=(const StLinkFlasher&) = delete;
};

#endif // STLINKFLASHER_H
