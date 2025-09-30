#ifndef NUMBERPARSER_H
#define NUMBERPARSER_H

#include <QObject>
#include <QString>
#include <QLocale>
#include <QDebug>
#include <math.h>

class NumberParser : public QObject
{
public:
    explicit NumberParser(QObject *parent = nullptr);
    static qreal parse(QString  text, bool &success);
    static qreal parse(QString  text);

    static qreal getNiceNumber (qreal number, int validDigits);
    static qreal roundToFive(qreal number);

signals:

};

#endif // NUMBERPARSER_H
