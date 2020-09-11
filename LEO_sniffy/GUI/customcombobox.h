#ifndef CUSTOMCOMBOBOX_H
#define CUSTOMCOMBOBOX_H

#include <QWidget>
#include <QComboBox>
#include <QMouseEvent>
#include <QDebug>

class CustomComboBox : public QComboBox
{
    Q_OBJECT
public:

    CustomComboBox(QWidget * parent = nullptr);
    void wheelEvent(QWheelEvent *e) override;
};

#endif // CUSTOMCOMBOBOX_H
