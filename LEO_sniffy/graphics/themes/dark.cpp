#include "dark.h"

Dark::Dark(QObject *parent)
{
    Q_UNUSED(parent);

    Graphics::COLOR_WINDOW_APP = DARK_WINDOW_APP;
    Graphics::COLOR_WINDOW_WIDGET = DARK_WINDOW_WIDGET;

    Graphics::COLOR_BACKGROUND_FOCUS_IN = DARK_BACKGROUND_FOCUS_IN;
    Graphics::COLOR_COMPONENT_DISABLED = DARK_COMPONENT_DISABLED;
    Graphics::COLOR_CONTROLS = DARK_CONTROLS;
    Graphics::COLOR_DATA_INPUT_AREA = DARK_DATA_INPUT_AREA;
    Graphics::COLOR_DISPLAY = DARK_DISPLAY;    
    Graphics::COLOR_CHART = DARK_CHART;
    Graphics::COLOR_CHART_GRIDLEG_DEFAULT = DARK_CHART_GRIDLEG_DEFAULT;
    Graphics::COLOR_CHART_GRIDLEG_LOW_CONTRAST = DARK_CHART_GRIDLEG_LOW_CONTRAST;
    Graphics::COLOR_TEXT_ALL = DARK_TEXT_ALL;
    Graphics::COLOR_TEXT_LABEL = DARK_TEXT_LABEL;
    Graphics::COLOR_TEXT_COMPONENT = DARK_TEXT_COMPONENT;
    Graphics::COLOR_WINDOW_CONTROL_HOVER = DARK_WINDOW_CONTROL_HOVER;
    Graphics::COLOR_WINDOW_EXIT_HOVER = DARK_WINDOW_EXIT_HOVER;
    Graphics::COLOR_WARNING = DARK_WARNING;
    Graphics::COLOR_ERROR = DARK_ERROR;
    Graphics::COLOR_RUNNING = DARK_RUNNING;
    Graphics::COLOR_UNUSED = DARK_UNUSED;

    Graphics::STYLE_PUSH_BUTTON = DARK_STYLE_PUSH_BUTTON;
    Graphics::STYLE_CHECK_BUTTON = DARK_STYLE_CHECK_BUTTON;
    Graphics::STYLE_PROGRESS_BAR = DARK_STYLE_PROGRESS_BAR;
    Graphics::STYLE_COMBO_BOX = DARK_STYLE_COMBO_BOX;

    Graphics::STYLE_CONTROL_BUTTON = DARK_STYLE_CONTROL_BUTTON;
    Graphics::STYLE_MODULE_BUTTON = DARK_STYLE_MODULE_BUTTON;
    Graphics::STYLE_HOLD_BUTTON = DARK_STYLE_HOLD_BUTTON;
    Graphics::STYLE_DIAL = DARK_STYLE_DIAL;
    Graphics::STYLE_CUSTOM_DIALS_USED = DARK_STYLE_USE_CUSTOM_DIALS;
    Graphics::STYLE_TEXTINPUT = DARK_STYLE_TEXTINPUT;
    Graphics::STYLE_DOCK_WIDGET = DARK_STYLE_MODULE_DOCK_WIDGET;
    Graphics::STYLE_DOCK_WINDOW = DARK_STYLE_OBJECT;
}

QString Dark::getAppGlobalStyle(){
    return DARK_STYLE_GLOBAL;
}

QString Dark::getGraphicsPath(){
    return DARK_GRAPHICS_PATH;
}

QString Dark::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}