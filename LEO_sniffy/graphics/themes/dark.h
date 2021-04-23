#ifndef DARK_H
#define DARK_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"
#include "../graphics.h"

#define DARK_GRAPHICS_PATH  ":/graphics/graphics/themes/dark/"
#define DARK_FONT           "'MS Shell Dlg 2'"

/***************** COLORS *****************/

/* WINDOWS */
#define DARK_WINDOW_APP "#303030"           //laft panel and dock widget window top
#define DARK_WINDOW_DOCK_WIDGET "#7c7c7c"   //actualy only undocked border color
#define DARK_WINDOW_WIDGET "#3a3a3a"        // background of all widgets

/* COMPONENTS */
#define DARK_CONTROLS "#8b9299"             //default color of controls
#define DARK_BACKGROUND_BUTTON "#262626"    //background of buttons which can be cliked including tabs and combo box
#define DARK_BACKGROUND_BUTTON_DISABLED "#303030" //background of disabled controls

/* TEXT */
#define DARK_TEXT_ALL "#e4e4e4"
#define DARK_TEXT_LABEL "#7c7c7c"
#define DARK_TEXT_COMPONENT "#e4e4e4"

/* SPECIAL PURPOSE */
#define DARK_COMPONENT_DISABLED "#575757"       //background color of diabled components
#define DARK_WINDOW_CONTROL_HOVER "#474c5e"     //Control color of window when hovered dock/undock/scroll
#define DARK_WINDOW_EXIT_HOVER "#783232"        //window exit hover color
#define DARK_ERROR "#bd4236"                    //error color
#define DARK_WARNING "#d2803c"                  //Warning or working color button
#define DARK_RUNNING "#1a9c25"                  //color of button when running
#define DARK_UNUSED "#7a7a7a"                   //color of button when its function is none or disable
#define DARK_BACKGROUND_FOCUS_IN "#303030"      //background area of data inputs when focused in by mouse
#define DARK_DATA_INPUT_AREA "#282828"          //backgrounf of user text / number inputs

/* DISPLAY & CHART & GRID & LEGEND */
#define DARK_CHART "#262626"                    //chart background colors
#define DARK_CHART_GRIDLEG_DEFAULT "#e4e4e4"
#define DARK_CHART_GRIDLEG_LOW_CONTRAST "#7c7c7c"
#define DARK_DISPLAY DARK_CHART

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


/***************** GENERAL GLOBAL QT STYLE SHEETS *****************/

#define DARK_STYLE_USE_TRANSPARENCY false  // if true, the arg %1 must be in some defines (# removed in code)
#define DARK_STYLE_USE_CUSTOM_DIALS true

#define DARK_STYLE_GLOBAL           DARK_STYLE_OBJECT \
                                    DARK_STYLE_SPLITTER DARK_STYLE_SCROLLAREA DARK_STYLE_CHECK_BUTTON \
                                    DARK_STYLE_SCROLLBAR DARK_STYLE_MENU DARK_STYLE_TAB \
                                    DARK_STYLE_TOOLTIP DARK_STYLE_COMBO_BOX DARK_STYLE_LINEEDIT \
                                    DARK_STYLE_LIST_VIEW DARK_STYLE_PROGRESS_BAR

#define DARK_STYLE_OBJECT \
"QObject{background-color:" DARK_WINDOW_APP ";font: 10pt" DARK_FONT ";color:" DARK_TEXT_ALL ";border:none;}"

#define DARK_STYLE_DOCK_WIDGET \
"QDockWidget{border: 3px solid; background-color:" DARK_WINDOW_DOCK_WIDGET ";}"

#define DARK_STYLE_WIDGET \
"QWidget{background-color:" DARK_WINDOW_WIDGET ";}"

#define DARK_STYLE_SPLITTER \
"QSplitter::handle{background-color:" DARK_CHART ";}"

#define DARK_STYLE_SCROLLAREA \
"QScrollArea{border:none}"

#define DARK_STYLE_SCROLLBAR \
"QScrollBar:vertical{border: none;width: 10px;margin: 0 0 0 0;}\
QScrollBar:horizontal{border: none;height: 10px;margin: 0 0 0 0;}\
QScrollBar::handle:vertical{border: none;background-color:" DARK_CONTROLS ";min-height: 38px;}\
QScrollBar::handle:horizontal {border: none;background-color:" DARK_CONTROLS ";min-width: 38px;}\
QScrollBar::add-line{height: 0px;border: none;background-color:" DARK_CONTROLS ";}\
QScrollBar::sub-line{height: 0px;border: none;background-color:" DARK_CONTROLS ";}\
QScrollBar::add-page,QScrollBar::sub-page{background-color:" DARK_WINDOW_APP";}\
QScrollBar:handle:hover{background-color:" DARK_WINDOW_CONTROL_HOVER ";}"

#define DARK_STYLE_LINEEDIT \
"QLineEdit::focus{border:none;background-color:" DARK_BACKGROUND_FOCUS_IN "}\
QLineEdit{border:none;background-color:" DARK_DATA_INPUT_AREA ";font:12pt " DARK_FONT ";padding:0px;margin:0px;}"

#define DARK_STYLE_MENU \
"QMenu::item{background-color:" DARK_DATA_INPUT_AREA ";}\
QMenu::item:selected{background-color:" DARK_WINDOW_CONTROL_HOVER ";}"

#define DARK_STYLE_TAB \
"QTabWidget::pane {border-top: 3px solid " DARK_BACKGROUND_BUTTON ";top:-3px;} QTabWidget::tab-bar {}\
QTabBar::tab {background-color:" DARK_BACKGROUND_BUTTON ";border-bottom: 3px solid rgb(128, 128, 128);padding: 7px;color: " DARK_TEXT_COMPONENT ";}\
QTabBar::tab:selected,QTabBar::tab:hover{background-color:" DARK_WINDOW_WIDGET ";}\
QTabBar::tab:selected{border-bottom-color:" DARK_WARNING ";}"

#define DARK_STYLE_TOOLTIP \
"QToolTip{border: 0px;color:" DARK_TEXT_ALL ";background-color:" DARK_WINDOW_APP ";}"

#define DARK_STYLE_LIST_VIEW \
"QListView{alternate-background-color:" DARK_WINDOW_WIDGET ";}"

#define DARK_STYLE_COMBO_BOX \
"QComboBox{background-color:" DARK_DATA_INPUT_AREA ";}\
QComboBox QAbstractItemView{selection-background-color:" DARK_WINDOW_CONTROL_HOVER ";background-color:" DARK_DATA_INPUT_AREA ";}"

#define DARK_STYLE_PUSH_BUTTON \
"QPushButton{padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}\
QPushButton{border:none;background-color: %1;color:" DARK_TEXT_COMPONENT ";}\
QPushButton:disabled{background-color:" DARK_BACKGROUND_BUTTON_DISABLED ";color:" DARK_COMPONENT_DISABLED ";}\
QPushButton:pressed{border: 2px solid " DARK_WINDOW_WIDGET ";}"

#define DARK_STYLE_CHECK_BUTTON \
"QPushButton{padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}\
QPushButton:checked{border: none;background-color:%1; color:" DARK_TEXT_COMPONENT ";}\
QPushButton{border: none;background-color:" DARK_BACKGROUND_BUTTON ";}\
QPushButton:disabled{background-color:" DARK_BACKGROUND_BUTTON_DISABLED "; color:" DARK_COMPONENT_DISABLED ";}"

#define DARK_STYLE_PROGRESS_BAR \
"QProgressBar{border:1px solid " DARK_TEXT_LABEL ";border-radius:1px;background-color:" DARK_DISPLAY ";}\
QProgressBar::chunk{width: 20px;background-color:%1;}"

/***************** CONCRETE STYLE SHEETS *****************/

#define DARK_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" DARK_WINDOW_APP ";qproperty-iconSize: 15px;padding: 5px}"

#define DARK_STYLE_MODULE_BUTTON \
"QPushButton {text-align:left;qproperty-iconSize: 35px;}\
QPushButton:hover{background-color:" DARK_BACKGROUND_BUTTON ";border-radius:5px;}"

#define DARK_STYLE_HOLD_BUTTON \
"QPushButton{padding-top: 0px;padding-bottom:1px;padding-left:0px;padding-right:0px;font-size:10px;}\
QPushButton{background-color:" DARK_WINDOW_WIDGET ";color:" DARK_TEXT_LABEL ";}\
QPushButton{border-style:solid; border-color:" DARK_TEXT_LABEL ";border-width:1px;border-radius:1px;}\
QPushButton:checked{background-color:" DARK_WINDOW_WIDGET "; color: " DARK_WARNING ";}\
QPushButton:checked{border-style:solid;border-color:" DARK_WARNING ";border-width:1px;border-radius:1px;}\
QPushButton:hover{border-width:2px;}"

#define DARK_STYLE_DIAL DARK_STYLE_LINEEDIT \
"QWidget:disabled{color:" DARK_COMPONENT_DISABLED ";}\
QWidget{color:" DARK_TEXT_COMPONENT ";}\
QPushButton{font: 12pt" DARK_FONT ";}"

#define DARK_STYLE_TEXTINPUT \
"QWidget:disabled{ color:" DARK_COMPONENT_DISABLED ";}\
QWidget{font: 10pt" DARK_FONT "; color:" DARK_TEXT_COMPONENT ";}"

#define DARK_STYLE_MODULE_DOCK_WIDGET DARK_STYLE_WIDGET DARK_STYLE_DOCK_WIDGET

class Dark : public AbstractTheme
{
    Q_OBJECT
public:
    explicit Dark(QObject *parent = nullptr);

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

#endif // DARK_H
