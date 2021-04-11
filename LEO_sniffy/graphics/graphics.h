#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QSharedPointer>
#include <QMetaType>

#include "graphics/themes/dark.h"
#include "graphics/themes/light.h"
#include "graphics/abstracttheme.h"

class Graphics : public QObject
{
    Q_OBJECT
public:
    explicit Graphics(QObject *parent = nullptr, int themeIndex = 0);

    static QString getAppGlobalStyle();
    static QString getGraphicsPath();
    static QString getChannelColor(int channelIndex);

    static QList<QString> *initThemesList();
    static QSharedPointer<AbstractTheme> getThemeInstance();

    static QString COLOR_WINDOW_APP;
    static QString COLOR_WINDOW_WIDGET;
    static QString COLOR_BACKGROUND_BUTTON;
    static QString COLOR_BACKGROUND_BUTTON_DISABLED;
    static QString COLOR_BACKGROUND_FOCUS_IN;
    static QString COLOR_COMPONENT_DISABLED;
    static QString COLOR_DATA_AREA;
    static QString COLOR_CONTROLS;
    static QString COLOR_DISPLAY;
    static QString COLOR_CHART;
    static QString COLOR_CHART_GRIDLEG_DEFAULT;
    static QString COLOR_CHART_GRIDLEG_LOW_CONTRAST;
    static QString COLOR_TEXT_ALL;
    static QString COLOR_TEXT_LABEL;
    static QString COLOR_TEXT_COMPONENT;
    static QString COLOR_HOVER;
    static QString COLOR_HOVER_EXIT;
    static QString COLOR_WARNING;
    static QString COLOR_ERROR;
    static QString COLOR_UNINITIALIZED;

private:
    static QSharedPointer<AbstractTheme> theme;
    static QList<QString> *themeList;
};

#define TOP_CHANNEL_COLOR_INDEX  7

#endif // GRAPHICS_H
