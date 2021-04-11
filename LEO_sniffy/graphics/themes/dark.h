#ifndef DARK_H
#define DARK_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"
#include "../graphics.h"

/* WINDOWS */
#define DARK_WINDOW_APP "#303030"
#define DARK_WINDOW_QDOCK_WIDGET "#6c6c6c"
#define DARK_WINDOW_WIDGET "#3a3a3a"

/* BACKGROUNDS */
#define DARK_BACKGROUND_BUTTON "#262626"
#define DARK_BACKGROUND_BUTTON_DISABLED "#303030"
#define DARK_BACKGROUND_FOCUS_IN "#303030"
#define DARK_DATA_AREA "#262626"

/* CONTROL COMPONENTS */
#define DARK_CONTROLS "#4c5259"

/* TEXT */
#define DARK_TEXT_ALL "#e4e4e4"
#define DARK_TEXT_LABEL "#7c7c7c"
#define DARK_TEXT_COMPONENT "#e4e4e4"

/* SPECIAL PURPOSE */
#define DARK_COMPONENT_DISABLED "#575757"
#define DARK_HOVER "#474c5e"
#define DARK_HOVER_EXIT "#783232"
#define DARK_ERROR "#bd4236"
#define DARK_WARNING "#d2803c"
#define DARK_UNINITIALIZED "#ffffff"

/* DISPLAY & CHART & GRID & LEGEND */
#define DARK_DISPLAY "#262626"
#define DARK_CHART "#262626"
#define DARK_CHART_GRIDLEG_DEFAULT "#ffffff"
#define DARK_CHART_GRIDLEG_LOW_CONTRAST "#7c7c7c"

/* COLORS */
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


class Dark : public AbstractTheme
{
    Q_OBJECT
public:
    explicit Dark(QObject *parent = nullptr);

    QString getThemeName();
    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);

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
#define DARK_STYLE_GLOBAL               DARK_STYLE_OBJECT DARK_STYLE_WIDGET DARK_STYLE_QDOCK_WIDGET \
                                        DARK_STYLE_SPLITTER DARK_STYLE_BUTTON DARK_STYLE_SCROLLAREA DARK_STYLE_SCROLLBAR \
                                        DARK_STYLE_QMENU

#define DARK_STYLE_OBJECT               "QObject{background-color:" DARK_WINDOW_APP ";font: 10pt 'MS Shell Dlg 2';color:" DARK_TEXT_ALL ";border:none;}"
#define DARK_STYLE_QDOCK_WIDGET         "QDockWidget{border: 3px solid; background-color:" DARK_WINDOW_QDOCK_WIDGET ";}"
#define DARK_STYLE_WIDGET               "QWidget{background-color:" DARK_WINDOW_APP ";}"
#define DARK_STYLE_SPLITTER             "QSplitter::handle{background-color:" DARK_CHART ";}"
#define DARK_STYLE_BUTTON               "QPushButton{padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}"
#define DARK_STYLE_SCROLLAREA           "QScrollArea {border:none}"
#define DARK_STYLE_SCROLLBAR            "QScrollBar:vertical {border: none;background:" DARK_BACKGROUND_FOCUS_IN ";width: 10px;margin: 0 0 0 0;}\
                                         QScrollBar:horizontal {border: none;background:" DARK_BACKGROUND_FOCUS_IN ";height: 10px;margin: 0 0 0 0;}\
                                         QScrollBar::handle:vertical {border: none;background:" DARK_CONTROLS ";min-height: 38px;}\
                                         QScrollBar::handle:horizontal {border: none;background:" DARK_CONTROLS ";min-width: 38px;}\
                                         QScrollBar::add-line {height: 0px;}\
                                         QScrollBar::sub-line {height: 0px;}\
                                         QScrollBar::up-arrow, QScrollBar::down-arrow {background: none;}\
                                         QScrollBar::add-page, QScrollBar::sub-page {background: none;}\
                                         QScrollBar:handle:hover{background:" DARK_HOVER ";}"
#define DARK_STYLE_QMENU                "QMenu::item{background-color:" DARK_BACKGROUND_BUTTON ";}\
                                         QMenu::item:selected{background-color:" DARK_HOVER ";}"

#endif // DARK_H
