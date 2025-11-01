#include "aurora.h"
#include "../graphics.h" // ThemePalette definition

Aurora::Aurora(QObject *parent){ Q_UNUSED(parent); }

QString Aurora::getAppGlobalStyle(){
    if(AURORA_STYLE_USE_TRANSPARENCY){
        QString color = QString(AURORA_CONTROLS).remove("#");
        return QString(AURORA_STYLE_GLOBAL).arg(color);
    }
    return QString(AURORA_STYLE_GLOBAL).arg(AURORA_CONTROLS);
}

QString Aurora::getGraphicsPath(){
    return AURORA_GRAPHICS_PATH;
}

QString Aurora::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}

ThemePalette Aurora::buildPalette(){
    ThemePalette p;
    p.windowApp = AURORA_WINDOW_APP;
    p.windowWidget = AURORA_WINDOW_WIDGET;
    p.backgroundFocusIn = AURORA_BACKGROUND_FOCUS_IN;
    p.componentDisabled = AURORA_COMPONENT_DISABLED;
    p.backgroundButton = AURORA_BACKGROUND_BUTTON;
    p.dataInputArea = AURORA_DATA_INPUT_AREA;
    p.controls = AURORA_CONTROLS;
    p.display = AURORA_DISPLAY;
    p.chart = AURORA_CHART;
    p.chartGridlegDefault = AURORA_CHART_GRIDLEG_DEFAULT;
    p.chartGridlegLowContrast = AURORA_CHART_GRIDLEG_LOW_CONTRAST;
    p.textAll = AURORA_TEXT_ALL;
    p.textLabel = AURORA_TEXT_LABEL;
    p.textComponent = AURORA_TEXT_COMPONENT;
    p.windowControlHover = AURORA_WINDOW_CONTROL_HOVER;
    p.windowExitHover = AURORA_WINDOW_EXIT_HOVER;
    p.warning = AURORA_WARNING;
    p.error = AURORA_ERROR;
    p.running = AURORA_RUNNING;
    p.unused = AURORA_UNUSED;
    p.stylePushButton = AURORA_STYLE_PUSH_BUTTON;
    p.styleCheckButton = AURORA_STYLE_CHECK_BUTTON;
    p.styleProgressBar = AURORA_STYLE_PROGRESS_BAR;
    p.styleComboBox = AURORA_STYLE_COMBO_BOX;
    p.styleControlButton = AURORA_STYLE_CONTROL_BUTTON;
    p.styleModuleButton = AURORA_STYLE_MODULE_BUTTON;
    p.styleHoldButton = AURORA_STYLE_HOLD_BUTTON;
    p.styleDial = AURORA_STYLE_DIAL;
    p.styleTextInput = AURORA_STYLE_TEXTINPUT;
    p.styleDockWidget = AURORA_STYLE_MODULE_DOCK_WIDGET;
    p.styleDockWindow = AURORA_STYLE_OBJECT;
    p.styleCustomDialsUsed = AURORA_STYLE_USE_CUSTOM_DIALS;
    p.styleTransparencyUsed = AURORA_STYLE_USE_TRANSPARENCY;
    if(AURORA_STYLE_USE_TRANSPARENCY){
        QString color = QString(AURORA_CONTROLS).remove("#");
        p.styleGlobal = QString(AURORA_STYLE_GLOBAL).arg(color);
    } else {
        p.styleGlobal = QString(AURORA_STYLE_GLOBAL).arg(AURORA_CONTROLS);
    }
    return p;
}
