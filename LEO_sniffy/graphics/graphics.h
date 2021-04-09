#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QObject>
#include <QColor>

#include "graphics/abstractgraphics.h"

#define BACKGROUND_COLOR_FOCUS_IN "#303030"
#define COLOR_GREY   "#7c7c7c"
#define COLOR_DARK_GREY   "#575757"
#define COLOR_WHITE  "#d6d6d6"
#define COLOR_BLUE   "#2fa0d0"
#define COLOR_ORANGE "#d2803c"
#define COLOR_GREEN "#2cb689"
#define COLOR_RED "#bd4236"

#define TOP_CHANNEL_COLOR_INDEX  7

class Graphics : public QObject
{
    Q_OBJECT
public:
    explicit Graphics(QObject *parent = nullptr, AbstractGraphics *colorTheme = nullptr);

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

    static QString IMAGE_UNITS_HZ;
    static QString IMAGE_UNITS_SEC;
    static QString IMAGE_UNITS_VOLT;
    static QString IMAGE_UNITS_VRIPPLE;
    static QString IMAGE_UNITS_VMAX;
    static QString IMAGE_UNITS_VMIN;
    static QString IMAGE_SIGN_PERCENT;
    static QString IMAGE_SIGN_ERR;
    static QString IMAGE_SIGN_ERRAVG;
    static QString IMAGE_SIGN_AVG;
    static QString IMAGE_SIGN_PLSMNS;
};

#endif // GRAPHICS_H
