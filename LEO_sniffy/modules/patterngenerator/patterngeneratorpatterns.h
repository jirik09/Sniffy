#ifndef PATTERNGENERATORPATTERNS_H
#define PATTERNGENERATORPATTERNS_H

#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include <math.h>

#include "patterngeneratordefs.h"
#include "patterngeneratorconfig.h"

typedef quint8 patttype;

#define CLOCK_MASK(i) (patttype)((i % 2) ? 0x7F : 0xFF)
#define CLOCK_CH8 0x80

class Pattern : public QObject
{
    Q_OBJECT
public:
    explicit Pattern(QObject *parent = nullptr);
    virtual QList<patttype> *getData() = 0;
    virtual QList<patttype> *create(int) = 0;
    virtual bool isExponencial() = 0;
};

class UserDefined : public Pattern
{
    Q_OBJECT
public:
    explicit UserDefined();
    QList<patttype> *getData() override;
    QList<patttype> *create(int len) override;
    bool isExponencial() override;

private:
    QList<patttype> *data;
    const bool exp = false;
};

class CounterClock : public Pattern
{
    Q_OBJECT
public:
    explicit CounterClock();
    QList<patttype> *getData() override;
    QList<patttype> *create(int len) override;
    bool isExponencial() override;

private:
    QList<patttype> *data;
    const bool exp = false;
};

class BinaryCode : public Pattern
{
    Q_OBJECT
public:
    explicit BinaryCode();
    QList<patttype> *getData() override;
    QList<patttype> *create(int chanNum) override;
    bool isExponencial() override;

private:
    QList<patttype> *data;
    const bool exp = true;
};

class GrayCode : public Pattern
{
    Q_OBJECT
public:
    explicit GrayCode();
    QList<patttype> *getData() override;
    QList<patttype> *create(int chanNum) override;
    bool isExponencial() override;

private:
    QList<patttype> *data;
    const bool exp = true;
};

class Quadrature : public Pattern
{
    Q_OBJECT
public:
    explicit Quadrature();
    QList<patttype> *getData() override;
    QList<patttype> *create(int len = PATT_DEFAULT_DATA_LENGTH) override;
    bool isExponencial() override;
    QList<patttype> *setSequence(bool seqAb);

private:
    QList<patttype> *data;
    bool seqAb = true;
    const bool exp = false;
};

// New patterns
class PRBS : public Pattern
{
    Q_OBJECT
public:
    explicit PRBS();
    QList<patttype> *getData() override;
    QList<patttype> *create(int order = PATT_DEFAULT_PRBS_ORDER) override;
    bool isExponencial() override;
    QList<patttype> *setOrder(int order);

private:
    QList<patttype> *data;
    int order;
    const bool exp = false;
};

class PWM : public Pattern
{
    Q_OBJECT
public:
    explicit PWM();
    QList<patttype> *getData() override;
    QList<patttype> *create(int duty = PATT_DEFAULT_PWM_DUTY) override;
    bool isExponencial() override;
    QList<patttype> *setDuty(int duty);

private:
    QList<patttype> *data;
    int duty;
    const bool exp = false;
};

class LineCode : public Pattern
{
    Q_OBJECT
public:
    explicit LineCode();
    QList<patttype> *getData() override;
    QList<patttype> *create(int type = (int)LineCodeType::NRZ_L) override;
    bool isExponencial() override;
    QList<patttype> *setType(LineCodeType type);

private:
    QList<patttype> *data;
    LineCodeType type;
    const bool exp = false;
};

class FourBFiveB : public Pattern
{
    Q_OBJECT
public:
    explicit FourBFiveB();
    QList<patttype> *getData() override;
    QList<patttype> *create(int groups = 8) override;
    bool isExponencial() override;
    QList<patttype> *setGroups(int groups);

private:
    QList<patttype> *data;
    int groups;
    const bool exp = false;
};

class JohnsonNPhase : public Pattern
{
    Q_OBJECT
public:
    explicit JohnsonNPhase();
    QList<patttype> *getData() override;
    QList<patttype> *create(int phases = PATT_DEFAULT_JOHNSON_PHASES) override;
    bool isExponencial() override;

private:
    QList<patttype> *data;
    int phases;
    const bool exp = false;
};

class PDM : public Pattern
{
    Q_OBJECT
public:
    explicit PDM();
    QList<patttype> *getData() override;
    QList<patttype> *create(int level = PATT_DEFAULT_PDM_LEVEL) override;
    bool isExponencial() override;

private:
    QList<patttype> *data;
    int level;
    const bool exp = false;
};

class ParallelBus : public Pattern
{
    Q_OBJECT
public:
    explicit ParallelBus();
    QList<patttype> *getData() override;
    QList<patttype> *create(int width = PATT_DEFAULT_PARBUS_WIDTH) override;
    bool isExponencial() override;

private:
    QList<patttype> *data;
    int width;
    const bool exp = false;
};

// UART pattern
class UARTPattern : public Pattern
{
    Q_OBJECT
public:
    explicit UARTPattern();
    QList<patttype> *getData() override;
    QList<patttype> *create(int dummy = 0) override; // uses internal params
    bool isExponencial() override;

    // parameter setters
    QList<patttype> *setBaud(int baud);
    QList<patttype> *setDataBits(int bits);
    QList<patttype> *setParity(UartParity p);
    QList<patttype> *setStopBits(int stop);
    QList<patttype> *setBitOrder(BitOrder order);
    QList<patttype> *setIdle(UartIdleLevel idle);
    QList<patttype> *setFramingError(bool on);
    QList<patttype> *setBreakCond(bool on);

private:
    QList<patttype> *data;
    int baud;
    int dataBits;
    UartParity parity;
    int stopBits;
    BitOrder bitOrder;
    UartIdleLevel idleLevel;
    bool framingError;
    bool breakCond;
    const bool exp = false;
};

// SPI pattern
class SPIPattern : public Pattern
{
    Q_OBJECT
public:
    explicit SPIPattern();
    QList<patttype> *getData() override;
    QList<patttype> *create(int dummy = 0) override;
    bool isExponencial() override;

    QList<patttype> *setClockHz(int hz);
    QList<patttype> *setMode(SpiMode m);
    QList<patttype> *setWordSize(int bits);
    QList<patttype> *setBitOrder(BitOrder order);
    QList<patttype> *setCsGating(bool on);
    QList<patttype> *setPauseTicks(int ticks);

private:
    QList<patttype> *data;
    int clockHz;
    SpiMode mode;
    int wordSize;
    BitOrder bitOrder;
    bool csGating;
    int pauseTicks;
    const bool exp = false;
};

// I2C pattern
class I2CPattern : public Pattern
{
    Q_OBJECT
public:
    explicit I2CPattern();
    QList<patttype> *getData() override;
    QList<patttype> *create(int dummy = 0) override;
    bool isExponencial() override;

    QList<patttype> *setSpeed(int hz);
    QList<patttype> *setAddrMode(I2cAddrMode m);
    QList<patttype> *setAddress(int addr);
    QList<patttype> *setReadWrite(ModeRW rw);
    QList<patttype> *setAck(bool ack);
    QList<patttype> *setClockStretch(bool on);
    QList<patttype> *setRepeatedStart(bool on);

private:
    QList<patttype> *data;
    int speed;
    I2cAddrMode addrMode;
    int address;
    ModeRW rw;
    bool ack;
    bool stretch;
    bool repstart;
    const bool exp = false;
};

class PatternGeneratorPatterns : public QObject
{
    Q_OBJECT
public:
    explicit PatternGeneratorPatterns(QObject *parent = nullptr);

    QList<patttype> *createPatternOrJustGetData(int index, int len);
    QList<patttype> *setDataLen(int index, int len);
    QList<patttype> *setDefault(int index);
    QList<patttype> *modifyPattern(int channel, int position);
    bool isExponencial();

    QList<patttype> *setQuadratureSequence(bool seqAb);
    // Setters for new patterns
    QList<patttype> *setPrbsOrder(int order);
    QList<patttype> *setPwmDuty(int duty);
    QList<patttype> *setLineCodeType(LineCodeType type);
    QList<patttype> *setFourBFiveBGroups(int groups);
    QList<patttype> *setJohnsonPhases(int phases);
    QList<patttype> *setPdmLevel(int level);
    QList<patttype> *setParBusWidth(int width);

    // UART setters (index 12)
    QList<patttype> *setUartBaud(int baud);
    QList<patttype> *setUartDataBits(int bits);
    QList<patttype> *setUartParity(UartParity p);
    QList<patttype> *setUartStopBits(int stop);
    QList<patttype> *setUartBitOrder(BitOrder order);
    QList<patttype> *setUartIdle(UartIdleLevel idle);
    QList<patttype> *setUartFramingError(bool on);
    QList<patttype> *setUartBreak(bool on);

    // SPI setters (index 13)
    QList<patttype> *setSpiClockHz(int hz);
    QList<patttype> *setSpiMode(SpiMode m);
    QList<patttype> *setSpiWordSize(int bits);
    QList<patttype> *setSpiBitOrder(BitOrder order);
    QList<patttype> *setSpiCsGating(bool on);
    QList<patttype> *setSpiPauseTicks(int ticks);

    // I2C setters (index 14)
    QList<patttype> *setI2cSpeed(int hz);
    QList<patttype> *setI2cAddrMode(I2cAddrMode m);
    QList<patttype> *setI2cAddress(int addr);
    QList<patttype> *setI2cReadWrite(ModeRW rw);
    QList<patttype> *setI2cAck(bool ack);
    QList<patttype> *setI2cStretch(bool on);
    QList<patttype> *setI2cRepeatedStart(bool on);

private:
    int index;
    QSharedPointer<Pattern> pattern[PATTERNS_NUM];
    inline void ensure(int i)
    {
        if (!pattern[i])
            pattern[i] = createPatternObj[i]();
    }

    QVector<std::function<QSharedPointer<Pattern>()>> createPatternObj = {
        []
        { return QSharedPointer<Pattern>(new UserDefined); },
        []
        { return QSharedPointer<Pattern>(new CounterClock); },
        []
        { return QSharedPointer<Pattern>(new BinaryCode); },
        []
        { return QSharedPointer<Pattern>(new GrayCode); },
        []
        { return QSharedPointer<Pattern>(new Quadrature); },
        []
        { return QSharedPointer<Pattern>(new PRBS); }, // index 5
        []
        { return QSharedPointer<Pattern>(new PWM); }, // 6
        []
        { return QSharedPointer<Pattern>(new LineCode); }, // 7
        []
        { return QSharedPointer<Pattern>(new FourBFiveB); }, // 8
        []
        { return QSharedPointer<Pattern>(new JohnsonNPhase); }, // 9
        []
        { return QSharedPointer<Pattern>(new PDM); }, // 10
        []
        { return QSharedPointer<Pattern>(new ParallelBus); }, // 11
        []
        { return QSharedPointer<Pattern>(new UARTPattern); }, // 12
        []
        { return QSharedPointer<Pattern>(new SPIPattern); }, // 13
        []
        { return QSharedPointer<Pattern>(new I2CPattern); }, // 14
    };
};

#endif // PATTERNGENERATORPATTERNS_H
