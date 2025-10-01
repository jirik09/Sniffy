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

QList<patttype> *PatternGeneratorPatterns::createPatternOrJustGetData(int index, int len)
{
    this->index = index;

    if (pattern[index] == nullptr)
    {
        pattern[index] = createPatternObj[index]();
            // Use sensible defaults per pattern on first creation
            switch(index){
            case 8: // PRBS
                static_cast<PRBS*>(pattern[index].get())->create(PATT_DEFAULT_PRBS_ORDER);
                break;
            case 9: // PWM
                static_cast<PWM*>(pattern[index].get())->create(PATT_DEFAULT_PWM_DUTY);
                break;
            case 10: // Line code
                static_cast<LineCode*>(pattern[index].get())->create((int)LineCodeType::NRZ_L);
                break;
            case 11: // 4B/5B
                static_cast<FourBFiveB*>(pattern[index].get())->create(8);
                break;
            case 12: // Johnson N-phase
                static_cast<JohnsonNPhase*>(pattern[index].get())->create(PATT_DEFAULT_JOHNSON_PHASES);
                break;
            case 13: // PDM
                static_cast<PDM*>(pattern[index].get())->create(PATT_DEFAULT_PDM_LEVEL);
                break;
            case 14: // Parallel bus
                static_cast<ParallelBus*>(pattern[index].get())->create(PATT_DEFAULT_PARBUS_WIDTH);
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
    case 8: // PRBS
        return static_cast<PRBS *>(pattern[index].get())->create(PATT_DEFAULT_PRBS_ORDER);
    case 9: // PWM
        return static_cast<PWM *>(pattern[index].get())->create(PATT_DEFAULT_PWM_DUTY);
    case 10: // Line code
        return static_cast<LineCode *>(pattern[index].get())->create((int)LineCodeType::NRZ_L);
    case 11: // 4B/5B
        return static_cast<FourBFiveB *>(pattern[index].get())->create(8);
    case 12: // Johnson
        return static_cast<JohnsonNPhase *>(pattern[index].get())->create(PATT_DEFAULT_JOHNSON_PHASES);
    case 13: // PDM
        return static_cast<PDM *>(pattern[index].get())->create(PATT_DEFAULT_PDM_LEVEL);
    case 14: // Parallel bus
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
    ensure(8);
    return static_cast<PRBS *>(pattern[8].get())->setOrder(order);
}

QList<patttype> *PatternGeneratorPatterns::setPwmDuty(int duty)
{
    ensure(9);
    return static_cast<PWM *>(pattern[9].get())->setDuty(duty);
}

QList<patttype> *PatternGeneratorPatterns::setLineCodeType(LineCodeType type)
{
    ensure(10);
    return static_cast<LineCode *>(pattern[10].get())->setType(type);
}

QList<patttype> *PatternGeneratorPatterns::setFourBFiveBGroups(int groups)
{
    ensure(11);
    return static_cast<FourBFiveB *>(pattern[11].get())->setGroups(groups);
}

QList<patttype> *PatternGeneratorPatterns::setJohnsonPhases(int phases)
{
    ensure(12);
    return static_cast<JohnsonNPhase *>(pattern[12].get())->create(phases);
}

QList<patttype> *PatternGeneratorPatterns::setPdmLevel(int level)
{
    ensure(13);
    return static_cast<PDM *>(pattern[13].get())->create(level);
}

QList<patttype> *PatternGeneratorPatterns::setParBusWidth(int width)
{
    ensure(14);
    return static_cast<ParallelBus *>(pattern[14].get())->create(width);
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
