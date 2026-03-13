#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QString>
#include <QGraphicsDropShadowEffect>
#include <QWidget>
#include "../graphics/graphics.h"

namespace StyleHelper {

static constexpr int GLOW_BLUR_RADIUS = 20;
static constexpr int GLOW_BLUR_RADIUS_BUTTONS = 45;
static constexpr int GLOW_BLUR_RADIUS_CONTROL_MODULE = 35;
static constexpr int GLOW_BLUR_RADIUS_DIAL = 25;
static constexpr int GLOW_BLUR_RADIUS_SWITCH = 35;

inline void applyGlowEffect(QWidget* widget, const QString& baseColorHex, int blurRadius = GLOW_BLUR_RADIUS) {
    if (!widget) return;
    if (Graphics::palette().isEmberTheme) {
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(widget);
        glow->setBlurRadius(blurRadius);
        glow->setOffset(0, 0);
        
        QColor c(baseColorHex);
        c.setAlpha(240);
        glow->setColor(c);
        
        widget->setGraphicsEffect(glow);
    } else {
        if (widget->graphicsEffect()) {
            widget->setGraphicsEffect(nullptr);
        }
    }
}

// Centralized helpers for commonly repeated style patterns.
inline QString pushButton(const QString &baseColorHex) {
    const auto &p = Graphics::palette();
    QString color = baseColorHex;
    QString c = p.styleTransparencyUsed ? QString(color).remove("#") : color;
    QString result = QString(p.stylePushButton).arg(c);
    QString tc = Graphics::channelTextColorForBg(baseColorHex);
    if (!tc.isEmpty())
        result += QStringLiteral("QPushButton{color:%1;}").arg(tc);
    return result;
}

inline QString checkButton(const QString &baseColorHex) {
    const auto &p = Graphics::palette();
    QString color = baseColorHex;
    QString c = p.styleTransparencyUsed ? QString(color).remove("#") : color;
    QString result = QString(p.styleCheckButton).arg(c);
    QString tc = Graphics::channelTextColorForBg(baseColorHex);
    if (!tc.isEmpty())
        result += QStringLiteral("QPushButton:checked{color:%1;}QPushButton:checked:hover{color:%1;}").arg(tc);
    return result;
}

inline QString dialWithTextColor(const QString &dialStyle, const QString &textColor){
    QString result = dialStyle + "QWidget{color:" + textColor + ";}";
    if (Graphics::palette().isEmberTheme)
        result += "QLineEdit::focus{border-bottom:2px solid " + textColor + ";}";
    return result;
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
    QString result = QString(p.styleCheckButton).arg(c);
    QString tc = Graphics::channelTextColorForBg(baseColor);
    if (!tc.isEmpty())
        result += QStringLiteral("QPushButton:checked{color:%1;}QPushButton:checked:hover{color:%1;}").arg(tc);
    return result;
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
