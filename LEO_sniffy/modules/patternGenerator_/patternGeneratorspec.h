#ifndef TEMPLATEMODULESPEC_H
#define TEMPLATEMODULESPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"

class TemplateModuleSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit TemplateModuleSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int specificationVariable;

};

#endif // TEMPLATEMODULESPEC_H
