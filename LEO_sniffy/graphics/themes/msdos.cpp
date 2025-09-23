#include "msdos.h"
#include "../graphics.h" // for ThemePalette definition

MSDos::MSDos(QObject *parent) { Q_UNUSED(parent); }

QString MSDos::getAppGlobalStyle(){
    return QString(MSDOS_STYLE_GLOBAL); // no dynamic arg needed
}

QString MSDos::getGraphicsPath(){ return MSDOS_GRAPHICS_PATH; }

QString MSDos::getChannelColor(int channelIndex){ return chanColor.at(channelIndex).name(); }

ThemePalette MSDos::buildPalette(){
    ThemePalette p;
    p.windowApp = MSDOS_WINDOW_APP;
    p.windowWidget = MSDOS_WINDOW_WIDGET;
    p.backgroundFocusIn = MSDOS_BACKGROUND_FOCUS_IN;
    p.componentDisabled = MSDOS_COMPONENT_DISABLED;
    p.backgroundButton = MSDOS_BACKGROUND_BUTTON;
    p.dataInputArea = MSDOS_DATA_INPUT_AREA;
    p.controls = MSDOS_CONTROLS;
    p.display = MSDOS_DISPLAY;
    p.chart = MSDOS_CHART;
    p.chartGridlegDefault = MSDOS_CHART_GRIDLEG_DEFAULT;
    p.chartGridlegLowContrast = MSDOS_CHART_GRIDLEG_LOW_CONTRAST;
    p.textAll = MSDOS_TEXT_ALL;
    p.textLabel = MSDOS_TEXT_LABEL;
    p.textComponent = MSDOS_TEXT_COMPONENT;
    p.windowControlHover = MSDOS_WINDOW_CONTROL_HOVER;
    p.windowExitHover = MSDOS_WINDOW_EXIT_HOVER;
    p.warning = MSDOS_WARNING;
    p.error = MSDOS_ERROR;
    p.running = MSDOS_RUNNING;
    p.unused = MSDOS_UNUSED;
    p.stylePushButton = MSDOS_STYLE_PUSH_BUTTON;
    p.styleCheckButton = MSDOS_STYLE_CHECK_BUTTON;
    p.styleProgressBar = MSDOS_STYLE_PROGRESS_BAR;
    p.styleComboBox = MSDOS_STYLE_COMBO_BOX;
    p.styleControlButton = MSDOS_STYLE_CONTROL_BUTTON;
    p.styleModuleButton = MSDOS_STYLE_MODULE_BUTTON;
    p.styleHoldButton = MSDOS_STYLE_HOLD_BUTTON;
    p.styleDial = MSDOS_STYLE_DIAL;
    p.styleTextInput = MSDOS_STYLE_TEXTINPUT;
    p.styleDockWidget = MSDOS_STYLE_MODULE_DOCK_WIDGET;
    p.styleDockWindow = MSDOS_STYLE_OBJECT;
    p.styleCustomDialsUsed = MSDOS_STYLE_USE_CUSTOM_DIALS;
    p.styleTransparencyUsed = MSDOS_STYLE_USE_TRANSPARENCY;
    p.styleGlobal = QString(MSDOS_STYLE_GLOBAL); // static
    return p;
}
