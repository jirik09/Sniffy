#include "graphics.h"

static AbstractGraphics *theme = nullptr;

QString Graphics::IMAGE_UNITS_HZ;
QString Graphics::IMAGE_UNITS_SEC;
QString Graphics::IMAGE_UNITS_VOLT;
QString Graphics::IMAGE_UNITS_VRIPPLE;
QString Graphics::IMAGE_UNITS_VMAX;
QString Graphics::IMAGE_UNITS_VMIN;
QString Graphics::IMAGE_SIGN_PERCENT;
QString Graphics::IMAGE_SIGN_ERR;
QString Graphics::IMAGE_SIGN_ERRAVG;
QString Graphics::IMAGE_SIGN_AVG;
QString Graphics::IMAGE_SIGN_PLSMNS;

Graphics::Graphics(QObject *parent, AbstractGraphics *colorTheme) : QObject(parent)
{
    theme = colorTheme;

    QString path = theme->getGraphicsPath();
    IMAGE_UNITS_HZ = "image: url("+path+"units_hz.png); border: none;";
    IMAGE_UNITS_SEC = "image: url("+path+"units_sec.png); border: none;";
    IMAGE_UNITS_VOLT = "image: url("+path+"units_v.png); border: none;";
    IMAGE_UNITS_VRIPPLE = "image: url("+path+"units_Vripple.png); border: none;";
    IMAGE_UNITS_VMAX = "image: url("+path+"units_Vmax.png); border: none;";
    IMAGE_UNITS_VMIN = "image: url("+path+"units_Vmin.png); border: none;";
    IMAGE_SIGN_PERCENT = "image: url("+path+"units_perc.png); border: none;";
    IMAGE_SIGN_ERR = "image: url("+path+"units_err.png); border: none;";
    IMAGE_SIGN_ERRAVG = "image: url("+path+"units_erravg.png); border: none;";
    IMAGE_SIGN_AVG = "image: url("+path+"units_avg.png); border: none;";
    IMAGE_SIGN_PLSMNS = "image: url("+path+"sign_pm.png); border: none;";
}

QString Graphics::getAppGlobalStyle(){
    return theme->getAppGlobalStyle();
}

QString Graphics::getGraphicsPath(){
    return theme->getGraphicsPath();
}

QString Graphics::getAppBackgroundColor(){
    return theme->getAppBackgroundColor();
}

QString Graphics::getDataAreaColor(){
    return theme->getDataAreaColor();
}

QString Graphics::getControlBackgroundColor(){
    return theme->getControlBackgroundColor();
}

QString Graphics::getButtonBackgroundColor(){
    return theme->getButtonBackgroundColor();
}

QString Graphics::getButtonDisabledBackgroundColor(){
    return theme->getButtonDisabledBackgroundColor();
}

QString Graphics::getTextDarkColor(){
    return theme->getTextDarkColor();
}

QString Graphics::getTextLightColor(){
    return theme->getTextLightColor();
}

QString Graphics::getControlsColor(){
    return theme->getControlsColor();
}

QString Graphics::getChannelColor(int channelIndex){
    if(channelIndex > TOP_CHANNEL_COLOR_INDEX)
        return theme->getChannelColor(TOP_CHANNEL_COLOR_INDEX);
    else
        return theme->getChannelColor(channelIndex);
}

QString Graphics::getHoverColor()
{
    return theme->getHoverColor();
}

