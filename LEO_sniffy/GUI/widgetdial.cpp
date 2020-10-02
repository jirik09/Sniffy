/*
Class for widget with dial and controls
- Options has to be added by addOption()
- Each option must have shownValue(QString), unit(QString), realValue(float)
- option selection is done by Plus / Mius buttons, ComboBox or from Dial
*/
#include "widgetdial.h"
#include "ui_widgetdial.h"


WidgetDial::WidgetDial(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetDial)
{
    ui->setupUi(this);
    ui->label_name->setText(name);
    ui->dial->setPageStep(1);

    connect(ui->pushButton_plus,SIGNAL(clicked()),this,SLOT(plusClicked()));
    connect(ui->pushButton_minus,SIGNAL(clicked()),this,SLOT(minusClicked()));
    connect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(valChanged(int)));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),ui->dial, SLOT(setValue(int)));
    options = new QList<params_dial>;
}

WidgetDial::~WidgetDial()
{
    delete ui;
}

QByteArray WidgetDial::saveGeometry()
{
    //TO DO pass all the data including the min max range color etc
    return QByteArray::number(selectedIndex);
}

void WidgetDial::restoreGeometry(QByteArray geom)
{
    setSelectedIndex(geom.toInt());
}

void WidgetDial::addOption (QString shownValue, QString unit,float realValue){

    params_dial *par = new params_dial();
    par->shownValue = shownValue;
    par->unit = unit;
    par->realValue = realValue;
    options->append(*par);

    ui->comboBox->addItem(shownValue + " " +unit);

    ui->dial->setMaximum(options->length()-1);

    if(options->length()==1){
        setSelectedIndex(0);
    }
}

int WidgetDial::getSelectedIndex() const
{
    return selectedIndex;
}

void WidgetDial::setSelectedIndex(int index){
    ui->dial->setValue(index);
    ui->comboBox->setCurrentIndex(index);

    ui->label_unit->setText(options->at(index).unit);
    ui->label_value->setText(options->at(index).shownValue);
    selectedIndex = index;
    emit valueChanged(options->at(index).realValue);
}

void WidgetDial::setDialColor(QString &textStyleSheet){
    ui->widget_dial->setStyleSheet(textStyleSheet);
}

void WidgetDial::setDialButtonsColor(QString &bckgndStyleSheet){
    ui->pushButton_plus->setStyleSheet(bckgndStyleSheet);
    ui->pushButton_minus->setStyleSheet(bckgndStyleSheet);
}

int WidgetDial::getDefaultIndex() const
{
    return defaultIndex;
}

void WidgetDial::setDefaultIndex(int index)
{
    defaultIndex = index;
    setSelectedIndex(index);
}

void WidgetDial::plusClicked(){
    selectedIndex++;
    if(selectedIndex>=options->length()){
        selectedIndex = options->length()-1;
    }
    setSelectedIndex(selectedIndex);
}

void WidgetDial::minusClicked(){
    selectedIndex--;
    if(selectedIndex<0){
        selectedIndex = 0;
    }
    setSelectedIndex(selectedIndex);
}

void WidgetDial::valChanged(int in){
    if(in>=0 && in<options->length()){
        setSelectedIndex(in);
    }
}








