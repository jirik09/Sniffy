#include "colors.h"

static AbstractColors *theme = nullptr;

Colors::Colors(QObject *parent, AbstractColors *colorTheme) : QObject(parent)
{
    theme = colorTheme;
}

QColor Colors::getAppBackgroundColor(){
    return theme->getAppBackgroundColor();
}

QString Colors::getAppBackgroundColorString(){
    return theme->getAppBackgroundColorString();
}

QColor Colors::getDataAreaColor(){
    return theme->getDataAreaColor();
}

QString Colors::getDataAreaColorString(){
    return theme->getDataAreaColorString();
}

QColor Colors::getButtonBackgroundColor(){
    return theme->getButtonBackgroundColor();
}

QString Colors::getButtonBackgroundColorString(){
    return theme->getButtonBackgroundColorString();
}

QColor Colors::getButtonDisabledBackgroundColor(){
    return theme->getButtonDisabledBackgroundColor();
}

QString Colors::getButtonDisabledBackgroundColorString(){
    return theme->getButtonDisabledBackgroundColorString();
}

QColor Colors::getTextDarkColor(){
    return theme->getTextDarkColor();
}

QString Colors::getTextDarkColorString(){
    return theme->getTextDarkColorString();
}

QColor Colors::getTextLightColor(){
    return theme->getTextLightColor();
}

QString Colors::getTextLightColorString(){
    return theme->getTextLightColorString();
}

QColor Colors::getControlsColor(){
    return theme->getControlsColor();
}

QString Colors::getControlsColorString(){
    return theme->getControlsColorString();
}

QColor Colors::getChannelColor(int channelIndex){
    if(channelIndex > TOP_CHANNEL_COLOR_INDEX)
        return theme->getChannelColor(TOP_CHANNEL_COLOR_INDEX);
    else
       return theme->getChannelColor(channelIndex);
}

QString Colors::getChannelColorString(int channelIndex){
    if(channelIndex > TOP_CHANNEL_COLOR_INDEX)
        return theme->getChannelColorString(TOP_CHANNEL_COLOR_INDEX);
    else
       return theme->getChannelColorString(channelIndex);
}


