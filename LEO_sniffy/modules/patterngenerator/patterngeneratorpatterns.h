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
        // Protocol placeholders to keep indexes consistent
        []
        { return QSharedPointer<Pattern>(new UserDefined); }, // UART placeholder
        []
        { return QSharedPointer<Pattern>(new UserDefined); }, // SPI placeholder
        []
        { return QSharedPointer<Pattern>(new UserDefined); }, // I2C placeholder
        []
        { return QSharedPointer<Pattern>(new PRBS); },
        []
        { return QSharedPointer<Pattern>(new PWM); },
        []
        { return QSharedPointer<Pattern>(new LineCode); },
        []
        { return QSharedPointer<Pattern>(new FourBFiveB); },
        []
        { return QSharedPointer<Pattern>(new JohnsonNPhase); },
        []
        { return QSharedPointer<Pattern>(new PDM); },
        []
        { return QSharedPointer<Pattern>(new ParallelBus); },
    };
};

#endif // PATTERNGENERATORPATTERNS_H
