#include "pinoutfunctionmap.h"

#include <QSet>

void PinoutFunctionMap::setBoardPins(const QList<PinoutBoardPin> &pins)
{
    m_boardPins = pins;
}

void PinoutFunctionMap::setFunctions(const QList<PinFunctionInfo> &functions)
{
    m_functions.clear();
    m_funcByPort.clear();
    m_funcByArduino.clear();

    QSet<QString> claimedPins;
    m_functions.reserve(functions.size());
    for(const PinFunctionInfo &function : functions){
        const QString canonicalPin = canonicalPinKey(function.pin);
        if(canonicalPin.isEmpty())
            continue;

        const QString claimKey = canonicalPin.toUpper();
        if(claimedPins.contains(claimKey))
            continue;

        claimedPins.insert(claimKey);
        PinFunctionInfo canonicalFunction = function;
        canonicalFunction.pin = canonicalPin;
        m_functions.append(canonicalFunction);
    }

    for(const PinFunctionInfo &function : m_functions){
        bool matchedBoardAlias = false;
        for(const PinoutBoardPin &pin : m_boardPins){
            if(pin.port.compare(function.pin, Qt::CaseInsensitive) != 0 &&
               pin.arduino.compare(function.pin, Qt::CaseInsensitive) != 0)
                continue;

            matchedBoardAlias = true;
            if(!pin.port.isEmpty())
                m_funcByPort.insert(pin.port, &function);
            if(!pin.arduino.isEmpty())
                m_funcByArduino.insert(pin.arduino, &function);
        }

        if(!matchedBoardAlias && !function.pin.isEmpty())
            m_funcByPort.insert(function.pin, &function);
    }
}

void PinoutFunctionMap::clear()
{
    m_functions.clear();
    m_funcByPort.clear();
    m_funcByArduino.clear();
}

const PinFunctionInfo *PinoutFunctionMap::pinFunction(const QString &port,
                                                      const QString &arduino) const
{
    if(!arduino.isEmpty()){
        auto arduinoIt = m_funcByArduino.find(arduino);
        if(arduinoIt != m_funcByArduino.end())
            return arduinoIt.value();
    }

    if(!port.isEmpty()){
        auto portIt = m_funcByPort.find(port);
        if(portIt != m_funcByPort.end())
            return portIt.value();
    }

    return nullptr;
}

QString PinoutFunctionMap::canonicalPinKey(const QString &pinName) const
{
    const QString normalizedPin = pinName.trimmed();
    if(normalizedPin.isEmpty())
        return QString();

    for(const PinoutBoardPin &pin : m_boardPins){
        if(pin.port.compare(normalizedPin, Qt::CaseInsensitive) == 0 ||
           pin.arduino.compare(normalizedPin, Qt::CaseInsensitive) == 0){
            if(!pin.port.isEmpty())
                return pin.port;
            if(!pin.arduino.isEmpty())
                return pin.arduino;
        }
    }

    return normalizedPin;
}