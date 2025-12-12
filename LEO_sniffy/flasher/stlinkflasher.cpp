#include "stlinkflasher.h"
#include <QDebug>
#include <QThread>

#include "stlink.h"

StLinkFlasher::StLinkFlasher(QObject *parent)
    : QObject(parent), m_flashing(false)
{
    // Connect signals from sub-modules
    connect(&m_connector, &StLinkConnector::logMessage, this, &StLinkFlasher::logMessage);

    connect(&m_writer, &StLinkWriter::logMessage, this, &StLinkFlasher::logMessage);
    connect(&m_writer, &StLinkWriter::progressChanged, this, &StLinkFlasher::progressChanged);
    connect(&m_writer, &StLinkWriter::operationStarted, this, &StLinkFlasher::operationStarted);
    connect(&m_writer, &StLinkWriter::operationFinished, [this](bool success, const QString &msg)
            {
        m_flashing = false;
        emit operationFinished(success, msg); });

    connect(&m_eraser, &StLinkEraser::logMessage, this, &StLinkFlasher::logMessage);
    connect(&m_eraser, &StLinkEraser::operationStarted, this, &StLinkFlasher::operationStarted);
    connect(&m_eraser, &StLinkEraser::operationFinished, [this](bool success, const QString &msg)
            {
        m_flashing = false;
        emit operationFinished(success, msg); });

    connect(&m_inspector, &StLinkInspector::logMessage, this, &StLinkFlasher::logMessage);
    connect(&m_inspector, &StLinkInspector::deviceUIDAvailable, this, &StLinkFlasher::deviceUIDAvailable);
    connect(&m_inspector, &StLinkInspector::deviceUIDError, this, &StLinkFlasher::deviceUIDError);
}

StLinkFlasher::~StLinkFlasher()
{
    disconnectDevice();
}

bool StLinkFlasher::isConnected() const
{
    return m_connector.isConnected();
}

void StLinkFlasher::connectDevice()
{
    QMutexLocker locker(&m_mutex);
    if (m_flashing)
    {
        emit logMessage("Flash in progress; ignoring connect request");
        return;
    }
    if (isConnected())
    {
        emit deviceConnected("Already connected");
        return;
    }

    if (m_connector.init())
    {
        // Get device info
        stlink_t *stlink = m_connector.handle();
        QString info = QString("Core ID: 0x%1, Chip ID: 0x%2")
                           .arg(stlink->core_id, 0, 16)
                           .arg(stlink->chip_id, 0, 16);

        emit deviceConnected(info);
    }
    else
    {
        emit operationFinished(false, "Failed to connect to ST-Link");
    }
}

void StLinkFlasher::disconnectDevice()
{
    QMutexLocker locker(&m_mutex);
    m_connector.cleanup();
    emit deviceDisconnected();
}

void StLinkFlasher::stopAndCleanup()
{
    disconnectDevice();
    QThread::currentThread()->quit();
}

void StLinkFlasher::flashFirmware(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

    if (!isConnected())
    {
        emit operationFinished(false, "Device not connected");
        return;
    }

    m_flashing = true;
    m_writer.flash(m_connector.handle(), filePath);
}

void StLinkFlasher::performMassErase()
{
    QMutexLocker locker(&m_mutex);

    if (!isConnected())
    {
        emit operationFinished(false, "Device not connected");
        return;
    }

    m_flashing = true;
    m_eraser.massErase(m_connector.handle());
}

void StLinkFlasher::readDeviceUID()
{
    QMutexLocker locker(&m_mutex);
    if (!isConnected())
    {
        emit deviceUIDError("Device not connected");
        return;
    }
    m_inspector.readUID(m_connector.handle());
}

QString StLinkFlasher::getDetectedMcu()
{
    QMutexLocker locker(&m_mutex);
    return m_inspector.getDetectedMcu(m_connector.handle());
}
