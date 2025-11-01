
#ifndef PATTERNCHART_H
#define PATTERNCHART_H

#include "../../GUI/widgetchart.h"
#include <QGraphicsSimpleTextItem>
#include <QtCharts/QChart>
#include <QFontMetrics>

// Localized chart with channel name labels used only by Pattern Generator
class PatternChart : public widgetChart
{
public:
    explicit PatternChart(QWidget *parent = nullptr, int maxTraces = 1);
    void setChannelNames(const QStringList &names);
    void clearChannelNames();

protected:
    void resizeEvent(QResizeEvent *ev) override;

private:
    void onPlotAreaChanged(const QRectF &);
    void layoutNames();
    void updateRightMargin();
    void clearItems();

    static constexpr int kFontPt = 8;
    static constexpr int kZ = 10;
    static constexpr qreal kVerticalFactor = 0.5; // keeps original visual placement

    int m_maxTraces = 1;
    QStringList m_names;
    QVector<QGraphicsSimpleTextItem *> m_items;
};

// Implementation
inline PatternChart::PatternChart(QWidget *parent, int maxTraces)
    : widgetChart(parent, maxTraces), m_maxTraces(maxTraces)
{
    if (auto *c = getChart())
    {
        connect(c, &QChart::plotAreaChanged, this, &PatternChart::onPlotAreaChanged);
        // Slight base margins to keep visuals consistent with original pattern generator look
        c->setMargins(QMargins(0, 0, 0, 0));
    }
}

inline void PatternChart::setChannelNames(const QStringList &names)
{
    m_names = names;
    clearItems();
    // Create new items
    m_items.reserve(m_maxTraces);
    for (int i = 0; i < m_maxTraces; ++i)
    {
        auto *item = new QGraphicsSimpleTextItem();
        item->setBrush(QBrush(QColor(Graphics::palette().chartGridlegLowContrast)));
        QFont f = item->font();
        f.setPointSize(kFontPt);
        item->setFont(f);
        item->setZValue(kZ);
        if (auto *c = getChart(); c && c->scene())
            c->scene()->addItem(item);
        m_items.push_back(item);
    }
    updateRightMargin();
    layoutNames();
}

inline void PatternChart::clearChannelNames()
{
    clearItems();
    m_names.clear();
    updateRightMargin();
}

inline void PatternChart::resizeEvent(QResizeEvent *ev)
{
    widgetChart::resizeEvent(ev);
    updateRightMargin();
    layoutNames();
}

inline void PatternChart::onPlotAreaChanged(const QRectF &)
{
    layoutNames();
}

inline void PatternChart::layoutNames()
{
    auto *c = getChart();
    if (!c)
        return;
    const QRectF plot = c->plotArea();
    if (plot.isEmpty())
        return;
    const qreal bandH = plot.height() / qMax(1, m_maxTraces);
    for (int ch = 0; ch < m_maxTraces && ch < m_items.size(); ++ch)
    {
        auto *item = m_items[ch];
        if (!item)
            continue;
        const QString label = (ch < m_names.size() && !m_names[ch].isEmpty()) ? m_names[ch] : QString("CH%1").arg(ch + 1);
        item->setText(label);
        const qreal yLocal = plot.top() + bandH * (ch + kVerticalFactor);
        QRectF b = item->boundingRect();
        const int marginRight = c->margins().right();
        const qreal minGap = 4.0;
        const qreal centeredOffset = (marginRight - b.width()) * 0.5;
        const qreal offset = qMax<qreal>(minGap, centeredOffset);
        const QPointF scenePt = c->mapToScene(QPointF(plot.right() + offset, yLocal));
        item->setPos(scenePt.x(), scenePt.y() - b.height() * 0.5);
    }
}

inline void PatternChart::updateRightMargin()
{
    auto *c = getChart();
    if (!c)
        return;
    // Determine the right margin based on the widest label so names remain visible
    constexpr int kFixedRightMargin = 0; // minimum
    int maxLabelWidth = 0;
    QFont f; f.setPointSize(kFontPt);
    QFontMetrics fm(f);
    for (const auto &name : m_names)
    {
        if (name.isEmpty()) continue;
        maxLabelWidth = qMax(maxLabelWidth, fm.horizontalAdvance(name));
    }
    // Add small padding so text doesn't butt against the edge
    const int padding = 0;
    const int needed = maxLabelWidth + padding;
    QMargins m = c->margins();
    const int finalRight = qMax(kFixedRightMargin, needed);
    if (m.right() != finalRight)
        m.setRight(finalRight);
    c->setMargins(m);
}

inline void PatternChart::clearItems()
{
    auto *c = getChart();
    auto *scene = c ? c->scene() : nullptr;
    for (auto *it : std::as_const(m_items))
    {
        if (it && scene)
            scene->removeItem(it);
        delete it;
    }
    m_items.clear();
}

#endif // PATTERNCHART_H
