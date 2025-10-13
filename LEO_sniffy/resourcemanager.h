#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QtGlobal>
#include <QJsonObject>
#include "modules/abstractmodule.h"

struct ResourceSet {
    int resources = 0;
    quint32 gpioA = 0;
    quint32 gpioB = 0;
    quint32 gpioC = 0;
    quint32 gpioD = 0;

    // Returns true if any resource or GPIO mask collides between the two sets
    static bool collide(const ResourceSet &a, const ResourceSet &b) {
        if (a.resources & b.resources) return true;
        if (a.gpioA & b.gpioA) return true;
        if (a.gpioB & b.gpioB) return true;
        if (a.gpioC & b.gpioC) return true;
        if (a.gpioD & b.gpioD) return true;
        return false;
    }

    // Merge (bitwise OR) masks from r into this set
    void mergeWith(const ResourceSet &r) {
        resources |= r.resources;
        gpioA |= r.gpioA;
        gpioB |= r.gpioB;
        gpioC |= r.gpioC;
        gpioD |= r.gpioD;
        qDebug() << "resource merge " << QString::number(resources,16)<<" "<< QString::number(gpioA,16)<<" "<< QString::number(gpioB,16)<<" "<< QString::number(gpioC,16)<<" "<< QString::number(gpioD,16);
    }

    // Subtract (bitwise AND NOT) masks from this set using r
    void subtract(const ResourceSet &r) {
        resources &= ~r.resources;
        qDebug() << "resource subtract " << QString::number(resources,16)<<" "<< QString::number(gpioA,16)<<" "<< QString::number(gpioB,16)<<" "<< QString::number(gpioC,16)<<" "<< QString::number(gpioD,16);
        gpioA &= ~r.gpioA;
        gpioB &= ~r.gpioB;
        gpioC &= ~r.gpioC;
        gpioD &= ~r.gpioD;
    }

    // JSON serialization methods
    QJsonObject toJson() const{
        QJsonObject obj;
        obj["resources"] = resources;
        obj["gpioA"] = static_cast<qint64>(gpioA);
        obj["gpioB"] = static_cast<qint64>(gpioB);
        obj["gpioC"] = static_cast<qint64>(gpioC);
        obj["gpioD"] = static_cast<qint64>(gpioD);
        return obj;
    }
    static ResourceSet fromJson(const QJsonObject &obj){
    ResourceSet s;
    s.resources = obj["resources"].toInt();
    s.gpioA = static_cast<quint32>(obj["gpioA"].toDouble());
    s.gpioB = static_cast<quint32>(obj["gpioB"].toDouble());
    s.gpioC = static_cast<quint32>(obj["gpioC"].toDouble());
    s.gpioD = static_cast<quint32>(obj["gpioD"].toDouble());
    return s;
}

    // Build ResourceSet from a module (optionally override resources)
    static ResourceSet fromModule(const QSharedPointer<AbstractModule> &module, int resourcesOverride = -1) {
        return fromModule(module.data(), resourcesOverride);
    }

    static ResourceSet fromModule(AbstractModule *module, int resourcesOverride = -1) {
        ResourceSet s;
        if (module) {
            s.resources = (resourcesOverride >= 0) ? resourcesOverride : module->getResources();
            s.gpioA = module->getGpioMaskA();
            s.gpioB = module->getGpioMaskB();
            s.gpioC = module->getGpioMaskC();
            s.gpioD = module->getGpioMaskD();
        } else {
            s.resources = (resourcesOverride >= 0) ? resourcesOverride : 0;
        }
        return s;
    }
};

class ResourceManager {
public:
    void reset() { inUse = {}; }
    void reserve(const ResourceSet &r) { inUse.mergeWith(r); }
    void release(const ResourceSet &r) { inUse.subtract(r); }
    bool conflicts(const ResourceSet &r) const { return ResourceSet::collide(inUse, r); }
    const ResourceSet &reserved() const { return inUse; }

private:
    ResourceSet inUse;
};

#endif // RESOURCEMANAGER_H
