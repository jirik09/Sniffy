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
    WidgetButtons *buttonSetDefault;

    WidgetSelection *comboPatternSelection;

    WidgetButtons *buttonUserDefLoadPattern;
    WidgetDialRange *dialUserDefFreq;

    WidgetDialRange *dialCounterFreq;

    WidgetDialRange *dialBinaryCodeFreq;

    WidgetDialRange *dialGrayCodeFreq;

    WidgetDialRange *dialQuadratureFreq;

    WidgetSelection *comboI2cClockFreq;
    WidgetSelection *comboI2cCommType;        

    void restoreSettingsAfterStartup(void);
    void enableGuiComponents(bool enable);
    qreal getFrequency();

private:
    PatternGeneratorConfig *config;

    QList<QString> pattList = {
        "User defined",
        "Counter",
        "Binary code",
        "Gray code",
        "Quadrature",
        "UART",
        "SPI",
        "I2C"
    };

    QWidget *patternArea[PATTERNS_NUM];

    void (PatternGeneratorSettings::*createPatternComponents[PATTERNS_NUM])(QWidget*,QVBoxLayout*,int) = {
            &PatternGeneratorSettings::createUserDefinedComponents,
            &PatternGeneratorSettings::createCounterComponents,
            &PatternGeneratorSettings::createBinaryCodeComponents,
            &PatternGeneratorSettings::createGrayCodeComponents,
            &PatternGeneratorSettings::createQuadratureComponents,
            &PatternGeneratorSettings::createUartComponents,
            &PatternGeneratorSettings::createSpiComponents,
            &PatternGeneratorSettings::createI2cComponents, };

    void createComponents(QWidget *parent, QVBoxLayout *destination);

    void createUserDefinedComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createCounterComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createBinaryCodeComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createGrayCodeComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createQuadratureComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createUartComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createSpiComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createI2cComponents(QWidget *parent, QVBoxLayout *destination, int index);

    WidgetDialRange *createDial(QWidget *parent, QString objName);

    void showComponents(int pattIndex, bool visible);

private slots:
    void patternSelectionChangedCallback(int index);

    void freqChangedDialsCallback(float val);
    void freqChangedCombosCallback(int index, float realVal);

signals:
    void patternSelectionChanged(int index);

};

#endif // PATTERNGENERATORSETTINGS_H
