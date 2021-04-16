#include "graphics.h"

QSharedPointer<AbstractTheme> Graphics::theme = nullptr;
QList<QString> *Graphics::themeList = nullptr;

QVector<std::function<QSharedPointer<AbstractTheme>()>> createTheme = {  
        [] { return QSharedPointer<AbstractTheme>(new Dark); },
        [] { return QSharedPointer<AbstractTheme>(new Light); },
};

QList<QString> *Graphics::initThemesList(){
    themeList = new QList<QString>;
    themeList->append(((QString)(typeid(Dark).name())).remove(0,1));
    themeList->append(((QString)(typeid(Light).name())).remove(0,1));
    return themeList;
}

QString Graphics::getGraphicsPath(){
    return theme->getGraphicsPath();
}

QString Graphics::getChannelColor(int channelIndex){
    if(channelIndex > TOP_CHANNEL_COLOR_INDEX)
        return theme->getChannelColor(TOP_CHANNEL_COLOR_INDEX);
    else
        return theme->getChannelColor(channelIndex);
}

QSharedPointer<AbstractTheme> Graphics::getThemeInstance(int themeIndex){
    theme = createTheme[themeIndex]();
    return theme;
}

QString Graphics::COLOR_WINDOW_APP;
QString Graphics::COLOR_WINDOW_WIDGET;
QString Graphics::COLOR_BACKGROUND_FOCUS_IN;
QString Graphics::COLOR_COMPONENT_DISABLED;
QString Graphics::COLOR_DATA_INPUT_AREA;
QString Graphics::COLOR_CONTROLS;
QString Graphics::COLOR_DISPLAY;
QString Graphics::COLOR_CHART;
QString Graphics::COLOR_CHART_GRIDLEG_DEFAULT;
QString Graphics::COLOR_CHART_GRIDLEG_LOW_CONTRAST;
QString Graphics::COLOR_TEXT_ALL;
QString Graphics::COLOR_TEXT_LABEL;
QString Graphics::COLOR_TEXT_COMPONENT;
QString Graphics::COLOR_WINDOW_CONTROL_HOVER;
QString Graphics::COLOR_WINDOW_EXIT_HOVER;
QString Graphics::COLOR_WARNING;
QString Graphics::COLOR_ERROR;
QString Graphics::COLOR_RUNNING;
QString Graphics::COLOR_UNUSED;

QString Graphics::STYLE_PUSH_BUTTON;
QString Graphics::STYLE_CHECK_BUTTON;
QString Graphics::STYLE_PROGRESS_BAR;
QString Graphics::STYLE_COMBO_BOX;

QString Graphics::STYLE_CONTROL_BUTTON;
QString Graphics::STYLE_MODULE_BUTTON;
QString Graphics::STYLE_HOLD_BUTTON;
QString Graphics::STYLE_DIAL;
bool    Graphics::STYLE_CUSTOM_DIALS_USED = false;
QString Graphics::STYLE_TEXTINPUT;
