#include "lighttheme.h"

LightTheme::LightTheme(QObject *parent)
{
    Q_UNUSED(parent);
}

QString LightTheme::getThemeName()
{
    return "Light";
}

QColor LightTheme::getAppBackgroundColor()
{
    return LIGHT_BACKGROUND_APP;
}

QString LightTheme::getAppBackgroundColorString()
{
    return LIGHT_BACKGROUND_APP.name();
}

QColor LightTheme::getDataAreaColor()
{
    return LIGHT_BACKGROUND_DATA_AREA;
}

QString LightTheme::getDataAreaColorString()
{
    return LIGHT_BACKGROUND_DATA_AREA.name();
}

QColor LightTheme::getControlBackgroundColor()
{
    return LIGHT_BACKGROUND_CONTROL;
}

QString LightTheme::getControlBackgroundColorString()
{
    return LIGHT_BACKGROUND_CONTROL.name();
}

QColor LightTheme::getButtonBackgroundColor()
{
    return LIGHT_BACKGROUND_BUTTON;
}

QString LightTheme::getButtonBackgroundColorString()
{
    return LIGHT_BACKGROUND_BUTTON.name();
}

QColor LightTheme::getButtonDisabledBackgroundColor()
{
    return LIGHT_BACKGROUND_BUTTON_DISABLED;
}

QString LightTheme::getButtonDisabledBackgroundColorString()
{
    return LIGHT_BACKGROUND_BUTTON_DISABLED.name();
}

QColor LightTheme::getTextDarkColor()
{
    return LIGHT_TEXT_DARK;
}

QString LightTheme::getTextDarkColorString()
{
    return LIGHT_TEXT_DARK.name();
}

QColor LightTheme::getTextLightColor()
{
    return LIGHT_TEXT_LIGHT;
}

QString LightTheme::getTextLightColorString()
{
    return LIGHT_TEXT_LIGHT.name();
}

QColor LightTheme::getControlsColor()
{
    return LIGHT_CONTROLS_GREY;
}

QString LightTheme::getControlsColorString()
{
    return LIGHT_CONTROLS_GREY.name();
}

QColor LightTheme::getChannelColor(int channelIndex)
{
    return chanColor.at(channelIndex);
}

QString LightTheme::getChannelColorString(int channelIndex)
{
    return chanColor.at(channelIndex).name();
}
