#ifndef ABSTRACTTHEME_H
#define ABSTRACTTHEME_H

#include <QObject>
// Forward declaration to avoid circular dependency with graphics.h
struct ThemePalette;

class AbstractTheme : public QObject
{
public:
    explicit AbstractTheme(QObject *parent = nullptr);

    virtual QString getAppGlobalStyle() = 0;
    virtual QString getGraphicsPath() = 0;
    virtual QString getChannelColor(int channelIndex) = 0;
    // New: provide palette snapshot without mutating globals
    virtual ThemePalette buildPalette() = 0;
};

#endif // ABSTRACTTHEME_H
