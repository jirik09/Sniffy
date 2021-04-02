#ifndef NUMBERPARSER_H
#define NUMBERPARSER_H

#include <QObject>
#include <QString>
#include <QLocale>

class NumberParser : public QObject
{
    Q_OBJECT
public:
    explicit NumberParser(QObject *parent = nullptr);
    static qreal parse(QString  text, bool &success);
    static qreal parse(QString  text);

signals:

};

#endif // NUMBERPARSER_H
