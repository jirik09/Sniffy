#include "widgettab.h"
#include "ui_widgettab.h"

widgetTab::widgetTab(QWidget *parent, int num) :
    QWidget(parent),
    ui(new Ui::widgetTab)
{
    ui->setupUi(this);
    // Default to the first tab (index 0)
    ui->tabWidget->setCurrentIndex(0);

    if(num<=5)
        ui->tabWidget->removeTab(5);
    if(num<=4)
        ui->tabWidget->removeTab(4);
    if(num<=3)
        ui->tabWidget->removeTab(3);
    if(num<=2)
        ui->tabWidget->removeTab(2);
    if(num<=1)
        ui->tabWidget->removeTab(1);
}

void widgetTab::setText(QString text, int index){
    ui->tabWidget->setTabText(index,text);
}

QVBoxLayout * widgetTab::getLayout (int index){
    if(index==0)
        return ui->verticalLayout_1;
    if(index==1)
        return ui->verticalLayout_2;
    if(index==2)
        return ui->verticalLayout_3;
    if(index==3)
        return ui->verticalLayout_4;
    if(index==4)
        return ui->verticalLayout_5;
    if(index==5)
        return ui->verticalLayout_6;
    return nullptr;
}

widgetTab::~widgetTab()
{
    delete ui;
}

QByteArray widgetTab::saveGeometry()
{
    return QByteArray::number(ui->tabWidget->currentIndex());
}

void widgetTab::restoreGeometry(QByteArray geom)
{
    ui->tabWidget->setCurrentIndex(geom.toInt());
    //emit tabBarClicked(geom.toInt());
}

void widgetTab::on_tabWidget_currentChanged(int index)
{
    emit currentChanged(index);
}

void widgetTab::on_tabWidget_tabBarClicked(int index)
{
    emit tabBarClicked(index);
}
