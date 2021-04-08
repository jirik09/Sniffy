#ifndef LIGHTTHEME_H
#define LIGHTTHEME_H

#include <QObject>
#include <QColor>

#include "../abstractcolors.h"

/* BACKGROUNDS */
#define LIGHT_BACKGROUND_APP "#303030"
#define LIGHT_BACKGROUND_DATA_AREA "#262626"
#define LIGHT_BACKGROUND_CONTROL "#3a3a3a"
#define LIGHT_BACKGROUND_BUTTON "#262626"
#define LIGHT_BACKGROUND_BUTTON_DISABLED "#303030"
#define LIGHT_BACKGROUND_FOCUS_IN "#303030"

/* TEXT */
#define LIGHT_TEXT_DARK "#7c7c7c"
#define LIGHT_TEXT_LIGHT "#d6d6d6"

/* SPECIAL PURPOSE */
#define LIGHT_HOVER "#474c5e"

/* CONTROLS */
#define LIGHT_CONTROLS "#4c5259"

/* COLORS */
#define LIGHT_FULL_BLACK "#000000"
#define LIGHT_FULL_WHITE "#ffffff"
#define LIGHT_BLUE "#2fa0d0"
#define LIGHT_ORANGE "#d2803c"
#define LIGHT_GREEN "#2cb689"
#define LIGHT_PURPLE "#b45464"
#define LIGHT_YELLOW "#b89b00"
#define LIGHT_RED "#bd4236"
#define LIGHT_BLUE_DARK "#4762cf"
#define LIGHT_PINK "#cd72bf"

/* CHANNELS */
#define LIGHT_CHANNEL_1 LIGHT_BLUE
#define LIGHT_CHANNEL_2 LIGHT_PURPLE
#define LIGHT_CHANNEL_3 LIGHT_GREEN
#define LIGHT_CHANNEL_4 LIGHT_YELLOW
#define LIGHT_CHANNEL_5 LIGHT_ORANGE
#define LIGHT_CHANNEL_6 LIGHT_RED
#define LIGHT_CHANNEL_7 LIGHT_BLUE_DARK
#define LIGHT_CHANNEL_8 LIGHT_PINK

class LightTheme : public AbstractColors
{
    Q_OBJECT
public:
    explicit LightTheme(QObject *parent = nullptr);

    QString getThemeName();
    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getAppBackgroundColor();
    QString getDataAreaColor();
    QString getControlBackgroundColor();
    QString getButtonBackgroundColor();
    QString getButtonDisabledBackgroundColor();
    QString getTextDarkColor();
    QString getTextLightColor();
    QString getControlsColor();
    QString getChannelColor(int channelIndex);
    QString getHoverColor();

private:

    QList<QColor> chanColor =
    {
        LIGHT_CHANNEL_1, LIGHT_CHANNEL_2,
        LIGHT_CHANNEL_3, LIGHT_CHANNEL_4,
        LIGHT_CHANNEL_5, LIGHT_CHANNEL_6,
        LIGHT_CHANNEL_7, LIGHT_CHANNEL_8
    };

signals:

};

#endif // LIGHTTHEME_H
