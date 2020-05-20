#include "scope.h"

Scope::Scope(QObject *parent) : QObject(parent)
{

}

void Scope::parseData(QByteArray data){
    qDebug() << "data are in scope parser" << data;

}
