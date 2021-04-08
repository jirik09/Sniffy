#ifndef DARKTHEME_H
#define DARKTHEME_H

#include <QObject>
#include <QColor>

#include "../abstractcolors.h"

/* BACKGROUNDS */
#define DARK_BACKGROUND_APP "#303030"
#define DARK_BACKGROUND_DATA_AREA "#262626"
#define DARK_BACKGROUND_CONTROL "#3a3a3a"
#define DARK_BACKGROUND_BUTTON "#262626"
#define DARK_BACKGROUND_BUTTON_DISABLED "#303030"
#define DARK_BACKGROUND_FOCUS_IN "#303030"

/* TEXT */
#define DARK_TEXT_DARK "#7c7c7c"
#define DARK_TEXT_LIGHT "#d6d6d6"

/* SPECIAL PURPOSE */
#define DARK_HOVER "#474c5e"

/* CONTROLS */
#define DARK_CONTROLS "#4c5259"

/* COLORS */
#define DARK_FULL_BLACK "#000000"
#define DARK_FULL_WHITE "#ffffff"
#define DARK_BLUE "#2fa0d0"
#define DARK_ORANGE "#d2803c"
#define DARK_GREEN "#2cb689"
#define DARK_PURPLE "#b45464"
#define DARK_YELLOW "#b89b00"
#define DARK_RED "#bd4236"
#define DARK_BLUE_DARK "#4762cf"
#define DARK_PINK "#cd72bf"

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
        DARK_CHANNEL_1, DARK_CHANNEL_2,
        DARK_CHANNEL_3, DARK_CHANNEL_4,
        DARK_CHANNEL_5, DARK_CHANNEL_6,
        DARK_CHANNEL_7, DARK_CHANNEL_8
    };
};

#define DARK_GRAPHICS_PATH              ":/graphics/graphics/"

/* GLOBAL APP STYLESHEET */
#define DARK_STYLE_GLOBAL               DARK_STYLE_OBJECT DARK_STYLE_SPLITTER \
                                        DARK_STYLE_BUTTON DARK_STYLE_SCROLL_GLOBAL \
                                        DARK_STYLE_HOVER

#define DARK_STYLE_OBJECT               "QObject{background-color: " DARK_BACKGROUND_APP ";font: 10pt 'MS Shell Dlg 2';color: rgb(228, 228, 228);border:none;}"
#define DARK_STYLE_SPLITTER             "QSplitter::handle{background-color:" DARK_BACKGROUND_BUTTON ";}"
#define DARK_STYLE_BUTTON               "QPushButton{padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}"

#define DARK_STYLE_SCROLL_GLOBAL        DARK_STYLE_SCROLLAREA DARK_STYLE_SCROLLBAR_VER DARK_STYLE_SCROLLBAR_HOR \
                                        DARK_STYLE_SCROLLBAR_HVER DARK_STYLE_SCROLLBAR_HHOR DARK_STYLE_SCROLLBAR_ADD_LINE \
                                        DARK_STYLE_SCROLLBAR_SUB_LINE DARK_STYLE_SCROLLBAR_UP_ARROW DARK_STYLE_SCROLLBAR_ADD_PAGE

#define DARK_STYLE_SCROLLAREA           "QScrollArea {border:none}"
#define DARK_STYLE_SCROLLBAR_VER        "QScrollBar:vertical {border: none;background:" DARK_BACKGROUND_FOCUS_IN ";width: 10px;margin: 0 0 0 0;}"
#define DARK_STYLE_SCROLLBAR_HOR        "QScrollBar:horizontal {border: none;background:" DARK_BACKGROUND_FOCUS_IN ";height: 10px;margin: 0 0 0 0;}"
#define DARK_STYLE_SCROLLBAR_HVER       "QScrollBar::handle:vertical {border: none;background:" DARK_CONTROLS ";min-height: 38px;}"
#define DARK_STYLE_SCROLLBAR_HHOR       "QScrollBar::handle:horizontal {border: none;background:" DARK_CONTROLS ";min-width: 38px;}"
#define DARK_STYLE_SCROLLBAR_ADD_LINE   "QScrollBar::add-line {height: 0px;}"
#define DARK_STYLE_SCROLLBAR_SUB_LINE   "QScrollBar::sub-line {height: 0px;}"
#define DARK_STYLE_SCROLLBAR_UP_ARROW   "QScrollBar::up-arrow, QScrollBar::down-arrow {background: none;}"
#define DARK_STYLE_SCROLLBAR_ADD_PAGE   "QScrollBar::add-page, QScrollBar::sub-page {background: none;}"

#define DARK_STYLE_HOVER                "QMenu::item{background-color: " DARK_BACKGROUND_BUTTON ";}\
                                         QMenu::item:selected{background-color: " DARK_HOVER ";}"

#endif // DARKTHEME_H
