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
}

WidgetSelection::~WidgetSelection()
{
    delete ui;
}

QByteArray WidgetSelection::saveGeometry()
{
    return  QByteArray::number(getSelected());
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
void WidgetSelection::setSelected(int index){
    ui->comboBox->setCurrentIndex(index);
    selectedIndex = index;
}

int WidgetSelection::getSelected(){
    return ui->comboBox->currentIndex();
}

int WidgetSelection::count(){
    return ui->comboBox->count();
}

void WidgetSelection::clear(){
    ui->comboBox->clear();
}
