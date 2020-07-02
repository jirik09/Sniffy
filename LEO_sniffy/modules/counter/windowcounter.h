#ifndef WINDOWSCOUNTER_H
#define WINDOWSCOUNTER_H

#include <QWidget>

namespace Ui {
class WindowCounter;
}

class WindowCounter : public QWidget
{
    Q_OBJECT

public:
    explicit WindowCounter(QWidget *parent = nullptr);
    ~WindowCounter();


private:
    Ui::WindowCounter *ui;

};

#endif // WINDOWSCOUNTER_H
