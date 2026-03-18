#include "ember.h"
#include "../graphics.h"

Ember::Ember(QObject *parent){ Q_UNUSED(parent); }

QString Ember::getAppGlobalStyle(){
    if(EMBER_STYLE_USE_TRANSPARENCY){
        QString color = QString(EMBER_CONTROLS).remove("#");
        return QString(EMBER_STYLE_GLOBAL).arg(color);
    }
    return QString(EMBER_STYLE_GLOBAL).arg(EMBER_CONTROLS);
}

QString Ember::getGraphicsPath(){
    return EMBER_GRAPHICS_PATH;
}

QString Ember::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}

ThemePalette Ember::buildPalette(){
    ThemePalette p;
    p.windowApp = EMBER_WINDOW_APP;
    p.windowWidget = EMBER_WINDOW_WIDGET;
    p.backgroundFocusIn = EMBER_BACKGROUND_FOCUS_IN;
    p.componentDisabled = EMBER_COMPONENT_DISABLED;
    p.backgroundButton = EMBER_BACKGROUND_BUTTON;
    p.dataInputArea = EMBER_DATA_INPUT_AREA;
    p.controls = EMBER_CONTROLS;
    p.display = EMBER_DISPLAY;
    p.chart = EMBER_CHART;
    p.chartGridlegDefault = EMBER_CHART_GRIDLEG_DEFAULT;
    p.chartGridlegLowContrast = EMBER_CHART_GRIDLEG_LOW_CONTRAST;
    p.textAll = EMBER_TEXT_ALL;
    p.textLabel = EMBER_TEXT_LABEL;
    p.textComponent = EMBER_TEXT_COMPONENT;
    p.windowControlHover = EMBER_WINDOW_CONTROL_HOVER;
    p.windowExitHover = EMBER_WINDOW_EXIT_HOVER;
    p.warning = EMBER_WARNING;
    p.error = EMBER_ERROR;
    p.running = EMBER_RUNNING;
    p.unused = EMBER_UNUSED;
    p.moduleActiveText = EMBER_TEXT_ALL;
    p.moduleHoverText = EMBER_TEXT_ALL;
    p.moduleDisabledTint = EMBER_WINDOW_CONTROL_HOVER;

    p.stylePushButton = EMBER_STYLE_PUSH_BUTTON;
    p.styleCheckButton = EMBER_STYLE_CHECK_BUTTON;
    p.styleProgressBar = EMBER_STYLE_PROGRESS_BAR;
    p.styleComboBox = EMBER_STYLE_COMBO_BOX;
    p.styleControlButton = EMBER_STYLE_CONTROL_BUTTON;
    p.styleModuleButton = EMBER_STYLE_MODULE_BUTTON;
    p.styleHoldButton = EMBER_STYLE_HOLD_BUTTON;
    p.styleDial = EMBER_STYLE_DIAL;
    p.styleTextInput = EMBER_STYLE_TEXTINPUT;
    p.styleDockWidget = EMBER_STYLE_MODULE_DOCK_WIDGET;
    p.styleDockWindow = EMBER_STYLE_OBJECT;
    p.styleCustomDialsUsed = EMBER_STYLE_USE_CUSTOM_DIALS;
    p.styleTransparencyUsed = EMBER_STYLE_USE_TRANSPARENCY;
    p.isEmberTheme = true;
    if(EMBER_STYLE_USE_TRANSPARENCY){
        QString color = QString(EMBER_CONTROLS).remove("#");
        p.styleGlobal = QString(EMBER_STYLE_GLOBAL).arg(color);
    } else {
        p.styleGlobal = QString(EMBER_STYLE_GLOBAL).arg(EMBER_CONTROLS);
    }
    return p;
}
