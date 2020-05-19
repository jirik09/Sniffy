/*
Widget to be shown on very bottom of the centralWidget
*/
#include "widgetfooter.h"
#include "ui_widgetfooter.h"

WidgetFooter::WidgetFooter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFooter)
{
    ui->setupUi(this);
}

WidgetFooter::~WidgetFooter()
{
    delete ui;
}

QPushButton * WidgetFooter::getPushButtonSize(){
    return ui->pushButton_setSize;

}
