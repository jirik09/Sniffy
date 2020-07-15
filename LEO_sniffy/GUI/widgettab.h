#ifndef WIDGETTAB_H
#define WIDGETTAB_H

#include <QWidget>
#include <QVBoxLayout>

namespace Ui {
class widgetTab;
}

class widgetTab : public QWidget
{
    Q_OBJECT

public:
    explicit widgetTab(QWidget *parent = nullptr, int num=2);
    ~widgetTab();

    void setText(QString text, int index);
    QVBoxLayout * getLayout (int index);

private slots:
    void on_tabWidget_tabBarClicked(int index);

private:
    Ui::widgetTab *ui;

signals:
    void tabBarClicked(int index);
};

#endif // WIDGETTAB_H
