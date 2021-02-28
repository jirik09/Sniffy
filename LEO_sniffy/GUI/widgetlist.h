#ifndef WIDGETLIST_H
#define WIDGETLIST_H

#include <QWidget>
#include <QStringListModel>
#include <QTextStream>
#include <QFile>

#include "../graphics/colors.h"

namespace Ui {
class WidgetList;
}

class WidgetList : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetList(QWidget *parent = nullptr);
    ~WidgetList();

    void appendNumber(float timeMark, QString prefix, float num, QString affix = "");
    void appendString(QString timeMark, QString str, QString affix = "");
    void associateNumber(QString prefix, float num, QString affix = "");
    void associateString(QString prefix, QString num, QString affix = "");
    void appendAssociatedString(QString str);
    void clearSample(int index);
    void clearLast();
    void clear();
    void saveList(QString name);

private:
    Ui::WidgetList *ui;
    QStringListModel *model;
    QStringList list;
};

#endif // WIDGETLIST_H
