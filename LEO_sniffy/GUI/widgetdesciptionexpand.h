#ifndef WIDGETDESCIPTIONEXPAND_H
#define WIDGETDESCIPTIONEXPAND_H

#include <QWidget>
#include <QVBoxLayout>
#include <QDebug>

#include "widgetlabel.h"

#include "../graphics/graphics.h"

namespace Ui {
class WidgetDesciptionExpand;
}

class WidgetDesciptionExpand : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetDesciptionExpand(QWidget *parent = nullptr, QString name = "");
    ~WidgetDesciptionExpand();
    void addLabel(QString name, QString value);
    void clearLabels();

private:
    Ui::WidgetDesciptionExpand *ui;

    QList<WidgetLabel*> labels; // children owned by ui->widget_info via layout parentage
    QVBoxLayout *labelsLayout {nullptr};
    bool isHidden = false;

private slots:
    bool eventFilter(QObject *obj, QEvent *event) override;

};

#endif // WIDGETDESCIPTIONEXPAND_H
