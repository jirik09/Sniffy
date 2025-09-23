#ifndef WIDGETSELECTION_H
#define WIDGETSELECTION_H

#include <QWidget>

#include "../graphics/graphics.h"

namespace Ui {
class WidgetSelection;
}

typedef struct
{
    QString shownValue;
    float realValue;
} params_sel;

class WidgetSelection : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSelection(QWidget *parent = nullptr, QString name = "");
    ~WidgetSelection();
    QByteArray saveGeometry();
    void restoreGeometry(QByteArray geom);

private:
    Ui::WidgetSelection *ui;
    QList<params_sel> options; // value semantics
    int selectedIndex;

public:
    void addOption (QString shownValue, float realValue);
    void clear();
    void setSelected(int index, bool silent = false);
    int getSelectedIndex();
    qreal getSelectedValue();
    int count();

private slots:
    void indexChanged(int index);
signals:
    void selectedIndexChanged(int index, float realValue = 0);
};

#endif // WIDGETSELECTION_H
