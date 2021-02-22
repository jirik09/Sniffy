#ifndef ABSTRACTSPECIFICATION_H
#define ABSTRACTSPECIFICATION_H

#include <QObject>

class AbstractSpecification : public QObject
{
    Q_OBJECT
public:
    explicit AbstractSpecification(QObject *parent = nullptr);

    virtual void parseSpecification(QByteArray spec) = 0;
    int getResources();
    bool isLoaded() const;

protected:
    int resources = 0;
    bool isSpecificationLoaded = false;

};

#endif // ABSTRACTSPECIFICATION_H
