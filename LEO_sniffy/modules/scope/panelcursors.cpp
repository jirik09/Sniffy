#include "panelcursors.h"

PanelCursors::PanelCursors(QVBoxLayout *destination, QWidget *parent)
{
    destination->addWidget(new WidgetSeparator(parent,"Cursors"));

    channelButtons = new WidgetButtons(parent,4,ButtonTypes::RADIO,"Channel selection");
    channelButtons->setObjectName("cursorChannSelection");
    destination->addWidget(channelButtons);
    channelButtons->setText("CH1",0);
    channelButtons->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(0)),0);
    channelButtons->setText("CH2",1);
    channelButtons->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(1)),1);
    channelButtons->setText("CH3",2);
    channelButtons->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(2)),2);
    channelButtons->setText("CH4",3);
    channelButtons->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(3)),3);

    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}
