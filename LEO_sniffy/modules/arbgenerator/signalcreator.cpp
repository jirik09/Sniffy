#include "signalcreator.h"

SignalCreator::SignalCreator(QObject *parent) : QObject(parent)
{

}

QList<int> SignalCreator::createSignal(SignalShape shape, int numSamples, qreal amplitude, qreal offset, qreal duty, qreal phase, int resolution)
{
    //todo
}
