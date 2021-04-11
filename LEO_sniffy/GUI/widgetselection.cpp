/*
Widget with combobox.
- Optional label on the laef ant comboBox on the right
- Options has to be added by addOption (params shownValue(QString) and realValue(float))
*/
#include "widgetselection.h"
#include "ui_widgetselection.h"

WidgetSelection::WidgetSelection(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetSelection)
{
    ui->setupUi(this);
    ui->label->setText(name);
    options = new QList<params_sel>;
    ui->comboBox->setStyleSheet("background-color:"+Graphics::COLOR_DATA_AREA+";");
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));
}

WidgetSelection::~WidgetSelection()
{
    delete ui;
}

QByteArray WidgetSelection::saveGeometry()
{
    return  QByteArray::number(getSelectedIndex());
}

void WidgetSelection::restoreGeometry(QByteArray geom)
{
    setSelected(geom.toInt());
}

void WidgetSelection::addOption (QString shownValue,float realValue){
    params_sel *par = new params_sel();
    par->shownValue = shownValue;
    par->realValue = realValue;
    options->append(*par);

    ui->comboBox->addItem(shownValue);

    if(options->length()==1){
        setSelected(0);
    }

}
void WidgetSelection::setSelected(int index, bool silent){
    if(silent)
        disconnect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));
    ui->comboBox->setCurrentIndex(index);
    selectedIndex = index;
    if(silent)
        connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));
}

int WidgetSelection::getSelectedIndex(){
    return ui->comboBox->currentIndex();
}

int WidgetSelection::count(){
    return ui->comboBox->count();
}

void WidgetSelection::indexChanged(int index)
{
    emit selectedIndexChanged(index);
}

void WidgetSelection::clear(){
    ui->comboBox->clear();
}
