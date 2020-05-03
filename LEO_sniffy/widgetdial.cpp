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

    dial = new CustomDial(ui->widget_dial);
    dial->setPageStep(1);

    ui->horizontalLayout_2->addWidget(dial);

    connect(ui->pushButton_plus,SIGNAL(clicked()),this,SLOT(plusClicked()));
    connect(ui->pushButton_minus,SIGNAL(clicked()),this,SLOT(minusClicked()));
    connect(dial,SIGNAL(valueChanged(int)),this,SLOT(valChanged(int)));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(valChanged(int)));
    options = new QList<params_dial>;
}

WidgetDial::~WidgetDial()
{
    delete ui;
}

void WidgetDial::addOption (QString shownValue, QString unit,float realValue){

    params_dial *par = new params_dial();
    par->shownValue = shownValue;
    par->unit = unit;
    par->realValue = realValue;
    options->append(*par);

    ui->comboBox->addItem(shownValue + " " +unit);

    dial->setMaximum(options->length()-1);

    if(options->length()==1){
        setSelected(0);
    }
}

void WidgetDial::setSelected(int index){
    dial->setValue(index);
    ui->comboBox->setCurrentIndex(index);
    ui->label_unit->setText(options->at(index).unit);
    ui->label_value->setText(options->at(index).shownValue);
    selectedIndex = index;
}

void WidgetDial::plusClicked(){
    selectedIndex++;
    if(selectedIndex>=options->length()){
        selectedIndex = options->length()-1;
    }
    setSelected(selectedIndex);
}

void WidgetDial::minusClicked(){
    selectedIndex--;
    if(selectedIndex<0){
        selectedIndex = 0;
    }
    setSelected(selectedIndex);
}

void WidgetDial::valChanged(int in){
    if(in>=0 && in<options->length()){
        setSelected(in);
    }
}








