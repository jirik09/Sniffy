#include "patterngeneratorpatterns.h"

/****************************** CONSTRUCTORS START :D ******************************/
Pattern::Pattern(QObject *parent) : QObject(parent){}

UserDefined::UserDefined(){ data = new QList<patttype>; }
CounterClock::CounterClock(){ data = new QList<patttype>; }
BinaryCode::BinaryCode(){ data = new QList<patttype>; }
GrayCode::GrayCode(){ data = new QList<patttype>; }
Quadrature::Quadrature(){ data = new QList<patttype>; }

PatternGeneratorPatterns::PatternGeneratorPatterns(QObject *parent) : QObject(parent)
{
    for(int i = 0; i < PATTERNS_NUM; i++)
        pattern[i] = nullptr;
}
/****************************** CONSTRUCTORS END :D ******************************/

QList<patttype> *UserDefined::getData(){ return data; }
QList<patttype> *CounterClock::getData(){ return data; }
QList<patttype> *BinaryCode::getData(){ return data; }
QList<patttype> *GrayCode::getData(){ return data; }
QList<patttype> *Quadrature::getData(){ return data; }

QList<patttype> *PatternGeneratorPatterns::createPatternOrJustGetData(int index, int len)
{
    this->index = index;

    if(pattern[index] == nullptr){
        pattern[index] = createPatternObj[index]();
        pattern[index]->create(len);
    }

    return pattern[index]->getData();
}

QList<patttype> *PatternGeneratorPatterns::setDataLen(int index, int len)
{
    return pattern[index]->create(len);
}

QList<patttype> *PatternGeneratorPatterns::setDefault(int index)
{
    return pattern[index]->create(PATT_DEFAULT_DATA_LENGTH);
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
    data->clear();

    for(int i = 0; i < len; i++)
        data->append((0 | CLOCK_CH8) & CLOCK_MASK(i));

    return data;
}

QList<patttype> *CounterClock::create(int len)
{
    data->clear();

    patttype counterShift = 1;
    for(int i = 0; i < len; i++){
        data->append(counterShift);
        counterShift <<= 1;
    }

    return data;
}

QList<patttype> *BinaryCode::create(int chanNum)
{
    data->clear();

    int val = 0;
    for(int i = 0; i < (1 << chanNum); i++){
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
    data->clear();

    return data;
}
