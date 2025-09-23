// Authentic MS-DOS / Norton Commander inspired theme
#ifndef MSDOS_H
#define MSDOS_H

#include <QObject>
#include <QColor>
#include "../abstracttheme.h"

#define MSDOS_GRAPHICS_PATH  ":/graphics/graphics/themes/msdos/"
#define MSDOS_FONT "'Lucida Console','Courier New',monospace"

// VGA palette subset
#define MSDOS_COL_BLACK        "#000000"
#define MSDOS_COL_BLUE         "#0000AA"
#define MSDOS_COL_LIGHT_BLUE   "#5555FF"
#define MSDOS_COL_LIGHT_GREEN  "#55FF55"
#define MSDOS_COL_LIGHT_CYAN   "#55FFFF"
#define MSDOS_COL_LIGHT_RED    "#FF5555"
#define MSDOS_COL_LIGHT_MAGENTA "#FF55FF"
#define MSDOS_COL_YELLOW       "#FFFF55"
#define MSDOS_COL_WHITE        "#FFFFFF"
#define MSDOS_COL_LIGHT_GRAY   "#AAAAAA"
#define MSDOS_COL_DARK_GRAY    "#555555"

// Mapping
#define MSDOS_WINDOW_APP          MSDOS_COL_BLUE
#define MSDOS_WINDOW_WIDGET       MSDOS_COL_BLUE
#define MSDOS_WINDOW_DOCK_WIDGET  MSDOS_COL_BLUE
#define MSDOS_BORDER_COLOR        MSDOS_COL_LIGHT_GREEN
#define MSDOS_BACKGROUND_BUTTON   MSDOS_COL_BLACK
#define MSDOS_BACKGROUND_BUTTON_DISABLED MSDOS_COL_DARK_GRAY
#define MSDOS_TEXT_ALL            MSDOS_COL_LIGHT_GRAY
#define MSDOS_TEXT_LABEL          MSDOS_COL_LIGHT_GREEN
#define MSDOS_TEXT_COMPONENT      MSDOS_COL_WHITE
#define MSDOS_CONTROLS            MSDOS_COL_LIGHT_GREEN
#define MSDOS_COMPONENT_DISABLED  MSDOS_COL_DARK_GRAY
#define MSDOS_WINDOW_CONTROL_HOVER MSDOS_COL_LIGHT_CYAN
#define MSDOS_WINDOW_EXIT_HOVER   MSDOS_COL_LIGHT_RED
#define MSDOS_ERROR               MSDOS_COL_LIGHT_RED
#define MSDOS_WARNING             MSDOS_COL_YELLOW
#define MSDOS_RUNNING             MSDOS_COL_LIGHT_CYAN
#define MSDOS_UNUSED              MSDOS_COL_DARK_GRAY
#define MSDOS_BACKGROUND_FOCUS_IN MSDOS_COL_LIGHT_BLUE
#define MSDOS_DATA_INPUT_AREA     MSDOS_COL_BLACK
#define MSDOS_CHART               MSDOS_COL_BLACK
#define MSDOS_CHART_GRIDLEG_DEFAULT MSDOS_COL_LIGHT_GRAY
#define MSDOS_CHART_GRIDLEG_LOW_CONTRAST MSDOS_COL_DARK_GRAY
#define MSDOS_DISPLAY             MSDOS_COL_BLACK

// Channels (distinct brights)
#define MSDOS_CHANNEL_1 MSDOS_COL_LIGHT_CYAN
#define MSDOS_CHANNEL_2 MSDOS_COL_YELLOW
#define MSDOS_CHANNEL_3 MSDOS_COL_LIGHT_RED
#define MSDOS_CHANNEL_4 MSDOS_COL_LIGHT_GREEN
#define MSDOS_CHANNEL_5 MSDOS_COL_LIGHT_MAGENTA
#define MSDOS_CHANNEL_6 MSDOS_COL_LIGHT_BLUE
#define MSDOS_CHANNEL_7 MSDOS_COL_LIGHT_GREEN
#define MSDOS_CHANNEL_8 MSDOS_COL_WHITE

#define MSDOS_STYLE_USE_TRANSPARENCY false
#define MSDOS_STYLE_USE_CUSTOM_DIALS false

#define MSDOS_STYLE_GLOBAL           MSDOS_STYLE_OBJECT \
                                     MSDOS_STYLE_SPLITTER MSDOS_STYLE_SCROLLAREA MSDOS_STYLE_CHECK_BUTTON \
                                     MSDOS_STYLE_SCROLLBAR MSDOS_STYLE_MENU MSDOS_STYLE_TAB \
                                     MSDOS_STYLE_TOOLTIP MSDOS_STYLE_COMBO_BOX MSDOS_STYLE_LINEEDIT \
                                     MSDOS_STYLE_LIST_VIEW MSDOS_STYLE_PROGRESS_BAR MSDOS_STYLE_LABEL

#define MSDOS_STYLE_OBJECT \
"QObject{background-color:" MSDOS_WINDOW_APP ";font:10pt " MSDOS_FONT ";color:" MSDOS_TEXT_ALL ";border:none;}"

#define MSDOS_STYLE_DOCK_WIDGET \
"QDockWidget{border:1px solid " MSDOS_BORDER_COLOR "; background-color:" MSDOS_WINDOW_DOCK_WIDGET ";}"

#define MSDOS_STYLE_WIDGET \
"QWidget{background-color:" MSDOS_WINDOW_WIDGET ";}"

#define MSDOS_STYLE_SPLITTER \
"QSplitter::handle{background-color:" MSDOS_WINDOW_WIDGET ";border:1px solid " MSDOS_BORDER_COLOR ";}"

#define MSDOS_STYLE_SCROLLAREA \
"QScrollArea{border:1px solid " MSDOS_BORDER_COLOR ";}"

#define MSDOS_STYLE_SCROLLBAR \
"QScrollBar:vertical{border:1px solid " MSDOS_BORDER_COLOR ";width:12px;margin:0;}\n" \
"QScrollBar:horizontal{border:1px solid " MSDOS_BORDER_COLOR ";height:12px;margin:0;}\n" \
"QScrollBar::handle:vertical{border:1px solid " MSDOS_BORDER_COLOR ";background-color:" MSDOS_BACKGROUND_BUTTON ";min-height:30px;}\n" \
"QScrollBar::handle:horizontal{border:1px solid " MSDOS_BORDER_COLOR ";background-color:" MSDOS_BACKGROUND_BUTTON ";min-width:30px;}\n" \
"QScrollBar::add-line,QScrollBar::sub-line{height:0px;width:0px;}\n" \
"QScrollBar::add-page,QScrollBar::sub-page{background-color:" MSDOS_WINDOW_WIDGET ";}"

#define MSDOS_STYLE_LINEEDIT \
"QLineEdit{border:1px solid " MSDOS_BORDER_COLOR ";background-color:" MSDOS_DATA_INPUT_AREA ";font:10pt " MSDOS_FONT ";padding:1px;margin:0;color:" MSDOS_TEXT_COMPONENT ";}"

#define MSDOS_STYLE_MENU \
"QMenu{border:1px solid " MSDOS_BORDER_COLOR ";background-color:" MSDOS_DATA_INPUT_AREA ";}QMenu::item{background-color:" MSDOS_DATA_INPUT_AREA ";color:" MSDOS_TEXT_COMPONENT ";}QMenu::item:selected{background-color:" MSDOS_WINDOW_CONTROL_HOVER ";}"

#define MSDOS_STYLE_TAB \
"QTabWidget::pane {border:1px solid " MSDOS_BORDER_COLOR "; top:-1px;} QTabWidget::tab-bar {}" \
"QTabBar::tab {background-color:" MSDOS_BACKGROUND_BUTTON ";border:1px solid " MSDOS_BORDER_COLOR ";padding:3px 6px;color:" MSDOS_TEXT_COMPONENT ";min-width:70px;}" \
"QTabBar::tab:selected,QTabBar::tab:hover{background-color:" MSDOS_WINDOW_APP ";}" \
"QTabBar::tab:selected{border:1px solid " MSDOS_WARNING ";}"

#define MSDOS_STYLE_TOOLTIP \
"QToolTip{border:1px solid " MSDOS_BORDER_COLOR ";color:" MSDOS_TEXT_ALL ";background-color:" MSDOS_WINDOW_APP ";}"

#define MSDOS_STYLE_LIST_VIEW \
"QListView{border:1px solid " MSDOS_BORDER_COLOR ";alternate-background-color:" MSDOS_WINDOW_WIDGET ";}"

#define MSDOS_STYLE_COMBO_BOX \
"QComboBox{background-color:" MSDOS_DATA_INPUT_AREA ";border:1px solid " MSDOS_BORDER_COLOR ";padding:2px;color:" MSDOS_TEXT_COMPONENT ";}" \
"QComboBox QAbstractItemView{border:1px solid " MSDOS_BORDER_COLOR ";selection-background-color:" MSDOS_WINDOW_CONTROL_HOVER ";background-color:" MSDOS_DATA_INPUT_AREA ";}"

#define MSDOS_STYLE_PUSH_BUTTON \
"QPushButton{padding:1px 5px;font:10pt " MSDOS_FONT ";background-color:" MSDOS_BACKGROUND_BUTTON ";color:" MSDOS_TEXT_COMPONENT ";border:1px solid " MSDOS_BORDER_COLOR ";}" \
"QPushButton:hover{background-color:" MSDOS_WINDOW_APP ";}" \
"QPushButton:pressed{background-color:" MSDOS_DATA_INPUT_AREA ";border:1px solid " MSDOS_WARNING ";}" \
"QPushButton:disabled{background-color:" MSDOS_BACKGROUND_BUTTON_DISABLED ";color:" MSDOS_COMPONENT_DISABLED ";}"

#define MSDOS_STYLE_CHECK_BUTTON \
"QPushButton{padding:1px 5px;font:10pt " MSDOS_FONT ";background-color:" MSDOS_BACKGROUND_BUTTON ";color:" MSDOS_CONTROLS ";border:1px solid " MSDOS_BORDER_COLOR ";}" \
"QPushButton:checked{background-color:" MSDOS_DATA_INPUT_AREA ";color:" MSDOS_TEXT_COMPONENT ";}" \
"QPushButton:hover{background-color:" MSDOS_WINDOW_APP ";}" \
"QPushButton:checked:hover{background-color:" MSDOS_WINDOW_APP ";}" \
"QPushButton:disabled{background-color:" MSDOS_BACKGROUND_BUTTON_DISABLED "; color:" MSDOS_COMPONENT_DISABLED ";}"

#define MSDOS_STYLE_PROGRESS_BAR \
"QProgressBar{border:1px solid " MSDOS_BORDER_COLOR ";border-radius:0;background-color:" MSDOS_DATA_INPUT_AREA ";color:" MSDOS_TEXT_COMPONENT ";text-align:center;}" \
"QProgressBar::chunk{width:10px;background-color:" MSDOS_CONTROLS ";margin:0;}"

#define MSDOS_STYLE_LABEL "QLabel{border:none;color:" MSDOS_TEXT_LABEL ";}"

#define MSDOS_STYLE_CONTROL_BUTTON \
"QPushButton{background-color:" MSDOS_WINDOW_APP ";qproperty-iconSize:15px;padding:2px;border:1px solid " MSDOS_BORDER_COLOR ";}"

#define MSDOS_STYLE_MODULE_BUTTON \
"QPushButton{text-align:left;qproperty-iconSize:26px;color:" MSDOS_TEXT_ALL ";border:1px solid " MSDOS_BORDER_COLOR ";border-radius:0;background-color:" MSDOS_BACKGROUND_BUTTON ";}" \
"QPushButton:disabled{color:" MSDOS_COMPONENT_DISABLED ";}" \
"QPushButton:hover{background-color:" MSDOS_WINDOW_APP ";}" \
"QPushButton:checked{background-color:" MSDOS_DATA_INPUT_AREA ";}" \
"QPushButton:checked:hover{background-color:" MSDOS_WINDOW_APP ";}"

#define MSDOS_STYLE_HOLD_BUTTON \
"QPushButton{padding:1px;font-size:10px;background-color:" MSDOS_BACKGROUND_BUTTON ";color:" MSDOS_TEXT_LABEL ";border:1px solid " MSDOS_BORDER_COLOR ";}" \
"QPushButton:checked{color:" MSDOS_WARNING ";border:1px solid " MSDOS_WARNING ";}" \
"QPushButton:hover{border:1px solid " MSDOS_WARNING ";}"

#define MSDOS_STYLE_DIAL MSDOS_STYLE_LINEEDIT \
"QWidget:disabled{color:" MSDOS_COMPONENT_DISABLED ";}" \
"QWidget{color:" MSDOS_TEXT_COMPONENT ";}" \
"QPushButton{font:10pt " MSDOS_FONT ";}"

#define MSDOS_STYLE_TEXTINPUT \
"QWidget:disabled{color:" MSDOS_COMPONENT_DISABLED ";}" \
"QWidget{font:10pt " MSDOS_FONT ";color:" MSDOS_TEXT_COMPONENT ";}"

#define MSDOS_STYLE_MODULE_DOCK_WIDGET MSDOS_STYLE_WIDGET MSDOS_STYLE_DOCK_WIDGET

class MSDos : public AbstractTheme
{
    Q_OBJECT
public:
    explicit MSDos(QObject *parent = nullptr);
    QString getAppGlobalStyle() override;
    QString getGraphicsPath() override;
    QString getChannelColor(int channelIndex) override;
    ThemePalette buildPalette() override; // provide palette snapshot
private:
    QList<QColor> chanColor = {
        QColor(MSDOS_CHANNEL_1), QColor(MSDOS_CHANNEL_2),
        QColor(MSDOS_CHANNEL_3), QColor(MSDOS_CHANNEL_4),
        QColor(MSDOS_CHANNEL_5), QColor(MSDOS_CHANNEL_6),
        QColor(MSDOS_CHANNEL_7), QColor(MSDOS_CHANNEL_8)
    };
};

#endif // MSDOS_H
