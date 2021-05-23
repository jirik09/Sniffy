#ifndef PATTERNGENERATORSETTINGS_H
#define PATTERNGENERATORSETTINGS_H

#include <QObject>

class PatternGeneratorSettings : public QObject
{
    Q_OBJECT
public:
    explicit PatternGeneratorSettings(QObject *parent = nullptr);

signals:

};

#endif // PATTERNGENERATORSETTINGS_H
