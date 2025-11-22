#ifndef SIMPLEHPROGRESSBAR_H
#define SIMPLEHPROGRESSBAR_H

#include <QWidget>
#include <QColor>
#include <QPaintEvent>
#include <QPainter>

class SimpleHProgressBar : public QWidget {
    Q_OBJECT
public:
    explicit SimpleHProgressBar(QWidget *parent=nullptr)
        : QWidget(parent) {
        setMinimumHeight(20); 
        setMaximumHeight(28);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_showPercent = false; 
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
        
        QRect outer = rect();
        // Frame rectangle (inset to match style)
        QRect frameRect = outer.adjusted(0, 0, -1, -1);

        // background
        p.fillRect(frameRect, m_background);

        // progress fill (left-to-right)
        const double span = double(m_max - m_min);
        double ratio = span>0 ? double(m_value - m_min)/span : 0.0;
        if(ratio < 0) ratio = 0; else if(ratio > 1) ratio = 1;

        const int fullWidth = frameRect.width();
        int fillW = int(fullWidth * ratio + 0.5);

        if(fillW > 0){
            int top = frameRect.top();
            int bottom = frameRect.bottom();
            int left = frameRect.left();
            
            QRect fillRect(left, top, fillW, bottom - top + 1);
            p.fillRect(fillRect, m_color);
        }

        // frame
        if(m_frame.alpha() > 0){
            p.setPen(QPen(m_frame,1));
            p.drawRect(frameRect);
        }

        if(m_showPercent){
            p.setPen(m_text);
            QString txt = QString::number(int(ratio*100+0.5)) + "%";
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

#endif // SIMPLEHPROGRESSBAR_H
