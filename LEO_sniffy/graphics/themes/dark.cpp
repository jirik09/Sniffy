#include "dark.h"
#include "../graphics.h" // For ThemePalette definition

Dark::Dark(QObject *parent) { Q_UNUSED(parent); }

QString Dark::getAppGlobalStyle(){
    // Fallback for any legacy call: compute on the fly (no caching needed here)
    if(DARK_STYLE_USE_TRANSPARENCY){
        QString color = QString(DARK_CONTROLS).remove("#");
        return QString(DARK_STYLE_GLOBAL).arg(color);
    }
    return QString(DARK_STYLE_GLOBAL).arg(DARK_CONTROLS);
}

QString Dark::getGraphicsPath(){
    return DARK_GRAPHICS_PATH;
}

QString Dark::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}

ThemePalette Dark::buildPalette(){
    ThemePalette p;
    p.windowApp = DARK_WINDOW_APP;
    p.windowWidget = DARK_WINDOW_WIDGET;
    p.backgroundFocusIn = DARK_BACKGROUND_FOCUS_IN;
    p.componentDisabled = DARK_COMPONENT_DISABLED;
    p.backgroundButton = DARK_BACKGROUND_BUTTON;
    p.dataInputArea = DARK_DATA_INPUT_AREA;
    p.controls = DARK_CONTROLS;
    p.display = DARK_DISPLAY;
    p.chart = DARK_CHART;
    p.chartGridlegDefault = DARK_CHART_GRIDLEG_DEFAULT;
    p.chartGridlegLowContrast = DARK_CHART_GRIDLEG_LOW_CONTRAST;
    p.textAll = DARK_TEXT_ALL;
    p.textLabel = DARK_TEXT_LABEL;
    p.textComponent = DARK_TEXT_COMPONENT;
    p.windowControlHover = DARK_WINDOW_CONTROL_HOVER;
    p.windowExitHover = DARK_WINDOW_EXIT_HOVER;
    p.warning = DARK_WARNING;
    p.error = DARK_ERROR;
    p.running = DARK_RUNNING;
    p.unused = DARK_UNUSED;
    p.stylePushButton = DARK_STYLE_PUSH_BUTTON;
    p.styleCheckButton = DARK_STYLE_CHECK_BUTTON;
    p.styleProgressBar = DARK_STYLE_PROGRESS_BAR;
    p.styleComboBox = DARK_STYLE_COMBO_BOX;
    p.styleControlButton = DARK_STYLE_CONTROL_BUTTON;
    p.styleModuleButton = DARK_STYLE_MODULE_BUTTON;
    p.styleHoldButton = DARK_STYLE_HOLD_BUTTON;
    p.styleDial = DARK_STYLE_DIAL;
    p.styleTextInput = DARK_STYLE_TEXTINPUT;
    p.styleDockWidget = DARK_STYLE_MODULE_DOCK_WIDGET;
    p.styleDockWindow = DARK_STYLE_OBJECT;
    p.styleCustomDialsUsed = DARK_STYLE_USE_CUSTOM_DIALS;
    p.styleTransparencyUsed = DARK_STYLE_USE_TRANSPARENCY;
    if(DARK_STYLE_USE_TRANSPARENCY){
        QString color = QString(DARK_CONTROLS).remove("#");
        p.styleGlobal = QString(DARK_STYLE_GLOBAL).arg(color);
    } else {
        p.styleGlobal = QString(DARK_STYLE_GLOBAL).arg(DARK_CONTROLS);
    }
    return p;
}
