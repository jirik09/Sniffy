#include "countertabhighfreq.h"
#include "counterdefs.h"

CounterTabHighFreq::CounterTabHighFreq(QVBoxLayout *destination, QWidget *parent) : QObject(parent)
{
    WidgetSeparator *separatorQuantitySwitch = new WidgetSeparator(parent,"Quantity switch");
    destination->addWidget(separatorQuantitySwitch);

    buttonsQuantitySwitch = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsQuantitySwitch->setText("Frequency", 0);
    buttonsQuantitySwitch->setText("Period", 1);
    destination->addWidget(buttonsQuantitySwitch);

    WidgetSeparator *separatorErrorSwitch = new WidgetSeparator(parent,"Error switch");
    destination->addWidget(separatorErrorSwitch);

    buttonsErrorSwitch = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsErrorSwitch->setText("Err", 0);
    buttonsErrorSwitch->setText("Err AVG", 1);
    destination->addWidget(buttonsErrorSwitch);

    dialAveraging = new WidgetDialRange(parent ,"Averaging");
    dialAveraging->setRange(2, AVERAGE_RANGE, "Samples", 1, 100, 2, false);
    dialAveraging->hideUnitSelection();
    destination->addWidget(dialAveraging);

    WidgetSeparator *separatorGateTime = new WidgetSeparator(parent,"Gate Time [s]");
    destination->addWidget(separatorGateTime);

    buttonsGateTime = new WidgetButtons(parent, 5, ButtonTypes::RADIO, "", 0);
    buttonsGateTime->setText("0.1", 0);
    buttonsGateTime->setText("0.5", 1);
    buttonsGateTime->setText("1.0", 2);
    buttonsGateTime->setText("5.0", 3);
    buttonsGateTime->setText("10.0", 4);
    destination->addWidget(buttonsGateTime);

//    WidgetSeparator *separatorAutoPrescaler = new WidgetSeparator(parent,"Automatic input prescaler");
//    destination->addWidget(separatorAutoPrescaler);

//    labelAutoInputPrescaler = new QLabel(parent);
//    labelAutoInputPrescaler->setNum(1);
//    labelAutoInputPrescaler->setAlignment(Qt::AlignCenter);
//    destination->addWidget(labelAutoInputPrescaler);
}
