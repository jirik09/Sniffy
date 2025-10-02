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
    explicit PatternChart(QWidget *parent = nullptr, int maxTraces = 1)
        : widgetChart(parent, maxTraces), m_maxTraces(maxTraces)
    {
        if (auto *c = getChart())
        {
            connect(c, &QChart::plotAreaChanged, this, &PatternChart::onPlotAreaChanged);
            // Slight base margins to keep visuals consistent with original pattern generator look
            c->setMargins(QMargins(0, -8, 0, 0));
        }
    }

    void setChannelNames(const QStringList &names)
    {
        m_names = names;
        // destroy old
        for (auto *it : m_items)
        {
            if (it && getChart() && getChart()->scene())
                getChart()->scene()->removeItem(it);
            delete it;
        }
        m_items.clear();
        // create new items
        for (int i = 0; i < m_maxTraces; ++i)
        {
            auto *item = new QGraphicsSimpleTextItem();
            item->setBrush(QBrush(QColor(Graphics::palette().chartGridlegLowContrast)));
            QFont f = item->font();
            f.setPointSize(8);
            item->setFont(f);
            item->setZValue(10);
            if (auto *c = getChart(); c && c->scene())
                c->scene()->addItem(item);
            m_items.push_back(item);
        }
        updateRightMargin();
        layoutNames();
    }

    void clearChannelNames()
    {
        for (auto *it : m_items)
        {
            if (it && getChart() && getChart()->scene())
                getChart()->scene()->removeItem(it);
            delete it;
        }
        m_items.clear();
        m_names.clear();
        updateRightMargin();
    }

protected:
    void resizeEvent(QResizeEvent *ev) override
    {
        widgetChart::resizeEvent(ev);
        updateRightMargin();
        layoutNames();
    }

private:
    void onPlotAreaChanged(const QRectF &) { layoutNames(); }

private:
    void layoutNames()
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
            const qreal yLocal = plot.top() + bandH * (ch + 0.6);
            QRectF b = item->boundingRect();
            // Center the label horizontally between the rightmost grid line (plot.right)
            // and the right edge of the chart (plot.right + right margin).
            const int marginRight = c->margins().right();
            const qreal leftGap = qMax<qreal>(0.0, (marginRight - b.width()) * 0.5);
            const QPointF scenePt = c->mapToScene(QPointF(plot.right() + leftGap, yLocal));
            item->setPos(scenePt.x(), scenePt.y() - b.height() * 0.5);
        }
    }

    void updateRightMargin()
    {
        auto *c = getChart();
        if (!c)
            return;
        QFontMetrics fm(QFont("", 8));
        int maxW = 0;
        for (int ch = 0; ch < m_maxTraces; ++ch)
        {
            const QString label = (ch < m_names.size() && !m_names[ch].isEmpty()) ? m_names[ch] : QString("CH%1").arg(ch + 1);
            maxW = qMax(maxW, fm.horizontalAdvance(label));
        }
        const int right = -3; // no extra padding, eliminate space to the right of the text
        QMargins m = c->margins();
        if (m.right() != right)
        {
            m.setRight(right);
        }
        c->setMargins(m);
    }

    int m_maxTraces;
    QStringList m_names;
    QVector<QGraphicsSimpleTextItem *> m_items;
};

#endif // PATTERNCHART_H
