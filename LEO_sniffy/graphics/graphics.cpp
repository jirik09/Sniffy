#include "graphics.h"
#include <QFileInfo>

QSharedPointer<AbstractTheme> Graphics::theme = nullptr;
QList<QString> *Graphics::themeList = nullptr;

QVector<std::function<QSharedPointer<AbstractTheme>()>> createTheme = {  
        [] { return QSharedPointer<AbstractTheme>(new Dark); },
        [] { return QSharedPointer<AbstractTheme>(new Light); },
    [] { return QSharedPointer<AbstractTheme>(new Dawn); },
    [] { return QSharedPointer<AbstractTheme>(new Aurora); },
    [] { return QSharedPointer<AbstractTheme>(new Neomorph); },
    [] { return QSharedPointer<AbstractTheme>(new Sunset); },
    [] { return QSharedPointer<AbstractTheme>(new TealAnalytics); },
    [] { return QSharedPointer<AbstractTheme>(new HudCyan); },
};

QList<QString> *Graphics::initThemesList(){
    themeList = new QList<QString>;
    themeList->append(((QString)(typeid(Dark).name())).remove(0,1));
    themeList->append(((QString)(typeid(Light).name())).remove(0,1));
    themeList->append(((QString)(typeid(Dawn).name())).remove(0,1));
    themeList->append(((QString)(typeid(Aurora).name())).remove(0,1));
    themeList->append(((QString)(typeid(Neomorph).name())).remove(0,1));
    themeList->append(((QString)(typeid(Sunset).name())).remove(0,1));
    themeList->append(((QString)(typeid(TealAnalytics).name())).remove(0,1));
    themeList->append(((QString)(typeid(HudCyan).name())).remove(0,1));
    return themeList;
}

QString Graphics::getGraphicsPath(){
    const QString base = theme->getGraphicsPath();
    // If Aurora is active but assets are not yet present, fallback to Dawn's assets until provided
    if(base.contains("/aurora/")){
        const QString testAsset = base + "logo_sniffy.png";
        if(!QFileInfo::exists(testAsset)){
            return QString(base).replace("/aurora/","/dawn/");
        }
    }
    // If Neomorph is active but assets are not yet present, fallback to Light assets
    if(base.contains("/neomorph/")){
        const QString testAsset = base + "logo_sniffy.png";
        if(!QFileInfo::exists(testAsset)){
            return QString(base).replace("/neomorph/","/light/");
        }
    }
    // Sunset theme falls back to Dawn assets if not present
    if(base.contains("/sunset/")){
        const QString testAsset = base + "logo_sniffy.png";
        if(!QFileInfo::exists(testAsset)){
            return QString(base).replace("/sunset/","/dawn/");
        }
    }
    // TealAnalytics and HudCyan fall back to Dark assets if not present
    if(base.contains("/tealanalytics/") || base.contains("/hudcyan/")){
        const QString testAsset = base + "logo_sniffy.png";
        if(!QFileInfo::exists(testAsset)){
            QString b = base;
            b.replace("/tealanalytics/","/dark/");
            b.replace("/hudcyan/","/dark/");
            return b;
        }
    }
    return base;
}

QString Graphics::getBoardImage(const QString &boardBaseName){
    const QString basePath = getGraphicsPath();
    if(boardBaseName.isEmpty())
        return basePath + "unknown_device.png";

    const QString candidate = basePath + boardBaseName + ".png";
    if(QFileInfo::exists(candidate))
        return candidate;
    // Fallback for themes missing images (e.g., during development)
    if(basePath.contains("/aurora/")){
        const QString dawnBase = QString(basePath).replace("/aurora/","/dawn/");
        const QString dawnCandidate = dawnBase + boardBaseName + ".png";
        if(QFileInfo::exists(dawnCandidate)) return dawnCandidate;
        return dawnBase + "unknown_device.png";
    }
    if(basePath.contains("/neomorph/")){
        const QString lightBase = QString(basePath).replace("/neomorph/","/light/");
        const QString lightCandidate = lightBase + boardBaseName + ".png";
        if(QFileInfo::exists(lightCandidate)) return lightCandidate;
        return lightBase + "unknown_device.png";
    }
    if(basePath.contains("/sunset/")){
        const QString dawnBase = QString(basePath).replace("/sunset/","/dawn/");
        const QString dawnCandidate = dawnBase + boardBaseName + ".png";
        if(QFileInfo::exists(dawnCandidate)) return dawnCandidate;
        return dawnBase + "unknown_device.png";
    }
    if(basePath.contains("/tealanalytics/")){
        const QString darkBase = QString(basePath).replace("/tealanalytics/","/dark/");
        const QString darkCandidate = darkBase + boardBaseName + ".png";
        if(QFileInfo::exists(darkCandidate)) return darkCandidate;
        return darkBase + "unknown_device.png";
    }
    if(basePath.contains("/hudcyan/")){
        const QString darkBase = QString(basePath).replace("/hudcyan/","/dark/");
        const QString darkCandidate = darkBase + boardBaseName + ".png";
        if(QFileInfo::exists(darkCandidate)) return darkCandidate;
        return darkBase + "unknown_device.png";
    }

    return basePath + "unknown_device.png";
}

QString Graphics::getBoardPinoutImage(const QString &boardBaseName){
    if(boardBaseName.isEmpty())
        return QString();

    const QString basePath = getGraphicsPath();
    const QString pinout = basePath + boardBaseName + "-pinout.png";
    if(QFileInfo::exists(pinout))
        return pinout;
    // Fallback for missing pinout images in new themes
    if(basePath.contains("/aurora/")){
        const QString dawnBase = QString(basePath).replace("/aurora/","/dawn/");
        const QString dawnPinout = dawnBase + boardBaseName + "-pinout.png";
        if(QFileInfo::exists(dawnPinout)) return dawnPinout;
    }
    if(basePath.contains("/neomorph/")){
        const QString lightBase = QString(basePath).replace("/neomorph/","/light/");
        const QString lightPinout = lightBase + boardBaseName + "-pinout.png";
        if(QFileInfo::exists(lightPinout)) return lightPinout;
    }
    if(basePath.contains("/sunset/")){
        const QString dawnBase = QString(basePath).replace("/sunset/","/dawn/");
        const QString dawnPinout = dawnBase + boardBaseName + "-pinout.png";
        if(QFileInfo::exists(dawnPinout)) return dawnPinout;
    }
    if(basePath.contains("/tealanalytics/")){
        const QString darkBase = QString(basePath).replace("/tealanalytics/","/dark/");
        const QString darkPinout = darkBase + boardBaseName + "-pinout.png";
        if(QFileInfo::exists(darkPinout)) return darkPinout;
    }
    if(basePath.contains("/hudcyan/")){
        const QString darkBase = QString(basePath).replace("/hudcyan/","/dark/");
        const QString darkPinout = darkBase + boardBaseName + "-pinout.png";
        if(QFileInfo::exists(darkPinout)) return darkPinout;
    }
        
    return QString();
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
QString Graphics::COLOR_BACKGROUND_BUTTON;
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
QString Graphics::STYLE_TEXTINPUT;
QString Graphics::STYLE_DOCK_WIDGET;
QString Graphics::STYLE_DOCK_WINDOW;

bool    Graphics::STYLE_CUSTOM_DIALS_USED = false;
bool    Graphics::STYLE_TRANSPARENCY_USED = false;

QString Graphics::STYLE_GLOBAL;
