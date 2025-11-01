#include "pwmgeneratorspec.h"

PWMGeneratorSpec::PWMGeneratorSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void PWMGeneratorSpec::parseSpecification(QByteArray spec)
{
    QDataStream stream(spec);

    stream >> specificationVariable;

    //TODO parse spec into public variables
    //example:
    /*
    stream >> resources >> maxSamplingRate12B ;
    char chars[4] = "";
    for(int i = 0; i < maxADCChannels; i++){
        stream.readRawData(chars, 4);
        channelPins[i] = QString(chars);
        channelPins[i].remove('_');
    }
    */

    // New: read 4x GPIO masks (A,B,C,D) appended by firmware
    quint32 gpioA = 0, gpioB = 0, gpioC = 0, gpioD = 0;
    stream >> gpioA >> gpioB >> gpioC >> gpioD;
    setGpioMasks(gpioA, gpioB, gpioC, gpioD);

}
