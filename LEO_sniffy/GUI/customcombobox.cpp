#include "customcombobox.h"

CustomComboBox::CustomComboBox(QWidget* parent)
         : QComboBox(parent)
{
}

void CustomComboBox::wheelEvent(QWheelEvent *e){
    //do nothing - ignor the wheel event
    Q_UNUSED(e);
}
