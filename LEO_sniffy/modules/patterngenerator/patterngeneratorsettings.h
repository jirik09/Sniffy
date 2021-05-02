#ifndef PATTERNGENERATORSETTINGS_H
#define PATTERNGENERATORSETTINGS_H

#include <QObject>
#include <QComboBox>
#include <QVBoxLayout>
#include <QMap>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetdialrange.h"

#include "patterngeneratorconfig.h"
#include "patterngeneratordefs.h"

class PatternGeneratorSettings : public QObject
{
    Q_OBJECT
public:
    explicit PatternGeneratorSettings(QVBoxLayout *destination, PatternGeneratorConfig *config, QWidget *parent = nullptr);    

    WidgetButtons *buttonStart;

    WidgetSelection *comboPatternSelection;

    WidgetButtons *buttonUserDefLoad;

    WidgetDialRange *dialCounterFreq;

    WidgetDialRange *dialBinaryCodeFreq;

    WidgetDialRange *dialGreyCodeFreq;

    WidgetSelection *comboI2cClockFreq;
    WidgetSelection *comboI2cCommType;

    void restoreSettingsPanel(void);
    void enableComponents(bool enable);

private:
    PatternGeneratorConfig *config;    

    QList<QString> pattList = {
        "User defined",
        "Counter",
        "Binary code",
        "Grey code",
        "Quadrature",
        "UART",
        "SPI",
        "I2C"
    };

    QWidget *patternArea[PATTERNS_NUM];

    void (PatternGeneratorSettings::*createPatternComponents[PATTERNS_NUM])(QWidget*,QVBoxLayout*) = {
            &PatternGeneratorSettings::createUserDefinedComponents,
            &PatternGeneratorSettings::createCounterComponents,
            &PatternGeneratorSettings::createBinaryCodeComponents,
            &PatternGeneratorSettings::createGreyCodeComponents,
            &PatternGeneratorSettings::createQuadratureComponents,
            &PatternGeneratorSettings::createUartComponents,
            &PatternGeneratorSettings::createSpiComponents,
            &PatternGeneratorSettings::createI2cComponents,   };

    void createComponents(QWidget *parent, QVBoxLayout *destination);

    void createUserDefinedComponents(QWidget *parent, QVBoxLayout *destination);
    void createCounterComponents(QWidget *parent, QVBoxLayout *destination);
    void createBinaryCodeComponents(QWidget *parent, QVBoxLayout *destination);
    void createGreyCodeComponents(QWidget *parent, QVBoxLayout *destination);
    void createQuadratureComponents(QWidget *parent, QVBoxLayout *destination);
    void createUartComponents(QWidget *parent, QVBoxLayout *destination);
    void createSpiComponents(QWidget *parent, QVBoxLayout *destination);
    void createI2cComponents(QWidget *parent, QVBoxLayout *destination);

    void showComponents(int pattIndex, bool visible);

private slots:
    void patternSelectionChangedCallback(int index);

signals:

};

#endif // PATTERNGENERATORSETTINGS_H
