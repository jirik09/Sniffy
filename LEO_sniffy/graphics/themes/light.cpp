#include "light.h"

Light::Light(QObject *parent)
{
    Q_UNUSED(parent);

    Graphics::COLOR_WINDOW_APP = LIGHT_BACKGROUND_APP;
    Graphics::COLOR_WINDOW_WIDGET = LIGHT_BACKGROUND_WIDGET;
}

QString Light::getThemeName(){
    return "Light";
}

QString Light::getAppGlobalStyle(){
    return 0;
}

QString Light::getGraphicsPath(){
    return 0;
}

QString Light::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}
