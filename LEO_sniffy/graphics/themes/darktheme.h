#ifndef DARKTHEME_H
#define DARKTHEME_H

#include <QObject>
#include <QColor>

#include "../abstractcolors.h"

/* BACKGROUNDS */
#define DARK_BACKGROUND_APP QColor(48,48,48)
#define DARK_BACKGROUND_DATA_AREA QColor(38,38,38)
#define DARK_BACKGROUND_CONTROL QColor(58,58,58)
#define DARK_BACKGROUND_BUTTON QColor(38,38,38)
#define DARK_BACKGROUND_BUTTON_DISABLED QColor(48,48,48)

/* TEXT */
#define DARK_TEXT_DARK QColor(124,124,124)
#define DARK_TEXT_LIGHT QColor(214,214,214)

/* SPECIAL PURPOSE */
#define DARK_COLOR_HOVER QColor(71,76,94)

/* CONTROLS */
#define DARK_CONTROLS QColor(76,82,89)

/* COLORS */
#define DARK_FULL_BLACK QColor(0,0,0)
#define DARK_FULL_WHITE QColor(255,255,255)
#define DARK_BLUE QColor(47,160,208)
#define DARK_ORANGE QColor(210,128,60)
#define DARK_GREEN QColor(44,182,137)
#define DARK_PURPLE QColor(142,84,161)
#define DARK_YELLOW QColor(184,155,0)
#define DARK_RED QColor(189,66,54)
#define DARK_BLUE_DARK QColor(71,98,207)
#define DARK_PINK QColor(205,114,191)

/* CHANNELS */
#define DARK_CHANNEL_1 DARK_BLUE
#define DARK_CHANNEL_2 DARK_PURPLE
#define DARK_CHANNEL_3 DARK_GREEN
#define DARK_CHANNEL_4 DARK_YELLOW
#define DARK_CHANNEL_5 DARK_ORANGE
#define DARK_CHANNEL_6 DARK_RED
#define DARK_CHANNEL_7 DARK_BLUE_DARK
#define DARK_CHANNEL_8 DARK_PINK


class DarkTheme : public AbstractColors
{
    Q_OBJECT
public:
    explicit DarkTheme(QObject *parent = nullptr);

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
        DARK_CHANNEL_1, DARK_CHANNEL_2,
        DARK_CHANNEL_3, DARK_CHANNEL_4,
        DARK_CHANNEL_5, DARK_CHANNEL_6,
        DARK_CHANNEL_7, DARK_CHANNEL_8
    };

signals:

};

#endif // DARKTHEME_H
