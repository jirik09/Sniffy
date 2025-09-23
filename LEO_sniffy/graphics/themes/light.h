#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"

#define LIGHT_GRAPHICS_PATH  ":/graphics/graphics/themes/light/"
#define LIGHT_FONT           "'MS Shell Dlg 2'"

/***************** COLORS *****************/

/* WINDOWS */
#define LIGHT_WINDOW_APP "#dfdfde"           //laft panel and dock widget window top
#define LIGHT_WINDOW_DOCK_WIDGET "#9c9c9c"   //actualy only undocked border color
#define LIGHT_WINDOW_WIDGET "#ebeaea"        // background of all widgets

/* COMPONENTS */
#define LIGHT_CONTROLS "#76829d"             //default color of controls
#define LIGHT_BACKGROUND_BUTTON "#dadee7"    //background of buttons which can be cliked including tabs and combo box
#define LIGHT_BACKGROUND_BUTTON_DISABLED "#d1d1d1" //background of disabled controls

/* TEXT */
#define LIGHT_TEXT_ALL "#1e1f1f"
#define LIGHT_TEXT_LABEL "#838383"
#define LIGHT_TEXT_COMPONENT "#1a1a1a"
#define LIGHT_TEXT_INVERTED "#eaeaed"

/* SPECIAL PURPOSE */
#define LIGHT_COMPONENT_DISABLED "#aaabac"       //background color of diabled components (text)
#define LIGHT_WINDOW_CONTROL_HOVER "#dadbe1"     //Control color of window when hovered dock/undock/scroll
#define LIGHT_WINDOW_EXIT_HOVER "#e16161"        //window exit hover color
#define LIGHT_ERROR "#ee4848"                    //error color
#define LIGHT_WARNING "#d3831b"                  //Warning or working color button
#define LIGHT_RUNNING "#1a9c25"                  //color of button when running
#define LIGHT_UNUSED "#7a7a7a"                   //color of button when its function is none or disable
#define LIGHT_BACKGROUND_FOCUS_IN "#eaeaea"      //background area of data inputs when focused in by mouse
#define LIGHT_DATA_INPUT_AREA "#f8f8f9"          //backgrounf of user text / number inputs

/* DISPLAY & CHART & GRID & LEGEND */
#define LIGHT_CHART "#f9f9f9"                    //chart background colors
#define LIGHT_CHART_GRIDLEG_DEFAULT "#141414"
#define LIGHT_CHART_GRIDLEG_LOW_CONTRAST "#9c9c9c"
#define LIGHT_DISPLAY LIGHT_CHART

/* COLORS */

#define LIGHT_BLUE   "#0068d3"
#define LIGHT_RED "#d4112f"
#define LIGHT_GREEN  "#129707"
#define LIGHT_ORANGE "#e77625"
#define LIGHT_PURPLE "#a841da"
#define LIGHT_BROWN "#af7a43"
#define LIGHT_YELLOW "#c89e00"
#define LIGHT_BLUE_DARK "#0800ad"
#define LIGHT_PINK "#ff6ba3"
//extend colors

/* CHANNELS */
#define LIGHT_CHANNEL_1 LIGHT_BLUE
#define LIGHT_CHANNEL_2 LIGHT_YELLOW
#define LIGHT_CHANNEL_3 LIGHT_RED
#define LIGHT_CHANNEL_4 LIGHT_GREEN
#define LIGHT_CHANNEL_5 LIGHT_PURPLE
#define LIGHT_CHANNEL_6 LIGHT_ORANGE
#define LIGHT_CHANNEL_7 LIGHT_BLUE_DARK
#define LIGHT_CHANNEL_8 LIGHT_PINK


/***************** GENERAL GLOBAL QT STYLE SHEETS *****************/

#define LIGHT_STYLE_USE_TRANSPARENCY true  // if true, the arg %1 must be in some defines (# removed in code)
#define LIGHT_STYLE_USE_CUSTOM_DIALS true

#define LIGHT_STYLE_GLOBAL           LIGHT_STYLE_OBJECT \
                                    LIGHT_STYLE_SPLITTER LIGHT_STYLE_SCROLLAREA LIGHT_STYLE_CHECK_BUTTON \
                                    LIGHT_STYLE_SCROLLBAR LIGHT_STYLE_MENU LIGHT_STYLE_TAB \
                                    LIGHT_STYLE_TOOLTIP LIGHT_STYLE_COMBO_BOX LIGHT_STYLE_LINEEDIT \
                                    LIGHT_STYLE_LIST_VIEW LIGHT_STYLE_PROGRESS_BAR LIGHT_STYLE_LABEL

#define LIGHT_STYLE_OBJECT \
"QObject{background-color:" LIGHT_WINDOW_APP ";font: 9pt" LIGHT_FONT ";color:" LIGHT_TEXT_ALL ";border:none;}"

#define LIGHT_STYLE_DOCK_WIDGET \
"QDockWidget{border: 3px solid; background-color:" LIGHT_WINDOW_DOCK_WIDGET ";}"

#define LIGHT_STYLE_WIDGET \
"QWidget{background-color:" LIGHT_WINDOW_WIDGET ";}"

#define LIGHT_STYLE_SPLITTER \
"QSplitter::handle{background-color:" LIGHT_BACKGROUND_BUTTON ";}"

#define LIGHT_STYLE_SCROLLAREA \
"QScrollArea{border:none}"

#define LIGHT_STYLE_SCROLLBAR \
"QScrollBar:vertical{border: none;width: 10px;margin: 0 0 0 0;}\
QScrollBar:horizontal{border: none;height: 10px;margin: 0 0 0 0;}\
QScrollBar::handle:vertical{border-radius: 4px;border: none;background-color:" LIGHT_BACKGROUND_BUTTON ";min-height: 38px;}\
QScrollBar::handle:horizontal {border-radius: 4px;border: none;background-color:" LIGHT_BACKGROUND_BUTTON ";min-width: 38px;}\
QScrollBar::add-line{height: 0px;border: none;background-color:" LIGHT_BACKGROUND_BUTTON ";}\
QScrollBar::sub-line{height: 0px;border: none;background-color:" LIGHT_BACKGROUND_BUTTON ";}\
QScrollBar::add-page,QScrollBar::sub-page{background-color:" LIGHT_DATA_INPUT_AREA";}\
QScrollBar:handle:hover{background-color:" LIGHT_CONTROLS ";}"

#define LIGHT_STYLE_LINEEDIT \
"QLineEdit::focus{border:none;background-color:" LIGHT_BACKGROUND_FOCUS_IN "}\
QLineEdit{border:none;background-color:" LIGHT_DATA_INPUT_AREA ";font:11pt " LIGHT_FONT ";padding:0px;margin:0px;}"

#define LIGHT_STYLE_MENU \
"QMenu::item{background-color:" LIGHT_DATA_INPUT_AREA ";}\
QMenu::item:selected{background-color:" LIGHT_WINDOW_CONTROL_HOVER ";}"

#define LIGHT_STYLE_TAB \
"QTabWidget::pane {border-top: 3px solid " LIGHT_BACKGROUND_BUTTON ";top:-3px;} QTabWidget::tab-bar {}\
QTabBar::tab {background-color:" LIGHT_BACKGROUND_BUTTON ";border-bottom: 3px solid rgb(128, 128, 128);padding: 7px;color: " LIGHT_TEXT_COMPONENT ";}\
QTabBar::tab:selected,QTabBar::tab:hover{background-color:" LIGHT_WINDOW_WIDGET ";}\
QTabBar::tab:selected{border-bottom-color:" LIGHT_WARNING ";}"

#define LIGHT_STYLE_TOOLTIP \
"QToolTip{border: 0px;color:" LIGHT_TEXT_ALL ";background-color:" LIGHT_WINDOW_APP ";}"

#define LIGHT_STYLE_LIST_VIEW \
"QListView{alternate-background-color:" LIGHT_WINDOW_WIDGET ";}"

#define LIGHT_STYLE_COMBO_BOX \
"QComboBox{background-color:" LIGHT_DATA_INPUT_AREA ";}\
QComboBox QAbstractItemView{selection-background-color:" LIGHT_WINDOW_CONTROL_HOVER ";background-color:" LIGHT_DATA_INPUT_AREA ";}"

#define LIGHT_STYLE_PUSH_BUTTON \
"QPushButton{padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}\
QPushButton{background-color:#FF%1;color:" LIGHT_TEXT_INVERTED ";}\
QPushButton:disabled{border:none;background-color:" LIGHT_BACKGROUND_BUTTON_DISABLED ";color:" LIGHT_COMPONENT_DISABLED ";}\
QPushButton:pressed{background-color: #88%1;border: 2px solid " LIGHT_WINDOW_WIDGET";}\
QPushButton:hover{background-color: #7B%1;}"

#define LIGHT_STYLE_CHECK_BUTTON \
"QPushButton{border:1px solid " LIGHT_CONTROLS ";padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}\
QPushButton{background-color:" LIGHT_BACKGROUND_BUTTON "; color: " LIGHT_TEXT_COMPONENT ";}\
QPushButton:checked{border:none; background-color:#FF%1; color:" LIGHT_TEXT_INVERTED ";}\
QPushButton:hover{background-color:#4C%1;}\
QPushButton:checked:hover{background-color:#BB%1;}\
QPushButton:disabled{border:none;background-color: " LIGHT_BACKGROUND_BUTTON_DISABLED "; color:" LIGHT_COMPONENT_DISABLED ";}"

#define LIGHT_STYLE_PROGRESS_BAR \
"QProgressBar{border:1px solid " LIGHT_TEXT_LABEL ";border-radius:1px;background-color:" LIGHT_DISPLAY ";}\
QProgressBar::chunk{width: 20px;background-color:#FF%1;}"

#define LIGHT_STYLE_LABEL "QLabel{border-radius: 2px;}"

/***************** CONCRETE STYLE SHEETS *****************/

#define LIGHT_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" LIGHT_WINDOW_APP ";qproperty-iconSize: 15px;padding: 5px}"

#define LIGHT_STYLE_MODULE_BUTTON \
"QPushButton {border:none;text-align:left;qproperty-iconSize: 32px;color: " LIGHT_TEXT_ALL "}\
QPushButton:disabled{color:" LIGHT_COMPONENT_DISABLED ";}\
QPushButton:hover{background-color:" LIGHT_BACKGROUND_BUTTON ";border:1px solid " LIGHT_CONTROLS ";}\
QPushButton:checked{background-color:" LIGHT_BACKGROUND_BUTTON ";}\
QPushButton:checked:hover{background-color:" LIGHT_BACKGROUND_BUTTON ";border:1px solid " LIGHT_CONTROLS ";}"

#define LIGHT_STYLE_HOLD_BUTTON \
"QPushButton{padding-top: 0px;padding-bottom:1px;padding-left:0px;padding-right:0px;font-size:10px;}\
QPushButton{background-color:" LIGHT_WINDOW_WIDGET ";color:" LIGHT_TEXT_LABEL ";}\
QPushButton{border-style:solid; border-color:" LIGHT_TEXT_LABEL ";border-width:1px;border-radius:1px;}\
QPushButton:checked{background-color:" LIGHT_WINDOW_WIDGET "; color: " LIGHT_WARNING ";}\
QPushButton:checked{border-style:solid;border-color:" LIGHT_WARNING ";border-width:1px;border-radius:1px;}\
QPushButton:hover{border-width:2px;}"

#define LIGHT_STYLE_DIAL LIGHT_STYLE_LINEEDIT \
"QWidget:disabled{color:" LIGHT_COMPONENT_DISABLED ";}\
QWidget{color:" LIGHT_TEXT_COMPONENT ";}\
QPushButton{font: 11pt" LIGHT_FONT ";}"

#define LIGHT_STYLE_TEXTINPUT \
"QWidget:disabled{ color:" LIGHT_COMPONENT_DISABLED ";}\
QWidget{font: 9pt" LIGHT_FONT "; color:" LIGHT_TEXT_COMPONENT ";}"

#define LIGHT_STYLE_MODULE_DOCK_WIDGET LIGHT_STYLE_WIDGET LIGHT_STYLE_DOCK_WIDGET

class Light : public AbstractTheme
{
    Q_OBJECT
public:
    explicit Light(QObject *parent = nullptr);

    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);
    ThemePalette buildPalette() override;

private:
    QList<QColor> chanColor =
    {
        LIGHT_CHANNEL_1, LIGHT_CHANNEL_2,
        LIGHT_CHANNEL_3, LIGHT_CHANNEL_4,
        LIGHT_CHANNEL_5, LIGHT_CHANNEL_6,
        LIGHT_CHANNEL_7, LIGHT_CHANNEL_8
    };
};

#endif // LIGHT_H
