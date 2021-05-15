#ifndef PATTERNGENERATORPATTERNS_H
#define PATTERNGENERATORPATTERNS_H

#include <QObject>
#include <QVector>
#include <QSharedPointer>

#include "patterngeneratordefs.h"

typedef quint8 patttype;

#define CLOCK_MASK(i) (patttype)((i%2) ? 0x7F : 0xFF)
#define CLOCK_CH8 0x80

class Pattern : public QObject
{
    Q_OBJECT
public:
    explicit Pattern(QObject *parent = nullptr);
    virtual QList<patttype> *getData() = 0;
};

class UserDefined : public Pattern
{
    Q_OBJECT
public:
    explicit UserDefined();
    QList<patttype> *getData() override;
private:
    QList<patttype> *data;
    QList<patttype> *create(int len);
};

class CounterClock : public Pattern
{
    Q_OBJECT
public:
    explicit CounterClock();
    QList<patttype> *getData() override;
private:
    QList<patttype> *data;
    QList<patttype> *create(int len);
};

class BinaryCode : public Pattern
{
    Q_OBJECT
public:
    explicit BinaryCode();
    QList<patttype> *getData() override;
private:
    QList<patttype> *data;
    QList<patttype> *create(int chanNum);
};

class GrayCode : public Pattern
{
    Q_OBJECT
public:
    explicit GrayCode();
    QList<patttype> *getData() override;
private:
    QList<patttype> *data;
    QList<patttype> *create(int chanNum);
};

class Quadrature : public Pattern
{
    Q_OBJECT
public:
    explicit Quadrature();
    QList<patttype> *getData() override;
private:
    QList<patttype> *data;
    QList<patttype> *create(int len);
};

class PatternGeneratorPatterns : public QObject
{
    Q_OBJECT
public:
    explicit PatternGeneratorPatterns(QObject *parent = nullptr);

    QList<patttype> *getData(int index);
    void modifyPattern(int channel, int position, bool level);

private:
    int index;
    QSharedPointer<Pattern> pattern[PATTERNS_NUM];

    QVector<std::function<QSharedPointer<Pattern>()>> createPattern = {
            [] { return QSharedPointer<Pattern>(new UserDefined); },
            [] { return QSharedPointer<Pattern>(new CounterClock); },
            [] { return QSharedPointer<Pattern>(new BinaryCode); },
            [] { return QSharedPointer<Pattern>(new GrayCode); },
            [] { return QSharedPointer<Pattern>(new Quadrature); },
    };
};

#endif // PATTERNGENERATORPATTERNS_H
