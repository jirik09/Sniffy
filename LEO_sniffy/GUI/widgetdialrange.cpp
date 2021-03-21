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


WidgetDialRange::WidgetDialRange(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetDialRange)
{
    ui->setupUi(this);
    ui->label_name->setText(name);
    setObjectName(name);
    units = new QList<params_unit>;
}

WidgetDialRange::~WidgetDialRange()
{
    delete ui;
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
    updateRange(rangeMin,rangeMax,true);
}

void WidgetDialRange::setColor(QString color){
    QString style = "color:"+color;
    ui->widget_dial->setStyleSheet(style);

    style = "background-color:"+color;
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

void WidgetDialRange::setRealValue(float value){
    realValue = value;
    updateControls(0);
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
    this->setMinimumSize(200,70);
}

void WidgetDialRange::plusClicked(){
    float tmpValue = labelValue + buttonStep;
    if(abs(labelValue)>1000){
        tmpValue = labelValue + buttonStep*10;
    }else if(abs(labelValue)>500){
        tmpValue = labelValue + buttonStep*5;
    }else if(abs(labelValue)>200){
        tmpValue = labelValue + buttonStep*2;
    }
    realValue = tmpValue * unitMult;
    updateControls(0);
}

void WidgetDialRange::minusClicked(){
    float tmpValue = labelValue - buttonStep;
    if(abs(labelValue)>1000){
        tmpValue = labelValue - buttonStep*10;
    }else if(abs(labelValue)>500){
        tmpValue = labelValue - buttonStep*5;
    }else if(abs(labelValue)>200){
        tmpValue = labelValue - buttonStep*2;
    }
    realValue = tmpValue * unitMult;
    updateControls(0);
}

void WidgetDialRange::dialValueChanged(int in){
    //dial change is marginal so do the action
    if(abs(realValue)>getRealValueFromDial(in+1.5) || abs(realValue)<getRealValueFromDial(in-1.5) ){
        realValue = getRealValueFromDial(in);
        realValue = (rangePrecision)*round(realValue/(rangePrecision));
        updateControls(0);
    }
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
    float flRead = QLocale().toFloat(text,&success); //try local decimal separator
    if(!success){  //try . as a separator
        flRead = text.toFloat(&success);
    }
    if (success){
        realValue = flRead * unitMult;
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
void WidgetDialRange::setRange(float min, float max, QString baseUnit, float buttonStep, float smalestUnitMult, float defaultValue, bool isLogaritmic ){
    rangeMax = max;
    rangeMin = min;
    rangePrecision = smalestUnitMult;
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
    connect(ui->lineEdit,SIGNAL(textEdited(const QString &)),this,SLOT(textEditChanged(const QString &)));
    connect(ui->lineEdit,SIGNAL(editingFinished()),this,SLOT(textEditFinished()));
    updateControls(0);
}

void WidgetDialRange::updateRange(float min, float max, bool silent)
{
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
    if(realValue>rangeMax){
        realValue = rangeMax;
    }
    if(realValue<rangeMin){
        realValue = rangeMin;
    }
    if(except!=2){
        //calculate the unit mult ans label value from real value and available ranges in units selection
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

            if(abs(tempLabelVal)>=1 && abs(tempLabelVal)<1000){
                break;
            }
        }
        ui->label_unit->setText(unitString);
        ui->lineEdit->setText(QString::number(labelValue,'f',2));
    }

    if(except!=1){
        disconnect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(dialValueChanged(int)));
        ui->dial->setValue(getValueForDial(realValue));
        connect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(dialValueChanged(int)));
    }
    if(!silent)
        emit valueChanged(realValue);
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








