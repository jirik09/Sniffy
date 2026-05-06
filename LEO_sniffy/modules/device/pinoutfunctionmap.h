#ifndef PINOUTFUNCTIONMAP_H
#define PINOUTFUNCTIONMAP_H

#include <QHash>
#include <QList>
#include <QString>

#include "../pinfunctioninfo.h"

struct PinoutBoardPin {
    QString port;
    QString arduino;
};

class PinoutFunctionMap
{
public:
    void setBoardPins(const QList<PinoutBoardPin> &pins);
    void setFunctions(const QList<PinFunctionInfo> &functions);
    void clear();

    const QList<PinFunctionInfo> &functions() const { return m_functions; }
    const PinFunctionInfo *pinFunction(const QString &port, const QString &arduino) const;

private:
    QString canonicalPinKey(const QString &pinName) const;

    QList<PinoutBoardPin> m_boardPins;
    QList<PinFunctionInfo> m_functions;
    QHash<QString, const PinFunctionInfo*> m_funcByPort;
    QHash<QString, const PinFunctionInfo*> m_funcByArduino;
};

#endif // PINOUTFUNCTIONMAP_H