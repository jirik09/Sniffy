#ifndef SUNSET_H
#define SUNSET_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"
#include "../graphics.h"

#define SUNSET_GRAPHICS_PATH  ":/graphics/graphics/themes/sunset/"
#define SUNSET_FONT           "'Segoe UI'"

/* Dark coral/pink gradient theme */
#define SUNSET_BG_APP        "#2b2f3a"    // slate/dark violet
#define SUNSET_BG_WIDGET     "#333847"
#define SUNSET_BG_BUTTON     "#3a4052"
#define SUNSET_BG_BUTTON_DIS "#2f3442"
#define SUNSET_INPUT_BG      "#2c3040"
#define SUNSET_FOCUS_BG      "#2a3146"

#define SUNSET_TEXT_MAIN     "#e8e9ee"
#define SUNSET_TEXT_LABEL    "#a9adbb"
#define SUNSET_TEXT_COMP     "#d9dbe4"

#define SUNSET_ORANGE        "#ff6e48"
#define SUNSET_PINK          "#ff5c83"
#define SUNSET_CORAL         "#ff7a6e"
#define SUNSET_CYAN          "#4ecdc4"

#define SUNSET_WARNING       SUNSET_ORANGE
#define SUNSET_ERROR         "#ff5b5b"
#define SUNSET_RUNNING       "#32d296"
#define SUNSET_DISABLED      "#6f7688"
#define SUNSET_WINDOW_HOVER  "#40465a"
#define SUNSET_EXIT_HOVER    "#ff6b6b"

#define SUNSET_CHART_BG      "#2a2f3a"
#define SUNSET_GRID_DEF      "#a4a8b5"
#define SUNSET_GRID_LOW      "#6b7181"

/* Channels tuned for dark slate */
#define SUNSET_CH1 SUNSET_ORANGE
#define SUNSET_CH2 SUNSET_PINK
#define SUNSET_CH3 "#ffd166"
#define SUNSET_CH4 "#06d6a0"
#define SUNSET_CH5 "#118ab2"
#define SUNSET_CH6 "#ef476f"
#define SUNSET_CH7 "#a29bfe"
#define SUNSET_CH8 "#8d99ae"

#define SUNSET_STYLE_USE_TRANSPARENCY true
#define SUNSET_STYLE_USE_CUSTOM_DIALS true

#define SUNSET_STYLE_GLOBAL           SUNSET_STYLE_OBJECT \
                                    SUNSET_STYLE_SPLITTER SUNSET_STYLE_SCROLLAREA SUNSET_STYLE_PUSH_BUTTON \
                                    SUNSET_STYLE_SCROLLBAR SUNSET_STYLE_MENU SUNSET_STYLE_TAB \
                                    SUNSET_STYLE_TOOLTIP SUNSET_STYLE_COMBO_BOX SUNSET_STYLE_LINEEDIT \
                                    SUNSET_STYLE_LIST_VIEW SUNSET_STYLE_PROGRESS_BAR SUNSET_STYLE_LABEL

#define SUNSET_STYLE_OBJECT \
"QObject{background-color:" SUNSET_BG_APP ";font: 9pt" SUNSET_FONT ";color:" SUNSET_TEXT_MAIN ";border:none;}"

#define SUNSET_STYLE_DOCK_WIDGET \
"QDockWidget{border: 3px solid; background-color:" SUNSET_BG_WIDGET ";}"

#define SUNSET_STYLE_WIDGET \
"QWidget{background-color:" SUNSET_BG_WIDGET ";}"

#define SUNSET_STYLE_SPLITTER \
"QSplitter::handle{background-color:" SUNSET_CHART_BG ";}"

#define SUNSET_STYLE_SCROLLAREA \
"QScrollArea{border:none}"

#define SUNSET_STYLE_SCROLLBAR \
"QScrollBar:vertical{border: none;width: 10px;margin: 0;}\
QScrollBar:horizontal{border: none;height: 10px;margin: 0;}\
QScrollBar::handle:vertical{border-radius: 3px;border: none;background-color:" SUNSET_TEXT_LABEL ";min-height: 38px;}\
QScrollBar::handle:horizontal{border-radius: 3px;border: none;background-color:" SUNSET_TEXT_LABEL ";min-width: 38px;}\
QScrollBar::add-line, QScrollBar::sub-line{height: 0px;border: none;}\
QScrollBar::add-page,QScrollBar::sub-page{background-color: transparent;}\
QScrollBar:handle:hover{background-color:#cfd2dc;}"

#define SUNSET_STYLE_LINEEDIT \
"QLineEdit{border:none;background-color:" SUNSET_INPUT_BG ";font:11pt " SUNSET_FONT ";padding:4px 6px;}\
QLineEdit::focus{border:none;background-color:" SUNSET_FOCUS_BG ";}"

#define SUNSET_STYLE_MENU \
"QMenu::item{background-color:" SUNSET_INPUT_BG ";}\
QMenu::item:selected{background-color:" SUNSET_WINDOW_HOVER ";}"

#define SUNSET_STYLE_TAB \
"QTabWidget::pane {border-top: 3px solid " SUNSET_BG_BUTTON ";top:-3px;} QTabWidget::tab-bar {}\
QTabBar::tab {background-color:" SUNSET_BG_BUTTON ";border-bottom: 3px solid rgb(128, 128, 128);padding: 7px;color:" SUNSET_TEXT_COMP ";}\
QTabBar::tab:selected,QTabBar::tab:hover{background-color:" SUNSET_WINDOW_HOVER ";}\
QTabBar::tab:selected{border-bottom-color: " SUNSET_PINK ";}"

#define SUNSET_STYLE_TOOLTIP \
"QToolTip{border: 0px;color:" SUNSET_TEXT_MAIN ";background-color:" SUNSET_BG_APP ";}"

#define SUNSET_STYLE_LIST_VIEW \
"QListView{alternate-background-color: #3a3f4e;}"

#define SUNSET_STYLE_COMBO_BOX \
"QComboBox{background-color:" SUNSET_INPUT_BG ";}\
QComboBox:hover{border: 1px solid " SUNSET_TEXT_LABEL ";}\
QComboBox QAbstractItemView{selection-background-color:" SUNSET_WINDOW_HOVER ";background-color:" SUNSET_INPUT_BG ";}\
QComboBox:disabled{background-color:" SUNSET_BG_BUTTON_DIS ";color:" SUNSET_DISABLED ";}"

#define SUNSET_STYLE_PUSH_BUTTON \
"QPushButton{font-weight : bold ;border-radius: 6px; padding:6px 10px;}\
QPushButton{background-color:" SUNSET_BG_BUTTON ";color: #FF%1;}\
QPushButton:disabled{border:none;background-color:" SUNSET_BG_BUTTON_DIS ";color:" SUNSET_DISABLED ";}\
QPushButton:pressed{background-color: #88%1;border: 2px solid " SUNSET_BG_WIDGET ";}\
QPushButton:hover{background-color: #3C%1;}"

#define SUNSET_STYLE_CHECK_BUTTON SUNSET_STYLE_PUSH_BUTTON

#define SUNSET_STYLE_PROGRESS_BAR \
"QProgressBar{border:1px solid " SUNSET_TEXT_LABEL ";border-radius:2px;background-color:" SUNSET_CHART_BG ";}\
QProgressBar::chunk{width: 20px;background-color: #FF%1;}"

#define SUNSET_STYLE_LABEL "QLabel{border-radius: 2px;}"

#define SUNSET_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" SUNSET_BG_APP ";qproperty-iconSize: 16px;padding: 6px;border-radius:6px}"

#define SUNSET_STYLE_MODULE_BUTTON \
"QPushButton{text-align:left; border: 2px solid " SUNSET_BG_WIDGET "; border-radius:8px;qproperty-iconSize: 36px;}\
QPushButton:hover{text-align:left; border: 2px solid " SUNSET_BG_WIDGET ";background:" SUNSET_WINDOW_HOVER ";color: #dcdde0;}\
QPushButton:checked{text-align:left; border: 2px solid " SUNSET_BG_WIDGET ";background: #11FFFFFF; color: " SUNSET_PINK ";}\
QPushButton:checked:hover{text-align:left; border: 2px solid " SUNSET_BG_WIDGET "; background:" SUNSET_WINDOW_HOVER ";}"

#define SUNSET_STYLE_HOLD_BUTTON \
"QPushButton{font-weight:bold;padding:4px 8px;font-size:10px;border-radius:6px;}\
QPushButton{border: 1px solid #EEff6e48; background-color:" SUNSET_BG_APP ";color:" SUNSET_TEXT_LABEL ";}\
QPushButton{border-color:" SUNSET_TEXT_LABEL ";}\
QPushButton:checked{border:none;background-color:" SUNSET_WARNING ";color:" SUNSET_BG_WIDGET ";}\
QPushButton:checked:hover{background-color:#FFff6e48;color:" SUNSET_BG_WIDGET ";}\
QPushButton:hover{background-color:#3a4356;border-color:" SUNSET_WARNING ";color:" SUNSET_WARNING ";}"

#define SUNSET_STYLE_DIAL SUNSET_STYLE_LINEEDIT \
"QWidget:disabled{color:" SUNSET_DISABLED ";}\
QWidget{color:" SUNSET_TEXT_COMP ";}\
QPushButton{font: 11pt" SUNSET_FONT ";}"

#define SUNSET_STYLE_TEXTINPUT \
"QWidget:disabled{ color:" SUNSET_DISABLED ";}\
QWidget{font: 9pt" SUNSET_FONT "; color:" SUNSET_TEXT_COMP ";}"

#define SUNSET_STYLE_MODULE_DOCK_WIDGET SUNSET_STYLE_WIDGET SUNSET_STYLE_DOCK_WIDGET

class Sunset : public AbstractTheme
{
    Q_OBJECT
public:
    explicit Sunset(QObject *parent = nullptr);

    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);

private:
    QList<QColor> chanColor =
    {
        SUNSET_CH1, SUNSET_CH2, SUNSET_CH3, SUNSET_CH4,
        SUNSET_CH5, SUNSET_CH6, SUNSET_CH7, SUNSET_CH8
    };
};

#endif // SUNSET_H
