#ifndef ABSTRACTSPECIFICATION_H
#define ABSTRACTSPECIFICATION_H

#include <QObject>
#include <QtGlobal>

class AbstractSpecification : public QObject
{
public:
    explicit AbstractSpecification(QObject *parent = nullptr);

    virtual void parseSpecification(QByteArray spec) = 0;
    int getResources();
    bool isLoaded() const;

protected:
    int resources = 0;
    bool isSpecificationLoaded = false;

    // GPIO occupancy bitmasks per port (A-D) provided by firmware at the end of spec stream
    quint32 gpioMaskA = 0;
    quint32 gpioMaskB = 0;
    quint32 gpioMaskC = 0;
    quint32 gpioMaskD = 0;

public:
    // Accessors for GPIO masks
    quint32 getGpioMaskA() const { return gpioMaskA; }
    quint32 getGpioMaskB() const { return gpioMaskB; }
    quint32 getGpioMaskC() const { return gpioMaskC; }
    quint32 getGpioMaskD() const { return gpioMaskD; }

protected:
    // Setter usable from derived classes after parsing
    void setGpioMasks(quint32 a, quint32 b, quint32 c, quint32 d) { gpioMaskA = a; gpioMaskB = b; gpioMaskC = c; gpioMaskD = d; }

};

#endif // ABSTRACTSPECIFICATION_H
