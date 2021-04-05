#include "darktheme.h"

DarkTheme::DarkTheme(QObject *parent)
{
    Q_UNUSED(parent);
}

QString DarkTheme::getThemeName()
{
    return "Dark";
}

QColor DarkTheme::getAppBackgroundColor()
{
    return DARK_BACKGROUND_APP;
}

QString DarkTheme::getAppBackgroundColorString()
{
    return DARK_BACKGROUND_APP.name();
}

QColor DarkTheme::getDataAreaColor()
{
    return DARK_BACKGROUND_DATA_AREA;
}

QString DarkTheme::getDataAreaColorString()
{
    return DARK_BACKGROUND_DATA_AREA.name();
}

QColor DarkTheme::getControlBackgroundColor()
{
    return DARK_BACKGROUND_CONTROL;
}

QString DarkTheme::getControlBackgroundColorString()
{
    return DARK_BACKGROUND_CONTROL.name();
}

QColor DarkTheme::getButtonBackgroundColor()
{
    return DARK_BACKGROUND_BUTTON;
}

QString DarkTheme::getButtonBackgroundColorString()
{
    return DARK_BACKGROUND_BUTTON.name();
}

QColor DarkTheme::getButtonDisabledBackgroundColor()
{
    return DARK_BACKGROUND_BUTTON_DISABLED;
}

QString DarkTheme::getButtonDisabledBackgroundColorString()
{
    return DARK_BACKGROUND_BUTTON_DISABLED.name();
}

QColor DarkTheme::getTextColor()
{
    return DARK_TEXT;
}

QColor DarkTheme::getTextColorString()
{
    return DARK_TEXT.name();
}

QColor DarkTheme::getTextDarkColor()
{
    return DARK_TEXT_DARK;
}

QString DarkTheme::getTextDarkColorString()
{
    return DARK_TEXT_DARK.name();
}

QColor DarkTheme::getTextLightColor()
{
    return DARK_TEXT_LIGHT;
}

QString DarkTheme::getTextLightColorString()
{
    return DARK_TEXT_LIGHT.name();
}

QColor DarkTheme::getControlsColor()
{
    return DARK_CONTROLS;
}

QString DarkTheme::getControlsColorString()
{
    return DARK_CONTROLS.name();
}

QColor DarkTheme::getChannelColor(int channelIndex)
{
    return chanColor.at(channelIndex);
}

QString DarkTheme::getChannelColorString(int channelIndex)
{
    return chanColor.at(channelIndex).name();
}
