#ifndef WIDGETLIST_H
#define WIDGETLIST_H

#include <QWidget>
#include <QStringListModel>

namespace Ui {
class WidgetList;
}

class WidgetList : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetList(QWidget *parent = nullptr, int traceCount = 1);
    ~WidgetList();

    void appendNumber(int listIndex, float timeMark, float num);
    void appendString(int listIndex, QString timeMark, QString str);
    void clearSample(int index);
    void clearLast();

private:
    Ui::WidgetList *ui;
    QStringListModel *model;
    QVector<QStringList> lists;
};

#endif // WIDGETLIST_H
