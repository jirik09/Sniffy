#include "neomorph.h"
#include "../graphics.h" // ThemePalette definition

Neomorph::Neomorph(QObject *parent){ Q_UNUSED(parent); }

QString Neomorph::getAppGlobalStyle(){
    return QString(NEO_STYLE_GLOBAL).arg(NEO_ACCENT);
}

QString Neomorph::getGraphicsPath(){
    return NEOMORPH_GRAPHICS_PATH;
}

QString Neomorph::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}

ThemePalette Neomorph::buildPalette(){
    ThemePalette p;
    p.windowApp = NEO_BASE_BG;
    p.windowWidget = NEO_PANEL_BG;
    p.backgroundFocusIn = NEO_FOCUS_BG;
    p.componentDisabled = NEO_DISABLED;
    p.backgroundButton = NEO_BUTTON_BG;
    p.dataInputArea = NEO_DATA_INPUT_BG;
    p.controls = NEO_CONTROLS;
    p.display = NEO_CHART_BG;
    p.chart = NEO_CHART_BG;
    p.chartGridlegDefault = NEO_GRID_DEFAULT;
    p.chartGridlegLowContrast = NEO_GRID_LOW;
    p.textAll = NEO_TEXT_MAIN;
    p.textLabel = NEO_TEXT_SECONDARY;
    p.textComponent = NEO_TEXT_COMPONENT;
    p.windowControlHover = "#eef2fb";
    p.windowExitHover = NEO_ERROR;
    p.warning = NEO_WARNING;
    p.error = NEO_ERROR;
    p.running = NEO_RUNNING;
    p.unused = NEO_DISABLED;
    p.stylePushButton = NEO_STYLE_PUSH_BUTTON;
    p.styleCheckButton = NEO_STYLE_CHECK_BUTTON;
    p.styleProgressBar = NEO_STYLE_PROGRESS_BAR;
    p.styleComboBox = NEO_STYLE_COMBO_BOX;
    p.styleControlButton = NEO_STYLE_CONTROL_BUTTON;
    p.styleModuleButton = NEO_STYLE_MODULE_BUTTON;
    p.styleHoldButton = NEO_STYLE_HOLD_BUTTON;
    p.styleDial = NEO_STYLE_DIAL;
    p.styleTextInput = NEO_STYLE_TEXTINPUT;
    p.styleDockWidget = NEO_STYLE_MODULE_DOCK_WIDGET;
    p.styleDockWindow = NEO_STYLE_OBJECT;
    p.styleCustomDialsUsed = NEO_STYLE_USE_CUSTOM_DIALS;
    p.styleTransparencyUsed = NEO_STYLE_USE_TRANSPARENCY;
    p.styleGlobal = QString(NEO_STYLE_GLOBAL).arg(NEO_ACCENT);
    return p;
}
