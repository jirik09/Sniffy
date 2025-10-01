#ifndef PATTERNGENERATORDEFS_H
#define PATTERNGENERATORDEFS_H

// Note: no Qt-specific includes required here

#define PATTERNS_USE_QCUSTOM_PLOT 0

#define PATTERNS_NUM 15
#define PATT_MAX_CHANNELS_NUM 8
#define PATT_MIN_GEN_FREQ (float)0.01

#define PATT_RANGE_CHAN1_LOG1 24
#define PATT_RANGE_CHAN1_LOG0 22

#define PATT_DEFAULT_GEN_FREQ 1000
#define PATT_DEFAULT_DATA_LENGTH 8

enum class ModeRW
{
    RAED = 0,
    WRITE
};

// New pattern-related enums and defaults
enum class LineCodeType
{
    NRZ_L = 0,
    RZ,
    Manchester,
    Miller,
    BiPhaseMark,
    BiPhaseSpace
};

// Reasonable defaults for new patterns
#define PATT_DEFAULT_PRBS_ORDER 7
#define PATT_DEFAULT_PWM_DUTY 50      // percent
#define PATT_DEFAULT_JOHNSON_PHASES 4 // channels used (max 8)
#define PATT_DEFAULT_PDM_LEVEL 50     // target level percent
#define PATT_DEFAULT_PARBUS_WIDTH 8   // bits

// UART/SPI/I2C protocol option enums and defaults
enum class UartParity
{
    None = 0,
    Even = 1,
    Odd = 2
};
enum class BitOrder
{
    LSB_First = 0,
    MSB_First = 1
};
enum class UartIdleLevel
{
    High = 0,
    Low = 1
};
enum class SpiMode
{
    Mode0 = 0,
    Mode1 = 1,
    Mode2 = 2,
    Mode3 = 3
};
enum class I2cAddrMode
{
    Addr7Bit = 0,
    Addr10Bit = 1
};

#define PATT_DEFAULT_UART_BAUD 115200
#define PATT_DEFAULT_UART_DATABITS 8
#define PATT_DEFAULT_UART_PARITY (int)UartParity::None
#define PATT_DEFAULT_UART_STOPBITS 1
#define PATT_DEFAULT_UART_ORDER (int)BitOrder::LSB_First
#define PATT_DEFAULT_UART_IDLE (int)UartIdleLevel::High

#define PATT_DEFAULT_SPI_MODE (int)SpiMode::Mode0
#define PATT_DEFAULT_SPI_WORDSIZE 8
#define PATT_DEFAULT_SPI_ORDER (int)BitOrder::MSB_First
#define PATT_DEFAULT_SPI_CS_GATING 1
#define PATT_DEFAULT_SPI_PAUSE_TICKS 0

#define PATT_DEFAULT_I2C_SPEED 100000
#define PATT_DEFAULT_I2C_ADDR_MODE (int)I2cAddrMode::Addr7Bit
#define PATT_DEFAULT_I2C_ADDRESS 0x50
#define PATT_DEFAULT_I2C_RW (int)ModeRW::WRITE
#define PATT_DEFAULT_I2C_ACK 1
#define PATT_DEFAULT_I2C_STRETCH 0
#define PATT_DEFAULT_I2C_REPSTART 0

#endif // PATTERNGENERATORDEFS_H
