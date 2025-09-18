#ifndef TEALANALYTICS_H
#define TEALANALYTICS_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"
#include "../graphics.h"

#define TEAL_GRAPHICS_PATH  ":/graphics/graphics/themes/tealanalytics/"
#define TEAL_FONT           "'Segoe UI'"

#define TEAL_APP_BG   "#0e1b22"
#define TEAL_WIDGET_BG "#10232c"
#define TEAL_BTN_BG   "#15303a"
#define TEAL_BTN_BG_DIS "#112731"
#define TEAL_INPUT_BG "#0f222b"
#define TEAL_FOCUS_BG "#14303b"

#define TEAL_TEXT_MAIN "#e9f1f5"
#define TEAL_TEXT_LABEL "#9fb3bd"
#define TEAL_TEXT_COMP "#d0e0e7"

#define TEAL_CYAN   "#17c0eb"
#define TEAL_GREEN  "#20bf6b"
#define TEAL_YELLOW "#f7b731"
#define TEAL_RED    "#ff6b6b"

#define TEAL_WARNING TEAL_YELLOW
#define TEAL_ERROR   TEAL_RED
#define TEAL_RUNNING TEAL_GREEN
#define TEAL_DISABLED "#6d828d"
#define TEAL_HOVER    "#1c3d4a"
#define TEAL_EXIT_HOVER TEAL_RED

#define TEAL_CHART_BG "#0c1a21"
#define TEAL_GRID_DEF "#b2c3ca"
#define TEAL_GRID_LOW "#6c7f87"

#define TEAL_CH1 TEAL_CYAN
#define TEAL_CH2 "#26de81"
#define TEAL_CH3 TEAL_YELLOW
#define TEAL_CH4 "#45aaf2"
#define TEAL_CH5 "#e84393"
#define TEAL_CH6 "#fd9644"
#define TEAL_CH7 "#a29bfe"
#define TEAL_CH8 "#778ca3"

#define TEAL_STYLE_USE_TRANSPARENCY true
#define TEAL_STYLE_USE_CUSTOM_DIALS true

#define TEAL_STYLE_GLOBAL           TEAL_STYLE_OBJECT \
                                    TEAL_STYLE_SPLITTER TEAL_STYLE_SCROLLAREA TEAL_STYLE_PUSH_BUTTON \
                                    TEAL_STYLE_SCROLLBAR TEAL_STYLE_MENU TEAL_STYLE_TAB \
                                    TEAL_STYLE_TOOLTIP TEAL_STYLE_COMBO_BOX TEAL_STYLE_LINEEDIT \
                                    TEAL_STYLE_LIST_VIEW TEAL_STYLE_PROGRESS_BAR TEAL_STYLE_LABEL

#define TEAL_STYLE_OBJECT \
"QObject{background-color:" TEAL_APP_BG ";font: 9pt" TEAL_FONT ";color:" TEAL_TEXT_MAIN ";border:none;}"

#define TEAL_STYLE_DOCK_WIDGET \
"QDockWidget{border: 3px solid; background-color:" TEAL_WIDGET_BG ";}"

#define TEAL_STYLE_WIDGET \
"QWidget{background-color:" TEAL_WIDGET_BG ";}"

#define TEAL_STYLE_SPLITTER \
"QSplitter::handle{background-color:" TEAL_CHART_BG ";}"

#define TEAL_STYLE_SCROLLAREA \
"QScrollArea{border:none}"

#define TEAL_STYLE_SCROLLBAR \
"QScrollBar:vertical{border: none;width: 10px;margin: 0;}\
QScrollBar:horizontal{border: none;height: 10px;margin: 0;}\
QScrollBar::handle:vertical{border-radius: 3px;border: none;background-color:" TEAL_TEXT_LABEL ";min-height: 38px;}\
QScrollBar::handle:horizontal{border-radius: 3px;border: none;background-color:" TEAL_TEXT_LABEL ";min-width: 38px;}\
QScrollBar::add-line, QScrollBar::sub-line{height: 0px;border: none;}\
QScrollBar::add-page,QScrollBar::sub-page{background-color: transparent;}\
QScrollBar:handle:hover{background-color:#cfd2dc;}"

#define TEAL_STYLE_LINEEDIT \
"QLineEdit{border:none;background-color:" TEAL_INPUT_BG ";font:11pt " TEAL_FONT ";padding:4px 6px;}\
QLineEdit::focus{border:none;background-color:" TEAL_FOCUS_BG ";}"

#define TEAL_STYLE_MENU \
"QMenu::item{background-color:" TEAL_INPUT_BG ";}\
QMenu::item:selected{background-color:" TEAL_HOVER ";}"

#define TEAL_STYLE_TAB \
"QTabWidget::pane {border-top: 3px solid " TEAL_BTN_BG ";top:-3px;} QTabWidget::tab-bar {}\
QTabBar::tab {background-color:" TEAL_BTN_BG ";border-bottom: 3px solid rgb(128, 128, 128);padding: 7px;color:" TEAL_TEXT_COMP ";}\
QTabBar::tab:selected,QTabBar::tab:hover{background-color:" TEAL_HOVER ";}\
QTabBar::tab:selected{border-bottom-color: " TEAL_CYAN ";}"

#define TEAL_STYLE_TOOLTIP \
"QToolTip{border: 0px;color:" TEAL_TEXT_MAIN ";background-color:" TEAL_APP_BG ";}"

#define TEAL_STYLE_LIST_VIEW \
"QListView{alternate-background-color: #1f2e38;}"

#define TEAL_STYLE_COMBO_BOX \
"QComboBox{background-color:" TEAL_INPUT_BG ";}\
QComboBox:hover{border: 1px solid " TEAL_TEXT_LABEL ";}\
QComboBox QAbstractItemView{selection-background-color:" TEAL_HOVER ";background-color:" TEAL_INPUT_BG ";}\
QComboBox:disabled{background-color:" TEAL_BTN_BG_DIS ";color:" TEAL_DISABLED ";}"

#define TEAL_STYLE_PUSH_BUTTON \
"QPushButton{font-weight : bold ;border-radius: 6px; padding:6px 10px;}\
QPushButton{background-color:" TEAL_BTN_BG ";color: #FF%1;}\
QPushButton:disabled{border:none;background-color:" TEAL_BTN_BG_DIS ";color:" TEAL_DISABLED ";}\
QPushButton:pressed{background-color: #88%1;border: 2px solid " TEAL_WIDGET_BG ";}\
QPushButton:hover{background-color: #2b5363;}"

#define TEAL_STYLE_CHECK_BUTTON TEAL_STYLE_PUSH_BUTTON

#define TEAL_STYLE_PROGRESS_BAR \
"QProgressBar{border:1px solid " TEAL_TEXT_LABEL ";border-radius:2px;background-color:" TEAL_CHART_BG ";}\
QProgressBar::chunk{width: 20px;background-color: #FF%1;}"

#define TEAL_STYLE_LABEL "QLabel{border-radius: 2px;}"

#define TEAL_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" TEAL_APP_BG ";qproperty-iconSize: 16px;padding: 6px;border-radius:6px}"

#define TEAL_STYLE_MODULE_BUTTON \
"QPushButton{text-align:left; border: 2px solid " TEAL_WIDGET_BG "; border-radius:8px;qproperty-iconSize: 36px;}\
QPushButton:hover{text-align:left; border: 2px solid " TEAL_WIDGET_BG ";background:" TEAL_HOVER ";color: #dcdde0;}\
QPushButton:checked{text-align:left; border: 2px solid " TEAL_WIDGET_BG ";background: #11FFFFFF; color: " TEAL_CYAN ";}\
QPushButton:checked:hover{text-align:left; border: 2px solid " TEAL_WIDGET_BG "; background:" TEAL_HOVER ";}"

#define TEAL_STYLE_HOLD_BUTTON \
"QPushButton{font-weight:bold;padding:4px 8px;font-size:10px;border-radius:6px;}\
QPushButton{border: 1px solid #EE17c0eb; background-color:" TEAL_APP_BG ";color:" TEAL_TEXT_LABEL ";}\
QPushButton{border-color:" TEAL_TEXT_LABEL ";}\
QPushButton:checked{border:none;background-color:" TEAL_WARNING ";color:" TEAL_WIDGET_BG ";}\
QPushButton:checked:hover{background-color:#FFf7b731;color:" TEAL_WIDGET_BG ";}\
QPushButton:hover{background-color:#2a4754;border-color:" TEAL_WARNING ";color:" TEAL_WARNING ";}"

#define TEAL_STYLE_DIAL TEAL_STYLE_LINEEDIT \
"QWidget:disabled{color:" TEAL_DISABLED ";}\
QWidget{color:" TEAL_TEXT_COMP ";}\
QPushButton{font: 11pt" TEAL_FONT ";}"

#define TEAL_STYLE_TEXTINPUT \
"QWidget:disabled{ color:" TEAL_DISABLED ";}\
QWidget{font: 9pt" TEAL_FONT "; color:" TEAL_TEXT_COMP ";}"

#define TEAL_STYLE_MODULE_DOCK_WIDGET TEAL_STYLE_WIDGET TEAL_STYLE_DOCK_WIDGET

class TealAnalytics : public AbstractTheme
{
    Q_OBJECT
public:
    explicit TealAnalytics(QObject *parent = nullptr);

    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);

private:
    QList<QColor> chanColor =
    {
        TEAL_CH1, TEAL_CH2, TEAL_CH3, TEAL_CH4,
        TEAL_CH5, TEAL_CH6, TEAL_CH7, TEAL_CH8
    };
};

#endif // TEALANALYTICS_H
