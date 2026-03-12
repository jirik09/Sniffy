#include "graphics.h"
#include <functional>
#include <QFileInfo>
#include <QPainter>
#include <QDir>
#include <QStandardPaths>
#include <QHash>

namespace Graphics {

static QSharedPointer<AbstractTheme> theme = nullptr;
static QList<QString> *themeList = nullptr;
static ThemePalette cachedPalette; // updated when setTheme called
static bool paletteInitialized = false;

QVector<std::function<QSharedPointer<AbstractTheme>()>> createTheme = {  
    [] { return QSharedPointer<AbstractTheme>(new Dark); },
    [] { return QSharedPointer<AbstractTheme>(new Light); },
    [] { return QSharedPointer<AbstractTheme>(new Dawn); },
    [] { return QSharedPointer<AbstractTheme>(new Greenfield); },
    [] { return QSharedPointer<AbstractTheme>(new MSDos); },
};

QList<QString> *initThemesList(){
    if(!themeList){
        themeList = new QList<QString>;
        themeList->reserve(createTheme.size());
        themeList->append(((QString)(typeid(Dark).name())).remove(0,1));
        themeList->append(((QString)(typeid(Light).name())).remove(0,1));
        themeList->append(((QString)(typeid(Dawn).name())).remove(0,1));
        themeList->append(((QString)(typeid(Greenfield).name())).remove(0,1));
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

QString getCommonPath(){
    return QStringLiteral(":/graphics/graphics/common/");
}

QString getBoardImage(const QString &boardBaseName){
    const QString basePath = getGraphicsPath();
    if(basePath.isEmpty()) return QString();
    if(boardBaseName.isEmpty())
        return getCommonPath() + "unknown_device.png";
    const QString candidate = basePath + boardBaseName + ".png";
    if(QFileInfo::exists(candidate)) return candidate;
    // applyPathFallback already contains generic fallback; re-run with board asset
    const QString fallbackPath = applyPathFallback(basePath, boardBaseName + ".png");
    const QString fallbackCandidate = fallbackPath + boardBaseName + ".png";
    if(QFileInfo::exists(fallbackCandidate)) return fallbackCandidate;
    return getCommonPath() + "unknown_device.png";
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

QString channelTextColorForBg(const QString &bgColor){
    if(cachedPalette.channelTextColors.isEmpty()) return QString();
    QString norm = bgColor.startsWith('#') ? bgColor : ("#" + bgColor);
    for(int ch = 0; ch < cachedPalette.channelTextColors.size(); ++ch){
        if(!cachedPalette.channelTextColors.at(ch).isEmpty() &&
           norm.compare(getChannelColor(ch), Qt::CaseInsensitive) == 0){
            return cachedPalette.channelTextColors.at(ch);
        }
    }
    return QString();
}

QSharedPointer<AbstractTheme> setTheme(int themeIndex){
    if(themeIndex < 0 || themeIndex >= createTheme.size()){
        return nullptr;
    }
    theme = createTheme[themeIndex]();
    if(theme){
        cachedPalette = theme->buildPalette();
        paletteInitialized = true;
        clearTintCache();
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
        {"/greenfield/", "/dawn/"}
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

// ── Tinting utilities ──────────────────────────────────────────
static QHash<QString, QString> tintFileCache;
static QString tintCacheDir;

static QString ensureTintCacheDir(){
    if(tintCacheDir.isEmpty()){
        tintCacheDir = QDir::tempPath() + "/sniffy_tinted";
        QDir().mkpath(tintCacheDir);
    }
    return tintCacheDir;
}

static QPixmap tintPixmap(const QPixmap &base, const QColor &color){
    QPixmap tinted(base.size());
    tinted.fill(Qt::transparent);
    QPainter p(&tinted);
    p.drawPixmap(0, 0, base);
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.fillRect(tinted.rect(), color);
    p.end();
    return tinted;
}

QPixmap tintedPixmap(const QString &path){
    QPixmap base(path);
    if(base.isNull()) return base;
    return tintPixmap(base, QColor(cachedPalette.textAll));
}

QPixmap tintedPixmap(const QString &path, const QColor &color){
    QPixmap base(path);
    if(base.isNull()) return base;
    return tintPixmap(base, color);
}

QIcon tintedIcon(const QString &path){
    QPixmap base(path);
    if(base.isNull()) return QIcon();
    QIcon icon;
    icon.addPixmap(tintPixmap(base, QColor(cachedPalette.textAll)), QIcon::Normal);
    icon.addPixmap(tintPixmap(base, QColor(cachedPalette.componentDisabled)), QIcon::Disabled);
    return icon;
}

QString tintedPath(const QString &path){
    auto it = tintFileCache.find(path);
    if(it != tintFileCache.end()) return *it;

    QPixmap pm = tintedPixmap(path);
    if(pm.isNull()) return path; // fallback to original

    const QString dir = ensureTintCacheDir();
    const QString outFile = dir + "/" + QFileInfo(path).fileName();
    pm.save(outFile, "PNG");
    tintFileCache.insert(path, outFile);
    return outFile;
}

QString tintedPath(const QString &path, const QColor &color){
    const QString key = path + "#" + color.name();
    auto it = tintFileCache.find(key);
    if(it != tintFileCache.end()) return *it;

    QPixmap pm = tintedPixmap(path, color);
    if(pm.isNull()) return path;

    const QString dir = ensureTintCacheDir();
    const QString outFile = dir + "/" + color.name().mid(1) + "_" + QFileInfo(path).fileName();
    pm.save(outFile, "PNG");
    tintFileCache.insert(key, outFile);
    return outFile;
}

void clearTintCache(){
    tintFileCache.clear();
}

// Legacy globals removed after palette migration.

} // namespace Graphics
