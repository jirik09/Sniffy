#include "tealanalytics.h"
#include "../graphics.h" // ThemePalette definition

TealAnalytics::TealAnalytics(QObject *parent){ Q_UNUSED(parent); }

QString TealAnalytics::getAppGlobalStyle(){
    QString accent = QString(TEAL_CYAN).remove("#");
    return QString(TEAL_STYLE_GLOBAL).arg(accent);
}

QString TealAnalytics::getGraphicsPath(){
    return TEAL_GRAPHICS_PATH;
}

QString TealAnalytics::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}

ThemePalette TealAnalytics::buildPalette(){
    ThemePalette p;
    p.windowApp = TEAL_APP_BG;
    p.windowWidget = TEAL_WIDGET_BG;
    p.backgroundFocusIn = TEAL_FOCUS_BG;
    p.componentDisabled = TEAL_DISABLED;
    p.backgroundButton = TEAL_BTN_BG;
    p.dataInputArea = TEAL_INPUT_BG;
    p.controls = TEAL_TEXT_MAIN;
    p.display = TEAL_CHART_BG;
    p.chart = TEAL_CHART_BG;
    p.chartGridlegDefault = TEAL_GRID_DEF;
    p.chartGridlegLowContrast = TEAL_GRID_LOW;
    p.textAll = TEAL_TEXT_MAIN;
    p.textLabel = TEAL_TEXT_LABEL;
    p.textComponent = TEAL_TEXT_COMP;
    p.windowControlHover = TEAL_HOVER;
    p.windowExitHover = TEAL_EXIT_HOVER;
    p.warning = TEAL_WARNING;
    p.error = TEAL_ERROR;
    p.running = TEAL_RUNNING;
    p.unused = TEAL_DISABLED;
    p.stylePushButton = TEAL_STYLE_PUSH_BUTTON;
    p.styleCheckButton = TEAL_STYLE_CHECK_BUTTON;
    p.styleProgressBar = TEAL_STYLE_PROGRESS_BAR;
    p.styleComboBox = TEAL_STYLE_COMBO_BOX;
    p.styleControlButton = TEAL_STYLE_CONTROL_BUTTON;
    p.styleModuleButton = TEAL_STYLE_MODULE_BUTTON;
    p.styleHoldButton = TEAL_STYLE_HOLD_BUTTON;
    p.styleDial = TEAL_STYLE_DIAL;
    p.styleTextInput = TEAL_STYLE_TEXTINPUT;
    p.styleDockWidget = TEAL_STYLE_MODULE_DOCK_WIDGET;
    p.styleDockWindow = TEAL_STYLE_OBJECT;
    p.styleCustomDialsUsed = TEAL_STYLE_USE_CUSTOM_DIALS;
    p.styleTransparencyUsed = TEAL_STYLE_USE_TRANSPARENCY;
    QString accent = QString(TEAL_CYAN).remove("#");
    p.styleGlobal = QString(TEAL_STYLE_GLOBAL).arg(accent);
    return p;
}
