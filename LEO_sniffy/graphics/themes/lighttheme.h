#ifndef LIGHTTHEME_H
#define LIGHTTHEME_H

#include <QObject>
#include <QColor>

#include "../abstractcolors.h"

/* BACKGROUNDS */
#define LIGHT_BACKGROUND_APP QColor(148,148,148)
#define LIGHT_BACKGROUND_DATA_AREA QColor(138,138,138)
#define LIGHT_BACKGROUND_CONTROL QColor(158,158,158)
#define LIGHT_BACKGROUND_BUTTON QColor(138,138,138)
#define LIGHT_BACKGROUND_BUTTON_DISABLED QColor(148,148,148)

/* TEXT */
#define LIGHT_TEXT_DARK QColor(124,124,124)
#define LIGHT_TEXT_LIGHT QColor(214,214,214)

/* SPECIAL PURPOSE */
#define LIGHT_COLOR_HOVER QColor(71,76,94)

/* CONTROLS */
#define LIGHT_CONTROLS_GREY QColor(95,95,95)

/* COLORS */
#define LIGHT_FULL_BLACK QColor(0,0,0)
#define LIGHT_FULL_WHITE QColor(255,255,255)
#define LIGHT_BLUE QColor(47,160,208)
#define LIGHT_ORANGE QColor(210,128,60)
#define LIGHT_GREEN QColor(44,182,137)
#define LIGHT_PURPLE QColor(142,84,161)
#define LIGHT_YELLOW QColor(184,155,0)
#define LIGHT_RED QColor(189,66,54)
#define LIGHT_BLUE_DARK QColor(71,98,207)
#define LIGHT_PINK QColor(205,114,191)

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

    QColor getAppBackgroundColor();
    QString getAppBackgroundColorString();

    QColor getDataAreaColor();
    QString getDataAreaColorString();

    QColor getControlBackgroundColor();
    QString getControlBackgroundColorString();

    QColor getButtonBackgroundColor();
    QString getButtonBackgroundColorString();

    QColor getButtonDisabledBackgroundColor();
    QString getButtonDisabledBackgroundColorString();

    QColor getTextDarkColor();
    QString getTextDarkColorString();

    QColor getTextLightColor();
    QString getTextLightColorString();

    QColor getControlsColor();
    QString getControlsColorString();

    QColor getChannelColor(int channelIndex);
    QString getChannelColorString(int channelIndex);

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
