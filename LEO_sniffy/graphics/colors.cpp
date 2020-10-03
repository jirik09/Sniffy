#include "colors.h"

Colors::Colors(QObject *parent) : QObject(parent)
{
}

QColor Colors::getChannelColor(int channelIndex)
{
    int index = channelIndex%10;

    switch (index) {
    case 0:
        return CHANNEL_1_QCOLOR;
    case 1:
        return CHANNEL_2_QCOLOR;
    case 2:
        return CHANNEL_3_QCOLOR;
    case 3:
        return CHANNEL_4_QCOLOR;
    case 4:
        return CHANNEL_5_QCOLOR;
    case 5:
        return CHANNEL_6_QCOLOR;
    case 6:
        return CHANNEL_7_QCOLOR;
    case 7:
        return CHANNEL_8_QCOLOR;
    case 8:
        return CHANNEL_9_QCOLOR;
    case 9:
        return CHANNEL_10_QCOLOR;
    case 10:
        return CHANNEL_11_QCOLOR;
    case 11:
        return CHANNEL_12_QCOLOR;
    default:
        return QCOLOR_GREY;
    }
}

QByteArray Colors::getChannelColorString(int channelIndex)
{
    int index = channelIndex%10;

    switch (index) {
    case 0:
        return CHANNEL_1_COLOR;
    case 1:
        return CHANNEL_2_COLOR;
    case 2:
        return CHANNEL_3_COLOR;
    case 3:
        return CHANNEL_4_COLOR;
    case 4:
        return CHANNEL_5_COLOR;
    case 5:
        return CHANNEL_6_COLOR;
    case 6:
        return CHANNEL_7_COLOR;
    case 7:
        return CHANNEL_8_COLOR;
    case 8:
        return CHANNEL_9_COLOR;
    case 9:
        return CHANNEL_10_COLOR;
    case 10:
        return CHANNEL_11_COLOR;
    case 11:
        return CHANNEL_12_COLOR;
    default:
        return COLOR_GREY;
    }
}
