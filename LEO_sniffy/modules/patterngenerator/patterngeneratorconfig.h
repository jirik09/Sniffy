#ifndef PATTERNGENERATORCONFIG_H
#define PATTERNGENERATORCONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"
#include "patterngeneratordefs.h"

class PatternGeneratorConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit PatternGeneratorConfig(QObject *parent = nullptr);

    void parse (QByteArray config);
    QByteArray serialize ();

//    enum class Pattern { USER_DEFINED = 0,
//                         COUNTER,
//                         BINARY_CODE,
//                         GREY_CODE,
//                         QUADRATURE,
//                         UART,
//                         SPI,
//                         I2C
//                       } pattern;

    enum class State { STOPPED = 0,
                       RUNNING = 1,
                       UPLOADING = 2
                     } state;

    int pattIndex = 0;

    qreal freq[PATTERNS_NUM];
};

#endif // PATTERNGENERATORCONFIG_H
