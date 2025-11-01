#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QString>
#include "../graphics/graphics.h"

namespace StyleHelper {

// Centralized helpers for commonly repeated style patterns.
inline QString pushButton(const QString &baseColorHex) {
    const auto &p = Graphics::palette();
    QString color = baseColorHex;
    QString c = p.styleTransparencyUsed ? QString(color).remove("#") : color;
    return QString(p.stylePushButton).arg(c);
}

inline QString checkButton(const QString &baseColorHex) {
    const auto &p = Graphics::palette();
    QString color = baseColorHex;
    QString c = p.styleTransparencyUsed ? QString(color).remove("#") : color;
    return QString(p.styleCheckButton).arg(c);
}

inline QString dialWithTextColor(const QString &dialStyle, const QString &textColor){
    return dialStyle + "QWidget{color:" + textColor + ";}";
}

inline QString separatorLabel(const QString &textColor){
    return QStringLiteral("QLabel{color:%1;}").arg(textColor);
}

inline QString textInputLabel(){
    return Graphics::palette().styleTextInput; // already full rule
}

inline QString textInputField(){
    const auto &p = Graphics::palette();
    return p.styleTextInput +
           "QWidget{background-color:" + p.dataInputArea + "} "
           "QWidget::focus{background-color:" + p.backgroundFocusIn + ";}";
}

inline QString switchSelected(const QString &baseColor){
    const auto &p = Graphics::palette();
    QString c = p.styleTransparencyUsed ? QString(baseColor).remove('#') : baseColor;
    return QString(p.styleCheckButton).arg(c);
}

inline QString switchNotSelected(const QString &selectedStyle){
    // Currently identical; kept for future differentiation
    return selectedStyle;
}

inline QString backgroundImage(const QString &path){
    return QStringLiteral("background-image: url(%1);").arg(path);
}

}

#endif // STYLEHELPER_H
