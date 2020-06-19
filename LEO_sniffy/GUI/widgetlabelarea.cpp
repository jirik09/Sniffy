#include "widgetlabelarea.h"
#include "ui_widgetlabelarea.h"

widgetLabelArea::widgetLabelArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetLabelArea)
{
    ui->setupUi(this);
}

widgetLabelArea::~widgetLabelArea()
{
    delete ui;
}
