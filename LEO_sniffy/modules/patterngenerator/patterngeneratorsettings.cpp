#include "patterngeneratorsettings.h"

PatternGeneratorSettings::PatternGeneratorSettings(QVBoxLayout *destination, QWidget *parent) : QObject(parent)
{
    WidgetSeparator *sepPatternSelection = new WidgetSeparator(parent, "Pattern selection");
    destination->addWidget(sepPatternSelection);

    QList<QString> *pattList = new QList<QString>;
    pattList->append("User defined");
    pattList->append("Counter");
    pattList->append("Grey code");
    pattList->append("Quadrature");
    pattList->append("UART");
    pattList->append("SPI");
    pattList->append("I2C");

    comboPatternSelection = new WidgetSelection(parent, "");
    for (int i=0; i<pattList->length(); i++)
        comboPatternSelection->addOption(pattList->at(i), i);

    destination->addWidget(comboPatternSelection);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}
