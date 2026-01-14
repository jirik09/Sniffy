#include "toastwidget.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QStyleOption>
#include <QPainter>
#include "../graphics/graphics.h"

ToastWidget::ToastWidget(QWidget *parent) : QWidget(parent)
{
    // Qt::ToolTip makes it a top-level window but transient
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_StyledBackground);
    const auto &pal = Graphics::palette();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(15, 10, 10, 10);
    layout->setSpacing(10);

    label = new QLabel(this);
    label->setStyleSheet(QString("color: %1; background-color: %2 C8; border: none;").arg(pal.textAll,pal.backgroundButton));
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    effect->setOpacity(0.8); // 0.0 to 1.0
    label->setGraphicsEffect(effect);
    layout->addWidget(label);

    QToolButton *closeBtn = new QToolButton(this);
    closeBtn->setText("✕");
    closeBtn->setCursor(Qt::PointingHandCursor);

    QString btnStyle = QString(
        "QToolButton { border: none; background: transparent; color: %1; }"
        "QToolButton:hover { color: %2; }"
    ).arg(pal.textLabel, pal.textAll);
    closeBtn->setStyleSheet(btnStyle);

    connect(closeBtn, &QToolButton::clicked, this, &ToastWidget::close);
    layout->addWidget(closeBtn);

    // Style: Theme background, rounded corners.
    QColor bg(pal.backgroundButton);
    bg.setAlpha(200);
    setStyleSheet(QString(
        "ToastWidget { background-color: %1; border: 1px solid %2; border-radius: 4px; }"
    ).arg(bg.name(QColor::HexArgb), pal.textLabel));

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &ToastWidget::fadeOut);
}

void ToastWidget::showMessage(const QString &text, int durationMs)
{
    label->setText(text);
    adjustSize();
    show();
    if (durationMs > 0)
        timer->start(durationMs);
}

void ToastWidget::fadeOut()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(500);
    a->setStartValue(1);
    a->setEndValue(0);
    // Use deleteWhenStopped to cleanup animation, and slot to close widget
    connect(a, &QPropertyAnimation::finished, this, &ToastWidget::onAnimationFinished);
    a->start(QAbstractAnimation::DeleteWhenStopped);
}

void ToastWidget::onAnimationFinished()
{
    close();
}

void ToastWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
