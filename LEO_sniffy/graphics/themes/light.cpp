#include "light.h"

Light::Light(QObject *parent)
{
    Q_UNUSED(parent);

    Graphics::COLOR_WINDOW_APP = LIGHT_BACKGROUND_APP;
    Graphics::COLOR_WINDOW_WIDGET = LIGHT_BACKGROUND_WIDGET;
    Graphics::COLOR_DATA_INPUT_AREA = LIGHT_DATA_INPUT_AREA;
    Graphics::COLOR_CHART = LIGHT_CHART;
}

QString Light::getThemeName(){
    return "Light";
}

QString Light::getAppGlobalStyle(){
    return "";
}

QString Light::getGraphicsPath(){
    return LIGHT_GRAPHICS_PATH;
}

QString Light::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}
