#include "countertabratio.h"

CounterTabRatio::CounterTabRatio(QVBoxLayout *destination, QWidget *parent) : QObject(parent)
{
    WidgetSeparator *separatorSampleCounte = new WidgetSeparator(parent,"Sampling of the reference input");
    destination->addWidget(separatorSampleCounte);

    dialSampleCount = new WidgetDialRange(parent, "Sample Count");
    dialSampleCount->setObjectName("dialRatSampleCount");
    dialSampleCount->setRange(2, REFERENCE_COUNT_MAX, "Smpls", 1, 1, REFERENCE_COUNT_DEFAULT, true);
    dialSampleCount->setColor(Colors::getControlsColorString());
    dialSampleCount->hideUnitSelection();
    destination->addWidget(dialSampleCount);

    WidgetSeparator *separatorRetrigButton = new WidgetSeparator(parent,"Sampling retrigger");
    destination->addWidget(separatorRetrigButton);

    buttonRetrigger = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, 0);
    buttonRetrigger->setObjectName("retrigger");
    buttonRetrigger->setText("Retrigger", 0);
    destination->addWidget(buttonRetrigger);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}
