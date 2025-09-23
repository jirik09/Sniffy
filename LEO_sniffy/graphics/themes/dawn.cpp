#include "dawn.h"
#include "../graphics.h" // For ThemePalette definition

Dawn::Dawn(QObject *parent){ Q_UNUSED(parent); }

QString Dawn::getAppGlobalStyle(){
    if(DAWN_STYLE_USE_TRANSPARENCY){
        QString color = QString(DAWN_CONTROLS).remove("#");
        return QString(DAWN_STYLE_GLOBAL).arg(color);
    }
    return QString(DAWN_STYLE_GLOBAL).arg(DAWN_CONTROLS);
}

QString Dawn::getGraphicsPath(){
    return DAWN_GRAPHICS_PATH;
}

QString Dawn::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}

ThemePalette Dawn::buildPalette(){
    ThemePalette p;
    p.windowApp = DAWN_WINDOW_APP;
    p.windowWidget = DAWN_WINDOW_WIDGET;
    p.backgroundFocusIn = DAWN_BACKGROUND_FOCUS_IN;
    p.componentDisabled = DAWN_COMPONENT_DISABLED;
    p.backgroundButton = DAWN_BACKGROUND_BUTTON;
    p.dataInputArea = DAWN_DATA_INPUT_AREA;
    p.controls = DAWN_CONTROLS;
    p.display = DAWN_DISPLAY;
    p.chart = DAWN_CHART;
    p.chartGridlegDefault = DAWN_CHART_GRIDLEG_DEFAULT;
    p.chartGridlegLowContrast = DAWN_CHART_GRIDLEG_LOW_CONTRAST;
    p.textAll = DAWN_TEXT_ALL;
    p.textLabel = DAWN_TEXT_LABEL;
    p.textComponent = DAWN_TEXT_COMPONENT;
    p.windowControlHover = DAWN_WINDOW_CONTROL_HOVER;
    p.windowExitHover = DAWN_WINDOW_EXIT_HOVER;
    p.warning = DAWN_WARNING;
    p.error = DAWN_ERROR;
    p.running = DAWN_RUNNING;
    p.unused = DAWN_UNUSED;
    p.stylePushButton = DAWN_STYLE_PUSH_BUTTON;
    p.styleCheckButton = DAWN_STYLE_CHECK_BUTTON;
    p.styleProgressBar = DAWN_STYLE_PROGRESS_BAR;
    p.styleComboBox = DAWN_STYLE_COMBO_BOX;
    p.styleControlButton = DAWN_STYLE_CONTROL_BUTTON;
    p.styleModuleButton = DAWN_STYLE_MODULE_BUTTON;
    p.styleHoldButton = DAWN_STYLE_HOLD_BUTTON;
    p.styleDial = DAWN_STYLE_DIAL;
    p.styleTextInput = DAWN_STYLE_TEXTINPUT;
    p.styleDockWidget = DAWN_STYLE_MODULE_DOCK_WIDGET;
    p.styleDockWindow = DAWN_STYLE_OBJECT;
    p.styleCustomDialsUsed = DAWN_STYLE_USE_CUSTOM_DIALS;
    p.styleTransparencyUsed = DAWN_STYLE_USE_TRANSPARENCY;
    if(DAWN_STYLE_USE_TRANSPARENCY){
        QString color = QString(DAWN_CONTROLS).remove("#");
        p.styleGlobal = QString(DAWN_STYLE_GLOBAL).arg(color);
    } else {
        p.styleGlobal = QString(DAWN_STYLE_GLOBAL).arg(DAWN_CONTROLS);
    }
    return p;
}
