/*
Class for widget with dial and controls
- Options has to be added by addOption()
- Each option must have shownValue(QString), unit(QString), realValue(float)
- option selection is done by Plus / Mius buttons, ComboBox or from Dial
*/
#include "widgetdial.h"
#include "ui_widgetdial.h"


WidgetDial::WidgetDial(QWidget *parent, QString name, int optionalEmitParam) :
    QWidget(parent),
    ui(new Ui::WidgetDial),
    optionalEmitParam(optionalEmitParam)
{
    ui->setupUi(this);
    ui->label_name->setText(name);
    ui->dial->setPageStep(1);
    setObjectName(name);

    connect(ui->pushButton_plus,SIGNAL(clicked()),this,SLOT(plusClicked()));
    connect(ui->pushButton_minus,SIGNAL(clicked()),this,SLOT(minusClicked()));
    connect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(valChanged(int)));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),ui->dial, SLOT(setValue(int)));
    options = new QList<params_dial>;

    QString style = "QWidget:disabled{color: "+QString::fromUtf8(COLOR_GREY)+"} "
                    "QWidget{color:"+QString::fromUtf8(COLOR_WHITE) +"}";

    ui->widget->setStyleSheet(style);
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
    disconnect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(valChanged(int)));
    setSelectedIndex(geom.toInt(),true);
    connect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(valChanged(int)));
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

qreal WidgetDial::getRealValue() const
{
    return options->at(selectedIndex).realValue;
}

void WidgetDial::setSelectedIndex(int index, bool silent){
    ui->dial->setValue(index);
    ui->comboBox->setCurrentIndex(index);

    ui->label_unit->setText(options->at(index).unit);
    ui->label_value->setText(options->at(index).shownValue);
    selectedIndex = index;
    if(!silent){
        emit valueChanged(options->at(index).realValue,optionalEmitParam);
    }
}

void WidgetDial::setColor(QString color){
    QString style = "QWidget:disabled{color:"+QString::fromUtf8(COLOR_DARK_GREY)+";}QWidget{color:"+color+";}";
    ui->widget_dial->setStyleSheet(style);

    style = "QPushButton:disabled{background-color:"+Graphics::getButtonDisabledBackgroundColor()+"; color:"+QString::fromUtf8(COLOR_GREY)+";}"
            "QPushButton:pressed{border: 2px solid"+Graphics::getAppBackgroundColor()+";}"
            "QPushButton{border: none;background-color:"+color+";}";
    ui->pushButton_plus->setStyleSheet(style);
    ui->pushButton_minus->setStyleSheet(style);
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
    ui->comboBox->setCurrentIndex(selectedIndex);
}

void WidgetDial::minusClicked(){
    selectedIndex--;
    if(selectedIndex<0){
        selectedIndex = 0;
    }
    ui->comboBox->setCurrentIndex(selectedIndex);
}

void WidgetDial::valChanged(int in){
    if(in>=0 && in<options->length()){
        setSelectedIndex(in);
    }
}








