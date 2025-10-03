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

    WidgetButtons *buttonStart = nullptr;
    WidgetButtons *buttonSetDefault = nullptr;

    WidgetSelection *comboPatternSelection = nullptr;

    WidgetButtons *buttonUserDefLoadPattern = nullptr;
    WidgetDialRange *dialUserDefFreq = nullptr;
    WidgetDialRange *dialUserDefLength = nullptr;

    WidgetDialRange *dialCounterFreq = nullptr;
    WidgetDialRange *dialCounterLength = nullptr;

    WidgetDialRange *dialBinaryCodeFreq = nullptr;
    WidgetDialRange *dialBinaryChanNum = nullptr;

    WidgetDialRange *dialGrayCodeFreq = nullptr;
    WidgetDialRange *dialGrayCodeChanNum = nullptr;

    WidgetDialRange *dialQuadratureFreq = nullptr;
    WidgetSelection *comboQuadratureSeqAbba = nullptr;

    // I2C controls (expanded later)
    WidgetSelection *comboI2cClockFreq = nullptr;
    WidgetSelection *comboI2cCommType = nullptr;
    WidgetSelection *comboI2cAddrMode = nullptr;
    WidgetDialRange *dialI2cAddress = nullptr;
    WidgetSelection *comboI2cAck = nullptr;
    WidgetSelection *comboI2cStretch = nullptr;
    WidgetSelection *comboI2cRepStart = nullptr;

    // New pattern controls
    WidgetDialRange *dialPrbsFreq = nullptr;
    WidgetSelection *comboPrbsOrder = nullptr;

    WidgetDialRange *dialPwmFreq = nullptr;
    WidgetDialRange *dialPwmDuty = nullptr;

    WidgetDialRange *dialLineCodeFreq = nullptr;
    WidgetSelection *comboLineCodeType = nullptr;

    WidgetDialRange *dial4b5bFreq = nullptr;
    WidgetDialRange *dial4b5bGroups = nullptr;

    WidgetDialRange *dialJohnsonFreq = nullptr;
    WidgetDialRange *dialJohnsonPhases = nullptr;

    WidgetDialRange *dialPdmFreq = nullptr;
    WidgetDialRange *dialPdmLevel = nullptr;

    WidgetDialRange *dialParBusFreq = nullptr;
    WidgetDialRange *dialParBusWidth = nullptr;

    // UART controls
    WidgetSelection *comboUartBaud = nullptr;
    WidgetDialRange *dialUartDataBits = nullptr;
    WidgetSelection *comboUartParity = nullptr;
    WidgetSelection *comboUartStopBits = nullptr;
    WidgetSelection *comboUartBitOrder = nullptr;
    WidgetSelection *comboUartIdle = nullptr;
    WidgetSelection *comboUartFramingErr = nullptr;
    WidgetSelection *comboUartBreak = nullptr;

    // SPI controls
    WidgetDialRange *dialSpiClockFreq = nullptr;
    WidgetSelection *comboSpiMode = nullptr;
    WidgetDialRange *dialSpiWordSize = nullptr;
    WidgetSelection *comboSpiBitOrder = nullptr;
    WidgetSelection *comboSpiCsGating = nullptr;
    WidgetDialRange *dialSpiPauseTicks = nullptr;

    void restoreSettingsAfterStartup(void);
    void setSpecification(PatternGeneratorSpec *spec);
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
        "PRBS",
        "PWM",
        "Line code",
        "4B/5B",
        "Johnson N-phase",
        "PDM",
        "Parallel bus",
        // Protocols
        "UART",
        "SPI",
        "I2C"};

    QWidget *patternArea[PATTERNS_NUM];

    void (PatternGeneratorSettings::*createPatternComponents[PATTERNS_NUM])(QWidget *, QVBoxLayout *, int) = {
        &PatternGeneratorSettings::createUserDefinedComponents,
        &PatternGeneratorSettings::createCounterComponents,
        &PatternGeneratorSettings::createBinaryCodeComponents,
        &PatternGeneratorSettings::createGrayCodeComponents,
        &PatternGeneratorSettings::createQuadratureComponents,
        &PatternGeneratorSettings::createPrbsComponents,
        &PatternGeneratorSettings::createPwmComponents,
        &PatternGeneratorSettings::createLineCodeComponents,
        &PatternGeneratorSettings::create4b5bComponents,
        &PatternGeneratorSettings::createJohnsonComponents,
        &PatternGeneratorSettings::createPdmComponents,
        &PatternGeneratorSettings::createParBusComponents,
        &PatternGeneratorSettings::createUartComponents,
        &PatternGeneratorSettings::createSpiComponents,
        &PatternGeneratorSettings::createI2cComponents,
    };

    void (PatternGeneratorSettings::*resetPatternComponents[PATTERNS_NUM])() = {
        &PatternGeneratorSettings::resetUserDefinedComponents,
        &PatternGeneratorSettings::resetCounterComponents,
        &PatternGeneratorSettings::resetBinaryCodeComponents,
        &PatternGeneratorSettings::resetGrayCodeComponents,
        &PatternGeneratorSettings::resetQuadratureComponents,
        &PatternGeneratorSettings::resetPrbsComponents,
        &PatternGeneratorSettings::resetPwmComponents,
        &PatternGeneratorSettings::resetLineCodeComponents,
        &PatternGeneratorSettings::reset4b5bComponents,
        &PatternGeneratorSettings::resetJohnsonComponents,
        &PatternGeneratorSettings::resetPdmComponents,
        &PatternGeneratorSettings::resetParBusComponents,
        &PatternGeneratorSettings::resetUartComponents,
        &PatternGeneratorSettings::resetSpiComponents,
        &PatternGeneratorSettings::resetI2cComponents,
    };

    void createComponents(QWidget *parent, QVBoxLayout *destination);

    void createUserDefinedComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createCounterComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createBinaryCodeComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createGrayCodeComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createQuadratureComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createUartComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createSpiComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createI2cComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createPrbsComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createPwmComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createLineCodeComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void create4b5bComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createJohnsonComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createPdmComponents(QWidget *parent, QVBoxLayout *destination, int index);
    void createParBusComponents(QWidget *parent, QVBoxLayout *destination, int index);

    WidgetDialRange *createFrequencyDial(QWidget *parent, QString objName);

    void resetUserDefinedComponents();
    void resetCounterComponents();
    void resetBinaryCodeComponents();
    void resetGrayCodeComponents();
    void resetQuadratureComponents();
    void resetUartComponents();
    void resetSpiComponents();
    void resetI2cComponents();
    void resetPrbsComponents();
    void resetPwmComponents();
    void resetLineCodeComponents();
    void reset4b5bComponents();
    void resetJohnsonComponents();
    void resetPdmComponents();
    void resetParBusComponents();

    void resetFreqAndDataLenDials(WidgetDialRange *freqDial, WidgetDialRange *dataLenDial);

signals:
    void patternSelectionChanged(int index);

private slots:
    void resetPatternCallback(int index);
};

#endif // PATTERNGENERATORSETTINGS_H
