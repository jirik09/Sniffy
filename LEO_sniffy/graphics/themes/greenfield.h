#ifndef GREENFIELD_H
#define GREENFIELD_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"
struct ThemePalette; // forward declaration

#define GREENFIELD_GRAPHICS_PATH  ":/graphics/graphics/themes/greenfield/"
#define GREENFIELD_FONT           "'Montserrat'"

/***************** COLORS (Greenfield – forest at dusk) *****************/

/* WINDOWS */
#define GREENFIELD_WINDOW_APP "#0A3323"              // deep forest – chrome, sidebar, footer
#define GREENFIELD_WINDOW_WIDGET "#1a372e"            // dark forest – content area background
#define GREENFIELD_WINDOW_DOCK_WIDGET "#0A3323"       // deep forest – undocked dock border

/* TEXT */
#define GREENFIELD_TEXT_ALL "#F7F4D5"                 // cream – primary text
#define GREENFIELD_TEXT_LABEL "#839958"               // olive sage – secondary / label text
#define GREENFIELD_TEXT_COMPONENT "#EDE8CB"           // light cream – input component text

/* COMPONENTS */
#define GREENFIELD_CONTROLS "#f7f4d5"                 // dusty rose – accent, CTA, highlights
#define GREENFIELD_BACKGROUND_BUTTON "#1c4134"        // forest – button and tab backgrounds
#define GREENFIELD_BACKGROUND_BUTTON_DISABLED "#132B23" // dim forest – disabled button bg
#define GREENFIELD_BACKGROUND_FOCUS_IN "rgb(30, 66, 55)"      // forest – focused input highlight
#define GREENFIELD_DATA_INPUT_AREA "#214d39"          // dark forest – text / number input background

/* SPECIAL PURPOSE */
#define GREENFIELD_COMPONENT_DISABLED "#3D6B55"       // muted forest – disabled text
#define GREENFIELD_WINDOW_CONTROL_HOVER "#224E3F"     // lighter forest – hover for controls
#define GREENFIELD_WINDOW_EXIT_HOVER "#E76F51"        // coral – exit hover
#define GREENFIELD_ERROR "#e85c4c"                    // coral – error indicator
#define GREENFIELD_WARNING "#c27b2a"                  // gold – warning indicator
#define GREENFIELD_RUNNING "#309b22"                  // teal – running indicator
#define GREENFIELD_UNUSED "#2B5E4A"                   // muted green – inactive / scrollbar handles

/* DISPLAY & CHART */
#define GREENFIELD_CHART "#112D24"                    // dark forest – chart background
#define GREENFIELD_CHART_GRIDLEG_DEFAULT "#839958"    // olive – grid lines and legend
#define GREENFIELD_CHART_GRIDLEG_LOW_CONTRAST "#2B5E4A" // subtle forest – minor grid
#define GREENFIELD_DISPLAY GREENFIELD_CHART

/* ACCENTS */
#define GREENFIELD_CYAN "#D3968C"
#define GREENFIELD_VIOLET "#6C5CE7"
#define GREENFIELD_LIME "#A855F7"
#define GREENFIELD_SUN "#E9C46A"
#define GREENFIELD_MAGENTA "#E76F51"
#define GREENFIELD_ORANGE "#F4A261"
#define GREENFIELD_RED "#E76F51"
#define GREENFIELD_GREEN "#839958"

/* CHANNEL COLORS – vivid, legible on dark forest */
#define GREENFIELD_CHANNEL_1 "#839958"
#define GREENFIELD_CHANNEL_2 "#f7f4d5"
#define GREENFIELD_CHANNEL_3 "#d3968c"
#define GREENFIELD_CHANNEL_4 "#105666"
#define GREENFIELD_CHANNEL_5 "#E9C46A"
#define GREENFIELD_CHANNEL_6 "#F4A261"
#define GREENFIELD_CHANNEL_7 "#E76F51"
#define GREENFIELD_CHANNEL_8 "#A855F7"

/* CHANNEL TEXT COLORS – per-channel override when checked */
#define GREENFIELD_CHANNEL_TEXT_1 "#105666"
#define GREENFIELD_CHANNEL_TEXT_2 "#839958"
#define GREENFIELD_CHANNEL_TEXT_3 "#f7f4d5"
#define GREENFIELD_CHANNEL_TEXT_4 "#d3968c"

/***************** GLOBAL QT STYLE SHEETS *****************/

#define GREENFIELD_STYLE_USE_TRANSPARENCY true
#define GREENFIELD_STYLE_USE_CUSTOM_DIALS true

#define GREENFIELD_STYLE_GLOBAL         GREENFIELD_STYLE_OBJECT \
                                    GREENFIELD_STYLE_SPLITTER GREENFIELD_STYLE_SCROLLAREA GREENFIELD_STYLE_PUSH_BUTTON \
                                    GREENFIELD_STYLE_SCROLLBAR GREENFIELD_STYLE_MENU GREENFIELD_STYLE_TAB \
                                    GREENFIELD_STYLE_TOOLTIP GREENFIELD_STYLE_COMBO_BOX GREENFIELD_STYLE_LINEEDIT \
                                    GREENFIELD_STYLE_LIST_VIEW GREENFIELD_STYLE_PROGRESS_BAR GREENFIELD_STYLE_LABEL

#define GREENFIELD_STYLE_OBJECT \
"QObject{background-color:" GREENFIELD_WINDOW_APP ";font: 9pt" GREENFIELD_FONT ";color:" GREENFIELD_TEXT_ALL ";border:none;}"

#define GREENFIELD_STYLE_DOCK_WIDGET \
"QDockWidget{border: 3px solid " GREENFIELD_WINDOW_APP "; background-color:" GREENFIELD_WINDOW_DOCK_WIDGET ";}"

#define GREENFIELD_STYLE_WIDGET \
"QWidget{background-color:" GREENFIELD_WINDOW_WIDGET ";}"

#define GREENFIELD_STYLE_SPLITTER \
"QSplitter::handle{background-color:" GREENFIELD_WINDOW_DOCK_WIDGET ";}"

#define GREENFIELD_STYLE_SCROLLAREA \
"QScrollArea{border:none}"

#define GREENFIELD_STYLE_SCROLLBAR \
"QScrollBar:vertical{border: none;width: 10px;margin: 0 0 0 0;}\
QScrollBar:horizontal{border: none;height: 10px;margin: 0 0 0 0;}\
QScrollBar::handle:vertical{border-radius: 4px;border: none;background-color:" GREENFIELD_UNUSED ";min-height: 38px;}\
QScrollBar::handle:horizontal{border-radius: 4px;border: none;background-color:" GREENFIELD_UNUSED ";min-width: 38px;}\
QScrollBar::add-line{height: 0px;border: none;}\
QScrollBar::sub-line{height: 0px;border: none;}\
QScrollBar::add-page,QScrollBar::sub-page{background-color:" GREENFIELD_BACKGROUND_BUTTON ";}\
QScrollBar:handle:hover{background-color:#3D6B55;}"

#define GREENFIELD_STYLE_LINEEDIT \
"QLineEdit{border:none;background-color:" GREENFIELD_DATA_INPUT_AREA ";font:11pt " GREENFIELD_FONT ";padding:0px;margin:0px;color:" GREENFIELD_TEXT_COMPONENT ";border-radius:4px;}\
QLineEdit::focus{border:none;background-color:" GREENFIELD_BACKGROUND_FOCUS_IN ";}"

#define GREENFIELD_STYLE_MENU \
"QMenu::item{background-color:" GREENFIELD_WINDOW_WIDGET ";color:" GREENFIELD_TEXT_ALL ";}\
QMenu::item:selected{background-color:" GREENFIELD_WINDOW_CONTROL_HOVER ";color:" GREENFIELD_TEXT_ALL ";}"

#define GREENFIELD_STYLE_TAB \
"QTabWidget::pane {border-top: 3px solid " GREENFIELD_BACKGROUND_BUTTON ";top:-3px;} QTabWidget::tab-bar {}\
QTabBar::tab {background-color:" GREENFIELD_BACKGROUND_BUTTON ";border:none;border-bottom: 3px solid " GREENFIELD_UNUSED ";padding: 7px;color:" GREENFIELD_TEXT_COMPONENT ";}\
QTabBar::tab:selected,QTabBar::tab:hover{background-color:" GREENFIELD_WINDOW_CONTROL_HOVER ";}\
QTabBar::tab:selected{border-bottom-color: " GREENFIELD_CONTROLS ";}"

#define GREENFIELD_STYLE_TOOLTIP \
"QToolTip{border: 0px;color:#D3968C;background-color:#105666;}"

#define GREENFIELD_STYLE_LIST_VIEW \
"QListView{alternate-background-color:" GREENFIELD_BACKGROUND_BUTTON ";}"

#define GREENFIELD_STYLE_COMBO_BOX \
"QComboBox{background-color:" GREENFIELD_DATA_INPUT_AREA ";border:none;border-radius:4px;padding:2px;color:" GREENFIELD_TEXT_COMPONENT ";}\
QComboBox:hover{background-color:" GREENFIELD_BACKGROUND_FOCUS_IN ";}\
QComboBox QAbstractItemView{selection-background-color:" GREENFIELD_WINDOW_CONTROL_HOVER ";background-color:" GREENFIELD_WINDOW_WIDGET ";}\
QComboBox:disabled{background-color:" GREENFIELD_BACKGROUND_BUTTON_DISABLED ";color:" GREENFIELD_COMPONENT_DISABLED ";}"

#define GREENFIELD_STYLE_PUSH_BUTTON \
"QPushButton{font-weight:bold;border:none;border-radius:4px;padding-top:4px;padding-bottom:4px;padding-left:8px;padding-right:8px;}\
QPushButton{background-color:#FF%1;color:" GREENFIELD_TEXT_ALL ";}\
QPushButton:disabled{border:none;background-color:" GREENFIELD_BACKGROUND_BUTTON_DISABLED ";color:" GREENFIELD_COMPONENT_DISABLED ";}\
QPushButton:pressed{background-color:#88%1;border:none;}\
QPushButton:hover{background-color:#CC%1;}"

#define GREENFIELD_STYLE_CHECK_BUTTON \
"QPushButton{font-weight:bold;border:none;border-radius:4px;padding-top:4px;padding-bottom:4px;padding-left:8px;padding-right:8px;}\
QPushButton{background-color:" GREENFIELD_BACKGROUND_BUTTON ";color:" GREENFIELD_TEXT_COMPONENT ";}\
QPushButton:checked{border:none;background-color:#FF%1;color:" GREENFIELD_TEXT_ALL ";}\
QPushButton:hover{background-color:#4C%1;}\
QPushButton:checked:hover{background-color:#CC%1;}\
QPushButton:disabled{border:none;background-color:" GREENFIELD_BACKGROUND_BUTTON_DISABLED ";color:" GREENFIELD_COMPONENT_DISABLED ";}"

#define GREENFIELD_STYLE_PROGRESS_BAR \
"QProgressBar{border:none;border-radius:4px;background-color:" GREENFIELD_BACKGROUND_BUTTON_DISABLED ";color:" GREENFIELD_TEXT_COMPONENT ";text-align:center;}\
QProgressBar::chunk{width:20px;background-color:#FF%1;border-radius:4px;}"

#define GREENFIELD_STYLE_LABEL "QLabel{border:none;border-radius:2px;}"

/***************** CONCRETE STYLE SHEETS *****************/

#define GREENFIELD_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" GREENFIELD_WINDOW_APP ";qproperty-iconSize: 15px;padding: 6px;border-radius:4px;border:none;}\
QPushButton:hover{background-color:#1A3D31;}"

#define GREENFIELD_STYLE_MODULE_BUTTON \
"QPushButton{text-align:left;border:none;border-radius:4px;qproperty-iconSize: 36px;color:" GREENFIELD_TEXT_ALL ";}\
QPushButton:hover{background-color:" GREENFIELD_WINDOW_CONTROL_HOVER ";color:" GREENFIELD_TEXT_ALL ";}\
QPushButton:checked{background-color:" GREENFIELD_WINDOW_CONTROL_HOVER ";color:" GREENFIELD_GREEN ";}\
QPushButton:checked:hover{background-color:#2B5E4A;}\
QPushButton:disabled{color:" GREENFIELD_COMPONENT_DISABLED ";}"

#define GREENFIELD_STYLE_HOLD_BUTTON \
"QPushButton{font-weight:bold;padding-top:3px;padding-bottom:3px;padding-left:5px;padding-right:5px;font-size:10px;border-radius:3px;border:none;}\
QPushButton{background-color:" GREENFIELD_BACKGROUND_BUTTON ";color:" GREENFIELD_TEXT_LABEL ";}\
QPushButton:checked{background-color:" GREENFIELD_WARNING ";color:" GREENFIELD_WINDOW_APP ";}\
QPushButton:checked:hover{background-color:#F0D080;color:" GREENFIELD_WINDOW_APP ";}\
QPushButton:hover{background-color:" GREENFIELD_WINDOW_CONTROL_HOVER ";color:" GREENFIELD_WARNING ";}\
QPushButton:disabled{color:" GREENFIELD_COMPONENT_DISABLED ";}"

#define GREENFIELD_STYLE_DIAL GREENFIELD_STYLE_LINEEDIT \
"QWidget:disabled{color:" GREENFIELD_COMPONENT_DISABLED ";}\
QWidget{color:" GREENFIELD_TEXT_COMPONENT ";}\
QPushButton{font: 11pt" GREENFIELD_FONT ";border:none;}"

#define GREENFIELD_STYLE_TEXTINPUT \
"QWidget:disabled{ color:" GREENFIELD_COMPONENT_DISABLED ";}\
QWidget{font: 9pt" GREENFIELD_FONT "; color:" GREENFIELD_TEXT_COMPONENT ";}"

#define GREENFIELD_STYLE_MODULE_DOCK_WIDGET GREENFIELD_STYLE_WIDGET GREENFIELD_STYLE_DOCK_WIDGET

class Greenfield : public AbstractTheme
{
public:
    explicit Greenfield(QObject *parent = nullptr);

    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);
    ThemePalette buildPalette() override; // new palette builder

private:
    QList<QColor> chanColor =
    {
        GREENFIELD_CHANNEL_1, GREENFIELD_CHANNEL_2,
        GREENFIELD_CHANNEL_3, GREENFIELD_CHANNEL_4,
        GREENFIELD_CHANNEL_5, GREENFIELD_CHANNEL_6,
        GREENFIELD_CHANNEL_7, GREENFIELD_CHANNEL_8
    };

};

#endif // GREENFIELD_H
