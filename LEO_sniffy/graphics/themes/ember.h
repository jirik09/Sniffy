#ifndef EMBER_H
#define EMBER_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"

#define EMBER_GRAPHICS_PATH  ":/graphics/graphics/themes/ember/"
#define EMBER_FONT           "'Montserrat'"

/***************** COLORS *****************/

/* BASE PALETTE */
#define EMBER_BLUE          "#00ccff"
#define EMBER_LIGHT_BLUE    "#81b1ed"
#define EMBER_YELLOW        "#f5a623"
#define EMBER_LIGHT_BROWN   "#252525" // Using slate-umber 
#define EMBER_BROWN         "#181818" // Slate-umber dark

/* DERIVED SHADES */
#define EMBER_BROWN_DARK    "#101010" // Deepest background
#define EMBER_BROWN_MID     "#343434" // Borders, lighter hovers
#define EMBER_BLUE_DARK     "#2a588f"
#define EMBER_BLUE_LIGHT    "#5e9ce6"
#define EMBER_YELLOW_DIM    "#c28011"

/* WINDOWS */
#define EMBER_WINDOW_APP        EMBER_BROWN_DARK
#define EMBER_WINDOW_DOCK_WIDGET EMBER_BROWN_DARK
#define EMBER_WINDOW_WIDGET     EMBER_BROWN_DARK

/* COMPONENTS */
#define EMBER_CONTROLS          "#b0aaa6"
#define EMBER_BACKGROUND_BUTTON         EMBER_LIGHT_BROWN
#define EMBER_BACKGROUND_BUTTON_DISABLED "#1e1e1e"

/* TEXT */
#define EMBER_TEXT_ALL          "#e8e0dc"
#define EMBER_TEXT_LABEL        "#c4b8b2"
#define EMBER_TEXT_COMPONENT    "#fcfaf9"

/* SPECIAL PURPOSE */
#define EMBER_COMPONENT_DISABLED    "#8e8886"
#define EMBER_WINDOW_CONTROL_HOVER  "#414141"
#define EMBER_WINDOW_EXIT_HOVER     "#e54545"
#define EMBER_ERROR                 "#e04a43"
#define EMBER_WARNING               EMBER_YELLOW
#define EMBER_RUNNING               "#4caf50"
#define EMBER_UNUSED                "#414141"
#define EMBER_BACKGROUND_FOCUS_IN   "#2a2828"
#define EMBER_DATA_INPUT_AREA       "#2a2828"

/* DISPLAY & CHART & GRID & LEGEND */
#define EMBER_CHART                     "#080706"
#define EMBER_CHART_GRIDLEG_DEFAULT     "#696969"
#define EMBER_CHART_GRIDLEG_LOW_CONTRAST "#414141"
#define EMBER_DISPLAY                   EMBER_CHART

/* CHANNELS - Vibrant for modern "Ember" feel */
#define EMBER_CHANNEL_1 "#ff5722"
#define EMBER_CHANNEL_2 "#00b0ff"
#define EMBER_CHANNEL_3 "#73d92f"
#define EMBER_CHANNEL_4 "#e542fb"
#define EMBER_CHANNEL_5 "#eee737"
#define EMBER_CHANNEL_6 "#ff5722"
#define EMBER_CHANNEL_7 "#1de9b6"
#define EMBER_CHANNEL_8 "#ff9100"


/***************** GENERAL GLOBAL QT STYLE SHEETS *****************/

#define EMBER_STYLE_USE_TRANSPARENCY true
#define EMBER_STYLE_USE_CUSTOM_DIALS true

#define EMBER_STYLE_GLOBAL           EMBER_STYLE_OBJECT \
                                     EMBER_STYLE_SPLITTER EMBER_STYLE_SCROLLAREA EMBER_STYLE_PUSH_BUTTON \
                                     EMBER_STYLE_SCROLLBAR EMBER_STYLE_MENU EMBER_STYLE_TAB \
                                     EMBER_STYLE_TOOLTIP EMBER_STYLE_COMBO_BOX EMBER_STYLE_LINEEDIT \
                                     EMBER_STYLE_LIST_VIEW EMBER_STYLE_PROGRESS_BAR EMBER_STYLE_LABEL

#define EMBER_STYLE_OBJECT \
"QObject{background-color:" EMBER_WINDOW_APP ";font: 8pt " EMBER_FONT ";color:" EMBER_TEXT_ALL ";border:none;}"

#define EMBER_STYLE_DOCK_WIDGET \
"QDockWidget{border: 3px solid; background-color:" EMBER_WINDOW_DOCK_WIDGET ";}"

#define EMBER_STYLE_WIDGET \
"QWidget{background-color:" EMBER_WINDOW_WIDGET ";}"

#define EMBER_STYLE_SPLITTER \
"QSplitter::handle{background-color:" EMBER_CHART ";}"

#define EMBER_STYLE_SCROLLAREA \
"QScrollArea{border:none;}"

#define EMBER_STYLE_SCROLLBAR \
"QScrollBar:vertical{border:none;width:10px;margin:0;background:" EMBER_WINDOW_APP ";border-radius:5px;}\
QScrollBar:horizontal{border:none;height:10px;margin:0;background:" EMBER_WINDOW_APP ";border-radius:5px;}\
QScrollBar::handle:vertical{border-radius:4px;border:none;background-color:qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 " EMBER_BROWN_MID ", stop:1 " EMBER_LIGHT_BROWN ");min-height:38px;}\
QScrollBar::handle:horizontal{border-radius:4px;border:none;background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_BROWN_MID ", stop:1 " EMBER_LIGHT_BROWN ");min-width:38px;}\
QScrollBar::add-line{height:0px;border:none;}\
QScrollBar::sub-line{height:0px;border:none;}\
QScrollBar::add-page,QScrollBar::sub-page{background:transparent;}\
QScrollBar::handle:hover{background-color:" EMBER_CHART_GRIDLEG_DEFAULT ";}"

#define EMBER_STYLE_LINEEDIT \
"QLineEdit{border:none;border-bottom:1px solid " EMBER_BROWN_MID ";border-top-left-radius:2px;border-top-right-radius:2px;border-bottom-left-radius:0px;border-bottom-right-radius:0px;background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_DATA_INPUT_AREA ", stop:1 " EMBER_BROWN_DARK ");font:9pt " EMBER_FONT ";padding:2px 4px;margin:0px;}\
QLineEdit::focus{border-bottom:2px solid " EMBER_TEXT_COMPONENT ";background-color:" EMBER_BACKGROUND_FOCUS_IN ";}"

#define EMBER_STYLE_MENU \
"QMenu{background-color:" EMBER_BROWN ";border:1px solid " EMBER_BROWN_MID ";border-radius:4px;padding:4px 0px;}\
QMenu::item{background-color:transparent;padding:6px 24px 6px 16px;border-left:3px solid transparent;margin:2px 4px;}\
QMenu::item:selected{background-color:" EMBER_LIGHT_BROWN ";border-left:3px solid " EMBER_BLUE ";color:#ffffff;}\
QMenu::separator{height:1px;background:" EMBER_BROWN_MID ";margin:4px 8px;}"

#define EMBER_STYLE_TAB \
"QTabWidget::pane{border-top:3px solid " EMBER_BROWN_MID ";top:-3px;}\
QTabBar::tab{background-color:" EMBER_BROWN ";border:none;border-bottom:3px solid transparent;padding:8px 12px;color:" EMBER_TEXT_ALL ";border-radius:0px;}\
QTabBar::tab:selected{border-bottom-color:#ffffff;color:#ffffff;background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_BROWN ", stop:1 " EMBER_LIGHT_BROWN ");}\
QTabBar::tab:hover{background-color:" EMBER_BROWN_MID ";}\
QTabBar::tab:selected:hover{background-color:" EMBER_LIGHT_BROWN ";border-bottom-color:#ffffff;}"

#define EMBER_STYLE_TOOLTIP \
"QToolTip{border:1px solid " EMBER_BROWN_MID ";border-radius:4px;color:" EMBER_TEXT_ALL ";background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_LIGHT_BROWN ", stop:1 " EMBER_BROWN ");padding:4px 8px;}"

#define EMBER_STYLE_LIST_VIEW \
"QListView{alternate-background-color:" EMBER_BROWN_MID ";border-radius:3px;}"

#define EMBER_STYLE_COMBO_BOX \
"QComboBox{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_BROWN ", stop:1 " EMBER_DATA_INPUT_AREA ");border:1px solid " EMBER_BROWN_MID ";border-radius:3px;padding:4px 8px;}\
QComboBox:hover{border:1px solid " EMBER_LIGHT_BROWN ";}\
QComboBox::drop-down{border:none;width:20px;}\
QComboBox QAbstractItemView{selection-background-color:transparent;selection-color:" EMBER_TEXT_ALL ";background-color:" EMBER_BROWN ";border:1px solid " EMBER_BROWN_MID ";border-radius:3px;outline:none;}\
QComboBox QAbstractItemView::item{padding:4px 8px;border-left:3px solid transparent;}\
QComboBox QAbstractItemView::item:selected{background:" EMBER_LIGHT_BROWN ";border-left:3px solid " EMBER_TEXT_COMPONENT ";color:" EMBER_TEXT_COMPONENT ";}\
QComboBox QAbstractItemView::item:hover{background:" EMBER_LIGHT_BROWN ";border-left:3px solid " EMBER_TEXT_COMPONENT ";}\
QComboBox:disabled{background-color:" EMBER_BACKGROUND_BUTTON_DISABLED ";color:" EMBER_COMPONENT_DISABLED ";border-color:" EMBER_BACKGROUND_BUTTON_DISABLED ";}"

/* ── PUSH BUTTON ── */
#define EMBER_STYLE_PUSH_BUTTON \
"QPushButton{font-weight:bold;border-radius:4px;padding:3px 6px;border:1px solid #aa%1;\
background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #28%1, stop:0.12 " EMBER_LIGHT_BROWN ", stop:0.88 " EMBER_BROWN_DARK ", stop:1 #18%1);color:#FF%1;}\
QPushButton:hover{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #50%1, stop:0.15 " EMBER_BROWN_MID ", stop:0.85 " EMBER_LIGHT_BROWN ", stop:1 #40%1);border-color:#FF%1;}\
QPushButton:pressed{padding:4px 6px 2px 6px;border:1px solid #FF%1;background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #40%1, stop:0.15 " EMBER_BROWN_DARK ", stop:0.85 " EMBER_LIGHT_BROWN ", stop:1 #30%1);}\
QPushButton:disabled{border:1px solid " EMBER_WINDOW_CONTROL_HOVER ";background-color:" EMBER_BACKGROUND_BUTTON_DISABLED ";color:" EMBER_WINDOW_CONTROL_HOVER ";}"

/* ── CHECK BUTTON ── */
#define EMBER_STYLE_CHECK_BUTTON \
"QPushButton{font-weight:bold;border-radius:4px;padding:3px 6px;border:1px solid #aa%1;\
background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #28%1, stop:0.12 " EMBER_LIGHT_BROWN ", stop:0.88 " EMBER_BROWN_DARK ", stop:1 #18%1);color:#FF%1;}\
QPushButton:checked{border:1px solid #FF%1;background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ee%1, stop:0.06 #dd%1, stop:0.5 #aa%1, stop:0.94 #aa%1, stop:1 #cc%1);color:" EMBER_BROWN_DARK ";}\
QPushButton:hover{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #50%1, stop:0.15 " EMBER_BROWN_MID ", stop:0.85 " EMBER_LIGHT_BROWN ", stop:1 #40%1);}\
QPushButton:checked:hover{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ff%1, stop:0.06 #ee%1, stop:0.5 #cc%1, stop:0.94 #cc%1, stop:1 #ee%1);color:" EMBER_BROWN_DARK ";}\
QPushButton:pressed{padding:4px 6px 2px 6px;border:1px solid #FF%1;}\
QPushButton:disabled{border:1px solid " EMBER_WINDOW_CONTROL_HOVER ";background-color:" EMBER_BACKGROUND_BUTTON_DISABLED ";color:" EMBER_WINDOW_CONTROL_HOVER ";}"

#define EMBER_STYLE_PROGRESS_BAR \
"QProgressBar{border:1px solid " EMBER_BROWN_MID ";border-radius:4px;background-color:" EMBER_BROWN ";}\
QProgressBar::chunk{border-radius:3px;background-color:qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #aa%1, stop:1 #ff%1);}"

#define EMBER_STYLE_LABEL "QLabel{border-radius:3px;}"


/***************** CONCRETE STYLE SHEETS *****************/

#define EMBER_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" EMBER_WINDOW_APP ";qproperty-iconSize:13px;padding:5px;border:none;border-radius:4px;color:" EMBER_TEXT_LABEL ";}\
QPushButton:hover{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_WINDOW_CONTROL_HOVER ", stop:1 " EMBER_BROWN ");color:#ffffff;}"

#define EMBER_STYLE_MODULE_BUTTON \
"QPushButton{text-align:left;border:none;border-radius:4px;padding:3px 6px;font:10pt " EMBER_FONT ";qproperty-iconSize:25px;border-bottom:3px solid transparent;background:" EMBER_WINDOW_APP ";color:" EMBER_TEXT_ALL ";}\
QPushButton:hover{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_WINDOW_CONTROL_HOVER ", stop:1 " EMBER_WINDOW_APP ");color:#ffffff;}\
QPushButton:checked{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_BROWN ", stop:1 " EMBER_BROWN_MID ");color:#ffffff;border-bottom:0px solid #ffffff;}\
QPushButton:checked:hover{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_BROWN_MID ", stop:1 " EMBER_LIGHT_BROWN ");color:#ffffff;}\
QPushButton:disabled{background:" EMBER_WINDOW_APP ";color:" EMBER_WINDOW_CONTROL_HOVER ";border-bottom:3px solid transparent;}"

#define EMBER_STYLE_HOLD_BUTTON \
"QPushButton{font-weight:bold;font-size:8px;border-radius:3px;padding:2px 5px;border:1px solid " EMBER_TEXT_LABEL ";background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_LIGHT_BROWN ", stop:1 " EMBER_WINDOW_APP ");color:" EMBER_TEXT_LABEL ";}\
QPushButton:checked{border-color:" EMBER_YELLOW ";background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_YELLOW ", stop:1 " EMBER_WARNING ");color:" EMBER_BROWN_DARK ";}\
QPushButton:checked:hover{background-color:" EMBER_YELLOW ";}\
QPushButton:hover{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " EMBER_BROWN_MID ", stop:1 " EMBER_LIGHT_BROWN ");border-color:" EMBER_WARNING ";color:" EMBER_WARNING ";}\
QPushButton:pressed{padding:3px 5px 1px 5px;}"

#define EMBER_STYLE_DIAL EMBER_STYLE_LINEEDIT \
"QWidget:disabled{color:" EMBER_WINDOW_CONTROL_HOVER ";}\
QWidget{color:" EMBER_COMPONENT_DISABLED ";}\
QPushButton{font:9pt " EMBER_FONT ";}"

#define EMBER_STYLE_TEXTINPUT \
"QWidget:disabled{color:" EMBER_WINDOW_CONTROL_HOVER ";}\
QWidget{font:9pt " EMBER_FONT ";color:" EMBER_TEXT_COMPONENT ";}"

#define EMBER_STYLE_MODULE_DOCK_WIDGET EMBER_STYLE_WIDGET EMBER_STYLE_DOCK_WIDGET


class Ember : public AbstractTheme
{
public:
    explicit Ember(QObject *parent = nullptr);

    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);
    ThemePalette buildPalette() override;

private:
    QList<QColor> chanColor =
    {
        EMBER_CHANNEL_1, EMBER_CHANNEL_2,
        EMBER_CHANNEL_3, EMBER_CHANNEL_4,
        EMBER_CHANNEL_5, EMBER_CHANNEL_6,
        EMBER_CHANNEL_7, EMBER_CHANNEL_8
    };
};

#endif // EMBER_H
