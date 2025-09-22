#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QSharedPointer>
#include <QMetaType>
#include <QtGlobal>

#include "graphics/themes/dark.h"
#include "graphics/themes/light.h"
#include "graphics/themes/dawn.h"
#include "graphics/themes/aurora.h"
#include "graphics/themes/neomorph.h"
#include "graphics/themes/tealanalytics.h"
#include "graphics/themes/hudcyan.h"
#include "graphics/abstracttheme.h"

// Aggregated palette (snapshot) of all color/style values. Keeping QString for existing usage.
struct ThemePalette {
    QString windowApp;
    QString windowWidget;
    QString backgroundFocusIn;
    QString componentDisabled;
    QString backgroundButton;
    QString dataInputArea;
    QString controls;
    QString display;
    QString chart;
    QString chartGridlegDefault;
    QString chartGridlegLowContrast;
    QString textAll;
    QString textLabel;
    QString textComponent;
    QString windowControlHover;
    QString windowExitHover;
    QString warning;
    QString error;
    QString running;
    QString unused;

    QString stylePushButton;
    QString styleCheckButton;
    QString styleProgressBar;
    QString styleComboBox;
    QString styleControlButton;
    QString styleModuleButton;
    QString styleHoldButton;
    QString styleDial;
    QString styleTextInput;
    QString styleDockWidget;
    QString styleDockWindow;

    bool styleCustomDialsUsed = false;
    bool styleTransparencyUsed = false;
    QString styleGlobal;
};

namespace Graphics {
    QString getGraphicsPath();
    QString getBoardImage(const QString &boardBaseName);
    QString getBoardPinoutImage(const QString &boardBaseName);
    QString getChannelColor(int channelIndex);

    QList<QString> *initThemesList();
    QSharedPointer<AbstractTheme> setTheme(int themeIndex);
    QSharedPointer<AbstractTheme> getThemeInstance();

    // Legacy static members (global variables in namespace scope)
    extern QString COLOR_WINDOW_APP;
    extern QString COLOR_WINDOW_WIDGET;
    extern QString COLOR_BACKGROUND_FOCUS_IN;
    extern QString COLOR_COMPONENT_DISABLED;
    extern QString COLOR_BACKGROUND_BUTTON;
    extern QString COLOR_DATA_INPUT_AREA;
    extern QString COLOR_CONTROLS;
    extern QString COLOR_DISPLAY;
    extern QString COLOR_CHART;
    extern QString COLOR_CHART_GRIDLEG_DEFAULT;
    extern QString COLOR_CHART_GRIDLEG_LOW_CONTRAST;
    extern QString COLOR_TEXT_ALL;
    extern QString COLOR_TEXT_LABEL;
    extern QString COLOR_TEXT_COMPONENT;
    extern QString COLOR_WINDOW_CONTROL_HOVER;
    extern QString COLOR_WINDOW_EXIT_HOVER;
    extern QString COLOR_WARNING;
    extern QString COLOR_ERROR;
    extern QString COLOR_RUNNING;
    extern QString COLOR_UNUSED;

    extern QString STYLE_PUSH_BUTTON;
    extern QString STYLE_CHECK_BUTTON;
    extern QString STYLE_PROGRESS_BAR;
    extern QString STYLE_COMBO_BOX;
    extern QString STYLE_CONTROL_BUTTON;
    extern QString STYLE_MODULE_BUTTON;
    extern QString STYLE_HOLD_BUTTON;
    extern QString STYLE_DIAL;
    extern QString STYLE_TEXTINPUT;
    extern QString STYLE_DOCK_WIDGET;
    extern QString STYLE_DOCK_WINDOW;

    extern bool STYLE_CUSTOM_DIALS_USED;
    extern bool STYLE_TRANSPARENCY_USED;
    extern QString STYLE_GLOBAL;

    ThemePalette currentPalette();
    // Lightweight accessor to a cached snapshot; refreshes lazily when theme changes.
    const ThemePalette &palette();

    // Internal helper
    QString applyPathFallback(const QString &base, const QString &testFileName);
}

#define TOP_CHANNEL_COLOR_INDEX  7

#endif // GRAPHICS_H
