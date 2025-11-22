#include "stlinkflasher.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include <QDir>

// Include stlink headers
#include "stlink.h"
extern "C"
{
#include "common_flash.h"
#include "read_write.h"
}

StLinkFlasher::StLinkFlasher(QObject *parent)
    : QObject(parent), m_stlink(nullptr), m_connected(false), m_flashing(false)
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
    if (m_flashing) {
        emit logMessage("Flash in progress; ignoring connect request");
        return;
    }
    if (m_connected)
    {
        emit deviceConnected("Already connected");
        return;
    }

    if (initStLink())
    {
        m_connected = true;

        // Get device info
        QString info = QString("Core ID: 0x%1, Chip ID: 0x%2")
                           .arg(m_stlink->core_id, 0, 16)
                           .arg(m_stlink->chip_id, 0, 16);

        emit deviceConnected(info);
        emit logMessage("Connected to ST-Link. " + info);
    }
    else
    {
        emit operationFinished(false, "Failed to connect to ST-Link");
    }
}

void StLinkFlasher::disconnectDevice()
{
    QMutexLocker locker(&m_mutex);
    cleanupStLink();
}

void StLinkFlasher::stopAndCleanup()
{
    disconnectDevice();
    QThread::currentThread()->quit();
}

bool StLinkFlasher::initStLink()
{
    // Set STLINK_CHIPS_DIR to help libstlink find chip definitions
    QString appDir = QCoreApplication::applicationDirPath();

    // Construct path to config/chips
    // Use QDir to handle separators, but force forward slashes for MinGW/libstlink compatibility
    QString chipsDir = QDir(appDir).filePath("config/chips");
    chipsDir = chipsDir.replace("\\", "/");

    if (QDir(chipsDir).exists())
    {
        qputenv("STLINK_CHIPS_DIR", chipsDir.toLocal8Bit());
        emit logMessage("Set STLINK_CHIPS_DIR to: " + chipsDir);
    }
    else
    {
        emit logMessage("Warning: Chips config dir not found at: " + chipsDir);
    }

    // Probe for devices
    stlink_t **stdevs;
    size_t count = stlink_probe_usb(&stdevs, CONNECT_NORMAL, 0); // 0 for default freq

    if (count == 0)
    {
        emit logMessage("No ST-Link devices found.");
        return false;
    }

    // Open the first device
    m_stlink = stdevs[0];

    // Free other devices if any
    if (count > 1)
    {
        for (size_t i = 1; i < count; i++)
        {
            stlink_close(stdevs[i]);
        }
    }
    free(stdevs);

    if (!m_stlink)
    {
        emit logMessage("Failed to open ST-Link device.");
        return false;
    }

    // Enter SWD mode
    if (stlink_enter_swd_mode(m_stlink))
    {
        emit logMessage("Failed to enter SWD mode.");
        cleanupStLink();
        return false;
    }

    // Force debug
    if (stlink_force_debug(m_stlink))
    {
        emit logMessage("Failed to force debug mode.");
        cleanupStLink();
        return false;
    }

    // Try to read core ID first to verify connection
    stlink_core_id(m_stlink);
    emit logMessage(QString("Core ID: 0x%1").arg(m_stlink->core_id, 0, 16));

    // Load device params (flash size, page size, etc.)
    // This relies on STLINK_CHIPS_DIR being set correctly to find chip definitions.
    if (stlink_load_device_params(m_stlink))
    {
        emit logMessage(QString("Failed to load device parameters automatically. Chip ID: 0x%1").arg(m_stlink->chip_id, 0, 16));
        
        if (!loadDeviceParamsFallback()) {
            cleanupStLink();
            return false;
        }
    }

    emit logMessage(QString("Device params loaded: Flash: %1KB, Page: %2B, SRAM: %3KB")
                        .arg(m_stlink->flash_size / 1024)
                        .arg(m_stlink->flash_pgsz)
                        .arg(m_stlink->sram_size / 1024));

    // Check status
    stlink_status(m_stlink);

    return true;
}

void StLinkFlasher::cleanupStLink()
{
    if (m_stlink)
    {
        stlink_exit_debug_mode(m_stlink);
        stlink_close(m_stlink);
        m_stlink = nullptr;
    }
    m_connected = false;
    // Ensure flashing flag cleared if cleanup happens mid-operation
    m_flashing = false;
}

void StLinkFlasher::flashFirmware(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

    if (!m_connected || !m_stlink)
    {
        emit operationFinished(false, "Device not connected");
        return;
    }

    emit operationStarted("Flashing");
    emit logMessage("Starting flash process for: " + filePath);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit operationFinished(false, "Could not open firmware file");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    if (data.isEmpty())
    {
        emit operationFinished(false, "Firmware file is empty");
        return;
    }

    // Mark flashing in progress now that validation passed
    m_flashing = true;

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
    if (page_size == 0)
        page_size = 2048; // Safety fallback

    uint32_t addr = flash_base;
    uint32_t end_addr = flash_base + size;

    int total_pages = (size + page_size - 1) / page_size;
    int erased_pages = 0;

    while (addr < end_addr)
    {
        int res = stlink_erase_flash_page(m_stlink, addr);
        if (res != 0)
        {
            m_flashing = false;
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

    // Ensure flash is unlocked before writing
    unlock_flash_if(m_stlink);

    const uint8_t *content = (const uint8_t *)data.constData();
    uint32_t write_addr = flash_base;
    uint32_t remaining = size;

    // Use page size for chunks if possible, or default to 2KB
    uint32_t chunk_size = m_stlink->flash_pgsz;
    if (chunk_size == 0 || chunk_size > 4096)
        chunk_size = 2048;

    int total_chunks = (size + chunk_size - 1) / chunk_size;
    int written_chunks = 0;

    while (remaining > 0)
    {
        uint32_t len = (remaining > chunk_size) ? chunk_size : remaining;

        // Create a mutable copy of the current chunk to avoid casting away constness.
        QByteArray chunkCopy((const char*)content, len);
        int res = stlink_write_flash(m_stlink, write_addr, reinterpret_cast<uint8_t *>(chunkCopy.data()), len, 0);

        if (res != 0)
        {
            m_flashing = false;
            emit operationFinished(false, QString("Failed to write at address 0x%1. Error: %2").arg(write_addr, 0, 16).arg(res));
            return;
        }

        write_addr += len;
        content += len;
        remaining -= len;
        written_chunks++;

        // Progress 50-100%
        int percent = 50 + (int)((double)written_chunks / total_chunks * 50.0);
        emit progressChanged(percent, 100);

        // Small delay to keep UI responsive and not flood USB
        QThread::msleep(5);
    }

    emit progressChanged(100, 100);
    emit logMessage("Flashing complete. Resetting device...");

    stlink_reset(m_stlink, RESET_HARD);
    stlink_run(m_stlink, RUN_NORMAL);

    m_flashing = false;
    emit operationFinished(true, "Firmware flashed successfully");
}

bool StLinkFlasher::loadDeviceParamsFallback()
{
    emit logMessage("Attempting manual parameter fallback...");
    
    QString chipsDir = qgetenv("STLINK_CHIPS_DIR");
    if (chipsDir.isEmpty())
    {
        QString appDir = QCoreApplication::applicationDirPath();
        chipsDir = QDir(appDir).filePath("config/chips");
    }

    QDir dir(chipsDir);
    QStringList filters;
    filters << "*.chip";
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    for (const QFileInfo &fileInfo : list)
    {
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            uint32_t fileChipId = 0;

            // Simple parser variables
            uint32_t p_flash_pagesize = 0;
            uint32_t p_sram_size = 0;
            uint32_t p_bootrom_base = 0;
            uint32_t p_bootrom_size = 0;
            uint32_t p_option_base = 0;
            uint32_t p_option_size = 0;
            uint32_t p_flags = 0;
            enum stm32_flash_type p_flash_type = STM32_FLASH_TYPE_UNKNOWN;

            while (!in.atEnd())
            {
                QString line = in.readLine().trimmed();
                if (line.startsWith("#") || line.isEmpty())
                    continue;

                // Remove comments
                int commentIdx = line.indexOf("//");
                if (commentIdx != -1)
                    line = line.left(commentIdx).trimmed();

                QStringList parts = line.split(QRegularExpression("\\s+"));
                if (parts.size() < 2)
                    continue;

                QString key = parts[0];
                QString value = parts[1];

                bool ok;
                if (key == "chip_id")
                {
                    fileChipId = value.toUInt(&ok, 0); // Auto-detect base (hex 0x...)
                }
                else if (key == "flash_pagesize")
                {
                    p_flash_pagesize = value.toUInt(&ok, 0);
                }
                else if (key == "sram_size")
                {
                    p_sram_size = value.toUInt(&ok, 0);
                }
                else if (key == "bootrom_base")
                {
                    p_bootrom_base = value.toUInt(&ok, 0);
                }
                else if (key == "bootrom_size")
                {
                    p_bootrom_size = value.toUInt(&ok, 0);
                }
                else if (key == "option_base")
                {
                    p_option_base = value.toUInt(&ok, 0);
                }
                else if (key == "option_size")
                {
                    p_option_size = value.toUInt(&ok, 0);
                }
                else if (key == "flash_type")
                {
                    if (value == "F0_F1_F3")
                        p_flash_type = STM32_FLASH_TYPE_F0_F1_F3;
                    else if (value == "F1_XL")
                        p_flash_type = STM32_FLASH_TYPE_F1_XL;
                    else if (value == "F2_F4")
                        p_flash_type = STM32_FLASH_TYPE_F2_F4;
                    else if (value == "F7")
                        p_flash_type = STM32_FLASH_TYPE_F7;
                    else if (value == "G0")
                        p_flash_type = STM32_FLASH_TYPE_G0;
                    else if (value == "G4")
                        p_flash_type = STM32_FLASH_TYPE_G4;
                    else if (value == "H7")
                        p_flash_type = STM32_FLASH_TYPE_H7;
                    else if (value == "L0_L1")
                        p_flash_type = STM32_FLASH_TYPE_L0_L1;
                    else if (value == "L4")
                        p_flash_type = STM32_FLASH_TYPE_L4;
                    else if (value == "L5_U5_H5")
                        p_flash_type = STM32_FLASH_TYPE_L5_U5_H5;
                    else if (value == "WB_WL")
                        p_flash_type = STM32_FLASH_TYPE_WB_WL;
                    else if (value == "C0")
                        p_flash_type = STM32_FLASH_TYPE_C0;
                }
            }

            if (fileChipId == m_stlink->chip_id)
            {
                emit logMessage("Found matching config file: " + fileInfo.fileName());

                m_stlink->flash_pgsz = p_flash_pagesize;
                m_stlink->sram_size = p_sram_size;
                m_stlink->sys_base = p_bootrom_base;
                m_stlink->sys_size = p_bootrom_size;
                m_stlink->option_base = p_option_base;
                m_stlink->option_size = p_option_size;
                m_stlink->flash_type = p_flash_type;
                m_stlink->chip_flags = p_flags;

                // Flash size is usually read from a register, but we can try to set a default
                // or let the library handle it if it can read the register now that it has params.
                // For F303RE, flash size register is at 0x1ffff7cc.
                // We can try to read it.
                uint32_t flash_size_kb = 0;
                // F3 specific register for flash size
                if (p_flash_type == STM32_FLASH_TYPE_F0_F1_F3)
                {
                    // Try to read from standard address for F0/F1/F3
                    // 0x1FFFF7CC is common for F3
                    uint32_t val;
                    if (!stlink_read_mem32(m_stlink, 0x1ffff7cc, 4))
                    {
                        val = *(uint32_t *)m_stlink->q_buf;
                        flash_size_kb = val & 0xFFFF;
                    }
                }

                if (flash_size_kb > 0)
                {
                    m_stlink->flash_size = flash_size_kb * 1024;
                }
                else
                {
                    // Fallback if read fails: 512KB (common for F303RE)
                    // Or we could parse 'flash_size_reg' from file and read it.
                    // For now, let's assume 512KB if we can't read it, or 64KB min.
                    m_stlink->flash_size = 512 * 1024;
                    emit logMessage("Warning: Could not read flash size, defaulting to 512KB");
                }

                m_stlink->flash_base = 0x08000000; // Standard base
                m_stlink->sram_base = 0x20000000;  // Standard base

                return true;
            }
        }
    }

    emit logMessage("No matching config file found for this chip.");
    return false;
}
