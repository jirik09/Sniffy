#include "stlinkinspector.h"
#include <algorithm>

#include "stlink.h"
extern "C"
{
#include "chipid.h"
#include "read_write.h"
}

StLinkInspector::StLinkInspector(QObject *parent) : QObject(parent)
{
}

QString StLinkInspector::getDetectedMcu(stlink_t* stlink)
{
    if (!stlink)
    {
        return "Unknown";
    }

    // 1. Determine Family (e.g. F303)
    QString family = "Unknown";
    struct stlink_chipid_params *params = stlink_chipid_get_params(stlink->chip_id);
    if (params && params->dev_type)
    {
        QString devType = QString::fromLatin1(params->dev_type);
        // Expected format: STM32F303_High_Density or STM32F446 etc.
        // We want to extract "F303" or "F446"
        // Remove "STM32" prefix if present
        if (devType.startsWith("STM32"))
        {
            devType = devType.mid(5);
        }

        // Extract the Fxxx part.
        // Sometimes dev_type is "F303_High_Density" or just "F446"
        // We take characters until we hit a non-alphanumeric or underscore
        int end = 0;
        while (end < devType.length() && (devType[end].isLetterOrNumber()))
        {
            end++;
        }
        family = devType.left(end);
    }
    else
    {
        // Fallback based on Chip ID if params not found
        // This is a partial list, can be expanded
        switch (stlink->chip_id)
        {
        case 0x446:
            family = "F303";
            break; // F303xD/E
        case 0x422:
            family = "F303";
            break; // F303xB/C
        case 0x438:
            family = "F303";
            break; // F303x6/8
        case 0x413:
            family = "F405";
            break; // F405/407
        case 0x419:
            family = "F427";
            break; // F427/437
        case 0x421:
            family = "F446";
            break;
        case 0x463:
            family = "G474";
            break; // G47x/G48x
        default:
            family = QString("UnknownID_%1").arg(stlink->chip_id, 0, 16);
            break;
        }
    }

    // 2. Determine Flash Size Character
    QChar flashChar = '?';
    uint32_t flashKb = stlink->flash_size / 1024;

    if (flashKb <= 16)
        flashChar = '4';
    else if (flashKb <= 32)
        flashChar = '6';
    else if (flashKb <= 64)
        flashChar = '8';
    else if (flashKb <= 128)
        flashChar = 'B';
    else if (flashKb <= 256)
        flashChar = 'C';
    else if (flashKb <= 384)
        flashChar = 'D';
    else if (flashKb <= 512)
        flashChar = 'E';
    else if (flashKb <= 768)
        flashChar = 'F';
    else if (flashKb <= 1024)
        flashChar = 'G';
    else if (flashKb <= 1536)
        flashChar = 'H';
    else if (flashKb >= 2048)
        flashChar = 'I';

    // 3. Determine Package (Pin count)
    // This is hard to detect via SWD without specific registers.
    // Default to 'R' (64 pins) as it is most common for Nucleo boards used in this project.
    QChar packageChar = 'R';

    return QString("%1%2%3").arg(family).arg(packageChar).arg(flashChar);
}

void StLinkInspector::readUID(stlink_t* stlink)
{
    if (!stlink)
    {
        emit deviceUIDError("Device not connected");
        return;
    }

    // Ensure target is responsive
    stlink_force_debug(stlink);

    // Select family-specific UID base from loaded device parameters
    uint32_t uidBase = 0;
    switch (stlink->flash_type)
    {
    case STM32_FLASH_TYPE_F2_F4:
        uidBase = 0x1FFF7A10u; // STM32F4x UID base
        break;
    case STM32_FLASH_TYPE_F0_F1_F3:
        uidBase = 0x1FFFF7ACu; // STM32F3x UID base
        break;
    case STM32_FLASH_TYPE_G4:
        uidBase = 0x1FFF7590u; // STM32G4x UID base
        break;
    default:
        emit deviceUIDError("Unsupported MCU family for UID read");
        return;
    }

    int res = stlink_read_mem32(stlink, uidBase, 12);
    if (res != 0)
    {
        emit deviceUIDError(QString("UID read failed at 0x%1").arg(uidBase, 0, 16));
        return;
    }

    QByteArray uid(reinterpret_cast<const char *>(stlink->q_buf), 12);
    bool allZero = std::all_of(uid.begin(), uid.end(), [](char c)
                               { return static_cast<unsigned char>(c) == 0x00; });
    bool allFF = std::all_of(uid.begin(), uid.end(), [](char c)
                             { return static_cast<unsigned char>(c) == 0xFF; });
    if (allZero || allFF)
    {
        emit deviceUIDError("UID appears invalid (all 00/FF)");
        return;
    }

    QString uidHex = uid.toHex().toUpper();
    QString mcu = getDetectedMcu(stlink);
    emit logMessage(QString("MCU: %1").arg(mcu));
    emit deviceUIDAvailable(uidHex, mcu);
}
