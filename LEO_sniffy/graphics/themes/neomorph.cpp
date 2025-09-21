#include "neomorph.h"

Neomorph::Neomorph(QObject *parent)
{
    Q_UNUSED(parent);

    Graphics::COLOR_WINDOW_APP = NEO_BASE_BG;
    Graphics::COLOR_WINDOW_WIDGET = NEO_PANEL_BG;

    Graphics::COLOR_BACKGROUND_FOCUS_IN = NEO_FOCUS_BG;
    Graphics::COLOR_COMPONENT_DISABLED = NEO_DISABLED;
    Graphics::COLOR_BACKGROUND_BUTTON = NEO_BUTTON_BG;
    Graphics::COLOR_CONTROLS = NEO_CONTROLS;
    Graphics::COLOR_DATA_INPUT_AREA = NEO_DATA_INPUT_BG;
    Graphics::COLOR_DISPLAY = NEO_CHART_BG;
    Graphics::COLOR_CHART = NEO_CHART_BG;
    Graphics::COLOR_CHART_GRIDLEG_DEFAULT = NEO_GRID_DEFAULT;
    Graphics::COLOR_CHART_GRIDLEG_LOW_CONTRAST = NEO_GRID_LOW;
    Graphics::COLOR_TEXT_ALL = NEO_TEXT_MAIN;
    Graphics::COLOR_TEXT_LABEL = NEO_TEXT_SECONDARY;
    Graphics::COLOR_TEXT_COMPONENT = NEO_TEXT_COMPONENT;
    Graphics::COLOR_WINDOW_CONTROL_HOVER = "#eef2fb";
    Graphics::COLOR_WINDOW_EXIT_HOVER = NEO_ERROR;
    Graphics::COLOR_WARNING = NEO_WARNING;
    Graphics::COLOR_ERROR = NEO_ERROR;
    Graphics::COLOR_RUNNING = NEO_RUNNING;
    Graphics::COLOR_UNUSED = NEO_DISABLED;

    /* These called with QString().arg() from code (must have %1) */
    Graphics::STYLE_PUSH_BUTTON = NEO_STYLE_PUSH_BUTTON;
    Graphics::STYLE_CHECK_BUTTON = NEO_STYLE_CHECK_BUTTON;
    Graphics::STYLE_PROGRESS_BAR = NEO_STYLE_PROGRESS_BAR;

    Graphics::STYLE_COMBO_BOX = NEO_STYLE_COMBO_BOX;
    Graphics::STYLE_CONTROL_BUTTON = NEO_STYLE_CONTROL_BUTTON;
    Graphics::STYLE_MODULE_BUTTON = NEO_STYLE_MODULE_BUTTON;
    Graphics::STYLE_HOLD_BUTTON = NEO_STYLE_HOLD_BUTTON;
    Graphics::STYLE_DIAL = NEO_STYLE_DIAL;
    Graphics::STYLE_TEXTINPUT = NEO_STYLE_TEXTINPUT;
    Graphics::STYLE_DOCK_WIDGET = NEO_STYLE_MODULE_DOCK_WIDGET;
    Graphics::STYLE_DOCK_WINDOW = NEO_STYLE_OBJECT;

    Graphics::STYLE_CUSTOM_DIALS_USED = NEO_STYLE_USE_CUSTOM_DIALS;
    Graphics::STYLE_TRANSPARENCY_USED = NEO_STYLE_USE_TRANSPARENCY;

    // Keep %1 placeholder coloring using full hex when transparency not used
    Graphics::STYLE_GLOBAL = QString(NEO_STYLE_GLOBAL).arg(NEO_ACCENT);
}

QString Neomorph::getAppGlobalStyle(){
    return Graphics::STYLE_GLOBAL;
}

QString Neomorph::getGraphicsPath(){
    return NEOMORPH_GRAPHICS_PATH;
}

QString Neomorph::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}
