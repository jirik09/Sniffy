#include "patterngeneratorsettings.h"

PatternGeneratorSettings::PatternGeneratorSettings(QVBoxLayout *destination, PatternGeneratorConfig *config, QWidget *parent) :
    QObject(parent),
    config(config)
{
    WidgetSeparator *sepStartPattern = new WidgetSeparator(parent, "Start pattern generation");
    destination->addWidget(sepStartPattern);

    buttonStart = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, "", 0);
    buttonStart->setText("Start");
    buttonStart->setObjectName("buttonPattGenStart");
    destination->addWidget(buttonStart);

    WidgetSeparator *sepPatternSelection = new WidgetSeparator(parent, "Pattern selection");
    destination->addWidget(sepPatternSelection);

    comboPatternSelection = new WidgetSelection(parent, "");
    for (int i=0; i<pattList.length(); i++)
        comboPatternSelection->addOption(pattList.at(i), i);

    destination->addWidget(comboPatternSelection);

    createComponents(parent, destination);

    QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);

    buttonSetDefault = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, "", 0);
    buttonSetDefault->setText("Reset");
    buttonSetDefault->setObjectName("buttonPattGenSetDefault");
    destination->addWidget(buttonSetDefault);

    connect(buttonSetDefault, &WidgetButtons::clicked, this, &PatternGeneratorSettings::resetPatternCallback);
}

void PatternGeneratorSettings::restoreSettingsAfterStartup()
{
    showComponents(config->pattIndex, true);
    comboPatternSelection->setSelected(config->pattIndex, true);
}

void PatternGeneratorSettings::setSpecification(PatternGeneratorSpec *spec)
{
    int maxSamplingFreq = spec->maxSamplingRate / 2;

    dialUserDefFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    dialCounterFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    dialBinaryCodeFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    dialGrayCodeFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    dialQuadratureFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
}

void PatternGeneratorSettings::createComponents(QWidget *parent, QVBoxLayout *destination)
{
    for (int i=0; i<pattList.length(); i++){
        QVBoxLayout *layout = new QVBoxLayout();
        patternArea[i] = new QWidget(parent, Qt::Window);
        patternArea[i]->setLayout(layout);
        (this->*createPatternComponents[i])(parent, layout, i);
        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(verticalSpacer);
        destination->addWidget(patternArea[i]);
        patternArea[i]->hide();
    }
}

void PatternGeneratorSettings::createUserDefinedComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    buttonUserDefLoadPattern = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, "", 0);
    buttonUserDefLoadPattern->setText("Load CSV");
    buttonUserDefLoadPattern->setObjectName("buttonLoadUserDef");
    destination->addWidget(buttonUserDefLoadPattern);

    dialUserDefFreq = createFrequencyDial(parent, "dialUserDefFreq");
    destination->addWidget(dialUserDefFreq);

    dialUserDefLength = new WidgetDialRange(parent, "");
    dialUserDefLength->setName("Data Length");
    dialUserDefLength->hideUnitSelection();
    dialUserDefLength->setRange(2, 100, "Tick", 1, 1, PATT_DEFAULT_DATA_LENGTH, false, 0);
    dialUserDefLength->setObjectName("dialUserDefLength");
    destination->addWidget(dialUserDefLength);

    config->freq[index] = dialUserDefFreq->getRealValue();
}

void PatternGeneratorSettings::createCounterComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialCounterFreq = createFrequencyDial(parent, "dialCounterFreq");
    destination->addWidget(dialCounterFreq);

    dialCounterLength = new WidgetDialRange(parent, "");
    dialCounterLength->setName("Data Length");
    dialCounterLength->hideUnitSelection();
    dialCounterLength->setRange(2, 100, "Tick", 1, 1, PATT_DEFAULT_DATA_LENGTH, false, 0);
    dialCounterLength->setObjectName("dialCounterLength");
    destination->addWidget(dialCounterLength);

    config->freq[index] = dialCounterFreq->getRealValue();
}

void PatternGeneratorSettings::createBinaryCodeComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialBinaryCodeFreq = createFrequencyDial(parent, "dialBinaryCodeFreq");
    destination->addWidget(dialBinaryCodeFreq);    

    dialBinaryChanNum = new WidgetDialRange(parent, "");
    dialBinaryChanNum->setName("Channel no.");
    dialBinaryChanNum->hideUnitSelection();
    dialBinaryChanNum->setRange(2, PATT_DEFAULT_DATA_LENGTH, "#", 1, 1, PATT_DEFAULT_DATA_LENGTH, false, 0);
    dialBinaryChanNum->setObjectName("dialBinaryChanNum");
    destination->addWidget(dialBinaryChanNum);

    config->freq[index] = dialCounterFreq->getRealValue();    
}

void PatternGeneratorSettings::createGrayCodeComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialGrayCodeFreq = createFrequencyDial(parent, "dialGrayCodeFreq");
    destination->addWidget(dialGrayCodeFreq);

    dialGrayCodeChanNum = new WidgetDialRange(parent, "");
    dialGrayCodeChanNum->setName("Channel no.");
    dialGrayCodeChanNum->hideUnitSelection();
    dialGrayCodeChanNum->setRange(2, PATT_DEFAULT_DATA_LENGTH, "#", 1, 1, PATT_DEFAULT_DATA_LENGTH, false, 0);
    dialGrayCodeChanNum->setObjectName("dialGrayCodeChanNum");
    destination->addWidget(dialGrayCodeChanNum);

    config->freq[index] = dialGrayCodeFreq->getRealValue();    
}

void PatternGeneratorSettings::createQuadratureComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialQuadratureFreq = createFrequencyDial(parent, "dialQuadratureFreq");
    destination->addWidget(dialQuadratureFreq);

    comboQuadratureSeqAbba = new WidgetSelection(parent, "Speed");
    comboQuadratureSeqAbba->addOption("A/B", 0);
    comboQuadratureSeqAbba->addOption("B/A", 1);
    destination->addWidget(comboQuadratureSeqAbba);

    config->freq[index] = dialQuadratureFreq->getRealValue();    
}

void PatternGeneratorSettings::createUartComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
Q_UNUSED(parent);Q_UNUSED(destination);Q_UNUSED(index);
}

void PatternGeneratorSettings::createSpiComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
Q_UNUSED(parent);Q_UNUSED(destination);Q_UNUSED(index);
}

void PatternGeneratorSettings::createI2cComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    comboI2cClockFreq = new WidgetSelection(parent, "Speed");
    comboI2cClockFreq->addOption("10 kbit/s", 10000);
    comboI2cClockFreq->addOption("100 kbit/s", 100000);
    comboI2cClockFreq->addOption("400 kbit/s", 400000);
    comboI2cClockFreq->addOption("3.4 Mbit/s", 3400000);
    comboI2cClockFreq->addOption("5 Mbit/s", 5000000);
    destination->addWidget(comboI2cClockFreq);    

    comboI2cCommType = new WidgetSelection(parent, "R/W");
    comboI2cCommType->addOption("Read", 0);
    comboI2cCommType->addOption("Write", 1);
    destination->addWidget(comboI2cCommType);    

    config->freq[index] = comboI2cClockFreq->getSelectedValue();    
}

WidgetDialRange *PatternGeneratorSettings::createFrequencyDial(QWidget *parent, QString objName)
{
    WidgetDialRange *dial = new WidgetDialRange(parent, "");
    dial->setName("Cycle frequency");
    dial->hideUnitSelection();
    dial->setRange(1, 4800000, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2);
    dial->setObjectName(objName);

    return dial;
}

/******************************************************************************************/

void PatternGeneratorSettings::resetPatternCallback(int index)
{
    Q_UNUSED(index);
    (this->*resetPatternComponents[config->pattIndex])();
}

void PatternGeneratorSettings::resetUserDefinedComponents(){
    resetFreqAndDataLenDials(dialUserDefFreq, dialUserDefLength);
}

void PatternGeneratorSettings::resetCounterComponents(){
    resetFreqAndDataLenDials(dialCounterFreq, dialCounterLength);
}

void PatternGeneratorSettings::resetBinaryCodeComponents(){
    resetFreqAndDataLenDials(dialBinaryCodeFreq, dialBinaryChanNum);
}

void PatternGeneratorSettings::resetGrayCodeComponents(){
    resetFreqAndDataLenDials(dialGrayCodeFreq, dialGrayCodeChanNum);
}

void PatternGeneratorSettings::resetQuadratureComponents()
{
    dialQuadratureFreq->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    comboQuadratureSeqAbba->setSelected(0, false);
    config->freq[config->pattIndex] = dialQuadratureFreq->getRealValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_DATA_LENGTH;
}

void PatternGeneratorSettings::resetUartComponents()
{

}

void PatternGeneratorSettings::resetSpiComponents()
{

}

void PatternGeneratorSettings::resetI2cComponents()
{

}

void PatternGeneratorSettings::resetFreqAndDataLenDials(WidgetDialRange *freqDial, WidgetDialRange *dataLenDial)
{
    freqDial->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    dataLenDial->setRealValue(PATT_DEFAULT_DATA_LENGTH, true);
    config->freq[config->pattIndex] = freqDial->getRealValue();
    config->dataLen[config->pattIndex] = dataLenDial->getRealValue();
}

/******************************************************************************************/

void PatternGeneratorSettings::showComponents(int pattIndex, bool visible)
{
    patternArea[pattIndex]->setVisible(visible);
}

void PatternGeneratorSettings::enableGuiComponents(bool enable)
{    
    comboPatternSelection->setEnabled(enable);
    patternArea[config->pattIndex]->setEnabled(enable);
    buttonSetDefault->setEnabled(enable);
}
