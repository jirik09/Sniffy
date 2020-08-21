#include "countertabratio.h"

CounterTabRatio::CounterTabRatio(QVBoxLayout *destination, QWidget *parent) : QObject(parent)
{
    WidgetSeparator *separatorSampleCounte = new WidgetSeparator(parent,"Sampling of the reference input");
    destination->addWidget(separatorSampleCounte);

    dialSampleCount = new WidgetDialRange(parent ,"Sample Count");
    dialSampleCount->setRange(2, REFERENCE_COUNT_MAX, "Samples", 1, 100, REFERENCE_COUNT_DEFAULT, true);
    dialSampleCount->hideUnitSelection();
    destination->addWidget(dialSampleCount);

    WidgetSeparator *separatorRetrigButton = new WidgetSeparator(parent,"Sampling retrigger");
    destination->addWidget(separatorRetrigButton);

    buttonRetrigger = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, 0);
    buttonRetrigger->setText("Retrigger", 0);
    destination->addWidget(buttonRetrigger);
}
