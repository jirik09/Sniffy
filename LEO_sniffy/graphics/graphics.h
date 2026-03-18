#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QSharedPointer>
#include <QMetaType>
#include <QtGlobal>
#include <QPixmap>
#include <QIcon>

#include "graphics/themes/dark.h"
#include "graphics/themes/light.h"
#include "graphics/themes/dawn.h"
#include "graphics/themes/greenfield.h"
#include "graphics/themes/msdos.h"
#include "graphics/themes/ember.h"
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
    QString moduleActiveText; // text color for checked/active module button
    QString moduleHoverText;  // text color for hovered module button (empty = no change)
    QString moduleDisabledTint; // icon tint for disabled/locked modules (empty = use componentDisabled)
    QStringList channelTextColors; // per-channel text color overrides (empty entry = use default)

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
    bool isEmberTheme = false;
    QString styleGlobal;
};

namespace Graphics {
    QString getGraphicsPath();
    QString getCommonPath();
    QString getBoardImage(const QString &boardBaseName);
    QString getBoardPinoutImage(const QString &boardBaseName);
    QString getChannelColor(int channelIndex);
    QString channelTextColorForBg(const QString &bgColor);

    QList<QString> *initThemesList();
    QSharedPointer<AbstractTheme> setTheme(int themeIndex);
    QSharedPointer<AbstractTheme> getThemeInstance();

    // Lightweight accessor to a cached snapshot; refreshes when setTheme called.
    const ThemePalette &palette();

    // Tint a common icon to the current theme's textAll color.
    QPixmap tintedPixmap(const QString &path);
    QPixmap tintedPixmap(const QString &path, const QColor &color);
    QIcon   tintedIcon(const QString &path);
    // Return a file-system path to a tinted copy (for CSS image:url() usage).
    QString tintedPath(const QString &path);
    QString tintedPath(const QString &path, const QColor &color);
    // Clear the tinted-file cache (called automatically on theme change).
    void    clearTintCache();

    // Internal helper
    QString applyPathFallback(const QString &base, const QString &testFileName);
}

#define TOP_CHANNEL_COLOR_INDEX  7

#endif // GRAPHICS_H
