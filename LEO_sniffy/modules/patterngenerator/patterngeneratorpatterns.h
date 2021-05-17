#ifndef PATTERNGENERATORPATTERNS_H
#define PATTERNGENERATORPATTERNS_H

#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include <math.h>

#include "patterngeneratordefs.h"
#include "patterngeneratorconfig.h"

typedef quint8 patttype;

#define CLOCK_MASK(i) (patttype)((i%2) ? 0x7F : 0xFF)
#define CLOCK_CH8 0x80

class Pattern : public QObject
{
    Q_OBJECT
public:
    explicit Pattern(QObject *parent = nullptr);
    virtual QList<patttype> *getData() = 0;
    virtual QList<patttype> *create(int) = 0;
    virtual  bool isExponencial() = 0;
};

class UserDefined : public Pattern
{
    Q_OBJECT
public:
    explicit UserDefined();
    QList<patttype> *getData() override;
    QList<patttype> *create(int len) override;
    bool  isExponencial() override;
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
    const bool exp = false;
    QList<patttype> *getData() override;
    QList<patttype> *create(int len) override;
    bool isExponencial() override;
private:
    QList<patttype> *data;
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

private:    
    int index;
    QSharedPointer<Pattern> pattern[PATTERNS_NUM];

    QVector<std::function<QSharedPointer<Pattern>()>> createPatternObj = {
            [] { return QSharedPointer<Pattern>(new UserDefined); },
            [] { return QSharedPointer<Pattern>(new CounterClock); },
            [] { return QSharedPointer<Pattern>(new BinaryCode); },
            [] { return QSharedPointer<Pattern>(new GrayCode); },
            [] { return QSharedPointer<Pattern>(new Quadrature); },
    };
};

#endif // PATTERNGENERATORPATTERNS_H
