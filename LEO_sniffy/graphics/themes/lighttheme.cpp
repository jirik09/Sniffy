#include "lighttheme.h"

LightTheme::LightTheme(QObject *parent)
{
    Q_UNUSED(parent);
}

QString LightTheme::getThemeName()
{
    return "Light";
}

QString LightTheme::getAppGlobalStyle()
{
    return 0;
}

QString LightTheme::getGraphicsPath()
{
    return 0;
}

QString LightTheme::getAppBackgroundColor()
{
    return LIGHT_BACKGROUND_APP;
}

QString LightTheme::getDataAreaColor()
{
    return LIGHT_BACKGROUND_DATA_AREA;
}

QString LightTheme::getControlBackgroundColor()
{
    return LIGHT_BACKGROUND_CONTROL;
}

QString LightTheme::getButtonBackgroundColor()
{
    return LIGHT_BACKGROUND_BUTTON;
}

QString LightTheme::getButtonDisabledBackgroundColor()
{
    return LIGHT_BACKGROUND_BUTTON_DISABLED;
}

QString LightTheme::getTextDarkColor()
{
    return LIGHT_TEXT_DARK;
}

QString LightTheme::getTextLightColor()
{
    return LIGHT_TEXT_LIGHT;
}

QString LightTheme::getControlsColor()
{
    return LIGHT_CONTROLS;
}

QString LightTheme::getChannelColor(int channelIndex)
{
    return chanColor.at(channelIndex).name();
}

QString LightTheme::getHoverColor()
{
    return 0;
}
