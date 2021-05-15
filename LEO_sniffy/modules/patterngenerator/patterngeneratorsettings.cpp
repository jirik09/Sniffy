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
    buttonSetDefault->setText("Default");
    buttonSetDefault->setObjectName("buttonPattGenSetDefault");
    destination->addWidget(buttonSetDefault);

    connect(comboPatternSelection, &WidgetSelection::selectedIndexChanged, this, &PatternGeneratorSettings::patternSelectionChangedCallback);
}

void PatternGeneratorSettings::restoreSettingsAfterStartup()
{
    showComponents(config->pattIndex, true);
    comboPatternSelection->setSelected(config->pattIndex, true);
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

    dialUserDefFreq = createDial(parent, "dialUserDefFreq");
    destination->addWidget(dialUserDefFreq);
    config->freq[index] = dialUserDefFreq->getRealValue();
    connect(dialUserDefFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorSettings::freqChangedDialsCallback);
}

void PatternGeneratorSettings::createCounterComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialCounterFreq = createDial(parent, "dialCounterFreq");
    destination->addWidget(dialCounterFreq);    
    config->freq[index] = dialCounterFreq->getRealValue();
    connect(dialCounterFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorSettings::freqChangedDialsCallback);
}

void PatternGeneratorSettings::createBinaryCodeComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialBinaryCodeFreq = createDial(parent, "dialBinaryCodeFreq");
    destination->addWidget(dialBinaryCodeFreq);    
    config->freq[index] = dialCounterFreq->getRealValue();
    connect(dialBinaryCodeFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorSettings::freqChangedDialsCallback);
}

void PatternGeneratorSettings::createGrayCodeComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialGrayCodeFreq = createDial(parent, "dialGrayCodeFreq");
    destination->addWidget(dialGrayCodeFreq);
    config->freq[index] = dialGrayCodeFreq->getRealValue();
    connect(dialGrayCodeFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorSettings::freqChangedDialsCallback);
}

void PatternGeneratorSettings::createQuadratureComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialQuadratureFreq = createDial(parent, "dialQuadratureFreq");
    destination->addWidget(dialQuadratureFreq);
    config->freq[index] = dialQuadratureFreq->getRealValue();
    connect(dialQuadratureFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorSettings::freqChangedDialsCallback);
}

void PatternGeneratorSettings::createUartComponents(QWidget *parent, QVBoxLayout *destination, int index)
{

}

void PatternGeneratorSettings::createSpiComponents(QWidget *parent, QVBoxLayout *destination, int index)
{

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
    connect(comboI2cClockFreq, &WidgetSelection::selectedIndexChanged, this, &PatternGeneratorSettings::freqChangedCombosCallback);
}

WidgetDialRange *PatternGeneratorSettings::createDial(QWidget *parent, QString objName)
{
    WidgetDialRange *dial = new WidgetDialRange(parent, "");
    dial->setName("Frequency");
    dial->hideUnitSelection();
    dial->setRange(1, 36000000, "Hz", 1, 1, 1000, true, 2);
    dial->setObjectName(objName);

    return dial;
}

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

qreal PatternGeneratorSettings::getFrequency()
{
    return config->freq[config->pattIndex];
}

/******************************** Callbacks *********************************/

void PatternGeneratorSettings::patternSelectionChangedCallback(int index)
{
    showComponents(config->pattIndex, false);
    showComponents(index, true);

    config->pattIndex = index;

    emit patternSelectionChanged(index);
}

void PatternGeneratorSettings::freqChangedDialsCallback(float val)
{
    config->freq[config->pattIndex] = val;
}

void PatternGeneratorSettings::freqChangedCombosCallback(int index, float realVal)
{
    Q_UNUSED(index);
    config->freq[config->pattIndex] = realVal;
}

