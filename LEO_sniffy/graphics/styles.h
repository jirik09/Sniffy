#ifndef STYLES_H
#define STYLES_H

#include "../graphics/colors.h"

#define IMAGE_UNITS_HZ    "image: url(:/graphics/graphics/units_hz.png); border: none;"
#define IMAGE_UNITS_SEC   "image: url(:/graphics/graphics/units_sec.png); border: none;"
#define IMAGE_UNITS_VOLT   "image: url(:/graphics/graphics/units_v.png); border: none;"
#define IMAGE_UNITS_VRIPPLE   "image: url(:/graphics/graphics/units_Vripple.png); border: none;"
#define IMAGE_UNITS_VMAX   "image: url(:/graphics/graphics/units_Vmax.png); border: none;"
#define IMAGE_UNITS_VMIN   "image: url(:/graphics/graphics/units_Vmin.png); border: none;"

#define IMAGE_SIGN_PERCENT "image: url(:/graphics/graphics/units_perc.png); border: none;"
#define IMAGE_SIGN_ERR     "image: url(:/graphics/graphics/units_err.png); border: none;"
#define IMAGE_SIGN_ERRAVG  "image: url(:/graphics/graphics/units_erravg.png); border: none;"
#define IMAGE_SIGN_AVG     "image: url(:/graphics/graphics/units_avg.png); border: none;"
#define IMAGE_SIGN_PLSMNS  "image: url(:/graphics/graphics/sign_pm.png); border: none;"

#define PATH_IMG_EDGE_RISING    ":/graphics/graphics/cnt_edge_rising.png"
#define PATH_IMG_EDGE_FALLING   ":/graphics/graphics/cnt_edge_falling.png"
#define PATH_IMG_EVENT_AB_RR    ":/graphics/graphics/cnt_ab_rr.png"
#define PATH_IMG_EVENT_AB_RF    ":/graphics/graphics/cnt_ab_rf.png"
#define PATH_IMG_EVENT_AB_FR    ":/graphics/graphics/cnt_ab_fr.png"
#define PATH_IMG_EVENT_AB_FF    ":/graphics/graphics/cnt_ab_ff.png"
#define PATH_IMG_EVENT_BA_RR    ":/graphics/graphics/cnt_ba_rr.png"
#define PATH_IMG_EVENT_BA_RF    ":/graphics/graphics/cnt_ba_rf.png"
#define PATH_IMG_EVENT_BA_FR    ":/graphics/graphics/cnt_ba_fr.png"
#define PATH_IMG_EVENT_BA_FF    ":/graphics/graphics/cnt_ba_ff.png"

#define CONTEXT_MENU_HOVER      QString::fromUtf8("QMenu::item{background-color: " BACKGROUND_COLOR_BUTTON "} QMenu::item:selected{background-color: " COLOR_HOVER "}")

/* GLOBAL APP STYLESHEET */
#define STYLESH_GLOBAL               STYLESH_OBJECT+STYLESH_SPLITTER+STYLESH_BUTTON+STYLESH_SCROLL_GLOBAL

#define STYLESH_OBJECT               QString::fromUtf8("QObject{background-color:" BACKGROUND_COLOR_APP "font: 10pt 'MS Shell Dlg 2';color: rgb(228, 228, 228);border:none;}")
#define STYLESH_SPLITTER             QString::fromUtf8("QSplitter::handle{background-color:" BACKGROUND_COLOR_BUTTON "}")
#define STYLESH_BUTTON               QString::fromUtf8("QPushButton{padding-top: 1px;padding-bottom: 1px;padding-left: 3px;padding-right: 3px;}")

#define STYLESH_SCROLL_GLOBAL        STYLESH_SCROLLAREA+STYLESH_SCROLLBAR_VER+STYLESH_SCROLLBAR_HOR+STYLESH_SCROLLBAR_HVER+STYLESH_SCROLLBAR_HHOR+STYLESH_SCROLLBAR_ADD_LINE+STYLESH_SCROLLBAR_SUB_LINE+STYLESH_SCROLLBAR_UP_ARROW+STYLESH_SCROLLBAR_ADD_PG
#define STYLESH_SCROLLAREA           QString::fromUtf8("QScrollArea {border:none}")
#define STYLESH_SCROLLBAR_VER        QString::fromUtf8("QScrollBar:vertical {border: none;background:" BACKGROUND_COLOR_FOCUS_IN "width: 10px;margin: 0 0 0 0;}")
#define STYLESH_SCROLLBAR_HOR        QString::fromUtf8("QScrollBar:horizontal {border: none;background:" BACKGROUND_COLOR_FOCUS_IN "height: 10px;margin: 0 0 0 0;}")
#define STYLESH_SCROLLBAR_HVER       QString::fromUtf8("QScrollBar::handle:vertical {border: none;background:" COLOR_COMMON_DARK_GREYBLUE "min-height: 38px;}")
#define STYLESH_SCROLLBAR_HHOR       QString::fromUtf8("QScrollBar::handle:horizontal {border: none;background:" COLOR_COMMON_DARK_GREYBLUE "min-width: 38px;}")
#define STYLESH_SCROLLBAR_ADD_LINE   QString::fromUtf8("QScrollBar::add-line {height: 0px;}")
#define STYLESH_SCROLLBAR_SUB_LINE   QString::fromUtf8("QScrollBar::sub-line {height: 0px;}")
#define STYLESH_SCROLLBAR_UP_ARROW   QString::fromUtf8("QScrollBar::up-arrow, QScrollBar::down-arrow {background: none;}")
#define STYLESH_SCROLLBAR_ADD_PG     QString::fromUtf8("QScrollBar::add-page, QScrollBar::sub-page {background: none;}")


#endif // STYLES_H
