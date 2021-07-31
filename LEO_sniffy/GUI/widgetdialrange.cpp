/*
Class for widget with dial and controls with range
- Wiget has to be filled with setRange(). -> for the time being it can be done only once (see more at function description)
- Wigget has several controls.
    Buttons Plus / Minus
    ComboBox with unit selection
    Editable QLineEdit - user can add his own value
    Dial - settable to be linear or logaritmic
*/

#include "widgetdialrange.h"
#include "ui_widgetdialrange.h"


WidgetDialRange::WidgetDialRange(QWidget *parent, QString name, int optionalEmitParam) :
    QWidget(parent),
    optionalEmitParam(optionalEmitParam),
    ui(new Ui::WidgetDialRange)
{
    ui->setupUi(this);
    setStyleSheet(Graphics::STYLE_DIAL);
    ui->comboBox->setStyleSheet(Graphics::STYLE_COMBO_BOX);

    //    QString style = "QPushButton:pressed{border: 2px solid rgb(48,48,48);}"
    //                    "QPushButton{border: none;color: rgb(214,214,214);"
    //                    "font: 99 12pt 'MS Shell Dlg 2';font-weight:bold;}";
    //    ui->widget_5->setStyleSheet(style);

    ui->label_name->setText(name);
    setObjectName(name);
    units = new QList<params_unit>;
    ui->dial->setCustomGraphics(Graphics::STYLE_CUSTOM_DIALS_USED);

    this->setMinimumSize(170,85);
    this->setMaximumSize(400,120);
    setColor(Graphics::COLOR_CONTROLS);
}

WidgetDialRange::~WidgetDialRange()
{
    delete ui;
}

void WidgetDialRange::setName(QString name){
    ui->label_name->setText(name);
}

void WidgetDialRange::setAdditionalLabelText(QString text){
    ui->label_addition->setText(text);
}

void WidgetDialRange::setAdditionalLabelColor(QString color){
    ui->label_addition->setStyleSheet("QLabel{color: "+color+";}");
}

QByteArray WidgetDialRange::saveGeometry()
{
    QByteArray *data;
    data = new QByteArray();
    QDataStream stream(data,QIODevice::WriteOnly);

    stream << realValue;
    stream << dialMaxValue;
    stream << rangeMin;
    stream << rangeMax;
    return *data;
}

void WidgetDialRange::restoreGeometry(QByteArray geom)
{
    QDataStream stream(geom);
    stream >> realValue;
    stream >> dialMaxValue;
    stream >> rangeMin;
    stream >> rangeMax;
    dialStep = (rangeMax-rangeMin)/dialMaxValue;

    if(logaritmic){
        logGain = dialMaxValue/log2(rangeMax/rangeMin);
        logOffset = -logGain*log2(rangeMin);
    }

    if(defaultRealValue<rangeMin){
        realValue=rangeMin;
        defaultRealValue = realValue;
    }else if(defaultRealValue>rangeMax){
        realValue=rangeMax;
        defaultRealValue = realValue;
    }
    updateControls(0,true);
}

void WidgetDialRange::setColor(QString color){
    QString style = Graphics::STYLE_DIAL+"QWidget{color:"+color+";}";
    ui->widget_dial->setStyleSheet(style);

    if(Graphics::STYLE_TRANSPARENCY_USED)
        color = color.remove("#");
    style = QString(Graphics::STYLE_PUSH_BUTTON).arg(color);
    ui->pushButton_plus->setStyleSheet(style);
    ui->pushButton_minus->setStyleSheet(style);
}

void WidgetDialRange::setDialColor(QString &textStyleSheet){
    ui->widget_dial->setStyleSheet(textStyleSheet);
    qDebug () << "OBSOLETE use setColor instead";
}

void WidgetDialRange::setDialButtonsColor(QString &bckgndStyleSheet){
    ui->pushButton_plus->setStyleSheet(bckgndStyleSheet);
    ui->pushButton_minus->setStyleSheet(bckgndStyleSheet);
    qDebug () << "OBSOLETE use setColor instead";
}

float WidgetDialRange::getDefaultRealValue() const{
    return defaultRealValue;
}

float WidgetDialRange::getRealValue() const
{
    return realValue;
}

void WidgetDialRange::disable(bool disable){
    this->setDisabled(disable);
    ui->label_addition->setStyleSheet("QLabel{color: "+Graphics::COLOR_COMPONENT_DISABLED+";}");
}

void WidgetDialRange::setNumOfDecimals(int value)
{
    numOfDecimals = value;
}

void WidgetDialRange::setRealValue(float value, bool silent){
    realValue = value;
    updateControls(0,silent);
}

void WidgetDialRange::addOption (QString unit,float mult){
    params_unit *tmpUnit = new params_unit();
    tmpUnit->mult = mult;
    tmpUnit->unit = unit;

    units->append(*tmpUnit);
    ui->comboBox->addItem(unit);
}

void WidgetDialRange::hideUnitSelection(void){
    ui->comboBox->close();
    ui->widget_4->close();
    this->setMinimumSize(170,60);
    this->setMaximumSize(400,95);
}

void WidgetDialRange::plusClicked(){
    float tmpValue = labelValue + buttonStep ;
    labelValue = labelValue + 0.000001;
    if(abs(labelValue)>=1000){
        tmpValue = labelValue + buttonStep*10;
    }else if(abs(labelValue)>=500){
        tmpValue = labelValue + buttonStep*5;
    }else if(abs(labelValue)>=200){
        tmpValue = labelValue + buttonStep*2;
    }else if(abs(labelValue)<2 && numOfDecimals>1){
        tmpValue = labelValue + buttonStep*0.05;
    }else if(abs(labelValue)<5 && numOfDecimals>1){
        tmpValue = labelValue + buttonStep*0.1;
    }else if(abs(labelValue)<10 && numOfDecimals>1){
        tmpValue = labelValue + buttonStep*0.2;
    }else if(abs(labelValue)<50&& numOfDecimals>=1){
        tmpValue = labelValue + buttonStep*0.5;
    }

    realValue = tmpValue * unitMult;
    updateControls(0);
}

void WidgetDialRange::minusClicked(){

    float tmpValue = labelValue - buttonStep;
    labelValue = labelValue - 0.000001;
    if(abs(labelValue)>=1000){
        tmpValue = labelValue - buttonStep*20;
    }else if(abs(labelValue)>=500){
        tmpValue = labelValue - buttonStep*10;
    }else if(abs(labelValue)>=200){
        tmpValue = labelValue - buttonStep*2;
    }else if(abs(labelValue)<2 && numOfDecimals>1){
        tmpValue = labelValue - buttonStep*0.05;
    }else if(abs(labelValue)<5 && numOfDecimals>1){
        tmpValue = labelValue - buttonStep*0.1;
    }else if(abs(labelValue)<10 && numOfDecimals>1){
        tmpValue = labelValue - buttonStep*0.2;
    }else if(abs(labelValue)<50 && numOfDecimals>=1){
        tmpValue = labelValue - buttonStep*0.5;
    }

    realValue = tmpValue * unitMult;
    updateControls(0);
}

void WidgetDialRange::dialValueChanged(int in){

    //dial change is marginal so do the action
    //if(abs(realValue)>getRealValueFromDial(in+1.5) || abs(realValue)<getRealValueFromDial(in-1.5) ){
    realValue = NumberParser::getNiceNumber(getRealValueFromDial(in),3);
    //realValue = getRealValueFromDial(in);
    realValue = (rangePrecision)*round(realValue/(rangePrecision));
    updateControls(1);
    // }
}

void WidgetDialRange::unitChanged(int in){
    realValue = realValue*units->at(in).mult/unitMult;
    updateControls(0);
}


void WidgetDialRange::textEditFinished(){
    updateControls(0);
}

void WidgetDialRange::textEditChanged(const QString & text){
    bool success = false ;
    double value =  NumberParser::parse(text,success);

    if (success){
        realValue = value * unitMult;
        updateControls(2);
    }
}

/*
Set range fills the dial with options.
@parmas
min - minimum value
max - maximum value
baseUnit - base unit to be used e.g. 's' for seconds (don't use ms or mV!)
buttonStep - one click step. Linked with curent label value -> not dependent on unit (1 by default)
smalestUnitMult -   smallest unit to be shown in comboBox unit selection (1 by default)
                    this value is also used a minimum for value rounding when set by dial (0.1 (Volts) means dial will set only values rounded to 100mV)
defaultValue - selected by default (0 by default)
log - bool type true=log scale, false=lin scale (false by default)
*/
void WidgetDialRange::setRange(float min, float max, QString baseUnit, float buttonStep, float smalestUnitMult, float defaultValue, bool isLogaritmic, int numOfDecimals, bool silent){

    if(units->length()>0){
        disconnect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged(int)));
        units->clear();
        ui->comboBox->clear();
        connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged(int)));
    }

    rangeMax = max;
    rangeMin = min;
    rangePrecision = smalestUnitMult;
    this->numOfDecimals = numOfDecimals;

    if(numOfDecimals ==0 && (max - min) <dialMaxValue){
        dialMaxValue = (max - min)*5;
    }

    dialStep = (max-min)/dialMaxValue;
    this->buttonStep = buttonStep;
    logaritmic = isLogaritmic;

    //min cannot be 0 for log scale --> set 0.001
    if(isLogaritmic && min==0){
        rangeMin=0.001;
    }

    if(isLogaritmic){
        logGain = dialMaxValue/log2(rangeMax/rangeMin);
        logOffset = -logGain*log2(rangeMin);
    }

    ui->dial->setRange(0,dialMaxValue);

    if(defaultValue<min){
        realValue=rangeMin;
    }else if(defaultValue>max){
        realValue=rangeMax;
    }else{
        realValue= defaultValue;
    }

    defaultRealValue = realValue;

    if (smalestUnitMult<=1e-6){
        addOption("n"+baseUnit,0.000000001);
    }
    if (smalestUnitMult<1e-3){
        addOption("u"+baseUnit,0.000001);
    }
    if (smalestUnitMult<1){
        addOption("m"+baseUnit,0.001);
        ui->comboBox->setCurrentIndex(units->length()-1);
        unitMult = units->last().mult;
    }

    //  if(max>9 && max<1000){
    addOption(baseUnit,1);
    ui->comboBox->setCurrentIndex(units->length()-1);
    // unitString = units->last().unit;
    unitMult = units->last().mult;
    //  }

    if(max>=1000){
        addOption("k"+baseUnit,1000);
    }if(max>1000000){
        addOption("M"+baseUnit,1000000);
    }

    connect(ui->pushButton_plus,SIGNAL(clicked()),this,SLOT(plusClicked()));
    connect(ui->pushButton_minus,SIGNAL(clicked()),this,SLOT(minusClicked()));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged(int)));
    connect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(dialValueChanged(int)));
    connect(ui->lineEdit,SIGNAL(textEdited(QString)),this,SLOT(textEditChanged(QString)));
    connect(ui->lineEdit,SIGNAL(editingFinished()),this,SLOT(textEditFinished()));
    //updateControls(0);
    setRealValue(defaultRealValue, silent);
}

void WidgetDialRange::updateRange(float min, float max, bool silent)
{
    if(rangeMax == max && rangeMin == min){
        return;
    }

    rangeMax = max;
    rangeMin = min;
    dialStep = (max-min)/dialMaxValue;

    //min cannot be 0 for log scale --> set 0.001
    if(logaritmic && min==0){
        rangeMin=0.001;
    }

    if(logaritmic){
        logGain = dialMaxValue/log2(rangeMax/rangeMin);
        logOffset = -logGain*log2(rangeMin);
    }

    if(defaultRealValue<min){
        realValue=rangeMin;
        defaultRealValue = realValue;
    }else if(defaultRealValue>max){
        realValue=rangeMax;
        defaultRealValue = realValue;
    }
    updateControls(0,silent);
}

void WidgetDialRange::enableFineMousePrecision()
{
    dialMaxValue = 2500;
    ui->dial->setRange(0,dialMaxValue);
    updateRange(rangeMin,rangeMax);
}

void WidgetDialRange::updateControls(int except, bool silent){
    // qDebug () << realValue << NumberParser::getNiceNumber(realValue/1000,numOfDecimals+1) << NumberParser::getNiceNumber(realValue,numOfDecimals+1);
    //qDebug () << "pico" << qPow(10,-1)<<qPow(10,0)<<qPow(10,1)<<qPow(10,2);
    if(realValue>rangeMax){
        realValue = rangeMax;
    }
    if(realValue<rangeMin){
        realValue = rangeMin;
    }
    if(except!=2){ //do not update line edit and unit
        //calculate the unit mult and label value from real value and available ranges in units selection
        int i = 0;
        float tempLabelVal = 0;
        for(i = units->length()-1;i>=0;i--){
            tempLabelVal = realValue / units->at(i).mult;
            unitMult = units->at(i).mult;
            unitString = units->at(i).unit;
            labelValue = tempLabelVal;

            //qDebug( "Handling exception not caught in slot. %d  %d", ui->comboBox->count(), i);
            disconnect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged(int)));
            ui->comboBox->setCurrentIndex(i);
            connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged(int)));

            if(abs(tempLabelVal)>=1 && abs(tempLabelVal)<=1000){
                break;
            }
        }
        ui->label_unit->setText(unitString);
        ui->lineEdit->setText(QString::number(labelValue,'f',numOfDecimals));
    }

    if(except!=1){ //do not update dial
        disconnect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(dialValueChanged(int)));
        ui->dial->setValue(getValueForDial(realValue));
        connect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(dialValueChanged(int)));
    }
    if(!silent){
        emit valueChanged(realValue,optionalEmitParam);
    }
}

float WidgetDialRange::getRealValueFromDial(int in){
    if(!logaritmic){
        return rangeMin+in*dialStep;
    }else{
        float ret = pow(2,(in-logOffset)/logGain);
        return ret;
    }
}

int WidgetDialRange::getValueForDial(float real){
    if(!logaritmic){
        return (real-rangeMin)/dialStep;//rangeMin+in*dialStep;
    }else{
        float ret = logOffset + logGain*log2(real);
        return ret;
    }
};








