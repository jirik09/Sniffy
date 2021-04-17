#ifndef DAWN_H
#define DAWN_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"
#include "../graphics.h"

#define DAWN_GRAPHICS_PATH  ":/graphics/graphics/themes/dawn/"
#define DAWN_FONT           "'MS Shell Dlg 2'"

/***************** COLORS *****************/

/* WINDOWS */
#define DAWN_WINDOW_APP "#0e131b"    //10151e        //laft panel and dock widget window top
#define DAWN_WINDOW_DOCK_WIDGET DAWN_WINDOW_WIDGET   //actualy only undocked border color
#define DAWN_WINDOW_WIDGET "#1e2739"                 // background of all widgets

/* COMPONENTS */
#define DAWN_CONTROLS DAWN_TEXT_ALL                  //default color of controls
#define DAWN_BACKGROUND_BUTTON "#141824"             //background of buttons which can be cliked including tabs and combo box
#define DAWN_BACKGROUND_BUTTON_DISABLED "#161a27"    //background of disabled controls

/* TEXT */
#define DAWN_TEXT_ALL "#a8abb5"
#define DAWN_TEXT_LABEL "#767984"
#define DAWN_TEXT_COMPONENT "#aeb1ba"

/* SPECIAL PURPOSE */
#define DAWN_COMPONENT_DISABLED "#5d606b"       //background color of diabled components
#define DAWN_WINDOW_CONTROL_HOVER "#2f323e"     //Control color of window when hovered dock/undock/scroll
#define DAWN_WINDOW_EXIT_HOVER "#9a2f36"        //window exit hover color
#define DAWN_ERROR "#ec3935"                    //error color
#define DAWN_WARNING DAWN_ORANGE                  //Warning or working color button
#define DAWN_RUNNING "#319c22"                  //color of button when running
#define DAWN_UNUSED "#434651"                   //color of button when its function is none or disable
#define DAWN_BACKGROUND_FOCUS_IN "#161b27"      //background area of data inputs when focused in by mouse
#define DAWN_DATA_INPUT_AREA "#121721" //"#141824" //backgrounf of user text / number inputs

/* DISPLAY & CHART & GRID & LEGEND */
#define DAWN_CHART DAWN_WINDOW_APP                   //chart background colors
#define DAWN_CHART_GRIDLEG_DEFAULT "#b0b4bd"
#define DAWN_CHART_GRIDLEG_LOW_CONTRAST "#6b6f79"
#define DAWN_DISPLAY DAWN_CHART

/* COLORS */
#define DAWN_BLUE "#3193e4"
#define DAWN_ORANGE "#fa961b"
#define DAWN_GREEN "#3aa79f"
#define DAWN_PURPLE "#a844b9"
#define DAWN_YELLOW "#f7c034"
#define DAWN_RED "#de5359"
#define DAWN_TURQUOISE "#43c7d9"
#define DAWN_PINK "#e63878"

/* CHANNELS */
#define DAWN_CHANNEL_1 DAWN_YELLOW
#define DAWN_CHANNEL_2 DAWN_BLUE
#define DAWN_CHANNEL_3 DAWN_GREEN
#define DAWN_CHANNEL_4 DAWN_RED
#define DAWN_CHANNEL_5 DAWN_ORANGE
#define DAWN_CHANNEL_6 DAWN_PURPLE
#define DAWN_CHANNEL_7 DAWN_PINK
#define DAWN_CHANNEL_8 DAWN_TURQUOISE


/***************** GENERAL GLOBAL QT STYLE SHEETS *****************/

#define DAWN_STYLE_USE_TRANSPARENCY true // arguments must define transparency ('#' removed in code)

#define DAWN_STYLE_GLOBAL           DAWN_STYLE_OBJECT \
                                    DAWN_STYLE_SPLITTER DAWN_STYLE_SCROLLAREA DAWN_STYLE_PUSH_BUTTON \
                                    DAWN_STYLE_SCROLLBAR DAWN_STYLE_MENU DAWN_STYLE_TAB \
                                    DAWN_STYLE_TOOLTIP DAWN_STYLE_COMBO_BOX DAWN_STYLE_LINEEDIT \
                                    DAWN_STYLE_LIST_VIEW DAWN_STYLE_PROGRESS_BAR DAWN_STYLE_LABEL

#define DAWN_STYLE_OBJECT \
"QObject{background-color:" DAWN_WINDOW_APP ";font: 10pt" DAWN_FONT ";color:" DAWN_TEXT_ALL ";border:none;}"

#define DAWN_STYLE_DOCK_WIDGET \
"QDockWidget{border: 3px solid; background-color:" DAWN_WINDOW_DOCK_WIDGET ";}"

#define DAWN_STYLE_WIDGET \
"QWidget{background-color:" DAWN_WINDOW_WIDGET ";}"

#define DAWN_STYLE_SPLITTER \
"QSplitter::handle{background-color:" DAWN_CHART ";}"

#define DAWN_STYLE_SCROLLAREA \
"QScrollArea{border:none}"

#define DAWN_STYLE_SCROLLBAR \
"QScrollBar:vertical{border: none;width: 10px;margin: 0 0 0 0;}\
QScrollBar:horizontal{border: none;height: 10px;margin: 0 0 0 0;}\
QScrollBar::handle:vertical{border-radius: 2px;border: none;background-color:" DAWN_CONTROLS ";min-height: 38px;}\
QScrollBar::handle:horizontal{border-radius: 2px;border: none;background-color:" DAWN_CONTROLS ";min-width: 38px;}\
QScrollBar::add-line{height: 0px;border: none;background-color:" DAWN_CONTROLS ";}\
QScrollBar::sub-line{height: 0px;border: none;background-color:" DAWN_CONTROLS ";}\
QScrollBar::add-page,QScrollBar::sub-page{background-color: #44a1a4ab;}\
QScrollBar:handle:hover{background-color: #bba1a4ab;}"

#define DAWN_STYLE_LINEEDIT \
"QLineEdit{border:none;background-color:" DAWN_DATA_INPUT_AREA ";font:12pt " DAWN_FONT ";padding:0px;margin:0px;}\
QLineEdit::focus{border:none;background-color:" DAWN_BACKGROUND_FOCUS_IN ";}"

#define DAWN_STYLE_MENU \
"QMenu::item{background-color:" DAWN_DATA_INPUT_AREA ";}\
QMenu::item:selected{background-color:" DAWN_WINDOW_CONTROL_HOVER ";}"

#define DAWN_STYLE_TAB \
"QTabWidget::pane {border-top: 3px solid " DAWN_BACKGROUND_BUTTON ";top:-3px;} QTabWidget::tab-bar {}\
QTabBar::tab {background-color:" DAWN_BACKGROUND_BUTTON ";border-bottom: 3px solid rgb(128, 128, 128);padding: 7px;color:" DAWN_TEXT_COMPONENT ";}\
QTabBar::tab:selected,QTabBar::tab:hover{background-color:" DAWN_WINDOW_CONTROL_HOVER ";}\
QTabBar::tab:selected{border-bottom-color: " DAWN_YELLOW ";}"

#define DAWN_STYLE_TOOLTIP \
"QToolTip{border: 0px;color:" DAWN_TEXT_ALL ";background-color:" DAWN_WINDOW_APP ";}"

#define DAWN_STYLE_LIST_VIEW \
"QListView{alternate-background-color: #262831;}"

#define DAWN_STYLE_COMBO_BOX \
"QComboBox{background-color:" DAWN_DATA_INPUT_AREA ";}\
QComboBox:hover{border: 1px solid " DAWN_COMPONENT_DISABLED ";}\
QComboBox QAbstractItemView{selection-background-color:" DAWN_WINDOW_CONTROL_HOVER ";background-color:" DAWN_DATA_INPUT_AREA ";}"

#define DAWN_STYLE_PUSH_BUTTON \
"QPushButton{font-weight : bold ;border-radius: 2px; padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}\
QPushButton{background-color:" DAWN_BACKGROUND_BUTTON ";color: #FF%1;}\
QPushButton:disabled{border:none;background-color:" DAWN_BACKGROUND_BUTTON_DISABLED ";color:" DAWN_COMPONENT_DISABLED ";}\
QPushButton:pressed{background-color: #88%1;}\
QPushButton:hover{background-color: #3C%1;}"

#define DAWN_STYLE_CHECK_BUTTON \
"QPushButton{font-weight : bold ;border-radius: 2px; padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}\
QPushButton{background-color:" DAWN_BACKGROUND_BUTTON "; color: #FF%1;}\
QPushButton:disabled{border:none;background-color: " DAWN_BACKGROUND_BUTTON_DISABLED "; color:" DAWN_COMPONENT_DISABLED ";}\
QPushButton:checked{border:none; background-color:#E4%1; color:" DAWN_WINDOW_WIDGET ";}\
QPushButton:hover{background-color:#2C%1;}\
QPushButton:checked:hover{background-color:#FF%1;}"

#define DAWN_STYLE_PROGRESS_BAR \
"QProgressBar{border:1px solid " DAWN_TEXT_LABEL ";border-radius:1px;background-color:" DAWN_DISPLAY ";}\
QProgressBar::chunk{width: 20px;background-color: #FF%1;}"

#define DAWN_STYLE_LABEL "QLabel{border-radius: 2px;}"

/***************** CONCRETE STYLE SHEETS *****************/

#define DAWN_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" DAWN_WINDOW_APP ";}"

#define DAWN_STYLE_MODULE_BUTTON \
"QPushButton {text-align:left; border: 3px solid " DAWN_WINDOW_APP "; border-radius:2px;}\
QPushButton:hover{text-align:left; border: 3px solid " DAWN_WINDOW_APP ";background:" DAWN_WINDOW_CONTROL_HOVER ";}\
QPushButton:checked{text-align:left; border: 3px solid " DAWN_WINDOW_APP ";background: #11FFFFFF; color: " DAWN_YELLOW ";}\
QPushButton:checked:hover{text-align:left; border: 3px solid " DAWN_WINDOW_APP "; background:" DAWN_WINDOW_CONTROL_HOVER ";}"

#define DAWN_STYLE_HOLD_BUTTON \
"QPushButton{padding-top: 0px;padding-bottom:1px;padding-left:0px;padding-right:0px;font-size:10px;}\
QPushButton{border: 1px solid #EEfa961b; background-color:" DAWN_WINDOW_APP ";color:" DAWN_TEXT_LABEL ";}\
QPushButton{border-color:" DAWN_TEXT_LABEL ";border-radius:1px;}\
QPushButton:checked{border:none;background-color:" DAWN_WARNING ";color:" DAWN_WINDOW_WIDGET ";}\
QPushButton:checked:hover{background-color:#FFfa961b;color:" DAWN_WINDOW_WIDGET ";}\
QPushButton:hover{background-color:#3Cfa961b;border-color:" DAWN_WARNING ";color:" DAWN_WARNING ";}"

#define DAWN_STYLE_DIAL DAWN_STYLE_LINEEDIT \
"QWidget:disabled{color:" DAWN_COMPONENT_DISABLED ";}\
QWidget{color:" DAWN_TEXT_COMPONENT ";}\
QPushButton{font: 12pt" DAWN_FONT ";}"

#define DAWN_STYLE_USE_CUSTOM_DIALS true

#define DAWN_STYLE_TEXTINPUT \
"QWidget:disabled{ color:" DAWN_COMPONENT_DISABLED ";}\
QWidget{font: 10pt" DAWN_FONT "; color:" DAWN_TEXT_COMPONENT ";}"

#define DAWN_STYLE_MODULE_DOCK_WIDGET DAWN_STYLE_WIDGET DAWN_STYLE_DOCK_WIDGET

class Dawn : public AbstractTheme
{
    Q_OBJECT
public:
    explicit Dawn(QObject *parent = nullptr);

    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);

private:
    QList<QColor> chanColor =
    {
        DAWN_CHANNEL_1, DAWN_CHANNEL_2,
        DAWN_CHANNEL_3, DAWN_CHANNEL_4,
        DAWN_CHANNEL_5, DAWN_CHANNEL_6,
        DAWN_CHANNEL_7, DAWN_CHANNEL_8
    };

};

#endif // DAWN_H
