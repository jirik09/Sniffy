#ifndef HUDCYAN_H
#define HUDCYAN_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"
struct ThemePalette; // forward declaration

#define HUD_GRAPHICS_PATH  ":/graphics/graphics/themes/hudcyan/"
#define HUD_FONT           "'Segoe UI'"

/* HUD Cyan: near-black UI with neon cyan accents */
#define HUD_APP_BG        "#07090b"
#define HUD_WIDGET_BG     "#0b0e12"
#define HUD_BTN_BG        "#0e1318"
#define HUD_BTN_BG_DIS    "#0a0f14"
#define HUD_INPUT_BG      "#0a0e12"
#define HUD_FOCUS_BG      "#0c1218"

#define HUD_TEXT_MAIN     "#e6fafd"
#define HUD_TEXT_LABEL    "#8aaab0"
#define HUD_TEXT_COMP     "#cfe7eb"

#define HUD_CYAN          "#00e5ff"
#define HUD_CYAN_SOFT     "#63f7ff"
#define HUD_GREEN         "#2bdc9c"
#define HUD_YELLOW        "#ffd166"
#define HUD_RED           "#ff6b6b"

#define HUD_WARNING       HUD_YELLOW
#define HUD_ERROR         HUD_RED
#define HUD_RUNNING       HUD_GREEN
#define HUD_DISABLED      "#5a6d72"
#define HUD_HOVER         "#10202a"
#define HUD_EXIT_HOVER    HUD_RED

#define HUD_CHART_BG      "#091016"
#define HUD_GRID_DEF      "#a6c8cd"
#define HUD_GRID_LOW      "#5c7b81"

/* Channels */
#define HUD_CH1 HUD_CYAN
#define HUD_CH2 "#29d0ff"
#define HUD_CH3 "#2bdc9c"
#define HUD_CH4 "#ff7ab8"
#define HUD_CH5 "#ff9f43"
#define HUD_CH6 "#67a0ff"
#define HUD_CH7 "#7af5ff"
#define HUD_CH8 "#b084f5"

#define HUD_STYLE_USE_TRANSPARENCY true
#define HUD_STYLE_USE_CUSTOM_DIALS true

#define HUD_STYLE_GLOBAL           HUD_STYLE_OBJECT \
                                    HUD_STYLE_SPLITTER HUD_STYLE_SCROLLAREA HUD_STYLE_PUSH_BUTTON \
                                    HUD_STYLE_SCROLLBAR HUD_STYLE_MENU HUD_STYLE_TAB \
                                    HUD_STYLE_TOOLTIP HUD_STYLE_COMBO_BOX HUD_STYLE_LINEEDIT \
                                    HUD_STYLE_LIST_VIEW HUD_STYLE_PROGRESS_BAR HUD_STYLE_LABEL

#define HUD_STYLE_OBJECT \
"QObject{background-color:" HUD_APP_BG ";font: 9pt" HUD_FONT ";color:" HUD_TEXT_MAIN ";border:none;}"

#define HUD_STYLE_DOCK_WIDGET \
"QDockWidget{border: 3px solid; background-color:" HUD_WIDGET_BG ";}"

#define HUD_STYLE_WIDGET \
"QWidget{background-color:" HUD_WIDGET_BG ";}"

#define HUD_STYLE_SPLITTER \
"QSplitter::handle{background-color:" HUD_CHART_BG ";}"

#define HUD_STYLE_SCROLLAREA \
"QScrollArea{border:none}"

#define HUD_STYLE_SCROLLBAR \
"QScrollBar:vertical{border: none;width: 10px;margin: 0;}\
QScrollBar:horizontal{border: none;height: 10px;margin: 0;}\
QScrollBar::handle:vertical{border-radius: 3px;border: none;background-color:" HUD_TEXT_LABEL ";min-height: 38px;}\
QScrollBar::handle:horizontal{border-radius: 3px;border: none;background-color:" HUD_TEXT_LABEL ";min-width: 38px;}\
QScrollBar::add-line, QScrollBar::sub-line{height: 0px;border: none;}\
QScrollBar::add-page,QScrollBar::sub-page{background-color: transparent;}\
QScrollBar:handle:hover{background-color:#cfd2dc;}"

#define HUD_STYLE_LINEEDIT \
"QLineEdit{border:none;background-color:" HUD_INPUT_BG ";font:11pt " HUD_FONT ";padding:4px 6px;}\
QLineEdit::focus{border:none;background-color:" HUD_FOCUS_BG ";}"

#define HUD_STYLE_MENU \
"QMenu::item{background-color:" HUD_INPUT_BG ";}\
QMenu::item:selected{background-color:" HUD_HOVER ";}"

#define HUD_STYLE_TAB \
"QTabWidget::pane {border-top: 3px solid " HUD_BTN_BG ";top:-3px;} QTabWidget::tab-bar {}\
QTabBar::tab {background-color:" HUD_BTN_BG ";border-bottom: 3px solid rgb(96, 96, 96);padding: 7px;color:" HUD_TEXT_COMP ";}\
QTabBar::tab:selected,QTabBar::tab:hover{background-color:" HUD_HOVER ";}\
QTabBar::tab:selected{border-bottom-color: " HUD_CYAN ";}"

#define HUD_STYLE_TOOLTIP \
"QToolTip{border: 0px;color:" HUD_TEXT_MAIN ";background-color:" HUD_APP_BG ";}"

#define HUD_STYLE_LIST_VIEW \
"QListView{alternate-background-color: #0f151b;}"

#define HUD_STYLE_COMBO_BOX \
"QComboBox{background-color:" HUD_INPUT_BG ";}\
QComboBox:hover{border: 1px solid " HUD_TEXT_LABEL ";}\
QComboBox QAbstractItemView{selection-background-color:" HUD_HOVER ";background-color:" HUD_INPUT_BG ";}\
QComboBox:disabled{background-color:" HUD_BTN_BG_DIS ";color:" HUD_DISABLED ";}"

#define HUD_STYLE_PUSH_BUTTON \
"QPushButton{font-weight : bold ;border-radius: 6px; padding:6px 10px;}\
QPushButton{background-color:" HUD_BTN_BG ";color: #FF%1;}\
QPushButton:disabled{border:none;background-color:" HUD_BTN_BG_DIS ";color:" HUD_DISABLED ";}\
QPushButton:pressed{background-color: #88%1;border: 2px solid " HUD_WIDGET_BG ";}\
QPushButton:hover{background-color: #1b2a33;}"

#define HUD_STYLE_CHECK_BUTTON HUD_STYLE_PUSH_BUTTON

#define HUD_STYLE_PROGRESS_BAR \
"QProgressBar{border:1px solid " HUD_TEXT_LABEL ";border-radius:2px;background-color:" HUD_CHART_BG ";}\
QProgressBar::chunk{width: 20px;background-color: #FF%1;}"

#define HUD_STYLE_LABEL "QLabel{border-radius: 2px;}"

#define HUD_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" HUD_APP_BG ";qproperty-iconSize: 16px;padding:  6px;border-radius:6px}"

#define HUD_STYLE_MODULE_BUTTON \
"QPushButton{text-align:left; border: 2px solid " HUD_WIDGET_BG "; border-radius:8px;qproperty-iconSize: 36px;}\
QPushButton:hover{text-align:left; border: 2px solid " HUD_WIDGET_BG ";background:" HUD_HOVER ";color: #dcdde0;}\
QPushButton:checked{text-align:left; border: 2px solid " HUD_WIDGET_BG ";background: #11FFFFFF; color: " HUD_CYAN ";}\
QPushButton:checked:hover{text-align:left; border: 2px solid " HUD_WIDGET_BG "; background:" HUD_HOVER ";}"

#define HUD_STYLE_HOLD_BUTTON \
"QPushButton{font-weight:bold;padding:4px 8px;font-size:10px;border-radius:6px;}\
QPushButton{border: 1px solid #EE00e5ff; background-color:" HUD_APP_BG ";color:" HUD_TEXT_LABEL ";}\
QPushButton{border-color:" HUD_TEXT_LABEL ";}\
QPushButton:checked{border:none;background-color:" HUD_WARNING ";color:" HUD_WIDGET_BG ";}\
QPushButton:checked:hover{background-color:#FFffd166;color:" HUD_WIDGET_BG ";}\
QPushButton:hover{background-color:#15232b;border-color:" HUD_WARNING ";color:" HUD_WARNING ";}"

#define HUD_STYLE_DIAL HUD_STYLE_LINEEDIT \
"QWidget:disabled{color:" HUD_DISABLED ";}\
QWidget{color:" HUD_TEXT_COMP ";}\
QPushButton{font: 11pt" HUD_FONT ";}"

#define HUD_STYLE_TEXTINPUT \
"QWidget:disabled{ color:" HUD_DISABLED ";}\
QWidget{font: 9pt" HUD_FONT "; color:" HUD_TEXT_COMP ";}"

#define HUD_STYLE_MODULE_DOCK_WIDGET HUD_STYLE_WIDGET HUD_STYLE_DOCK_WIDGET

class HudCyan : public AbstractTheme
{
    Q_OBJECT
public:
    explicit HudCyan(QObject *parent = nullptr);

    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);
    ThemePalette buildPalette() override; // new palette builder

private:
    QList<QColor> chanColor =
    {
        HUD_CH1, HUD_CH2, HUD_CH3, HUD_CH4,
        HUD_CH5, HUD_CH6, HUD_CH7, HUD_CH8
    };
};

#endif // HUDCYAN_H
