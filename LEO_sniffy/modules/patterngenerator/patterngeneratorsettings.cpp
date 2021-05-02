#include "patterngeneratorsettings.h"

PatternGeneratorSettings::PatternGeneratorSettings(QVBoxLayout *destination, PatternGeneratorConfig *config, QWidget *parent) :
    QObject(parent),
    config(config)
{
    WidgetSeparator *sepPatternSelection = new WidgetSeparator(parent, "Pattern selection");
    destination->addWidget(sepPatternSelection);

    comboPatternSelection = new WidgetSelection(parent, "");
    for (int i=0; i<pattList.length(); i++)
        comboPatternSelection->addOption(pattList.at(i), i);

    destination->addWidget(comboPatternSelection);

    createUserDefinedComponents(parent, destination);
    createCounterComponents(parent, destination);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);

    connect(comboPatternSelection, &WidgetSelection::selectedIndexChanged, this, &PatternGeneratorSettings::patternSelectionChangedCallback);
}

void PatternGeneratorSettings::createUserDefinedComponents(QWidget *parent, QVBoxLayout *destination)
{
    buttonLoadUserDef = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, "", 0);
    buttonLoadUserDef->setObjectName("buttonLoadUserDef");
    destination->addWidget(buttonLoadUserDef);
}

void PatternGeneratorSettings::createCounterComponents(QWidget *parent, QVBoxLayout *destination)
{
    dialFreq = new WidgetDialRange(parent, "");
    dialFreq->setObjectName("dialCounterFreq");
    destination->addWidget(dialFreq);
}

void PatternGeneratorSettings::showUserDefinedComponents(bool show)
{
    buttonLoadUserDef->setButtonHidden(show, 0);
}

void PatternGeneratorSettings::showCounterComponents(bool show)
{
    dialFreq->setVisible(show);
}

void PatternGeneratorSettings::showGreyCodeComponents(bool show)
{

}

void PatternGeneratorSettings::showQuadratureComponents(bool show)
{

}

void PatternGeneratorSettings::showUartComponents(bool show)
{

}

void PatternGeneratorSettings::showSpiComponents(bool show)
{

}

void PatternGeneratorSettings::showI2cComponents(bool show)
{

}

void PatternGeneratorSettings::patternSelectionChangedCallback(int index)
{
    this->*selectPatternComponents[config->prevIndex](false);
//    selectPatternComponents[index](true);
    config->prevIndex = index;
}
