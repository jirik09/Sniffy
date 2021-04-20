#include "countertabratio.h"

CounterTabRatio::CounterTabRatio(QVBoxLayout *destination, QWidget *parent) : QObject(parent)
{
    WidgetSeparator *separatorSampleCounte = new WidgetSeparator(parent,"Sampling of the reference input");
    destination->addWidget(separatorSampleCounte);

    dialSampleCount = new WidgetDialRange(parent, "Sample Count");
    dialSampleCount->setObjectName("dialRatSampleCount");
    dialSampleCount->setRange(2, REFERENCE_COUNT_MAX, "Smpls", 1, 1, REFERENCE_COUNT_DEFAULT, true, 0);
    dialSampleCount->setColor(Graphics::COLOR_CONTROLS);
    dialSampleCount->hideUnitSelection();
    destination->addWidget(dialSampleCount);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}
