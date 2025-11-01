#ifndef AURORA_H
#define AURORA_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"
struct ThemePalette; // forward declaration

#define AURORA_GRAPHICS_PATH  ":/graphics/graphics/themes/aurora/"
#define AURORA_FONT           "'Segoe UI'"

/***************** COLORS (Aurora) *****************/

/* WINDOWS */
#define AURORA_WINDOW_APP "#0b0f14"              // app chrome
#define AURORA_WINDOW_WIDGET "#111823"          // widgets background
#define AURORA_WINDOW_DOCK_WIDGET AURORA_WINDOW_WIDGET

/* TEXT */
#define AURORA_TEXT_ALL "#e6eaf2"
#define AURORA_TEXT_LABEL "#a5b1c2"
#define AURORA_TEXT_COMPONENT "#d1d7e0"

/* COMPONENTS */
#define AURORA_CONTROLS AURORA_TEXT_ALL
#define AURORA_BACKGROUND_BUTTON "#172233"
#define AURORA_BACKGROUND_BUTTON_DISABLED "#121a28"
#define AURORA_BACKGROUND_FOCUS_IN "#0f1724"
#define AURORA_DATA_INPUT_AREA "#0c121d"

/* SPECIAL PURPOSE */
#define AURORA_COMPONENT_DISABLED "#6b7380"
#define AURORA_WINDOW_CONTROL_HOVER "#22314a"
#define AURORA_WINDOW_EXIT_HOVER "#e34f4f"
#define AURORA_ERROR "#ff6b6b"
#define AURORA_WARNING "#ffb020"
#define AURORA_RUNNING "#2fd680"
#define AURORA_UNUSED "#3a475a"

/* DISPLAY & CHART */
#define AURORA_CHART "#0d121a"
#define AURORA_CHART_GRIDLEG_DEFAULT "#b7c1d1"
#define AURORA_CHART_GRIDLEG_LOW_CONTRAST "#6f7b8c"
#define AURORA_DISPLAY AURORA_CHART

/* ACCENTS */
#define AURORA_CYAN "#19c8ff"
#define AURORA_VIOLET "#8b6cf0"
#define AURORA_LIME "#5be58c"
#define AURORA_SUN "#ffc857"
#define AURORA_MAGENTA "#ff4dcc"
#define AURORA_ORANGE "#ff9f43"
#define AURORA_RED "#ff6b6b"

/* CHANNEL COLORS */
#define AURORA_CHANNEL_1 AURORA_CYAN
#define AURORA_CHANNEL_2 AURORA_VIOLET
#define AURORA_CHANNEL_3 AURORA_LIME
#define AURORA_CHANNEL_4 AURORA_RED
#define AURORA_CHANNEL_5 AURORA_ORANGE
#define AURORA_CHANNEL_6 AURORA_MAGENTA
#define AURORA_CHANNEL_7 "#67d4ff"
#define AURORA_CHANNEL_8 "#7f92ff"

/***************** GLOBAL QT STYLE SHEETS *****************/

#define AURORA_STYLE_USE_TRANSPARENCY true
#define AURORA_STYLE_USE_CUSTOM_DIALS true

#define AURORA_STYLE_GLOBAL           AURORA_STYLE_OBJECT \
                                    AURORA_STYLE_SPLITTER AURORA_STYLE_SCROLLAREA AURORA_STYLE_PUSH_BUTTON \
                                    AURORA_STYLE_SCROLLBAR AURORA_STYLE_MENU AURORA_STYLE_TAB \
                                    AURORA_STYLE_TOOLTIP AURORA_STYLE_COMBO_BOX AURORA_STYLE_LINEEDIT \
                                    AURORA_STYLE_LIST_VIEW AURORA_STYLE_PROGRESS_BAR AURORA_STYLE_LABEL

#define AURORA_STYLE_OBJECT \
"QObject{background-color:" AURORA_WINDOW_APP ";font: 9pt" AURORA_FONT ";color:" AURORA_TEXT_ALL ";border:none;}"

#define AURORA_STYLE_DOCK_WIDGET \
"QDockWidget{border: 3px solid; background-color:" AURORA_WINDOW_DOCK_WIDGET ";}"

#define AURORA_STYLE_WIDGET \
"QWidget{background-color:" AURORA_WINDOW_WIDGET ";}"

#define AURORA_STYLE_SPLITTER \
"QSplitter::handle{background-color:" AURORA_CHART ";}"

#define AURORA_STYLE_SCROLLAREA \
"QScrollArea{border:none}"

#define AURORA_STYLE_SCROLLBAR \
"QScrollBar:vertical{border: none;width: 10px;margin: 0 0 0 0;}\
QScrollBar:horizontal{border: none;height: 10px;margin: 0 0 0 0;}\
QScrollBar::handle:vertical{border-radius: 3px;border: none;background-color:" AURORA_TEXT_LABEL ";min-height: 38px;}\
QScrollBar::handle:horizontal{border-radius: 3px;border: none;background-color:" AURORA_TEXT_LABEL ";min-width: 38px;}\
QScrollBar::add-line{height: 0px;border: none;background-color:" AURORA_TEXT_LABEL ";}\
QScrollBar::sub-line{height: 0px;border: none;background-color:" AURORA_TEXT_LABEL ";}\
QScrollBar::add-page,QScrollBar::sub-page{background-color: #44a1a4ab;}\
QScrollBar:handle:hover{background-color: #bba1a4ab;}"

#define AURORA_STYLE_LINEEDIT \
"QLineEdit{border:none;background-color:" AURORA_DATA_INPUT_AREA ";font:11pt " AURORA_FONT ";padding:0px;margin:0px;}\
QLineEdit::focus{border:none;background-color:" AURORA_BACKGROUND_FOCUS_IN ";}"

#define AURORA_STYLE_MENU \
"QMenu::item{background-color:" AURORA_DATA_INPUT_AREA ";}\
QMenu::item:selected{background-color:" AURORA_WINDOW_CONTROL_HOVER ";}"

#define AURORA_STYLE_TAB \
"QTabWidget::pane {border-top: 3px solid " AURORA_BACKGROUND_BUTTON ";top:-3px;} QTabWidget::tab-bar {}\
QTabBar::tab {background-color:" AURORA_BACKGROUND_BUTTON ";border-bottom: 3px solid rgb(128, 128, 128);padding: 7px;color:" AURORA_TEXT_COMPONENT ";}\
QTabBar::tab:selected,QTabBar::tab:hover{background-color:" AURORA_WINDOW_CONTROL_HOVER ";}\
QTabBar::tab:selected{border-bottom-color: " AURORA_SUN ";}"

#define AURORA_STYLE_TOOLTIP \
"QToolTip{border: 0px;color:" AURORA_TEXT_ALL ";background-color:" AURORA_WINDOW_APP ";}"

#define AURORA_STYLE_LIST_VIEW \
"QListView{alternate-background-color: #1c2431;}"

#define AURORA_STYLE_COMBO_BOX \
"QComboBox{background-color:" AURORA_DATA_INPUT_AREA ";}\
QComboBox:hover{border: 1px solid " AURORA_COMPONENT_DISABLED ";}\
QComboBox QAbstractItemView{selection-background-color:" AURORA_WINDOW_CONTROL_HOVER ";background-color:" AURORA_DATA_INPUT_AREA ";}\
QComboBox:disabled{background-color:" AURORA_BACKGROUND_BUTTON_DISABLED ";color:" AURORA_COMPONENT_DISABLED ";}"

#define AURORA_STYLE_PUSH_BUTTON \
"QPushButton{font-weight : bold ;border-radius: 6px; padding-top: 6px;padding-bottom: 6px;padding-left: 10px;padding-right: 10px;}\
QPushButton{background-color:" AURORA_BACKGROUND_BUTTON ";color: #FF%1;}\
QPushButton:disabled{border:none;background-color:" AURORA_BACKGROUND_BUTTON_DISABLED ";color:" AURORA_COMPONENT_DISABLED ";}\
QPushButton:pressed{background-color: #88%1;border: 2px solid " AURORA_WINDOW_WIDGET ";}\
QPushButton:hover{background-color: #2b3a55;}"

#define AURORA_STYLE_CHECK_BUTTON \
"QPushButton{font-weight : bold ;border-radius: 6px; padding-top: 6px;padding-bottom: 6px;padding-left: 10px;padding-right: 10px;}\
QPushButton{background-color:" AURORA_BACKGROUND_BUTTON "; color: #FF%1;}\
QPushButton:checked{border:none; background-color:#E4%1; color:" AURORA_WINDOW_WIDGET ";}\
QPushButton:hover{background-color:#263650;}\
QPushButton:checked:hover{background-color:#FF%1;}\
QPushButton:disabled{border:none;background-color: " AURORA_BACKGROUND_BUTTON_DISABLED "; color:" AURORA_COMPONENT_DISABLED ";}"

#define AURORA_STYLE_PROGRESS_BAR \
"QProgressBar{border:1px solid " AURORA_TEXT_LABEL ";border-radius:2px;background-color:" AURORA_DISPLAY ";}\
QProgressBar::chunk{width: 20px;background-color: #FF%1;}"

#define AURORA_STYLE_LABEL "QLabel{border-radius: 2px;}"

/***************** CONCRETE STYLE SHEETS *****************/

#define AURORA_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" AURORA_WINDOW_APP ";qproperty-iconSize: 15px;padding: 6px;border-radius:6px}"

#define AURORA_STYLE_MODULE_BUTTON \
"QPushButton{text-align:left; border: 2px solid " AURORA_WINDOW_WIDGET "; border-radius:8px;qproperty-iconSize: 36px;}\
QPushButton:hover{text-align:left; border: 2px solid " AURORA_WINDOW_WIDGET ";background:" AURORA_WINDOW_CONTROL_HOVER ";color: #dcdde0;}\
QPushButton:checked{text-align:left; border: 2px solid " AURORA_WINDOW_WIDGET ";background: #11FFFFFF; color: " AURORA_SUN ";}\
QPushButton:checked:hover{text-align:left; border: 2px solid " AURORA_WINDOW_WIDGET "; background:" AURORA_WINDOW_CONTROL_HOVER ";}"

#define AURORA_STYLE_HOLD_BUTTON \
"QPushButton{font-weight:bold;padding-top: 4px;padding-bottom:4px;padding-left:6px;padding-right:6px;font-size:10px;border-radius:4px;}\
QPushButton{border: 1px solid #EEffb020; background-color:" AURORA_WINDOW_APP ";color:" AURORA_TEXT_LABEL ";}\
QPushButton{border-color:" AURORA_TEXT_LABEL ";}\
QPushButton:checked{border:none;background-color:" AURORA_WARNING ";color:" AURORA_WINDOW_WIDGET ";}\
QPushButton:checked:hover{background-color:#FFffb020;color:" AURORA_WINDOW_WIDGET ";}\
QPushButton:hover{background-color:#20304a;border-color:" AURORA_WARNING ";color:" AURORA_WARNING ";}"

#define AURORA_STYLE_DIAL AURORA_STYLE_LINEEDIT \
"QWidget:disabled{color:" AURORA_COMPONENT_DISABLED ";}\
QWidget{color:" AURORA_TEXT_COMPONENT ";}\
QPushButton{font: 11pt" AURORA_FONT ";}"

#define AURORA_STYLE_TEXTINPUT \
"QWidget:disabled{ color:" AURORA_COMPONENT_DISABLED ";}\
QWidget{font: 9pt" AURORA_FONT "; color:" AURORA_TEXT_COMPONENT ";}"

#define AURORA_STYLE_MODULE_DOCK_WIDGET AURORA_STYLE_WIDGET AURORA_STYLE_DOCK_WIDGET

class Aurora : public AbstractTheme
{
public:
    explicit Aurora(QObject *parent = nullptr);

    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);
    ThemePalette buildPalette() override; // new palette builder

private:
    QList<QColor> chanColor =
    {
        AURORA_CHANNEL_1, AURORA_CHANNEL_2,
        AURORA_CHANNEL_3, AURORA_CHANNEL_4,
        AURORA_CHANNEL_5, AURORA_CHANNEL_6,
        AURORA_CHANNEL_7, AURORA_CHANNEL_8
    };

};

#endif // AURORA_H
