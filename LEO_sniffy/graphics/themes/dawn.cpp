#include "dawn.h"

Dawn::Dawn(QObject *parent)
{
    Q_UNUSED(parent);

    Graphics::COLOR_WINDOW_APP = DAWN_WINDOW_APP;
    Graphics::COLOR_WINDOW_WIDGET = DAWN_WINDOW_WIDGET;

    Graphics::COLOR_BACKGROUND_FOCUS_IN = DAWN_BACKGROUND_FOCUS_IN;
    Graphics::COLOR_COMPONENT_DISABLED = DAWN_COMPONENT_DISABLED;
    Graphics::COLOR_CONTROLS = DAWN_CONTROLS;
    Graphics::COLOR_DATA_INPUT_AREA = DAWN_DATA_INPUT_AREA;
    Graphics::COLOR_DISPLAY = DAWN_DISPLAY;
    Graphics::COLOR_CHART = DAWN_CHART;
    Graphics::COLOR_CHART_GRIDLEG_DEFAULT = DAWN_CHART_GRIDLEG_DEFAULT;
    Graphics::COLOR_CHART_GRIDLEG_LOW_CONTRAST = DAWN_CHART_GRIDLEG_LOW_CONTRAST;
    Graphics::COLOR_TEXT_ALL = DAWN_TEXT_ALL;
    Graphics::COLOR_TEXT_LABEL = DAWN_TEXT_LABEL;
    Graphics::COLOR_TEXT_COMPONENT = DAWN_TEXT_COMPONENT;
    Graphics::COLOR_WINDOW_CONTROL_HOVER = DAWN_WINDOW_CONTROL_HOVER;
    Graphics::COLOR_WINDOW_EXIT_HOVER = DAWN_WINDOW_EXIT_HOVER;
    Graphics::COLOR_WARNING = DAWN_WARNING;
    Graphics::COLOR_ERROR = DAWN_ERROR;
    Graphics::COLOR_RUNNING = DAWN_RUNNING;
    Graphics::COLOR_UNUSED = DAWN_UNUSED;

    /* These called with QString().arg() from code (must have %1) */
    Graphics::STYLE_PUSH_BUTTON = DAWN_STYLE_PUSH_BUTTON;
    Graphics::STYLE_CHECK_BUTTON = DAWN_STYLE_CHECK_BUTTON;
    Graphics::STYLE_PROGRESS_BAR = DAWN_STYLE_PROGRESS_BAR;

    Graphics::STYLE_COMBO_BOX = DAWN_STYLE_COMBO_BOX;
    Graphics::STYLE_CONTROL_BUTTON = DAWN_STYLE_CONTROL_BUTTON;
    Graphics::STYLE_MODULE_BUTTON = DAWN_STYLE_MODULE_BUTTON;
    Graphics::STYLE_HOLD_BUTTON = DAWN_STYLE_HOLD_BUTTON;
    Graphics::STYLE_DIAL = DAWN_STYLE_DIAL;
    Graphics::STYLE_TEXTINPUT = DAWN_STYLE_TEXTINPUT;
    Graphics::STYLE_DOCK_WIDGET = DAWN_STYLE_MODULE_DOCK_WIDGET;
    Graphics::STYLE_DOCK_WINDOW = DAWN_STYLE_OBJECT;

    Graphics::STYLE_CUSTOM_DIALS_USED = DAWN_STYLE_USE_CUSTOM_DIALS;
    Graphics::STYLE_TRANSPARENCY_USED = DAWN_STYLE_USE_TRANSPARENCY;

    if(DAWN_STYLE_USE_TRANSPARENCY){
        QString color = QString(DAWN_CONTROLS).remove("#");
        Graphics::STYLE_GLOBAL = QString(DAWN_STYLE_GLOBAL).arg(color);
    }else{
        Graphics::STYLE_GLOBAL = QString(DAWN_STYLE_GLOBAL).arg(DAWN_CONTROLS);
    }
}

QString Dawn::getAppGlobalStyle(){
    return Graphics::STYLE_GLOBAL;
}

QString Dawn::getGraphicsPath(){
    return DAWN_GRAPHICS_PATH;
}

QString Dawn::getChannelColor(int channelIndex){
    return chanColor.at(channelIndex).name();
}
