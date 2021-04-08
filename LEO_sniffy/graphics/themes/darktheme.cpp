#include "darktheme.h"

DarkTheme::DarkTheme(QObject *parent)
{
    Q_UNUSED(parent);
}

QString DarkTheme::getThemeName()
{
    return "Dark";
}

QString DarkTheme::getAppGlobalStyle()
{
    return DARK_STYLE_GLOBAL;
}

QString DarkTheme::getGraphicsPath()
{
    return DARK_GRAPHICS_PATH;
}

QString DarkTheme::getAppBackgroundColor()
{
    return DARK_BACKGROUND_APP;
}

QString DarkTheme::getDataAreaColor()
{
    return DARK_BACKGROUND_DATA_AREA;
}

QString DarkTheme::getControlBackgroundColor()
{
    return DARK_BACKGROUND_CONTROL;
}

QString DarkTheme::getButtonBackgroundColor()
{
    return DARK_BACKGROUND_BUTTON;
}

QString DarkTheme::getButtonDisabledBackgroundColor()
{
    return DARK_BACKGROUND_BUTTON_DISABLED;
}

QString DarkTheme::getTextDarkColor()
{
    return DARK_TEXT_DARK;
}

QString DarkTheme::getTextLightColor()
{
    return DARK_TEXT_LIGHT;
}

QString DarkTheme::getControlsColor()
{
    return DARK_CONTROLS;
}

QString DarkTheme::getChannelColor(int channelIndex)
{
    return chanColor.at(channelIndex).name();
}

QString DarkTheme::getHoverColor()
{
    return DARK_HOVER;
}
