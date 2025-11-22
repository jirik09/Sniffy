#include "stlinkflasher.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include <QDir>

// Include stlink headers
#include "stlink.h"
extern "C" {
    #include "common_flash.h"
}

StLinkFlasher::StLinkFlasher(QObject *parent)
    : QObject(parent), m_stlink(nullptr), m_connected(false)
{
}

StLinkFlasher::~StLinkFlasher()
{
    disconnectDevice();
}

bool StLinkFlasher::isConnected() const
{
    return m_connected;
}

void StLinkFlasher::connectDevice()
{
    QMutexLocker locker(&m_mutex);
    if (m_connected) {
        emit deviceConnected("Already connected");
        return;
    }

    if (initStLink()) {
        m_connected = true;
        
        // Get device info
        QString info = QString("Core ID: 0x%1, Chip ID: 0x%2")
                           .arg(m_stlink->core_id, 0, 16)
                           .arg(m_stlink->chip_id, 0, 16);
        
        emit deviceConnected(info);
        emit logMessage("Connected to ST-Link. " + info);
    } else {
        emit operationFinished(false, "Failed to connect to ST-Link");
    }
}

void StLinkFlasher::disconnectDevice()
{
    QMutexLocker locker(&m_mutex);
    cleanupStLink();
    m_connected = false;
    emit deviceDisconnected();
}

bool StLinkFlasher::initStLink()
{
    // Set STLINK_CHIPS_DIR to help libstlink find chip definitions
    QString appDir = QCoreApplication::applicationDirPath();
    QString chipsDir = QDir::toNativeSeparators(appDir + "/config/chips");
    
    if (QDir(chipsDir).exists()) {
        qputenv("STLINK_CHIPS_DIR", chipsDir.toLocal8Bit());
        emit logMessage("Set STLINK_CHIPS_DIR to: " + chipsDir);
    } else {
        emit logMessage("Warning: Chips config dir not found at: " + chipsDir);
    }

    // Probe for devices
    stlink_t **stdevs;
    size_t count = stlink_probe_usb(&stdevs, CONNECT_NORMAL, 0); // 0 for default freq

    if (count == 0) {
        emit logMessage("No ST-Link devices found.");
        return false;
    }

    // Open the first device
    m_stlink = stdevs[0];
    
    // Free the array but keep the first device (which we are using)
    // Note: stlink_probe_usb allocates an array of pointers. 
    // We need to free the array and other devices if we were not using them.
    // Since we take the first one, we should free others if count > 1.
    if (count > 1) {
        for (size_t i = 1; i < count; i++) {
            stlink_close(stdevs[i]);
        }
    }
    free(stdevs);

    if (!m_stlink) {
        emit logMessage("Failed to open ST-Link device.");
        return false;
    }

    // Enter SWD mode
    if (stlink_enter_swd_mode(m_stlink)) {
        emit logMessage("Failed to enter SWD mode.");
        cleanupStLink();
        return false;
    }

    // Force debug
    if (stlink_force_debug(m_stlink)) {
        emit logMessage("Failed to force debug mode.");
        cleanupStLink();
        return false;
    }

    // Reset - Use RESET_AUTO or avoid RESET_HARD immediately before params if it causes issues
    // stlink_reset(m_stlink, RESET_HARD); 
    // Try to read core ID first to verify connection
    stlink_core_id(m_stlink);
    emit logMessage(QString("Core ID: 0x%1").arg(m_stlink->core_id, 0, 16));

    // Load device params (flash size, page size, etc.)
    if (stlink_load_device_params(m_stlink)) {
        emit logMessage(QString("Failed to load device parameters. Chip ID: 0x%1").arg(m_stlink->chip_id, 0, 16));
        
        // Fallback: If chip ID is 0x446 (STM32F303RE), we can try to manually set params if config fails
        if (m_stlink->chip_id == 0x446) {
             emit logMessage("Attempting manual parameter fallback for STM32F303RE...");
             m_stlink->flash_size = 512 * 1024; // 512KB
             m_stlink->flash_pgsz = 2048;       // 2KB page
             m_stlink->flash_base = 0x08000000;
             m_stlink->sram_size = 64 * 1024;   // 64KB
             m_stlink->sram_base = 0x20000000;
             m_stlink->sys_base = 0x1FFFD800;
             m_stlink->sys_size = 0x2000;
             m_stlink->flash_type = STM32_FLASH_TYPE_F0_F1_F3; // Set correct flash type
             m_stlink->chip_flags = 0; // Initialize flags
             // If we manually set params, we can proceed
        } else {
            cleanupStLink();
            return false;
        }
    }

    // Check status
    stlink_status(m_stlink);

    return true;
}

void StLinkFlasher::cleanupStLink()
{
    if (m_stlink) {
        stlink_exit_debug_mode(m_stlink);
        stlink_close(m_stlink);
        m_stlink = nullptr;
    }
}

void StLinkFlasher::flashFirmware(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_connected) {
        emit operationFinished(false, "Device not connected");
        return;
    }

    emit operationStarted("Flashing");
    emit logMessage("Starting flash process for: " + filePath);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit operationFinished(false, "Could not open firmware file");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    if (data.isEmpty()) {
        emit operationFinished(false, "Firmware file is empty");
        return;
    }

    // Ensure we are in the right mode
    stlink_reset(m_stlink, RESET_HARD);
    stlink_force_debug(m_stlink);
    stlink_status(m_stlink);

    // Unlock flash if necessary
    unlock_flash_if(m_stlink);

    stm32_addr_t flash_base = m_stlink->flash_base;
    uint32_t size = data.size();
    
    emit logMessage("Erasing flash...");
    emit progressChanged(0, 100);

    // Use page erase instead of mass erase to avoid timeouts and provide progress
    uint32_t page_size = m_stlink->flash_pgsz;
    if (page_size == 0) page_size = 2048; // Safety fallback

    uint32_t addr = flash_base;
    uint32_t end_addr = flash_base + size;
    
    int total_pages = (size + page_size - 1) / page_size;
    int erased_pages = 0;

    while (addr < end_addr) {
        int res = stlink_erase_flash_page(m_stlink, addr);
        if (res != 0) {
             emit operationFinished(false, QString("Failed to erase page at 0x%1").arg(addr, 0, 16));
             return;
        }
        addr += page_size;
        erased_pages++;
        
        // Progress 0-50%
        int percent = (int)((double)erased_pages / total_pages * 50.0);
        emit progressChanged(percent, 100);
        
        // Keep GUI responsive
        QThread::msleep(10);
    }

    emit logMessage("Flash erased. Writing firmware...");
    emit progressChanged(50, 100);

    // Use stlink_fwrite_flash for stability. It handles the write loop internally.
    // Note: This will block until writing is done, so progress will jump to 100%.
    // We convert QString path to local 8-bit string for C API.
    QByteArray pathBytes = filePath.toLocal8Bit();
    int res = stlink_fwrite_flash(m_stlink, pathBytes.constData(), flash_base);

    if (res != 0) {
        emit operationFinished(false, QString("Failed to write firmware. Error code: %1").arg(res));
        return;
    }

    emit progressChanged(100, 100);
    emit logMessage("Flashing complete. Resetting device...");

    stlink_reset(m_stlink, RESET_HARD);
    stlink_run(m_stlink, RUN_NORMAL);

    emit operationFinished(true, "Firmware flashed successfully");
}
