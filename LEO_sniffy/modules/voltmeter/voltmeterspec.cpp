#include "voltmeterspec.h"
#include "modules/specutils.h"

VoltmeterSpec::VoltmeterSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void VoltmeterSpec::parseSpecification(QByteArray spec)
{
    QDataStream stream(spec);

    int tmpVref;
    int tmpVrefInt;

    stream >> resources >> maxSamplingRate12B >> maxSamplingRate8BInterleaved ;
    stream >> memorySize>> maxADCChannels >> tmpVref >> tmpVrefInt;

    Vref = tmpVref;
    VrefCalibration = tmpVrefInt;

    SpecParsing::readPins4(stream, maxADCChannels, [&](int i, const QString &pin){ channelPins[i] = pin; });

}
