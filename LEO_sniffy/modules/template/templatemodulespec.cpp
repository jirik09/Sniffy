#include "templatemodulespec.h"

TemplateModuleSpec::TemplateModuleSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void TemplateModuleSpec::parseSpecification(QByteArray spec)
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

}
