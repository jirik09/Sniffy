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

private:
    Ui::WidgetList *ui;

    int traceCount;
};

#endif // WIDGETLIST_H
