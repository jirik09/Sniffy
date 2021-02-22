#include "voltmeterspec.h"

VoltmeterSpec::VoltmeterSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void VoltmeterSpec::parseSpecification(QByteArray spec)
{
    QDataStream stream(spec);

    stream >> resources;

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

}
