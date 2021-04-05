#ifndef COLORS_H
#define COLORS_H

#include <QObject>
#include <QColor>

#include "graphics/abstractcolors.h"

//backgrounds
#define BACKGROUND_QCOLOR_DATA_AREA QColor(38,38,38)
#define BACKGROUND_QCOLOR_BUTTON QColor(38,38,38)
#define BACKGROUND_QCOLOR_CONTROL QColor(58,58,58)
#define BACKGROUND_QCOLOR_APP QColor(48,48,48)
#define BACKGROUND_QCOLOR_BUTTON_DISABLED QColor(48,48,48)

#define BACKGROUND_COLOR_DATA_AREA "rgb(38,38,38);"
#define BACKGROUND_COLOR_BUTTON "rgb(38,38,38);"
#define BACKGROUND_COLOR_CONTROL "rgb(58,58,58);"
#define BACKGROUND_COLOR_APP "rgb(48,48,48);"
#define BACKGROUND_COLOR_FOCUS_IN "rgb(48,48,48);"
#define BACKGROUND_COLOR_BUTTON_DISABLED "rgb(48,48,48);"

//Text colors
#define COLOR_GREY   "rgb(124,124,124);"
#define COLOR_DARK_GREY   "rgb(87,87,87);"
#define COLOR_WHITE  "rgb(214,214,214);"
#define QCOLOR_GREY   QColor(124,124,124)
#define QCOLOR_DARK_GREY   QColor(87,87,87)
#define QCOLOR_WHITE  QColor(214,214,214)

//Special purpose color
#define COLOR_HOVER "rgb(71,76,94);"

#define COLOR_COMMON_DARK_GREYBLUE "rgb(76,82,89);"
#define COLOR_COMMON_DARK_GREY "rgb(95,95,95);"
#define QCOLOR_COMMON_DARK_GREYBLUE QColor(76,82,89)
#define QCOLOR_COMMON_DARK_GREY QColor(95,95,95)

#define COLOR_COMMON_LIGHT_GREY "rgb(180,180,180);"
#define QCOLOR_COMMON_LIGHT_GREY QColor(180,180,180)

//Common
#define COLOR_BLACK "rgb(0,0,0);"
#define COLOR_FULL_WHITE "rgb(255,255,255);"

#define COLOR_BLUE   "rgb(47,160,208);"
#define COLOR_ORANGE "rgb(210,128,60);"
#define COLOR_GREEN "rgb(44,182,137);"
#define COLOR_PURPLE "rgb(142,84,161);"
#define COLOR_YELLOW "rgb(184,155,0);"
#define COLOR_RED "rgb(189,66,54);"
#define COLOR_DARK_BLUE "rgb(71,98,207);"
#define COLOR_PINK  "rgb(205,114,191)"

#define QCOLOR_BLACK QColor(0,0,0)
#define QCOLOR_FULL_WHITE QColor(255,255,255)

#define QCOLOR_BLUE   QColor(47,160,208)
#define QCOLOR_ORANGE QColor(210,128,60)
#define QCOLOR_GREEN QColor(44,182,137)
#define QCOLOR_PURPLE QColor(142,84,161)
#define QCOLOR_YELLOW QColor(184,155,0)
#define QCOLOR_RED    QColor(189,66,54)
#define QCOLOR_DARK_BLUE QColor(71,98,207)
#define QCOLOR_PINK  QColor(205,114,191)

// Channel colors
#define CHANNEL_1_COLOR COLOR_BLUE
#define CHANNEL_2_COLOR COLOR_PURPLE
#define CHANNEL_3_COLOR COLOR_GREEN
#define CHANNEL_4_COLOR COLOR_YELLOW
#define CHANNEL_5_COLOR COLOR_ORANGE
#define CHANNEL_6_COLOR COLOR_RED
#define CHANNEL_7_COLOR COLOR_DARK_BLUE
#define CHANNEL_8_COLOR COLOR_PINK

#define CHANNEL_1_QCOLOR QCOLOR_BLUE
#define CHANNEL_2_QCOLOR QCOLOR_PURPLE
#define CHANNEL_3_QCOLOR QCOLOR_GREEN
#define CHANNEL_4_QCOLOR QCOLOR_YELLOW
#define CHANNEL_5_QCOLOR QCOLOR_ORANGE
#define CHANNEL_6_QCOLOR QCOLOR_RED
#define CHANNEL_7_QCOLOR QCOLOR_DARK_BLUE
#define CHANNEL_8_QCOLOR QCOLOR_PINK

enum colorTheme{
    DARK = 0,
    LIGHT
};

#define TOP_CHANNEL_COLOR_INDEX  7

class Colors : public QObject
{
    Q_OBJECT
public:
    explicit Colors(QObject *parent = nullptr, AbstractColors *colorTheme = nullptr);

    static QColor getAppBackgroundColor();
    static QString getAppBackgroundColorString();

    static QColor getDataAreaColor();
    static QString getDataAreaColorString();

    static QColor getControlBackgroundColor();
    static QString getControlBackgroundColorString();

    static QColor getButtonBackgroundColor();
    static QString getButtonBackgroundColorString();

    static QColor getButtonDisabledBackgroundColor();
    static QString getButtonDisabledBackgroundColorString();

    static QColor getTextColor();
    static QColor getTextColorString();

    static QColor getTextDarkColor();
    static QString getTextDarkColorString();

    static QColor getTextLightColor();
    static QString getTextLightColorString();

    static QColor getControlsColor();
    static QString getControlsColorString();

    static QColor getChannelColor(int channelIndex);
    static QString getChannelColorString(int channelIndex);
};

#endif // COLORS_H
