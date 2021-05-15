#include "patterngeneratorpatterns.h"

Pattern::Pattern(QObject *parent) : QObject(parent){}

UserDefined::UserDefined(){ data = new QList<patttype>; create(8); }
CounterClock::CounterClock(){ data = new QList<patttype>; create(8); }
BinaryCode::BinaryCode(){ data = new QList<patttype>; create(4); }
GrayCode::GrayCode(){ data = new QList<patttype>; create(4); }
Quadrature::Quadrature(){ data = new QList<patttype>; create(8); }

QList<patttype> *UserDefined::getData(){ return data; }
QList<patttype> *CounterClock::getData(){ return data; }
QList<patttype> *BinaryCode::getData(){ return data; }
QList<patttype> *GrayCode::getData(){ return data; }
QList<patttype> *Quadrature::getData(){ return data; }

PatternGeneratorPatterns::PatternGeneratorPatterns(QObject *parent) : QObject(parent)
{
    for(int i = 0; i < PATTERNS_NUM; i++)
        pattern[i] = nullptr;
}

QList<patttype> *PatternGeneratorPatterns::getData(int index)
{
    this->index = index;

    if(pattern[index] == nullptr)
        pattern[index] = createPattern[index]();

    return pattern[index]->getData();
}

void PatternGeneratorPatterns::modifyPattern(int channel, int position, bool level)
{
    patttype rData = pattern[index]->getData()->at(position);

    patttype wData;
    if(level)
        wData = rData | (1<<channel);
    else
        wData = rData & (patttype)~(1<<channel);

    pattern[index]->getData()->replace(position, wData);
}

QList<patttype> *UserDefined::create(int len)
{        
    for(int i = 0; i < len; i++)
        data->append(0 & CLOCK_MASK(i));

    return data;
}

QList<patttype> *CounterClock::create(int len)
{
    patttype counterShift = 1;
    for(int i = 0; i < len; i++){
        data->append((counterShift | CLOCK_CH8) & CLOCK_MASK(i));
        counterShift <<= 1;
    }

    return data;
}

QList<patttype> *BinaryCode::create(int chanNum)
{
    int i = 0;
    for(patttype val = 0; val < (chanNum*chanNum); val++){
        data->append((val | CLOCK_CH8) & CLOCK_MASK(i));
        i++;
    }

    return data;
}

QList<patttype> *GrayCode::create(int chanNum)
{
    int i = 0;
    for(patttype val = 0; val < (chanNum*chanNum); val++){
        val ^= (val >> 1);
        data->append((val | CLOCK_CH8) & CLOCK_MASK(i));
        i++;
    }

    return data;
}

QList<patttype> *Quadrature::create(int len)
{


    return data;
}
