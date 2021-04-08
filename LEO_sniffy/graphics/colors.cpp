#include "colors.h"

static AbstractColors *theme = nullptr;

Colors::Colors(QObject *parent, AbstractColors *colorTheme) : QObject(parent)
{
    theme = colorTheme;
}

QString Colors::getAppGlobalStyle(){
    return theme->getAppGlobalStyle();
}

QString Colors::getGraphicsPath(){
    return theme->getGraphicsPath();
}

QString Colors::getAppBackgroundColor(){
    return theme->getAppBackgroundColor();
}

QString Colors::getDataAreaColor(){
    return theme->getDataAreaColor();
}

QString Colors::getControlBackgroundColor(){
    return theme->getControlBackgroundColor();
}

QString Colors::getButtonBackgroundColor(){
    return theme->getButtonBackgroundColor();
}

QString Colors::getButtonDisabledBackgroundColor(){
    return theme->getButtonDisabledBackgroundColor();
}

QString Colors::getTextDarkColor(){
    return theme->getTextDarkColor();
}

QString Colors::getTextLightColor(){
    return theme->getTextLightColor();
}

QString Colors::getControlsColor(){
    return theme->getControlsColor();
}

QString Colors::getChannelColor(int channelIndex){
    if(channelIndex > TOP_CHANNEL_COLOR_INDEX)
        return theme->getChannelColor(TOP_CHANNEL_COLOR_INDEX);
    else
        return theme->getChannelColor(channelIndex);
}

QString Colors::getHoverColor()
{
    return theme->getHoverColor();
}

