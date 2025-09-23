#include "hudcyan.h"
#include "../graphics.h" // ThemePalette definition

HudCyan::HudCyan(QObject *parent){ Q_UNUSED(parent); }

QString HudCyan::getAppGlobalStyle(){
    QString accent = QString(HUD_CYAN).remove("#");
    return QString(HUD_STYLE_GLOBAL).arg(accent);
}

QString HudCyan::getGraphicsPath(){
    return HUD_GRAPHICS_PATH;
}

QString HudCyan::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}

ThemePalette HudCyan::buildPalette(){
    ThemePalette p;
    p.windowApp = HUD_APP_BG;
    p.windowWidget = HUD_WIDGET_BG;
    p.backgroundFocusIn = HUD_FOCUS_BG;
    p.componentDisabled = HUD_DISABLED;
    p.backgroundButton = HUD_BTN_BG;
    p.dataInputArea = HUD_INPUT_BG;
    p.controls = HUD_TEXT_MAIN;
    p.display = HUD_CHART_BG;
    p.chart = HUD_CHART_BG;
    p.chartGridlegDefault = HUD_GRID_DEF;
    p.chartGridlegLowContrast = HUD_GRID_LOW;
    p.textAll = HUD_TEXT_MAIN;
    p.textLabel = HUD_TEXT_LABEL;
    p.textComponent = HUD_TEXT_COMP;
    p.windowControlHover = HUD_HOVER;
    p.windowExitHover = HUD_EXIT_HOVER;
    p.warning = HUD_WARNING;
    p.error = HUD_ERROR;
    p.running = HUD_RUNNING;
    p.unused = HUD_DISABLED;
    p.stylePushButton = HUD_STYLE_PUSH_BUTTON;
    p.styleCheckButton = HUD_STYLE_CHECK_BUTTON;
    p.styleProgressBar = HUD_STYLE_PROGRESS_BAR;
    p.styleComboBox = HUD_STYLE_COMBO_BOX;
    p.styleControlButton = HUD_STYLE_CONTROL_BUTTON;
    p.styleModuleButton = HUD_STYLE_MODULE_BUTTON;
    p.styleHoldButton = HUD_STYLE_HOLD_BUTTON;
    p.styleDial = HUD_STYLE_DIAL;
    p.styleTextInput = HUD_STYLE_TEXTINPUT;
    p.styleDockWidget = HUD_STYLE_MODULE_DOCK_WIDGET;
    p.styleDockWindow = HUD_STYLE_OBJECT;
    p.styleCustomDialsUsed = HUD_STYLE_USE_CUSTOM_DIALS;
    p.styleTransparencyUsed = HUD_STYLE_USE_TRANSPARENCY;
    QString accent = QString(HUD_CYAN).remove("#");
    p.styleGlobal = QString(HUD_STYLE_GLOBAL).arg(accent);
    return p;
}
