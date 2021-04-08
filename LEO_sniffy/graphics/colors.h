#ifndef COLORS_H
#define COLORS_H

#include <QObject>
#include <QColor>

#include "graphics/abstractcolors.h"

#define BACKGROUND_COLOR_FOCUS_IN "#303030"

//Text colors
#define COLOR_GREY   "#7c7c7c"
#define COLOR_DARK_GREY   "#575757"
#define COLOR_WHITE  "#d6d6d6"

//Common
#define COLOR_BLACK "#000000"
#define COLOR_FULL_WHITE "#ffffff"
#define COLOR_BLUE   "#2fa0d0"
#define COLOR_ORANGE "#d2803c"
#define COLOR_GREEN "#2cb689"
#define COLOR_PURPLE "#8e54a1"
#define COLOR_YELLOW "#b89b00"
#define COLOR_RED "#bd4236"
#define COLOR_DARK_BLUE "#4762cf"
#define COLOR_PINK  "#cd72bf"

// Channel colors
#define CHANNEL_1_COLOR COLOR_BLUE
#define CHANNEL_2_COLOR COLOR_PURPLE
#define CHANNEL_3_COLOR COLOR_GREEN
#define CHANNEL_4_COLOR COLOR_YELLOW
#define CHANNEL_5_COLOR COLOR_ORANGE
#define CHANNEL_6_COLOR COLOR_RED
#define CHANNEL_7_COLOR COLOR_DARK_BLUE
#define CHANNEL_8_COLOR COLOR_PINK

#define TOP_CHANNEL_COLOR_INDEX  7

class Colors : public QObject
{
    Q_OBJECT
public:
    explicit Colors(QObject *parent = nullptr, AbstractColors *colorTheme = nullptr);

    static QString getAppGlobalStyle();
    static QString getGraphicsPath();

    static QString getAppBackgroundColor();
    static QString getDataAreaColor();
    static QString getControlBackgroundColor();
    static QString getButtonBackgroundColor();
    static QString getButtonDisabledBackgroundColor();
    static QString getTextDarkColor();
    static QString getTextLightColor();
    static QString getControlsColor();
    static QString getChannelColor(int channelIndex);    
    static QString getHoverColor();
};

#define IMAGE_UNITS_HZ          "image: url(:/graphics/graphics/units_hz.png); border: none;"
#define IMAGE_UNITS_SEC         "image: url(:/graphics/graphics/units_sec.png); border: none;"
#define IMAGE_UNITS_VOLT        "image: url(:/graphics/graphics/units_v.png); border: none;"
#define IMAGE_UNITS_VRIPPLE     "image: url(:/graphics/graphics/units_Vripple.png); border: none;"
#define IMAGE_UNITS_VMAX        "image: url(:/graphics/graphics/units_Vmax.png); border: none;"
#define IMAGE_UNITS_VMIN        "image: url(:/graphics/graphics/units_Vmin.png); border: none;"

#define IMAGE_SIGN_PERCENT      "image: url(:/graphics/graphics/units_perc.png); border: none;"
#define IMAGE_SIGN_ERR          "image: url(:/graphics/graphics/units_err.png); border: none;"
#define IMAGE_SIGN_ERRAVG       "image: url(:/graphics/graphics/units_erravg.png); border: none;"
#define IMAGE_SIGN_AVG          "image: url(:/graphics/graphics/units_avg.png); border: none;"
#define IMAGE_SIGN_PLSMNS       "image: url(:/graphics/graphics/sign_pm.png); border: none;"

#endif // COLORS_H
