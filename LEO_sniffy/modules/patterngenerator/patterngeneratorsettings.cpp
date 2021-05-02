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

    connect(comboPatternSelection, &WidgetSelection::selectedIndexChanged, this, &PatternGeneratorSettings::patternSelectionChangedCallback);
}

void PatternGeneratorSettings::createComponents(QWidget *parent, QVBoxLayout *destination)
{
    for (int i=0; i<pattList.length(); i++){
        QVBoxLayout *layout = new QVBoxLayout();
        patternArea[i] = new QWidget(parent, Qt::Window);
        patternArea[i]->setLayout(layout);
        (this->*createPatternComponents[i])(parent, layout);
        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(verticalSpacer);
        destination->addWidget(patternArea[i]);
        patternArea[i]->hide();
    }
}

void PatternGeneratorSettings::createUserDefinedComponents(QWidget *parent, QVBoxLayout *destination)
{
    buttonUserDefLoad = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, "", 0);
    buttonUserDefLoad->setText("Load CSV");
    buttonUserDefLoad->setObjectName("buttonLoadUserDef");
    destination->addWidget(buttonUserDefLoad);
}

void PatternGeneratorSettings::createCounterComponents(QWidget *parent, QVBoxLayout *destination)
{
    dialCounterFreq = new WidgetDialRange(parent, "");
    dialCounterFreq->setName("Frequency");
    dialCounterFreq->hideUnitSelection();
    dialCounterFreq->setRange(1, 36000000, "Hz", 1, 1, 1000, true, 2);
    dialCounterFreq->setObjectName("dialCounterFreq");
    destination->addWidget(dialCounterFreq);
}

void PatternGeneratorSettings::createBinaryCodeComponents(QWidget *parent, QVBoxLayout *destination)
{
    dialBinaryCodeFreq = new WidgetDialRange(parent, "");
    dialBinaryCodeFreq->setName("Frequency");
    dialBinaryCodeFreq->hideUnitSelection();
    dialBinaryCodeFreq->setRange(1, 36000000, "Hz", 1, 1, 1000, true, 2);
    dialBinaryCodeFreq->setObjectName("dialBinaryCodeFreq");
    destination->addWidget(dialBinaryCodeFreq);
}

void PatternGeneratorSettings::createGreyCodeComponents(QWidget *parent, QVBoxLayout *destination)
{
    dialGreyCodeFreq = new WidgetDialRange(parent, "");
    dialGreyCodeFreq->setName("Frequency");
    dialGreyCodeFreq->hideUnitSelection();
    dialGreyCodeFreq->setRange(1, 36000000, "Hz", 1, 1, 1000, true, 2);
    dialGreyCodeFreq->setObjectName("dialGreyCodeFreq");
    destination->addWidget(dialGreyCodeFreq);
}

void PatternGeneratorSettings::createQuadratureComponents(QWidget *parent, QVBoxLayout *destination)
{

}

void PatternGeneratorSettings::createUartComponents(QWidget *parent, QVBoxLayout *destination)
{

}

void PatternGeneratorSettings::createSpiComponents(QWidget *parent, QVBoxLayout *destination)
{

}

void PatternGeneratorSettings::createI2cComponents(QWidget *parent, QVBoxLayout *destination)
{
    comboI2cClockFreq = new WidgetSelection(parent, "Speed");
    comboI2cClockFreq->addOption("10 kbit/s", 0);
    comboI2cClockFreq->addOption("100 kbit/s", 1);
    comboI2cClockFreq->addOption("400 kbit/s", 2);
    comboI2cClockFreq->addOption("3.4 Mbit/s", 3);
    comboI2cClockFreq->addOption("5 Mbit/s", 4);
    destination->addWidget(comboI2cClockFreq);

    comboI2cCommType = new WidgetSelection(parent, "R/W");
    comboI2cCommType->addOption("Read", 0);
    comboI2cCommType->addOption("Write", 1);
    destination->addWidget(comboI2cCommType);
}

void PatternGeneratorSettings::showComponents(int pattIndex, bool visible)
{
    patternArea[pattIndex]->setVisible(visible);
}

void PatternGeneratorSettings::enableComponents(bool enable)
{
    patternArea[config->prevIndex]->setEnabled(enable);
}

void PatternGeneratorSettings::restoreSettingsPanel()
{
    showComponents(config->prevIndex, true);
    comboPatternSelection->setSelected(config->prevIndex, true);
}

void PatternGeneratorSettings::patternSelectionChangedCallback(int index)
{
    showComponents(config->prevIndex, false);
    showComponents(index, true);
    config->prevIndex = index;
}
