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

#include "patterngeneratorspec.h"
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
    WidgetDialRange *dialUserDefLength;

    WidgetDialRange *dialCounterFreq;    
    WidgetDialRange *dialCounterLength;

    WidgetDialRange *dialBinaryCodeFreq;    
    WidgetDialRange *dialBinaryChanNum;

    WidgetDialRange *dialGrayCodeFreq;
    WidgetDialRange *dialGrayCodeChanNum;

    WidgetDialRange *dialQuadratureFreq;
    WidgetSelection *comboQuadratureSeqAbba;

    WidgetSelection *comboI2cClockFreq;
    WidgetSelection *comboI2cCommType;        

    void restoreSettingsAfterStartup(void);
    void setSpecification(PatternGeneratorSpec* spec);
    void enableGuiComponents(bool enable);
    void showComponents(int pattIndex, bool visible);

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

    void (PatternGeneratorSettings::*resetPatternComponents[PATTERNS_NUM])() = {
            &PatternGeneratorSettings::resetUserDefinedComponents,
            &PatternGeneratorSettings::resetCounterComponents,
            &PatternGeneratorSettings::resetBinaryCodeComponents,
            &PatternGeneratorSettings::resetGrayCodeComponents,
            &PatternGeneratorSettings::resetQuadratureComponents,
            &PatternGeneratorSettings::resetUartComponents,
            &PatternGeneratorSettings::resetSpiComponents,
            &PatternGeneratorSettings::resetI2cComponents, };

    void createComponents(QWidget *parent, QVBoxLayout *destination);

    void createUserDefinedComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createCounterComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createBinaryCodeComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createGrayCodeComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createQuadratureComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createUartComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createSpiComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createI2cComponents(QWidget *parent, QVBoxLayout *destination, int index);

    WidgetDialRange *createFrequencyDial(QWidget *parent, QString objName);

    void resetUserDefinedComponents();
    void resetCounterComponents();
    void resetBinaryCodeComponents();
    void resetGrayCodeComponents();
    void resetQuadratureComponents();
    void resetUartComponents();
    void resetSpiComponents();
    void resetI2cComponents();

    void resetFreqAndDataLenDials(WidgetDialRange *freqDial, WidgetDialRange *dataLenDial);

signals:
    void patternSelectionChanged(int index);

private slots:
    void resetPatternCallback(int index);
};

#endif // PATTERNGENERATORSETTINGS_H
