#ifndef ABSTRACTSPECIFICATION_H
#define ABSTRACTSPECIFICATION_H

#include <QObject>

class AbstractSpecification : public QObject
{
    Q_OBJECT
public:
    explicit AbstractSpecification(QObject *parent = nullptr);

    virtual void parseSpecification(QByteArray spec) = 0;
    bool isLoaded() const;

protected:
    bool isSpecificationLoaded = false;

};

#endif // ABSTRACTSPECIFICATION_H
