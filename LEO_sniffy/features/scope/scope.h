#ifndef SCOPE_H
#define SCOPE_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include <QPointF>

#include "scopeconfig.h"
#include "scopespec.h"

class Scope : public QObject
{
    Q_OBJECT
public:
    explicit Scope(QObject *parent = nullptr);

signals:
    void scopeSpecified();
    void send(QByteArray);
    void scopeDataReceived(QVector<QVector<QPointF>>);

public slots:
    void parseData(QByteArray);
    void stopScope();
    void startScope();
    void restartScope();
    void sendData(QByteArray);

private:
    QVector<QVector<QPointF>> scopeData;

};

#endif // SCOPE_H
