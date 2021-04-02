#include "colors.h"

colorTheme theme = DARK;

Colors::Colors(QObject *parent) : QObject(parent)
{
}

QColor Colors::getChannelColor(int channelIndex)
{
    int index = channelIndex;

    if(theme==DARK) {
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
        default:
            return QCOLOR_BLACK;
        }
    }else if(theme==LIGHT){

    }else {

    }

    return QCOLOR_BLACK;
}

QByteArray Colors::getChannelColorString(int channelIndex)
{
    int index = channelIndex;

    if(theme==DARK) {
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
        default:
            return COLOR_BLACK;
        }
    }else if(theme==LIGHT){

    }else {

    }

    return COLOR_BLACK;
}

QColor Colors::getCommonColor()
{
    switch (theme) {
    case DARK:
        return QCOLOR_COMMON_DARK_GREY;
    case LIGHT:
        return QCOLOR_COMMON_LIGHT_GREY;
    default:
        return QCOLOR_BLACK;
    }
}

QByteArray Colors::getCommonColorString()
{
    switch (theme) {
    case DARK:
        return COLOR_COMMON_DARK_GREY;
    case LIGHT:
        return COLOR_COMMON_LIGHT_GREY;
    default:
        return COLOR_BLACK;
    }
}
