#ifndef SCOPE_H
#define SCOPE_H

#include <QObject>
#include <QDebug>

#include "scopeconfig.h"
#include "scopespec.h"

class Scope : public QObject
{
    Q_OBJECT
public:
    explicit Scope(QObject *parent = nullptr);

signals:

public slots:
    void parseData(QByteArray);

};

#endif // SCOPE_H
