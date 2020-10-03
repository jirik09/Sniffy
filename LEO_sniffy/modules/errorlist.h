#ifndef ERRORLIST_H
#define ERRORLIST_H

#include <QObject>

class ErrorList : public QObject
{

    Q_OBJECT
public:
    explicit ErrorList(QObject *parent = nullptr);
    static QString GetErrMessage(uint value);

signals:

};

#endif // ERRORLIST_H
