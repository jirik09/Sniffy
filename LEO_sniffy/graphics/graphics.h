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
#include "graphics/themes/msdos.h"
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

    // Lightweight accessor to a cached snapshot; refreshes when setTheme called.
    const ThemePalette &palette();

    // Internal helper
    QString applyPathFallback(const QString &base, const QString &testFileName);
}

#define TOP_CHANNEL_COLOR_INDEX  7

#endif // GRAPHICS_H
