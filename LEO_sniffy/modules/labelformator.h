#ifndef LABELFORMATOR_H
#define LABELFORMATOR_H

#include <QObject>

class LabelFormator : public QObject
{
    Q_OBJECT
public:
    explicit LabelFormator(QObject *parent = nullptr);
    static QString formatOutout(qreal value, QString unit, int precision);
    static QString formatOutout(qreal value, QString unit);

signals:

};

#endif // LABELFORMATOR_H
