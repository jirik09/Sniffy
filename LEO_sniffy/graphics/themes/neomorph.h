#ifndef NEOMORPH_H
#define NEOMORPH_H

#include <QObject>
#include <QColor>

#include "../abstracttheme.h"
struct ThemePalette; // forward declaration

#define NEOMORPH_GRAPHICS_PATH  ":/graphics/graphics/themes/neomorph/"
#define NEOMORPH_FONT           "'Segoe UI'"

/***************** COLORS (Neumorphic Light) *****************/

/* BASE */
#define NEO_BASE_BG "#ecf0f3"
#define NEO_PANEL_BG "#f5f7fb"

/* TEXT */
#define NEO_TEXT_MAIN "#2e3140"
#define NEO_TEXT_SECONDARY "#6b7280"
#define NEO_TEXT_COMPONENT "#394056"

/* ACCENT (Purple like in the screenshot) */
#define NEO_ACCENT "#6c5ce7"
#define NEO_ACCENT_LIGHT "#8e79ff"
#define NEO_ACCENT_DARK "#4b3db6"

/* COMPONENT COLORS */
#define NEO_CONTROLS NEO_TEXT_MAIN
#define NEO_BUTTON_BG "#f1f4f8"
#define NEO_BUTTON_BG_DISABLED "#e6eaef"
#define NEO_DATA_INPUT_BG "#f8fafd"
#define NEO_FOCUS_BG "#e9eef7"
#define NEO_ERROR "#ff6b6b"
#define NEO_WARNING "#ffb020"
#define NEO_RUNNING "#2fd680"
#define NEO_DISABLED "#aeb8c6"

/* CHART & GRID */
#define NEO_CHART_BG "#f2f5f8"
#define NEO_GRID_DEFAULT "#aeb8c6"
#define NEO_GRID_LOW "#cfd6df"

/* CHANNEL COLORS (keep readable on light) */
#define NEO_CH1 "#6c5ce7"  // purple
#define NEO_CH2 "#00b894"  // teal
#define NEO_CH3 "#e17055"  // orange
#define NEO_CH4 "#0984e3"  // blue
#define NEO_CH5 "#d63031"  // red
#define NEO_CH6 "#fd79a8"  // pink
#define NEO_CH7 "#00cec9"  // cyan
#define NEO_CH8 "#636e72"  // gray

/***************** GLOBAL QT STYLE SHEETS (Neumorphic) *****************/

#define NEO_STYLE_USE_TRANSPARENCY false
#define NEO_STYLE_USE_CUSTOM_DIALS true

#define NEO_STYLE_GLOBAL           NEO_STYLE_OBJECT \
                                    NEO_STYLE_WIDGET NEO_STYLE_SPLITTER NEO_STYLE_SCROLLAREA \
                                    NEO_STYLE_SCROLLBAR NEO_STYLE_MENU NEO_STYLE_TAB \
                                    NEO_STYLE_TOOLTIP NEO_STYLE_COMBO_BOX NEO_STYLE_LINEEDIT \
                                    NEO_STYLE_LIST_VIEW NEO_STYLE_PROGRESS_BAR NEO_STYLE_LABEL \
                                    NEO_STYLE_PUSH_BUTTON

/* Qt Style Sheets don't support CSS box-shadow; leave these empty to avoid warnings */
#define NEO_SHADOW_OUTER ""
#define NEO_SHADOW_INNER ""

#define NEO_STYLE_OBJECT \
"QObject{background-color:" NEO_BASE_BG ";font: 9pt" NEOMORPH_FONT ";color:" NEO_TEXT_MAIN ";border:none;}"

#define NEO_STYLE_DOCK_WIDGET \
"QDockWidget{border: 1px solid #e1e6ee; background-color:" NEO_PANEL_BG ";}"

#define NEO_STYLE_WIDGET \
"QWidget{background-color:" NEO_PANEL_BG ";}"

#define NEO_STYLE_SPLITTER \
"QSplitter::handle{background-color:" NEO_CHART_BG ";}"

#define NEO_STYLE_SCROLLAREA \
"QScrollArea{border:none}"

#define NEO_STYLE_SCROLLBAR \
"QScrollBar:vertical{border: none;width: 12px;margin: 0 0 0 0;}\
QScrollBar:horizontal{border: none;height: 12px;margin: 0 0 0 0;}\
QScrollBar::handle:vertical{border-radius: 6px;border: 1px solid #dde3eb;background-color:#e6ecf4;min-height: 38px;}\
QScrollBar::handle:horizontal{border-radius: 6px;border: 1px solid #dde3eb;background-color:#e6ecf4;min-width: 38px;}\
QScrollBar::add-line, QScrollBar::sub-line{height: 0px;border: none;}\
QScrollBar::add-page,QScrollBar::sub-page{background-color: transparent;}\
QScrollBar:handle:hover{border-color:#c8cfda;}"

#define NEO_STYLE_LINEEDIT \
"QLineEdit{border:1px solid #e1e6ee;border-radius:10px;background-color:" NEO_DATA_INPUT_BG ";font:10pt " NEOMORPH_FONT ";padding:6px 8px;" NEO_SHADOW_INNER "}\
QLineEdit::focus{border:1px solid " NEO_ACCENT ";background-color:" NEO_FOCUS_BG ";}"

#define NEO_STYLE_MENU \
"QMenu::item{background-color:" NEO_PANEL_BG ";}\
QMenu::item:selected{background-color:#eef2fb;}"

#define NEO_STYLE_TAB \
"QTabWidget::pane {border-top: 3px solid #e6eaef;top:-3px;} QTabWidget::tab-bar {}\
QTabBar::tab {background-color:" NEO_BUTTON_BG ";border:1px solid #e1e6ee; border-radius:10px; padding:8px 12px;color:" NEO_TEXT_COMPONENT ";" NEO_SHADOW_OUTER "}\
QTabBar::tab:selected,QTabBar::tab:hover{background-color:#eef2fb;}\
QTabBar::tab:selected{border:1px solid " NEO_ACCENT ";}"

#define NEO_STYLE_TOOLTIP \
"QToolTip{border: 1px solid #e1e6ee;color:" NEO_TEXT_MAIN ";background-color:" NEO_PANEL_BG ";}"

#define NEO_STYLE_LIST_VIEW \
"QListView{alternate-background-color: #f1f5fb;}"

#define NEO_STYLE_COMBO_BOX \
"QComboBox{background-color:" NEO_DATA_INPUT_BG ";border:1px solid #e1e6ee;border-radius:10px;padding:6px 8px;" NEO_SHADOW_OUTER "}\
QComboBox:hover{border: 1px solid #cfd6df;}\
QComboBox QAbstractItemView{selection-background-color:#eef2fb;background-color:" NEO_PANEL_BG ";}\
QComboBox:disabled{background-color:" NEO_BUTTON_BG_DISABLED ";color:" NEO_DISABLED ";}"

#define NEO_STYLE_PUSH_BUTTON \
"QPushButton{font-weight : 600 ;border-radius: 12px; padding:8px 14px;border:1px solid #e1e6ee;background-color:" NEO_BUTTON_BG ";color:" NEO_TEXT_MAIN ";border-color:%1;" NEO_SHADOW_OUTER "}\
QPushButton:disabled{border:1px solid #e1e6ee;background-color:" NEO_BUTTON_BG_DISABLED ";color:" NEO_DISABLED ";}\
QPushButton:pressed{border:1px solid #dde3eb; background-color:#eef2fb; " NEO_SHADOW_INNER "}\
QPushButton:hover{background-color:#f6f9ff;border-color:#cfd6df;}"

#define NEO_STYLE_CHECK_BUTTON \
"QPushButton{font-weight : 600 ;border-radius: 12px; padding:8px 14px;border:1px solid #e1e6ee;background-color:" NEO_BUTTON_BG "; color:" NEO_TEXT_MAIN ";border-color:%1;" NEO_SHADOW_OUTER "}\
QPushButton:checked{border:1px solid " NEO_ACCENT "; background-color:#edeaff; color:" NEO_TEXT_MAIN ";}\
QPushButton:hover{background-color:#f6f9ff;border-color:#cfd6df;}\
QPushButton:checked:hover{background-color:#edeaff;}\
QPushButton:disabled{border:1px solid #e1e6ee;background-color: " NEO_BUTTON_BG_DISABLED "; color:" NEO_DISABLED ";}"

#define NEO_STYLE_PROGRESS_BAR \
"QProgressBar{border:1px solid #e1e6ee;border-radius:10px;background-color:" NEO_CHART_BG ";" NEO_SHADOW_INNER "}\
QProgressBar::chunk{border-radius:10px; margin:2px;background-color:%1;}"

#define NEO_STYLE_LABEL "QLabel{border-radius: 2px;}"

/***************** CONCRETE STYLE SHEETS *****************/

#define NEO_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" NEO_PANEL_BG ";qproperty-iconSize: 16px;padding: 6px;border-radius:10px;" NEO_SHADOW_OUTER "}"

#define NEO_STYLE_MODULE_BUTTON \
"QPushButton{text-align:left; border: 1px solid #e1e6ee; border-radius:12px;qproperty-iconSize: 36px;background-color:" NEO_BUTTON_BG ";" NEO_SHADOW_OUTER "}\
QPushButton:hover{text-align:left; background-color:#f6f9ff;color:" NEO_TEXT_MAIN ";}\
QPushButton:checked{text-align:left; background-color:#edeaff; color:" NEO_TEXT_MAIN "; border:1px solid " NEO_ACCENT ";}\
QPushButton:checked:hover{text-align:left; background-color:#edeaff;}"

#define NEO_STYLE_HOLD_BUTTON \
"QPushButton{font-weight:600;padding:6px 10px;font-size:10px;border-radius:10px;}\
QPushButton{border: 1px solid #e1e6ee; background-color:" NEO_PANEL_BG ";color:" NEO_TEXT_SECONDARY ";border-color:%1;" NEO_SHADOW_OUTER "}\
QPushButton:checked{border:1px solid " NEO_ACCENT ";background-color:#edeaff;color:" NEO_TEXT_MAIN ";}\
QPushButton:checked:hover{background-color:#edeaff;}\
QPushButton:hover{background-color:#f6f9ff;border-color:#cfd6df;}"

#define NEO_STYLE_DIAL NEO_STYLE_LINEEDIT \
"QWidget:disabled{color:" NEO_DISABLED ";}\
QWidget{color:" NEO_TEXT_COMPONENT ";}\
QPushButton{font: 11pt" NEOMORPH_FONT ";}"

#define NEO_STYLE_TEXTINPUT \
"QWidget:disabled{ color:" NEO_DISABLED ";}\
QWidget{font: 9pt" NEOMORPH_FONT "; color:" NEO_TEXT_COMPONENT ";}"

#define NEO_STYLE_MODULE_DOCK_WIDGET NEO_STYLE_WIDGET NEO_STYLE_DOCK_WIDGET

class Neomorph : public AbstractTheme
{
    Q_OBJECT
public:
    explicit Neomorph(QObject *parent = nullptr);

    QString getAppGlobalStyle();
    QString getGraphicsPath();

    QString getChannelColor(int channelIndex);
    ThemePalette buildPalette() override; // new palette builder

private:
    QList<QColor> chanColor =
    {
        NEO_CH1, NEO_CH2, NEO_CH3, NEO_CH4,
        NEO_CH5, NEO_CH6, NEO_CH7, NEO_CH8
    };
};

#endif // NEOMORPH_H
