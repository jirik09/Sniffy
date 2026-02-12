#ifndef TOASTWIDGET_H
#define TOASTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QTimer>
#include <QHBoxLayout>

class ToastWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToastWidget(QWidget *parent = nullptr);

    void showMessage(const QString &text, int durationMs = 10000);
    QString text() const; // For deduplication

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void fadeOut();
    void onAnimationFinished();

private:
    QLabel *label;
    QTimer *timer;
};

#endif // TOASTWIDGET_H
