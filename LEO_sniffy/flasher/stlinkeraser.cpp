#include "stlinkeraser.h"
#include <QThread>

#include "stlink.h"
extern "C"
{
#include "common_flash.h"
#include "read_write.h"
#include "option_bytes.h"
}

StLinkEraser::StLinkEraser(QObject *parent) : QObject(parent)
{
}

void StLinkEraser::massErase(stlink_t* stlink)
{
    if (!stlink)
    {
        emit operationFinished(false, "Device not connected");
        return;
    }

    emit operationStarted("Mass Erase");
    emit logMessage("Starting Mass Erase...");

    // Ensure we are in debug mode
    if (stlink_current_mode(stlink) != STLINK_DEV_DEBUG_MODE)
    {
        stlink_force_debug(stlink);
    }
    stlink_status(stlink);

    // Helper: detect F1 family by chip id (affects RDP Level 0 value)
    auto isF1Chip = [stlink]() -> bool
    {
        const uint32_t id = stlink->chip_id;
        switch (id)
        {
        case 0x410:
        case 0x412:
        case 0x414:
        case 0x418:
        case 0x420:
        case 0x428:
        case 0x430:
            return true;
        default:
            return false;
        }
    };

    struct RdpInfo
    {
        bool valid = false;
        uint8_t rdp = 0x00;
        uint8_t level0 = 0xAA;
    };

    // Helper: read RDP info (current level and level-0 value per family)
    auto readRdpInfo = [stlink, &isF1Chip]() -> RdpInfo
    {
        RdpInfo info{};
        uint32_t ob = 0;
        switch (stlink->flash_type)
        {
        case STM32_FLASH_TYPE_F2_F4:
        case STM32_FLASH_TYPE_F7:
        {
            if (stlink_read_option_control_register_f4(stlink, &ob) == 0)
            {
                info.valid = true;
                info.rdp = static_cast<uint8_t>((ob >> 8) & 0xFF);
                info.level0 = 0xAA;
            }
            break;
        }
        case STM32_FLASH_TYPE_G4:
        case STM32_FLASH_TYPE_L4:
        case STM32_FLASH_TYPE_WB_WL:
        {
            if (stlink_read_option_control_register_gx(stlink, &ob) == 0)
            {
                info.valid = true;
                info.rdp = static_cast<uint8_t>(ob & 0xFF);
                info.level0 = 0xAA;
            }
            break;
        }
        case STM32_FLASH_TYPE_F0_F1_F3:
        {
            if (stlink_read_option_bytes32(stlink, &ob) == 0)
            {
                info.valid = true;
                info.rdp = static_cast<uint8_t>(ob & 0xFF);
                info.level0 = isF1Chip() ? 0xA5 : 0xAA;
            }
            break;
        }
        default:
            // Unknown/unsupported: treat as not valid, do mass erase
            break;
        }
        return info;
    };

    // Helper: perform RDP regression to Level 0; returns true on success
    auto performRdpRegression = [stlink](uint32_t rawOption, uint8_t level0) -> bool
    {
        int res = -1;
        switch (stlink->flash_type)
        {
        case STM32_FLASH_TYPE_F2_F4:
        case STM32_FLASH_TYPE_F7:
        {
            uint32_t optcr = rawOption;
            optcr &= ~0xFF00u; // Clear RDP
            optcr |= (uint32_t(level0) << 8);
            optcr |= 2u; // OPTSTRT
            unlock_flash_option_if(stlink);
            res = stlink_write_debug32(stlink, 0x40023C14u, optcr);
            wait_flash_busy(stlink);
            break;
        }
        case STM32_FLASH_TYPE_G4:
        case STM32_FLASH_TYPE_L4:
        {
            uint32_t optr = rawOption;
            optr &= ~0xFFu;
            optr |= level0;
            optr |= 2u; // OPTSTRT
            unlock_flash_option_if(stlink);
            res = stlink_write_debug32(stlink, 0x40022020u, optr);
            wait_flash_busy(stlink);
            break;
        }
        case STM32_FLASH_TYPE_WB_WL:
        {
            uint32_t optr = rawOption;
            optr &= ~0xFFu;
            optr |= level0;
            optr |= 2u; // OPTSTRT
            unlock_flash_option_if(stlink);
            res = stlink_write_debug32(stlink, 0x58004020u, optr);
            wait_flash_busy(stlink);
            break;
        }
        case STM32_FLASH_TYPE_F0_F1_F3:
        {
            uint8_t rdp_val = level0;
            unlock_flash_option_if(stlink);
            res = stlink_write_option_bytes(stlink, stlink->option_base, &rdp_val, 1);
            break;
        }
        default:
            return false;
        }
        return (res == 0);
    };

    // Read current RDP state
    uint32_t rawOptionBytes = 0;
    RdpInfo rdp = readRdpInfo();
    bool needsRegression = false;

    if (rdp.valid)
    {
        if (rdp.rdp == 0x00)
        {
            emit logMessage("RDP Level 0x00 detected. Treating as unprotected (Mass Erase).");
        }
        else if (rdp.rdp != rdp.level0)
        {
            needsRegression = true;
            emit logMessage(QString("RDP Level detected: 0x%1. Regression needed.").arg(rdp.rdp, 2, 16, QChar('0')));
        }

        // Preserve raw option word for regression writes where needed
        // For families using control registers, we need the original to modify.
        // Re-read using the appropriate accessor to get the raw value.
        switch (stlink->flash_type)
        {
        case STM32_FLASH_TYPE_F2_F4:
        case STM32_FLASH_TYPE_F7:
            stlink_read_option_control_register_f4(stlink, &rawOptionBytes);
            break;
        case STM32_FLASH_TYPE_G4:
        case STM32_FLASH_TYPE_L4:
        case STM32_FLASH_TYPE_WB_WL:
            stlink_read_option_control_register_gx(stlink, &rawOptionBytes);
            break;
        case STM32_FLASH_TYPE_F0_F1_F3:
            stlink_read_option_bytes32(stlink, &rawOptionBytes);
            break;
        default:
            break;
        }
    }
    else
    {
        emit logMessage("Unknown flash type for RDP check. Proceeding with standard Mass Erase.");
    }

    // Execute
    if (needsRegression)
    {
        emit logMessage("Performing RDP regression...");
        const bool ok = performRdpRegression(rawOptionBytes, rdp.level0);
        if (ok)
        {
            emit logMessage("RDP regression command sent. Waiting for erase...");
            QThread::sleep(2);
            stlink_reset(stlink, RESET_HARD);
            emit operationFinished(true, "RDP Regression / Mass Erase successful.");
        }
        else
        {
            emit operationFinished(false, "RDP regression failed.");
        }
    }
    else
    {
        emit logMessage("Performing standard Mass Erase...");
        if (stlink_erase_flash_mass(stlink) == 0)
        {
            emit operationFinished(true, "Mass Erase successful.");
        }
        else
        {
            emit operationFinished(false, "Mass Erase failed.");
        }
    }
}
