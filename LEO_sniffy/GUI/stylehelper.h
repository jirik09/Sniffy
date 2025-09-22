#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QString>
#include "../graphics/graphics.h"

namespace StyleHelper {

// Centralized helpers for commonly repeated style patterns.
inline QString pushButton(const QString &baseColorHex) {
    QString color = baseColorHex;
    QString c = Graphics::STYLE_TRANSPARENCY_USED ? QString(color).remove("#") : color;
    return QString(Graphics::STYLE_PUSH_BUTTON).arg(c);
}

inline QString checkButton(const QString &baseColorHex) {
    QString color = baseColorHex;
    QString c = Graphics::STYLE_TRANSPARENCY_USED ? QString(color).remove("#") : color;
    return QString(Graphics::STYLE_CHECK_BUTTON).arg(c);
}

inline QString dialWithTextColor(const QString &dialStyle, const QString &textColor){
    return dialStyle + "QWidget{color:" + textColor + ";}";
}

inline QString separatorLabel(const QString &textColor){
    return QStringLiteral("QLabel{color:%1;}").arg(textColor);
}

inline QString textInputLabel(){
    return Graphics::STYLE_TEXTINPUT; // already full rule
}

inline QString textInputField(){
    return Graphics::STYLE_TEXTINPUT +
           "QWidget{background-color:" + Graphics::COLOR_DATA_INPUT_AREA + "} "
           "QWidget::focus{background-color:" + Graphics::COLOR_BACKGROUND_FOCUS_IN + ";}";
}

inline QString switchSelected(const QString &baseColor){
    QString c = Graphics::STYLE_TRANSPARENCY_USED ? QString(baseColor).remove('#') : baseColor;
    return QString(Graphics::STYLE_CHECK_BUTTON).arg(c);
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
