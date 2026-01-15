#include "graphics.h"
#include <functional>
#include <QFileInfo>

namespace Graphics {

static QSharedPointer<AbstractTheme> theme = nullptr;
static QList<QString> *themeList = nullptr;
static ThemePalette cachedPalette; // updated when setTheme called
static bool paletteInitialized = false;

QVector<std::function<QSharedPointer<AbstractTheme>()>> createTheme = {  
    [] { return QSharedPointer<AbstractTheme>(new Dark); },
    [] { return QSharedPointer<AbstractTheme>(new Light); },
    [] { return QSharedPointer<AbstractTheme>(new Dawn); },
    [] { return QSharedPointer<AbstractTheme>(new Aurora); },
    [] { return QSharedPointer<AbstractTheme>(new MSDos); },
};

QList<QString> *initThemesList(){
    if(!themeList){
        themeList = new QList<QString>;
        themeList->reserve(createTheme.size());
        themeList->append(((QString)(typeid(Dark).name())).remove(0,1));
        themeList->append(((QString)(typeid(Light).name())).remove(0,1));
        themeList->append(((QString)(typeid(Dawn).name())).remove(0,1));
        themeList->append(((QString)(typeid(Aurora).name())).remove(0,1));
        themeList->append(((QString)(typeid(MSDos).name())).remove(0,1));
    }
    return themeList;
}

QString getGraphicsPath(){
    if(!theme){
        qWarning("Graphics::getGraphicsPath called before theme initialization");
        Q_ASSERT(false && "Theme not initialized");
        return QString();
    }
    const QString base = theme->getGraphicsPath();
    return applyPathFallback(base, "logo_sniffy.png");
}

QString getBoardImage(const QString &boardBaseName){
    const QString basePath = getGraphicsPath();
    if(basePath.isEmpty()) return QString();
    if(boardBaseName.isEmpty())
        return basePath + "unknown_device.png";
    const QString candidate = basePath + boardBaseName + ".png";
    if(QFileInfo::exists(candidate)) return candidate;
    // applyPathFallback already contains generic fallback; re-run with board asset
    const QString fallbackPath = applyPathFallback(basePath, boardBaseName + ".png");
    const QString fallbackCandidate = fallbackPath + boardBaseName + ".png";
    if(QFileInfo::exists(fallbackCandidate)) return fallbackCandidate;
    return fallbackPath + "unknown_device.png";
}

QString getBoardPinoutImage(const QString &boardBaseName){
    if(boardBaseName.isEmpty()) return QString();
    const QString basePath = getGraphicsPath();
    if(basePath.isEmpty()) return QString();
    const QString pinout = basePath + boardBaseName + "-pinout.png";
    if(QFileInfo::exists(pinout)) return pinout;
    const QString fallbackPath = applyPathFallback(basePath, boardBaseName + "-pinout.png");
    const QString fallbackCandidate = fallbackPath + boardBaseName + "-pinout.png";
    if(QFileInfo::exists(fallbackCandidate)) return fallbackCandidate;
    return QString();
}

QString getChannelColor(int channelIndex){
    if(!theme){
        qWarning("Graphics::getChannelColor called before theme initialization");
        Q_ASSERT(false && "Theme not initialized");
        return QString();
    }
    if(channelIndex < 0) channelIndex = 0;
    if(channelIndex > TOP_CHANNEL_COLOR_INDEX)
        return theme->getChannelColor(TOP_CHANNEL_COLOR_INDEX);
    return theme->getChannelColor(channelIndex);
}

QSharedPointer<AbstractTheme> setTheme(int themeIndex){
    if(themeIndex < 0 || themeIndex >= createTheme.size()){
        return nullptr;
    }
    theme = createTheme[themeIndex]();
    if(theme){
        cachedPalette = theme->buildPalette();
        paletteInitialized = true;
    }
    return theme;
}

QSharedPointer<AbstractTheme> getThemeInstance(){
    return theme;
}

const ThemePalette &palette(){
    return cachedPalette;
}

QString applyPathFallback(const QString &base, const QString &testFileName){
    const QString full = base + testFileName;
    if(QFileInfo::exists(full)) return base; // no change

    struct PatternFallback { const char* pattern; const char* replacement; };
    static const PatternFallback orderedFallbacks[] = {
        {"/msdos/", "/dark/"}, // MSDOS theme currently reuses dark assets if not provided
        {"/aurora/", "/dawn/"}
    };
    for(const auto &pf : orderedFallbacks){
        if(base.contains(pf.pattern)){
            QString repl = QString(base).replace(pf.pattern, pf.replacement);
            if(QFileInfo::exists(repl + testFileName)){
                return repl;
            }
        }
    }
    return base; // last resort return original even if file missing
}

// Legacy globals removed after palette migration.

} // namespace Graphics
