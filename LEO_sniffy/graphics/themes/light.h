#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"
#include "../graphics.h"

/* BACKGROUNDS */
#define LIGHT_BACKGROUND_APP "#adadad"
#define LIGHT_BACKGROUND_WIDGET "#f0f0f0"
#define LIGHT_BACKGROUND_DATA_AREA "#ffffff"
#define LIGHT_BACKGROUND_CONTROL "#adadad"
#define LIGHT_BACKGROUND_BUTTON "#adadad"
#define LIGHT_BACKGROUND_BUTTON_DISABLED "#adadad"
#define LIGHT_BACKGROUND_FOCUS_IN "#ffffff"

/* TEXT */
#define LIGHT_TEXT_DARK "#7c7c7c"
#define LIGHT_TEXT_LIGHT "#d6d6d6"

/* SPECIAL PURPOSE */
#define LIGHT_HOVER "#e5f3ff"
#define LIGHT_COMPONENT_DISABLED "#b7b7b7"
#define LIGHT_ERROR "#bd4236"
#define LIGHT_WARNING "#d2803c"

/* CHART GRID & LEGEND */
#define LIGHT_CHART_DARK "#333333"
#define LIGHT_CHART_LIGHT "#878787"

/* CONTROLS */
#define LIGHT_CONTROLS "#4c5259"

/* COLORS */
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

class Light : public AbstractTheme
{
    Q_OBJECT
public:
    explicit Light(QObject *parent = nullptr);

    QString getThemeName();
    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);

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

#endif // LIGHT_H
