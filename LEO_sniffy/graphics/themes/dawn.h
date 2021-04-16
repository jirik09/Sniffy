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
#define DAWN_WINDOW_APP "#131722"           //laft panel and dock widget window top
#define DAWN_WINDOW_DOCK_WIDGET "#2a2e39"   //actualy only undocked border color
#define DAWN_WINDOW_WIDGET "#131722"        // background of all widgets

/* COMPONENTS */
#define DAWN_CONTROLS "#9598a1"             //default color of controls
#define DAWN_BACKGROUND_BUTTON "#1a2332"    //background of buttons which can be cliked including tabs and combo box
#define DAWN_BACKGROUND_BUTTON_DISABLED "#1a2332" //background of disabled controls

/* TEXT */
#define DAWN_TEXT_ALL "#a9acb6"
#define DAWN_TEXT_LABEL "#767984"
#define DAWN_TEXT_COMPONENT "#aeb1ba"

/* SPECIAL PURPOSE */
#define DAWN_COMPONENT_DISABLED "#131722"       //background color of diabled components
#define DAWN_WINDOW_CONTROL_HOVER "#172d48"     //Control color of window when hovered dock/undock/scroll
#define DAWN_WINDOW_EXIT_HOVER "#854b52"        //window exit hover color
#define DAWN_ERROR "#ec3935"                    //error color
#define DAWN_WARNING "#ef7c21"                  //Warning or working color button
#define DAWN_RUNNING "#92de5e"                  //color of button when running
#define DAWN_UNUSED "#434651"                   //color of button when its function is none or disable
#define DAWN_BACKGROUND_FOCUS_IN "#1a3351"      //background area of data inputs when focused in by mouse
#define DAWN_DATA_INPUT_AREA "#131722"          //backgrounf of user text / number inputs

/* DISPLAY & CHART & GRID & LEGEND */
#define DAWN_CHART "#151924"                    //chart background colors
#define DAWN_CHART_GRIDLEG_DEFAULT "#b0b4bd"
#define DAWN_CHART_GRIDLEG_LOW_CONTRAST "#6b6f79"
#define DAWN_DISPLAY DAWN_CHART

/* COLORS */
#define DAWN_BLUE "#3193e4"
#define DAWN_ORANGE "#ef7c21"
#define DAWN_GREEN "#3aa79f"
#define DAWN_PURPLE "#a844b9"
#define DAWN_YELLOW "#f7c034"
#define DAWN_RED "#de5359"
#define DAWN_TURQUOISE "#43c7d9"
#define DAWN_PINK "#e63878"

/* CHANNELS */
#define DAWN_CHANNEL_1 DAWN_BLUE
#define DAWN_CHANNEL_2 DAWN_YELLOW
#define DAWN_CHANNEL_3 DAWN_GREEN
#define DAWN_CHANNEL_4 DAWN_RED
#define DAWN_CHANNEL_5 DAWN_ORANGE
#define DAWN_CHANNEL_6 DAWN_PURPLE
#define DAWN_CHANNEL_7 DAWN_PINK
#define DAWN_CHANNEL_8 DAWN_TURQUOISE


/***************** GENERAL GLOBAL QT STYLE SHEETS *****************/

#define DAWN_STYLE_GLOBAL           DAWN_STYLE_OBJECT \
                                    DAWN_STYLE_SPLITTER DAWN_STYLE_SCROLLAREA DAWN_STYLE_PUSH_BUTTON \
                                    DAWN_STYLE_CHECK_BUTTON DAWN_STYLE_SCROLLBAR DAWN_STYLE_MENU \
                                    DAWN_STYLE_TAB DAWN_STYLE_TOOLTIP DAWN_STYLE_COMBO_BOX DAWN_STYLE_LINEEDIT \
                                    DAWN_STYLE_LIST_VIEW DAWN_STYLE_PROGRESS_BAR

#define DAWN_STYLE_OBJECT           "QObject{background-color:" DAWN_WINDOW_APP ";font: 10pt" DAWN_FONT ";color:" DAWN_TEXT_ALL ";border:none;}"

#define DAWN_STYLE_DOCK_WIDGET      "QDockWidget{border: 3px solid; background-color:" DAWN_WINDOW_DOCK_WIDGET ";}"

#define DAWN_STYLE_WIDGET           "QWidget{background-color:" DAWN_WINDOW_WIDGET ";}"

#define DAWN_STYLE_SPLITTER         "QSplitter::handle{background-color:" DAWN_CHART ";}"

#define DAWN_STYLE_SCROLLAREA       "QScrollArea{border:none}"

#define DAWN_STYLE_SCROLLBAR        "QScrollBar:vertical{border: none;width: 10px;margin: 0 0 0 0;}\
                                    QScrollBar:horizontal{border: none;height: 10px;margin: 0 0 0 0;}\
                                    QScrollBar::handle:vertical{border: none;background:" DAWN_CONTROLS ";min-height: 38px;}\
                                    QScrollBar::handle:horizontal {border: none;background:" DAWN_CONTROLS ";min-width: 38px;}\
                                    QScrollBar::add-line{height: 0px;border: none;background:" DAWN_CONTROLS ";}\
                                    QScrollBar::sub-line{height: 0px;border: none;background:" DAWN_CONTROLS ";}\
                                    QScrollBar::add-page,QScrollBar::sub-page{background:" DAWN_WINDOW_APP";}\
                                    QScrollBar:handle:hover{background:" DAWN_WINDOW_CONTROL_HOVER ";}"

#define DAWN_STYLE_LINEEDIT         "QLineEdit::focus{border:none;background-color:" DAWN_BACKGROUND_FOCUS_IN "}\
                                    QLineEdit{border:none;background-color:" DAWN_DATA_INPUT_AREA ";font:12pt " DAWN_FONT ";padding:0px;margin:0px;}"

#define DAWN_STYLE_PUSH_BUTTON      "QPushButton{padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}\
                                    QPushButton{border:none;background-color:" DAWN_CONTROLS ";color:" DAWN_TEXT_COMPONENT ";}\
                                    QPushButton:disabled{background-color:" DAWN_BACKGROUND_BUTTON_DISABLED ";color:" DAWN_COMPONENT_DISABLED ";}\
                                    QPushButton:pressed{border: 2px solid " DAWN_WINDOW_WIDGET ";}"

#define DAWN_STYLE_CHECK_BUTTON     "QPushButton{padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}\
                                    QPushButton:checked{border: none;background-color:" DAWN_CONTROLS ";color:" DAWN_TEXT_COMPONENT ";}\
                                    QPushButton{border: none;background-color:" DAWN_BACKGROUND_BUTTON ";}\
                                    QPushButton:disabled{background-color:" DAWN_BACKGROUND_BUTTON_DISABLED "; color:" DAWN_COMPONENT_DISABLED ";}"

#define DAWN_STYLE_MENU             "QMenu::item{background-color:" DAWN_DATA_INPUT_AREA ";}\
                                    QMenu::item:selected{background-color:" DAWN_WINDOW_CONTROL_HOVER ";}"

#define DAWN_STYLE_TAB              "QTabWidget::pane {border-top: 3px solid " DAWN_BACKGROUND_BUTTON ";top:-3px;} QTabWidget::tab-bar {}\
                                    QTabBar::tab {background:" DAWN_BACKGROUND_BUTTON ";border-bottom: 3px solid rgb(128, 128, 128);padding: 7px;color: " DAWN_TEXT_COMPONENT ";}\
                                    QTabBar::tab:selected,QTabBar::tab:hover{background:" DAWN_WINDOW_WIDGET ";}\
                                    QTabBar::tab:selected{border-bottom-color:" DAWN_WARNING ";}"

#define DAWN_STYLE_TOOLTIP          "QToolTip{border: 0px;color:" DAWN_TEXT_ALL ";background-color:" DAWN_WINDOW_APP ";}"

#define DAWN_STYLE_LIST_VIEW        "QListView{alternate-background-color:" DAWN_WINDOW_WIDGET ";}"

#define DAWN_STYLE_PROGRESS_BAR     "QProgressBar{border:1px solid " DAWN_TEXT_LABEL ";border-radius:1px;background:" DAWN_DISPLAY ";}\
                                    QProgressBar::chunk{width: 20px;background-color:" DAWN_CHANNEL_1 ";}"

#define DAWN_STYLE_COMBO_BOX        "QComboBox{background-color:" DAWN_DATA_INPUT_AREA ";}\
                                    QComboBox QAbstractItemView{selection-background-color:" DAWN_WINDOW_CONTROL_HOVER ";background-color:" DAWN_DATA_INPUT_AREA ";}"


/***************** CONCRETE STYLE SHEETS *****************/

#define DAWN_STYLE_CONTROL_BUTTON   "QPushButton{background-color:" DAWN_WINDOW_APP ";}"

#define DAWN_STYLE_MODULE_BUTTON    "QPushButton {text-align:left;}\
                                    QPushButton:hover{background-color:" DAWN_BACKGROUND_BUTTON ";text-align:left;border-radius:5px;}"

#define DAWN_STYLE_HOLD_BUTTON      "QPushButton{padding-top: 0px;padding-bottom:1px;padding-left:0px;padding-right:0px;font-size:10px;}\
                                    QPushButton{background-color:" DAWN_WINDOW_WIDGET ";color:" DAWN_TEXT_LABEL ";}\
                                    QPushButton{border-style:solid; border-color:" DAWN_TEXT_LABEL ";border-width:1px;border-radius:1px;}\
                                    QPushButton:checked{background-color:" DAWN_WINDOW_WIDGET "; color: " DAWN_WARNING ";}\
                                    QPushButton:checked{border-style:solid;border-color:" DAWN_WARNING ";border-width:1px;border-radius:1px;}\
                                    QPushButton:hover{border-width:2px;}"

#define DAWN_STYLE_DIAL             DAWN_STYLE_LINEEDIT \
                                    "QWidget:disabled{color:" DAWN_COMPONENT_DISABLED ";}\
                                    QWidget{color:" DAWN_TEXT_COMPONENT ";}\
                                    QPushButton{font: 12pt" DAWN_FONT ";}"
#define DAWN_STYLE_USE_CUSTOM_DIALS true

#define DAWN_STYLE_TEXTINPUT        "QWidget:disabled{ color:" DAWN_COMPONENT_DISABLED ";}QWidget{font: 10pt" DAWN_FONT "; color:" DAWN_TEXT_COMPONENT ";}"

#define DAWN_STYLE_MODULE_DOCK_WIDGET        DAWN_STYLE_WIDGET DAWN_STYLE_DOCK_WIDGET

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
