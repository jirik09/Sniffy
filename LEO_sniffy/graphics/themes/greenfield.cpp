#include "greenfield.h"
#include "../graphics.h" // ThemePalette definition

Greenfield::Greenfield(QObject *parent){ Q_UNUSED(parent); }

QString Greenfield::getAppGlobalStyle(){
    if(GREENFIELD_STYLE_USE_TRANSPARENCY){
        QString color = QString(GREENFIELD_CONTROLS).remove("#");
        return QString(GREENFIELD_STYLE_GLOBAL).arg(color);
    }
    return QString(GREENFIELD_STYLE_GLOBAL).arg(GREENFIELD_CONTROLS);
}

QString Greenfield::getGraphicsPath(){
    return GREENFIELD_GRAPHICS_PATH;
}

QString Greenfield::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}

ThemePalette Greenfield::buildPalette(){
    ThemePalette p;
    p.windowApp = GREENFIELD_WINDOW_APP;
    p.windowWidget = GREENFIELD_WINDOW_WIDGET;
    p.backgroundFocusIn = GREENFIELD_BACKGROUND_FOCUS_IN;
    p.componentDisabled = GREENFIELD_COMPONENT_DISABLED;
    p.backgroundButton = GREENFIELD_BACKGROUND_BUTTON;
    p.dataInputArea = GREENFIELD_DATA_INPUT_AREA;
    p.controls = GREENFIELD_CONTROLS;
    p.display = GREENFIELD_DISPLAY;
    p.chart = GREENFIELD_CHART;
    p.chartGridlegDefault = GREENFIELD_CHART_GRIDLEG_DEFAULT;
    p.chartGridlegLowContrast = GREENFIELD_CHART_GRIDLEG_LOW_CONTRAST;
    p.textAll = GREENFIELD_TEXT_ALL;
    p.textLabel = GREENFIELD_TEXT_LABEL;
    p.textComponent = GREENFIELD_TEXT_COMPONENT;
    p.windowControlHover = GREENFIELD_WINDOW_CONTROL_HOVER;
    p.windowExitHover = GREENFIELD_WINDOW_EXIT_HOVER;
    p.warning = GREENFIELD_WARNING;
    p.error = GREENFIELD_ERROR;
    p.running = GREENFIELD_RUNNING;
    p.unused = GREENFIELD_UNUSED;
    p.moduleActiveText = GREENFIELD_GREEN;
    p.stylePushButton = GREENFIELD_STYLE_PUSH_BUTTON;
    p.styleCheckButton = GREENFIELD_STYLE_CHECK_BUTTON;
    p.styleProgressBar = GREENFIELD_STYLE_PROGRESS_BAR;
    p.styleComboBox = GREENFIELD_STYLE_COMBO_BOX;
    p.styleControlButton = GREENFIELD_STYLE_CONTROL_BUTTON;
    p.styleModuleButton = GREENFIELD_STYLE_MODULE_BUTTON;
    p.styleHoldButton = GREENFIELD_STYLE_HOLD_BUTTON;
    p.styleDial = GREENFIELD_STYLE_DIAL;
    p.styleTextInput = GREENFIELD_STYLE_TEXTINPUT;
    p.styleDockWidget = GREENFIELD_STYLE_MODULE_DOCK_WIDGET;
    p.styleDockWindow = GREENFIELD_STYLE_OBJECT;
    p.styleCustomDialsUsed = GREENFIELD_STYLE_USE_CUSTOM_DIALS;
    p.styleTransparencyUsed = GREENFIELD_STYLE_USE_TRANSPARENCY;
    if(GREENFIELD_STYLE_USE_TRANSPARENCY){
        QString color = QString(GREENFIELD_CONTROLS).remove("#");
        p.styleGlobal = QString(GREENFIELD_STYLE_GLOBAL).arg(color);
    } else {
        p.styleGlobal = QString(GREENFIELD_STYLE_GLOBAL).arg(GREENFIELD_CONTROLS);
    }
    p.channelTextColors = {
        GREENFIELD_CHANNEL_TEXT_1, GREENFIELD_CHANNEL_TEXT_2,
        GREENFIELD_CHANNEL_TEXT_3, GREENFIELD_CHANNEL_TEXT_4,
        "", "", "", ""
    };
    return p;
}
