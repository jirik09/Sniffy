#ifndef SIMPLEVPROGRESSBAR_H
#define SIMPLEVPROGRESSBAR_H

#include <QWidget>
#include <QColor>
#include <QPaintEvent>
#include <QPainter>

class SimpleVProgressBar : public QWidget {

public:
    explicit SimpleVProgressBar(QWidget *parent=nullptr)
        : QWidget(parent) {
        setMinimumWidth(20); // a bit wider so both frame lines have room
        setMaximumWidth(28);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_showPercent = false; // default: no number displayed
    }

    void setRange(int minimum, int maximum){
        if(maximum <= minimum) maximum = minimum+1;
        m_min = minimum; m_max = maximum;
        if(m_value < m_min) m_value = m_min;
        if(m_value > m_max) m_value = m_max;
        update();
    }

    void setValue(int v){
        int cl = qBound(m_min, v, m_max);
        if(cl != m_value){
            m_value = cl; update();
        }
    }

    // Set only the fill color. Frame color is managed externally (e.g. to match lcdNumber text color).
    void setColor(const QColor &c){ m_color = c; update(); }
    void setBackgroundColor(const QColor &c){ m_background = c; update(); }
    void setFrameColor(const QColor &c){ m_frame = c; update(); }
    void setShowPercent(bool on){ m_showPercent = on; update(); }

    int value() const { return m_value; }
    int minimum() const { return m_min; }
    int maximum() const { return m_max; }

protected:
    void paintEvent(QPaintEvent *e) override {
        Q_UNUSED(e);
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, false);
        // Use full rect minus 1px to avoid painting outside bounds; keeps consistent frame
        // Full widget rect
        QRect outer = rect();
        // Frame rectangle (4px inset on right & bottom gives consistent lines)
        QRect frameRect = outer.adjusted(+9,+4,0,-7);
        // background first
        p.fillRect(frameRect, m_background);
        // progress fill (bottom-up)
        const double span = double(m_max - m_min);
        double ratio = span>0 ? double(m_value - m_min)/span : 0.0;
        if(ratio < 0) ratio = 0; else if(ratio > 1) ratio = 1;
        const int fullHeight = frameRect.height();
        int fillH = int(fullHeight * ratio + 0.5);
        if(fillH > 0){
            // Interior area strictly inside frame: x in [1 .. frameRect.width()-2]
            // To avoid a perceived left gap, start fill exactly at frameRect.left()+1.
            int left = frameRect.left();
            int right = frameRect.right()-1; // keep one pixel for frame on right
            if(right <= left) right = left; // safety
            int innerWidth = right - left + 2;
            int bottomY = frameRect.bottom();
            int topY = bottomY - fillH + 1;
            if(topY < frameRect.top()+1) topY = frameRect.top(); // do not overdraw top frame line
            QRect fillRect(left, topY, innerWidth, bottomY - topY + 1);
            p.fillRect(fillRect, m_color);
        }
        // frame last
        if(m_frame.alpha() > 0){
            p.setPen(QPen(m_frame,1));
            p.drawRect(frameRect);
        }
        if(m_showPercent){
            p.setPen(m_text);
            QString txt = QString::number(int(ratio*100+0.5));
            p.drawText(frameRect, Qt::AlignCenter, txt);
        }
    }

private:
    int m_min = 0;
    int m_max = 100;
    int m_value = 0;
    QColor m_color = QColor(0,255,0);
    QColor m_background = QColor(34,34,34);
    QColor m_frame = QColor(60,60,60);
    QColor m_text = Qt::white;
    bool m_showPercent = true;
};

#endif // SIMPLEVPROGRESSBAR_H
