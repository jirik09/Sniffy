#include "graphics.h"
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
        [] { return QSharedPointer<AbstractTheme>(new Neomorph); },
        [] { return QSharedPointer<AbstractTheme>(new TealAnalytics); },
        [] { return QSharedPointer<AbstractTheme>(new HudCyan); },
};

QList<QString> *initThemesList(){
    if(!themeList){
        themeList = new QList<QString>;
        themeList->reserve(createTheme.size());
        themeList->append(((QString)(typeid(Dark).name())).remove(0,1));
        themeList->append(((QString)(typeid(Light).name())).remove(0,1));
        themeList->append(((QString)(typeid(Dawn).name())).remove(0,1));
        themeList->append(((QString)(typeid(Aurora).name())).remove(0,1));
        themeList->append(((QString)(typeid(Neomorph).name())).remove(0,1));
        themeList->append(((QString)(typeid(TealAnalytics).name())).remove(0,1));
        themeList->append(((QString)(typeid(HudCyan).name())).remove(0,1));
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
    cachedPalette = currentPalette();
    paletteInitialized = true;
    return theme;
}

QSharedPointer<AbstractTheme> getThemeInstance(){
    return theme;
}

ThemePalette currentPalette(){
    ThemePalette p;
    p.windowApp = COLOR_WINDOW_APP;
    p.windowWidget = COLOR_WINDOW_WIDGET;
    p.backgroundFocusIn = COLOR_BACKGROUND_FOCUS_IN;
    p.componentDisabled = COLOR_COMPONENT_DISABLED;
    p.backgroundButton = COLOR_BACKGROUND_BUTTON;
    p.dataInputArea = COLOR_DATA_INPUT_AREA;
    p.controls = COLOR_CONTROLS;
    p.display = COLOR_DISPLAY;
    p.chart = COLOR_CHART;
    p.chartGridlegDefault = COLOR_CHART_GRIDLEG_DEFAULT;
    p.chartGridlegLowContrast = COLOR_CHART_GRIDLEG_LOW_CONTRAST;
    p.textAll = COLOR_TEXT_ALL;
    p.textLabel = COLOR_TEXT_LABEL;
    p.textComponent = COLOR_TEXT_COMPONENT;
    p.windowControlHover = COLOR_WINDOW_CONTROL_HOVER;
    p.windowExitHover = COLOR_WINDOW_EXIT_HOVER;
    p.warning = COLOR_WARNING;
    p.error = COLOR_ERROR;
    p.running = COLOR_RUNNING;
    p.unused = COLOR_UNUSED;
    p.stylePushButton = STYLE_PUSH_BUTTON;
    p.styleCheckButton = STYLE_CHECK_BUTTON;
    p.styleProgressBar = STYLE_PROGRESS_BAR;
    p.styleComboBox = STYLE_COMBO_BOX;
    p.styleControlButton = STYLE_CONTROL_BUTTON;
    p.styleModuleButton = STYLE_MODULE_BUTTON;
    p.styleHoldButton = STYLE_HOLD_BUTTON;
    p.styleDial = STYLE_DIAL;
    p.styleTextInput = STYLE_TEXTINPUT;
    p.styleDockWidget = STYLE_DOCK_WIDGET;
    p.styleDockWindow = STYLE_DOCK_WINDOW;
    p.styleCustomDialsUsed = STYLE_CUSTOM_DIALS_USED;
    p.styleTransparencyUsed = STYLE_TRANSPARENCY_USED;
    p.styleGlobal = STYLE_GLOBAL;
    return p;
}

const ThemePalette &palette(){
    if(!paletteInitialized){
        cachedPalette = currentPalette();
        paletteInitialized = true;
    }
    return cachedPalette;
}

QString applyPathFallback(const QString &base, const QString &testFileName){
    const QString full = base + testFileName;
    if(QFileInfo::exists(full)) return base; // no change

    struct PatternFallback { const char* pattern; const char* replacement; };
    static const PatternFallback orderedFallbacks[] = {
        {"/aurora/", "/dawn/"},
        {"/neomorph/", "/light/"},
        {"/tealanalytics/", "/dark/"},
        {"/hudcyan/", "/dark/"}
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

QString COLOR_WINDOW_APP;
QString COLOR_WINDOW_WIDGET;
QString COLOR_BACKGROUND_FOCUS_IN;
QString COLOR_COMPONENT_DISABLED;
QString COLOR_BACKGROUND_BUTTON;
QString COLOR_DATA_INPUT_AREA;
QString COLOR_CONTROLS;
QString COLOR_DISPLAY;
QString COLOR_CHART;
QString COLOR_CHART_GRIDLEG_DEFAULT;
QString COLOR_CHART_GRIDLEG_LOW_CONTRAST;
QString COLOR_TEXT_ALL;
QString COLOR_TEXT_LABEL;
QString COLOR_TEXT_COMPONENT;
QString COLOR_WINDOW_CONTROL_HOVER;
QString COLOR_WINDOW_EXIT_HOVER;
QString COLOR_WARNING;
QString COLOR_ERROR;
QString COLOR_RUNNING;
QString COLOR_UNUSED;

QString STYLE_PUSH_BUTTON;
QString STYLE_CHECK_BUTTON;
QString STYLE_PROGRESS_BAR;
QString STYLE_COMBO_BOX;
QString STYLE_CONTROL_BUTTON;
QString STYLE_MODULE_BUTTON;
QString STYLE_HOLD_BUTTON;
QString STYLE_DIAL;
QString STYLE_TEXTINPUT;
QString STYLE_DOCK_WIDGET;
QString STYLE_DOCK_WINDOW;

bool STYLE_CUSTOM_DIALS_USED = false;
bool STYLE_TRANSPARENCY_USED = false;
QString STYLE_GLOBAL;

} // namespace Graphics
