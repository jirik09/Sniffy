#include "countertabhighfreq.h"

CounterTabHighFreq::CounterTabHighFreq(QVBoxLayout *destination, QWidget *parent) : QObject(parent)
{
    WidgetSeparator *separatorQuantitySwitch = new WidgetSeparator(parent,"Quantity switch");
    destination->addWidget(separatorQuantitySwitch);

    buttonsQuantitySwitch = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsQuantitySwitch->setObjectName("quantitySwitch");
    buttonsQuantitySwitch->setText("Frequency", 0);
    buttonsQuantitySwitch->setText("Period", 1);
    destination->addWidget(buttonsQuantitySwitch);

    WidgetSeparator *separatorErrorSwitch = new WidgetSeparator(parent,"Error switch");
    destination->addWidget(separatorErrorSwitch);

    buttonsErrorSwitch = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsErrorSwitch->setObjectName("errorSwitch");
    buttonsErrorSwitch->setText("Err", 0);
    buttonsErrorSwitch->setText("Err AVG", 1);
    destination->addWidget(buttonsErrorSwitch);

    dialAveraging = new WidgetDialRange(parent ,"Averaging");
    dialAveraging->setObjectName("dialHfAveraging");
    dialAveraging->setRange(2, AVERAGE_RANGE, "Smpls", 1, 1, 2, false, 0);
    dialAveraging->setColor(Graphics::COLOR_CONTROLS);
    dialAveraging->hideUnitSelection();
    destination->addWidget(dialAveraging);

    WidgetSeparator *separatorGateTime = new WidgetSeparator(parent,"Gate Time [s]");
    destination->addWidget(separatorGateTime);

    buttonsGateTime = new WidgetButtons(parent, 5, ButtonTypes::RADIO, "", 0);
    buttonsGateTime->setObjectName("gateTime");
    buttonsGateTime->setText("0.1", 0);
    buttonsGateTime->setText("0.5", 1);
    buttonsGateTime->setText("1.0", 2);
    buttonsGateTime->setText("5.0", 3);
    buttonsGateTime->setText("10.0", 4);
    destination->addWidget(buttonsGateTime);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}
