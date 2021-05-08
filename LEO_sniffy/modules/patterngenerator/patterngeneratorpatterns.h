#ifndef PATTERNGENERATORPATTERNS_H
#define PATTERNGENERATORPATTERNS_H

#include <QObject>
#include <QVector>
#include <QSharedPointer>

#include "patterngeneratordefs.h"

typedef quint8 patttype;

//#define B1 (patttype)0x01
//#define B2 (patttype)0x02
//#define B3 (patttype)0x04
//#define B4 (patttype)0x08
//#define B5 (patttype)0x10
//#define B6 (patttype)0x20
//#define B7 (patttype)0x40
//#define B8 (patttype)0x80

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
