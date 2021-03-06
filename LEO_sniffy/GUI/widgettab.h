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
    QByteArray saveGeometry();
    void restoreGeometry(QByteArray geom);

    void setText(QString text, int index);
    QVBoxLayout * getLayout (int index);

private:
    Ui::widgetTab *ui;

private slots:
    void on_tabWidget_currentChanged(int index);
    void on_tabWidget_tabBarClicked(int index);

signals:
    void currentChanged(int index);
    void tabBarClicked(int index);
};

#endif // WIDGETTAB_H
