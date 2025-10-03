#include "patterngeneratorpatterns.h"

/****************************** CONSTRUCTORS START :D ******************************/
PatternGeneratorPatterns::PatternGeneratorPatterns(QObject *parent) : QObject(parent)
{
    for (int i = 0; i < PATTERNS_NUM; i++)
        pattern[i] = nullptr;
}

Pattern::Pattern(QObject *parent) : QObject(parent) {}

UserDefined::UserDefined() { data = new QList<patttype>; }
CounterClock::CounterClock() { data = new QList<patttype>; }
BinaryCode::BinaryCode() { data = new QList<patttype>; }
GrayCode::GrayCode() { data = new QList<patttype>; }
Quadrature::Quadrature() { data = new QList<patttype>; }
PRBS::PRBS()
{
    data = new QList<patttype>;
    order = PATT_DEFAULT_PRBS_ORDER;
}
PWM::PWM()
{
    data = new QList<patttype>;
    duty = PATT_DEFAULT_PWM_DUTY;
}
LineCode::LineCode()
{
    data = new QList<patttype>;
    type = LineCodeType::NRZ_L;
}
FourBFiveB::FourBFiveB()
{
    data = new QList<patttype>;
    groups = 8;
}
JohnsonNPhase::JohnsonNPhase()
{
    data = new QList<patttype>;
    phases = PATT_DEFAULT_JOHNSON_PHASES;
}
PDM::PDM()
{
    data = new QList<patttype>;
    level = PATT_DEFAULT_PDM_LEVEL;
}
ParallelBus::ParallelBus()
{
    data = new QList<patttype>;
    width = PATT_DEFAULT_PARBUS_WIDTH;
}

UARTPattern::UARTPattern()
{
    data = new QList<patttype>;
    baud = PATT_DEFAULT_UART_BAUD;
    dataBits = PATT_DEFAULT_UART_DATABITS;
    parity = (UartParity)PATT_DEFAULT_UART_PARITY;
    stopBits = PATT_DEFAULT_UART_STOPBITS;
    bitOrder = (BitOrder)PATT_DEFAULT_UART_ORDER;
    idleLevel = (UartIdleLevel)PATT_DEFAULT_UART_IDLE;
    framingError = false;
    breakCond = false;
}

SPIPattern::SPIPattern()
{
    data = new QList<patttype>;
    clockHz = PATT_DEFAULT_GEN_FREQ; // logical ticks
    mode = (SpiMode)PATT_DEFAULT_SPI_MODE;
    wordSize = PATT_DEFAULT_SPI_WORDSIZE;
    bitOrder = (BitOrder)PATT_DEFAULT_SPI_ORDER;
    csGating = PATT_DEFAULT_SPI_CS_GATING != 0;
    pauseTicks = PATT_DEFAULT_SPI_PAUSE_TICKS;
}

I2CPattern::I2CPattern()
{
    data = new QList<patttype>;
    speed = PATT_DEFAULT_I2C_SPEED;
    addrMode = (I2cAddrMode)PATT_DEFAULT_I2C_ADDR_MODE;
    address = PATT_DEFAULT_I2C_ADDRESS;
    rw = (ModeRW)PATT_DEFAULT_I2C_RW;
    ack = PATT_DEFAULT_I2C_ACK != 0;
    stretch = PATT_DEFAULT_I2C_STRETCH != 0;
    repstart = PATT_DEFAULT_I2C_REPSTART != 0;
}

/****************************** CONSTRUCTORS END :D ******************************/

QList<patttype> *UserDefined::getData() { return data; }
QList<patttype> *CounterClock::getData() { return data; }
QList<patttype> *BinaryCode::getData() { return data; }
QList<patttype> *GrayCode::getData() { return data; }
QList<patttype> *Quadrature::getData() { return data; }
QList<patttype> *PRBS::getData() { return data; }
QList<patttype> *PWM::getData() { return data; }
QList<patttype> *LineCode::getData() { return data; }
QList<patttype> *FourBFiveB::getData() { return data; }
QList<patttype> *JohnsonNPhase::getData() { return data; }
QList<patttype> *PDM::getData() { return data; }
QList<patttype> *ParallelBus::getData() { return data; }
QList<patttype> *UARTPattern::getData() { return data; }
QList<patttype> *SPIPattern::getData() { return data; }
QList<patttype> *I2CPattern::getData() { return data; }

bool UserDefined::isExponencial() { return exp; }
bool CounterClock::isExponencial() { return exp; }
bool BinaryCode::isExponencial() { return exp; }
bool GrayCode::isExponencial() { return exp; }
bool Quadrature::isExponencial() { return exp; }
bool PRBS::isExponencial() { return exp; }
bool PWM::isExponencial() { return exp; }
bool LineCode::isExponencial() { return exp; }
bool FourBFiveB::isExponencial() { return exp; }
bool JohnsonNPhase::isExponencial() { return exp; }
bool PDM::isExponencial() { return exp; }
bool ParallelBus::isExponencial() { return exp; }
bool UARTPattern::isExponencial() { return exp; }
bool SPIPattern::isExponencial() { return exp; }
bool I2CPattern::isExponencial() { return exp; }

QList<patttype> *PatternGeneratorPatterns::createPatternOrJustGetData(int index, int len)
{
    this->index = index;

    if (pattern[index] == nullptr)
    {
        pattern[index] = createPatternObj[index]();
        // Use sensible defaults per pattern on first creation with new indices
        switch (index)
        {
        case 5: // PRBS
            static_cast<PRBS *>(pattern[index].get())->create(PATT_DEFAULT_PRBS_ORDER);
            break;
        case 6: // PWM
            static_cast<PWM *>(pattern[index].get())->create(PATT_DEFAULT_PWM_DUTY);
            break;
        case 7: // Line code
            static_cast<LineCode *>(pattern[index].get())->create((int)LineCodeType::NRZ_L);
            break;
        case 8: // 4B/5B
            static_cast<FourBFiveB *>(pattern[index].get())->create(8);
            break;
        case 9: // Johnson N-phase
            static_cast<JohnsonNPhase *>(pattern[index].get())->create(PATT_DEFAULT_JOHNSON_PHASES);
            break;
        case 10: // PDM
            static_cast<PDM *>(pattern[index].get())->create(PATT_DEFAULT_PDM_LEVEL);
            break;
        case 11: // Parallel bus
            static_cast<ParallelBus *>(pattern[index].get())->create(PATT_DEFAULT_PARBUS_WIDTH);
            break;
        default:
            pattern[index]->create(len);
            break;
        }
    }

    return pattern[index]->getData();
}

QList<patttype> *PatternGeneratorPatterns::setDataLen(int index, int len)
{
    return pattern[index]->create(len);
}

QList<patttype> *PatternGeneratorPatterns::setDefault(int index)
{
    // Create if needed
    if (pattern[index] == nullptr)
    {
        pattern[index] = createPatternObj[index]();
    }
    switch (index)
    {
    case 5: // PRBS
        return static_cast<PRBS *>(pattern[index].get())->create(PATT_DEFAULT_PRBS_ORDER);
    case 6: // PWM
        return static_cast<PWM *>(pattern[index].get())->create(PATT_DEFAULT_PWM_DUTY);
    case 7: // Line code
        return static_cast<LineCode *>(pattern[index].get())->create((int)LineCodeType::NRZ_L);
    case 8: // 4B/5B
        return static_cast<FourBFiveB *>(pattern[index].get())->create(8);
    case 9: // Johnson
        return static_cast<JohnsonNPhase *>(pattern[index].get())->create(PATT_DEFAULT_JOHNSON_PHASES);
    case 10: // PDM
        return static_cast<PDM *>(pattern[index].get())->create(PATT_DEFAULT_PDM_LEVEL);
    case 11: // Parallel bus
        return static_cast<ParallelBus *>(pattern[index].get())->create(PATT_DEFAULT_PARBUS_WIDTH);
    default:
        return pattern[index]->create(PATT_DEFAULT_DATA_LENGTH);
    }
}

QList<patttype> *PatternGeneratorPatterns::modifyPattern(int channel, int position)
{
    patttype rData = pattern[index]->getData()->at(position);
    patttype wData = rData ^ (1 << channel);
    pattern[index]->getData()->replace(position, wData);

    return pattern[index]->getData();
}

bool PatternGeneratorPatterns::isExponencial()
{
    return pattern[index]->isExponencial();
}

QList<patttype> *PatternGeneratorPatterns::setQuadratureSequence(bool seqAb)
{
    return static_cast<Quadrature *>(pattern[4].get())->setSequence(seqAb);
}

QList<patttype> *PatternGeneratorPatterns::setPrbsOrder(int order)
{
    ensure(5);
    return static_cast<PRBS *>(pattern[5].get())->setOrder(order);
}

QList<patttype> *PatternGeneratorPatterns::setPwmDuty(int duty)
{
    ensure(6);
    return static_cast<PWM *>(pattern[6].get())->setDuty(duty);
}

QList<patttype> *PatternGeneratorPatterns::setLineCodeType(LineCodeType type)
{
    ensure(7);
    return static_cast<LineCode *>(pattern[7].get())->setType(type);
}

QList<patttype> *PatternGeneratorPatterns::setFourBFiveBGroups(int groups)
{
    ensure(8);
    return static_cast<FourBFiveB *>(pattern[8].get())->setGroups(groups);
}

QList<patttype> *PatternGeneratorPatterns::setJohnsonPhases(int phases)
{
    ensure(9);
    return static_cast<JohnsonNPhase *>(pattern[9].get())->create(phases);
}

QList<patttype> *PatternGeneratorPatterns::setPdmLevel(int level)
{
    ensure(10);
    return static_cast<PDM *>(pattern[10].get())->create(level);
}

QList<patttype> *PatternGeneratorPatterns::setParBusWidth(int width)
{
    ensure(11);
    return static_cast<ParallelBus *>(pattern[11].get())->create(width);
}

// UART setters
QList<patttype> *PatternGeneratorPatterns::setUartBaud(int baud)
{
    ensure(12);
    return static_cast<UARTPattern *>(pattern[12].get())->setBaud(baud);
}
QList<patttype> *PatternGeneratorPatterns::setUartDataBits(int bits)
{
    ensure(12);
    return static_cast<UARTPattern *>(pattern[12].get())->setDataBits(bits);
}
QList<patttype> *PatternGeneratorPatterns::setUartParity(UartParity p)
{
    ensure(12);
    return static_cast<UARTPattern *>(pattern[12].get())->setParity(p);
}
QList<patttype> *PatternGeneratorPatterns::setUartStopBits(int stop)
{
    ensure(12);
    return static_cast<UARTPattern *>(pattern[12].get())->setStopBits(stop);
}
QList<patttype> *PatternGeneratorPatterns::setUartBitOrder(BitOrder order)
{
    ensure(12);
    return static_cast<UARTPattern *>(pattern[12].get())->setBitOrder(order);
}
QList<patttype> *PatternGeneratorPatterns::setUartIdle(UartIdleLevel idle)
{
    ensure(12);
    return static_cast<UARTPattern *>(pattern[12].get())->setIdle(idle);
}
QList<patttype> *PatternGeneratorPatterns::setUartFramingError(bool on)
{
    ensure(12);
    return static_cast<UARTPattern *>(pattern[12].get())->setFramingError(on);
}
QList<patttype> *PatternGeneratorPatterns::setUartBreak(bool on)
{
    ensure(12);
    return static_cast<UARTPattern *>(pattern[12].get())->setBreakCond(on);
}

// SPI setters
QList<patttype> *PatternGeneratorPatterns::setSpiClockHz(int hz)
{
    ensure(13);
    return static_cast<SPIPattern *>(pattern[13].get())->setClockHz(hz);
}
QList<patttype> *PatternGeneratorPatterns::setSpiMode(SpiMode m)
{
    ensure(13);
    return static_cast<SPIPattern *>(pattern[13].get())->setMode(m);
}
QList<patttype> *PatternGeneratorPatterns::setSpiWordSize(int bits)
{
    ensure(13);
    return static_cast<SPIPattern *>(pattern[13].get())->setWordSize(bits);
}
QList<patttype> *PatternGeneratorPatterns::setSpiBitOrder(BitOrder order)
{
    ensure(13);
    return static_cast<SPIPattern *>(pattern[13].get())->setBitOrder(order);
}
QList<patttype> *PatternGeneratorPatterns::setSpiCsGating(bool on)
{
    ensure(13);
    return static_cast<SPIPattern *>(pattern[13].get())->setCsGating(on);
}
QList<patttype> *PatternGeneratorPatterns::setSpiPauseTicks(int ticks)
{
    ensure(13);
    return static_cast<SPIPattern *>(pattern[13].get())->setPauseTicks(ticks);
}

// I2C setters
QList<patttype> *PatternGeneratorPatterns::setI2cSpeed(int hz)
{
    ensure(14);
    return static_cast<I2CPattern *>(pattern[14].get())->setSpeed(hz);
}
QList<patttype> *PatternGeneratorPatterns::setI2cAddrMode(I2cAddrMode m)
{
    ensure(14);
    return static_cast<I2CPattern *>(pattern[14].get())->setAddrMode(m);
}
QList<patttype> *PatternGeneratorPatterns::setI2cAddress(int addr)
{
    ensure(14);
    return static_cast<I2CPattern *>(pattern[14].get())->setAddress(addr);
}
QList<patttype> *PatternGeneratorPatterns::setI2cReadWrite(ModeRW rw)
{
    ensure(14);
    return static_cast<I2CPattern *>(pattern[14].get())->setReadWrite(rw);
}
QList<patttype> *PatternGeneratorPatterns::setI2cAck(bool ack)
{
    ensure(14);
    return static_cast<I2CPattern *>(pattern[14].get())->setAck(ack);
}
QList<patttype> *PatternGeneratorPatterns::setI2cStretch(bool on)
{
    ensure(14);
    return static_cast<I2CPattern *>(pattern[14].get())->setClockStretch(on);
}
QList<patttype> *PatternGeneratorPatterns::setI2cRepeatedStart(bool on)
{
    ensure(14);
    return static_cast<I2CPattern *>(pattern[14].get())->setRepeatedStart(on);
}

/****************************** CREATE PATTERN FUNS ******************************/

QList<patttype> *UserDefined::create(int len)
{
    data->clear();

    for (int i = 0; i < len; i++)
        data->append((0 | CLOCK_CH8) & CLOCK_MASK(i));

    return data;
}

QList<patttype> *CounterClock::create(int len)
{
    data->clear();

    patttype counterShift = 1;
    for (int i = 0; i < len; i++)
    {
        data->append(counterShift);
        counterShift <<= 1;
    }

    return data;
}

QList<patttype> *BinaryCode::create(int chanNum)
{
    data->clear();

    int val = 0;
    for (int i = 0; i < (1 << chanNum); i++)
    {
        data->append(val);
        val++;
    }

    return data;
}

QList<patttype> *GrayCode::create(int chanNum)
{
    data->clear();

    int val;
    for (int i = 0; i < (1 << chanNum); i++)
    {
        val = (i ^ (i >> 1));
        data->append(val);
    }

    return data;
}

QList<patttype> *Quadrature::create(int len)
{
    Q_UNUSED(len);
    data->clear();

    const int chan = 2;
    if (seqAb)
    {
        for (int i = 0; i < 2; i++)
        {
            data->append(0 | ((0 ^ 0) << chan));
            data->append(1 | ((1 ^ 0) << chan));
            data->append(3 | ((1 ^ 1) << chan));
            data->append(2 | ((0 ^ 1) << chan));
        }
    }
    else
    {
        for (int i = 0; i < 2; i++)
        {
            data->append(0 | (0 << chan));
            data->append(2 | (1 << chan));
            data->append(3 | (0 << chan));
            data->append(1 | (1 << chan));
        }
    }

    return data;
}

QList<patttype> *Quadrature::setSequence(bool seqAb)
{
    this->seqAb = seqAb;
    return create();
}

// PRBS using simple LFSR for given order. Taps chosen for common polynomials.
static inline quint32 prbs_next(quint32 state, int order)
{
    // Default taps for PRBS7/9/11/15/23/31
    quint32 taps = 0;
    switch (order)
    {
    case 7:
        taps = (1u << 6) | (1u << 5);
        break; // x^7 + x^6 + 1
    case 9:
        taps = (1u << 4) | (1u << 0);
        break; // x^9 + x^5 + 1 (approx)
    case 11:
        taps = (1u << 2) | (1u << 0);
        break; // x^11 + x^9 + 1 (approx)
    case 15:
        taps = (1u << 14) | (1u << 13);
        break; // x^15 + x^14 + 1
    case 23:
        taps = (1u << 22) | (1u << 17);
        break; // x^23 + x^18 + 1
    case 31:
        taps = (1u << 30) | (1u << 27);
        break; // x^31 + x^28 + 1
    default:
        taps = (1u << (order - 1)) | (1u << (order - 2));
        break;
    }
    quint32 feedback = 0;
    quint32 masked = state & taps;
    // parity of masked bits
    while (masked)
    {
        feedback ^= (masked & 1u);
        masked >>= 1;
    }
    quint32 out = (state >> 1) | (feedback << (order - 1));
    return out & ((1u << order) - 1);
}

QList<patttype> *PRBS::create(int order)
{
    this->order = order;
    data->clear();
    if (order < 2)
        order = 2;
    quint32 state = (1u << order) - 1;        // non-zero seed
    int len = qMin(64, 1 << qMin(order, 10)); // cap length for display
    for (int i = 0; i < len; i++)
    {
        // map bit stream to channel 0
        patttype v = (state & 1u) ? 0x01 : 0x00;
        data->append(v);
        state = prbs_next(state, order);
    }
    return data;
}

QList<patttype> *PRBS::setOrder(int order)
{
    return create(order);
}

QList<patttype> *PWM::create(int duty)
{
    this->duty = qBound(0, duty, 100);
    data->clear();
    const int period = 100; // 100 samples per cycle
    for (int i = 0; i < period; i++)
    {
        patttype v = (i < this->duty) ? 0x01 : 0x00;
        data->append(v);
    }
    return data;
}

QList<patttype> *PWM::setDuty(int duty)
{
    return create(duty);
}

QList<patttype> *LineCode::create(int type)
{
    this->type = static_cast<LineCodeType>(type);
    data->clear();
    // base data: 16-bit 10110010 01101001 pattern
    quint16 bits = 0b1011001001101001;
    auto push_bit = [&](int ch, bool high)
    { data->append(high ? (1 << ch) : 0); };
    switch (this->type)
    {
    case LineCodeType::NRZ_L:
        for (int i = 15; i >= 0; i--)
            push_bit(0, (bits >> i) & 1);
        break;
    case LineCodeType::RZ:
        for (int i = 15; i >= 0; i--)
        {
            bool b = (bits >> i) & 1;
            push_bit(0, b);
            push_bit(0, false);
        }
        break;
    case LineCodeType::Manchester:
    {
        bool last = false;
        for (int i = 15; i >= 0; i--)
        {
            bool b = (bits >> i) & 1;
            push_bit(0, b);
            push_bit(0, !b);
            last = !b;
            Q_UNUSED(last);
        }
        break;
    }
    case LineCodeType::Miller:
    {
        bool lastLevel = false;
        for (int i = 15; i >= 0; i--)
        {
            bool b = (bits >> i) & 1;
            if (!b)
                lastLevel = !lastLevel;
            push_bit(0, lastLevel);
        }
        break;
    }
    case LineCodeType::BiPhaseMark:
        for (int i = 15; i >= 0; i--)
        {
            bool b = (bits >> i) & 1;
            push_bit(0, b);
            push_bit(0, true);
        }
        break;
    case LineCodeType::BiPhaseSpace:
        for (int i = 15; i >= 0; i--)
        {
            bool b = (bits >> i) & 1;
            push_bit(0, b);
            push_bit(0, false);
        }
        break;
    }
    return data;
}

QList<patttype> *LineCode::setType(LineCodeType type)
{
    return create((int)type);
}

QList<patttype> *FourBFiveB::create(int groups)
{
    this->groups = qBound(1, groups, 32);
    data->clear();
    // map a simple nibble ramp to 5-bit codes (symbolic mapping)
    static const quint8 map[16] = {
        0b11110, 0b01001, 0b10100, 0b10101, 0b01010, 0b01011, 0b01110, 0b01111,
        0b10010, 0b10011, 0b10110, 0b10111, 0b11010, 0b11011, 0b11100, 0b11101};
    for (int g = 0; g < this->groups; ++g)
    {
        quint8 code = map[g % 16];
        for (int b = 4; b >= 0; --b)
        {
            patttype v = ((code >> b) & 1) ? 0x01 : 0x00;
            data->append(v);
        }
    }
    return data;
}

QList<patttype> *FourBFiveB::setGroups(int groups)
{
    return create(groups);
}

QList<patttype> *JohnsonNPhase::create(int phases)
{
    this->phases = qBound(2, phases, PATT_MAX_CHANNELS_NUM);
    data->clear();
    // Johnson counter pattern across channels: a walking ones and zeros
    int len = this->phases * 2;
    for (int i = 0; i < len; i++)
    {
        patttype v = 0;
        for (int p = 0; p < this->phases; p++)
        {
            bool high = (i >= p) && (i < p + this->phases);
            if (high)
                v |= (1 << p);
        }
        data->append(v);
    }
    return data;
}

QList<patttype> *PDM::create(int level)
{
    this->level = qBound(0, level, 100);
    data->clear();
    // Simple first-order sigma-delta tracking target level
    double target = this->level / 100.0;
    double acc = 0.0;
    const int N = 256;
    for (int i = 0; i < N; i++)
    {
        double out = acc >= target ? 0.0 : 1.0;
        data->append(out > 0.5 ? 0x01 : 0x00);
        acc = acc + target - out;
        if (acc < 0)
            acc = 0;
        if (acc > 1)
            acc = 1;
    }
    return data;
}

QList<patttype> *ParallelBus::create(int width)
{
    this->width = qBound(1, width, 16);
    data->clear();
    // Generate a simple counting pattern across the selected width on the lower bits
    int countLen = qMin(1 << qMin(this->width, 8), 64);
    for (int i = 0; i < countLen; i++)
    {
        data->append((patttype)(i & 0xFF));
    }
    return data;
}

// ---------------- UART -----------------
static inline void uart_append_bit(QList<patttype> *data, bool high)
{
    data->append(high ? 0x01 : 0x00); // channel 0 is TXD
}

QList<patttype> *UARTPattern::create(int)
{
    data->clear();
    // Compose a frame of one byte 0xA5 as demo
    auto append_frame = [&](quint8 byte)
    {
        bool idleHigh = (idleLevel == UartIdleLevel::High);
        // Start bit (opposite of idle)
        uart_append_bit(data, !idleHigh);
        // Data bits
        for (int i = 0; i < dataBits; ++i)
        {
            int bitIndex = (bitOrder == BitOrder::LSB_First) ? i : (dataBits - 1 - i);
            bool b = ((byte >> bitIndex) & 0x1) != 0;
            uart_append_bit(data, b ^ (idleHigh ? 0 : 1)); // if idle low, invert logic
        }
        // Parity
        if (parity != UartParity::None)
        {
            int ones = 0;
            for (int i = 0; i < dataBits; ++i)
                ones += ((byte >> i) & 1);
            bool par = (parity == UartParity::Even) ? ((ones % 2) == 0) : ((ones % 2) != 0);
            uart_append_bit(data, par ^ (idleHigh ? 0 : 1));
        }
        // Stop bits
        for (int s = 0; s < qMax(1, stopBits); ++s)
            uart_append_bit(data, idleHigh);
    };

    if (breakCond)
    {
        // hold low (opposite of idle high) for 12 bit times
        for (int i = 0; i < 12; ++i)
            uart_append_bit(data, (idleLevel == UartIdleLevel::High) ? false : true);
    }

    // Example two bytes
    append_frame(0xA5);
    append_frame(0x5A);

    if (framingError && !data->isEmpty())
    {
        // Flip last stop bit to emulate framing error
        patttype last = data->last();
        data->replace(data->size() - 1, last ^ 0x01);
    }

    return data;
}

QList<patttype> *UARTPattern::setBaud(int b)
{
    baud = b;
    return create();
}
QList<patttype> *UARTPattern::setDataBits(int b)
{
    dataBits = qBound(5, b, 9);
    return create();
}
QList<patttype> *UARTPattern::setParity(UartParity p)
{
    parity = p;
    return create();
}
QList<patttype> *UARTPattern::setStopBits(int s)
{
    stopBits = qBound(1, s, 2);
    return create();
}
QList<patttype> *UARTPattern::setBitOrder(BitOrder o)
{
    bitOrder = o;
    return create();
}
QList<patttype> *UARTPattern::setIdle(UartIdleLevel l)
{
    idleLevel = l;
    return create();
}
QList<patttype> *UARTPattern::setFramingError(bool on)
{
    framingError = on;
    return create();
}
QList<patttype> *UARTPattern::setBreakCond(bool on)
{
    breakCond = on;
    return create();
}

// ---------------- SPI -----------------
static inline void spi_append(QList<patttype> *data, bool sck, bool mosi, bool cs)
{
    patttype v = 0;
    if (mosi)
        v |= 0x01; // ch0 MOSI
    if (sck)
        v |= 0x02; // ch1 SCK
    if (!cs)
        v |= 0x04; // ch2 CS active low represented as high
    data->append(v);
}

QList<patttype> *SPIPattern::create(int)
{
    data->clear();
    // Emit one word 0xA5 for demo
    quint16 word = 0xA5;
    bool cpol = (mode == SpiMode::Mode2 || mode == SpiMode::Mode3);
    bool cpha = (mode == SpiMode::Mode1 || mode == SpiMode::Mode3);
    bool cs = true;
    int bits = qBound(4, wordSize, 16);

    auto getBit = [&](int i)
    { int bi = (bitOrder == BitOrder::MSB_First) ? (bits-1-i) : i; return (word >> bi) & 1; };

    if (csGating)
    {
        cs = false; // active
    }
    bool sck = cpol;
    // initial idle
    spi_append(data, sck, 0, cs);
    for (int i = 0; i < bits; ++i)
    {
        int bit = getBit(i);
        if (!cpha)
        {
            // sample on first edge, change on second
            sck = !sck;
            spi_append(data, sck, bit, cs);
            sck = !sck;
            spi_append(data, sck, bit, cs);
        }
        else
        {
            // change on first, sample on second
            sck = !sck;
            spi_append(data, sck, bit, cs);
            sck = !sck;
            spi_append(data, sck, bit, cs);
        }
    }
    if (csGating)
    {
        cs = true;
        spi_append(data, sck, 0, cs);
    }
    for (int p = 0; p < qMax(0, pauseTicks); ++p)
        spi_append(data, cpol, 0, true);
    return data;
}

QList<patttype> *SPIPattern::setClockHz(int hz)
{
    clockHz = hz;
    return create();
}
QList<patttype> *SPIPattern::setMode(SpiMode m)
{
    mode = m;
    return create();
}
QList<patttype> *SPIPattern::setWordSize(int b)
{
    wordSize = b;
    return create();
}
QList<patttype> *SPIPattern::setBitOrder(BitOrder o)
{
    bitOrder = o;
    return create();
}
QList<patttype> *SPIPattern::setCsGating(bool on)
{
    csGating = on;
    return create();
}
QList<patttype> *SPIPattern::setPauseTicks(int t)
{
    pauseTicks = t;
    return create();
}

// ---------------- I2C -----------------
static inline void i2c_append(QList<patttype> *data, bool scl, bool sda)
{
    patttype v = 0;
    if (scl)
        v |= 0x01; // ch0 SCL
    if (sda)
        v |= 0x02; // ch1 SDA
    data->append(v);
}

QList<patttype> *I2CPattern::create(int)
{
    data->clear();
    bool scl = true, sda = true; // idle high
    auto start = [&]()
    { sda = false; i2c_append(data, scl, sda); };
    auto stop = [&]()
    { sda = false; i2c_append(data, scl, sda); sda = true; i2c_append(data, scl, sda); };
    auto clock_bit = [&](int b)
    {
        sda = b;
        i2c_append(data, scl, sda);
        scl = false;
        i2c_append(data, scl, sda);
        scl = true;
        i2c_append(data, scl, sda);
    };
    auto ack_cycle = [&](bool a)
    {
        sda = !a; // ack = 0
        i2c_append(data, scl, sda);
        scl = false;
        i2c_append(data, scl, sda);
        scl = true;
        i2c_append(data, scl, sda);
    };

    start();
    // address
    int addrBits = (addrMode == I2cAddrMode::Addr10Bit) ? 10 : 7;
    int frame = (address & ((1 << addrBits) - 1));
    frame = (frame << 1) | ((rw == ModeRW::WRITE) ? 0 : 1);
    for (int i = addrBits; i >= 0; --i) // include R/W bit
        clock_bit((frame >> i) & 1);
    ack_cycle(ack);
    // one data byte 0xA5
    int byte = 0xA5;
    for (int i = 7; i >= 0; --i)
        clock_bit((byte >> i) & 1);
    ack_cycle(ack);
    if (repstart)
    {
        // repeated start then stop
        start();
    }
    stop();
    if (stretch)
    {
        // emulate stretch as extra SCL low segments
        for (int i = 0; i < 2; ++i)
        {
            scl = false;
            i2c_append(data, scl, sda);
        }
        scl = true;
        i2c_append(data, scl, sda);
    }
    return data;
}

QList<patttype> *I2CPattern::setSpeed(int hz)
{
    speed = hz;
    return create();
}
QList<patttype> *I2CPattern::setAddrMode(I2cAddrMode m)
{
    addrMode = m;
    return create();
}
QList<patttype> *I2CPattern::setAddress(int a)
{
    address = a;
    return create();
}
QList<patttype> *I2CPattern::setReadWrite(ModeRW r)
{
    rw = r;
    return create();
}
QList<patttype> *I2CPattern::setAck(bool a)
{
    ack = a;
    return create();
}
QList<patttype> *I2CPattern::setClockStretch(bool on)
{
    stretch = on;
    return create();
}
QList<patttype> *I2CPattern::setRepeatedStart(bool on)
{
    repstart = on;
    return create();
}
