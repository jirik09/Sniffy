#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QSharedPointer>
#include <QMetaType>

#include "graphics/themes/dark.h"
#include "graphics/themes/light.h"
#include "graphics/themes/dawn.h"
#include "graphics/abstracttheme.h"

class Graphics : public QObject
{
    Q_OBJECT

public:
    static QString getGraphicsPath();
    static QString getChannelColor(int channelIndex);

    static QList<QString> *initThemesList();
    static QSharedPointer<AbstractTheme> getThemeInstance(int themeIndex);

    static QString COLOR_WINDOW_APP;
    static QString COLOR_WINDOW_WIDGET;
    static QString COLOR_BACKGROUND_FOCUS_IN;
    static QString COLOR_COMPONENT_DISABLED;
    static QString COLOR_DATA_INPUT_AREA;
    static QString COLOR_CONTROLS;
    static QString COLOR_DISPLAY;
    static QString COLOR_CHART;
    static QString COLOR_CHART_GRIDLEG_DEFAULT;
    static QString COLOR_CHART_GRIDLEG_LOW_CONTRAST;
    static QString COLOR_TEXT_ALL;
    static QString COLOR_TEXT_LABEL;
    static QString COLOR_TEXT_COMPONENT;
    static QString COLOR_WINDOW_CONTROL_HOVER;
    static QString COLOR_WINDOW_EXIT_HOVER;
    static QString COLOR_WARNING;
    static QString COLOR_ERROR;
    static QString COLOR_RUNNING;
    static QString COLOR_UNUSED;

    static QString STYLE_PUSH_BUTTON;
    static QString STYLE_CHECK_BUTTON;
    static QString STYLE_PROGRESS_BAR;
    static QString STYLE_COMBO_BOX;

    static QString STYLE_CONTROL_BUTTON;
    static QString STYLE_MODULE_BUTTON;
    static QString STYLE_HOLD_BUTTON;
    static QString STYLE_DIAL;
    static bool STYLE_CUSTOM_DIALS_USED;
    static QString STYLE_TEXTINPUT;
    static QString STYLE_DOCK_WIDGET;
    static QString STYLE_DOCK_WINDOW;

private:
    static QSharedPointer<AbstractTheme> theme;
    static QList<QString> *themeList;
};

#define TOP_CHANNEL_COLOR_INDEX  7

#endif // GRAPHICS_H
