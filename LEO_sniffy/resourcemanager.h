#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QtGlobal>
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
    }

    // Subtract (bitwise AND NOT) masks from this set using r
    void subtract(const ResourceSet &r) {
        resources &= ~r.resources;
        gpioA &= ~r.gpioA;
        gpioB &= ~r.gpioB;
        gpioC &= ~r.gpioC;
        gpioD &= ~r.gpioD;
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
