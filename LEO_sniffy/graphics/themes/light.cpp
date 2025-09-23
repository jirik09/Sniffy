#include "light.h"
#include "../graphics.h" // For ThemePalette definition

Light::Light(QObject *parent){ Q_UNUSED(parent); }


QString Light::getAppGlobalStyle(){
    if(LIGHT_STYLE_USE_TRANSPARENCY){
        QString color = QString(LIGHT_CONTROLS).remove("#");
        return QString(LIGHT_STYLE_GLOBAL).arg(color);
    }
    return QString(LIGHT_STYLE_GLOBAL).arg(LIGHT_CONTROLS);
}

QString Light::getGraphicsPath(){
    return LIGHT_GRAPHICS_PATH;
}

QString Light::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}

ThemePalette Light::buildPalette(){
    ThemePalette p;
    p.windowApp = LIGHT_WINDOW_APP;
    p.windowWidget = LIGHT_WINDOW_WIDGET;
    p.backgroundFocusIn = LIGHT_BACKGROUND_FOCUS_IN;
    p.componentDisabled = LIGHT_COMPONENT_DISABLED;
    p.backgroundButton = LIGHT_BACKGROUND_BUTTON;
    p.dataInputArea = LIGHT_DATA_INPUT_AREA;
    p.controls = LIGHT_CONTROLS;
    p.display = LIGHT_DISPLAY;
    p.chart = LIGHT_CHART;
    p.chartGridlegDefault = LIGHT_CHART_GRIDLEG_DEFAULT;
    p.chartGridlegLowContrast = LIGHT_CHART_GRIDLEG_LOW_CONTRAST;
    p.textAll = LIGHT_TEXT_ALL;
    p.textLabel = LIGHT_TEXT_LABEL;
    p.textComponent = LIGHT_TEXT_COMPONENT;
    p.windowControlHover = LIGHT_WINDOW_CONTROL_HOVER;
    p.windowExitHover = LIGHT_WINDOW_EXIT_HOVER;
    p.warning = LIGHT_WARNING;
    p.error = LIGHT_ERROR;
    p.running = LIGHT_RUNNING;
    p.unused = LIGHT_UNUSED;
    p.stylePushButton = LIGHT_STYLE_PUSH_BUTTON;
    p.styleCheckButton = LIGHT_STYLE_CHECK_BUTTON;
    p.styleProgressBar = LIGHT_STYLE_PROGRESS_BAR;
    p.styleComboBox = LIGHT_STYLE_COMBO_BOX;
    p.styleControlButton = LIGHT_STYLE_CONTROL_BUTTON;
    p.styleModuleButton = LIGHT_STYLE_MODULE_BUTTON;
    p.styleHoldButton = LIGHT_STYLE_HOLD_BUTTON;
    p.styleDial = LIGHT_STYLE_DIAL;
    p.styleTextInput = LIGHT_STYLE_TEXTINPUT;
    p.styleDockWidget = LIGHT_STYLE_MODULE_DOCK_WIDGET;
    p.styleDockWindow = LIGHT_STYLE_OBJECT;
    p.styleCustomDialsUsed = LIGHT_STYLE_USE_CUSTOM_DIALS;
    p.styleTransparencyUsed = LIGHT_STYLE_USE_TRANSPARENCY;
    if(LIGHT_STYLE_USE_TRANSPARENCY){
        QString color = QString(LIGHT_CONTROLS).remove("#");
        p.styleGlobal = QString(LIGHT_STYLE_GLOBAL).arg(color);
    } else {
        p.styleGlobal = QString(LIGHT_STYLE_GLOBAL).arg(LIGHT_CONTROLS);
    }
    return p;
}
