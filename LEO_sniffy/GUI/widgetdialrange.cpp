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

    dial = new CustomDial(ui->widget_dial);

    ui->horizontalLayout_2->addWidget(dial);

    units = new QList<params_unit>;


}

WidgetDialRange::~WidgetDialRange()
{
    delete ui;
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

void WidgetDialRange::setSelected(int index){
    ui->comboBox->setCurrentIndex(index);
}

void WidgetDialRange::plusClicked(){
    float tmpValue = labelValue + buttonStep;
    realValue = tmpValue * unitMult;
    //qDebug ("plus action real:%f step:%f tmp:%f label:%f mult:%f",realValue,buttonStep, tmpValue,labelValue,unitMult);
    updateControls(0);
}

void WidgetDialRange::minusClicked(){
    float tmpValue = labelValue - buttonStep;
    realValue = tmpValue * unitMult;
    //qDebug ("plus action real:%f step:%f tmp:%f label:%f mult:%f",realValue,buttonStep, tmpValue,labelValue,unitMult);
    updateControls(0);

}

void WidgetDialRange::dialValueChanged(int in){


    //dial change is marginal so do the action
    if(abs(realValue)>getRealValueFromDial(in+1.5) || abs(realValue)<getRealValueFromDial(in-1.5) ){
        realValue = getRealValueFromDial(in);
        //qDebug ("dial action real:%f in:%d step:%f",realValue,in, dialStep);
        updateControls(0);
    }
}

void WidgetDialRange::unitChanged(int in){
    //qDebug("unit changed : index:%d real:%f",in,realValue);
    realValue = realValue*units->at(in).mult/unitMult;
    //qDebug("unit changed : new real:%f",realValue);
    updateControls(0);
}


void WidgetDialRange::textEditFinished(){
    //qDebug("finished");
    updateControls(0);
}

void WidgetDialRange::textEditChanged(const QString & text){
    //qDebug("plain text edited: %s",text.toUtf8().data());
    bool success = false ;

    //try local decimal separator
    float flRead = QLocale().toFloat(text,&success);

    //try . as a separator
    if(!success){
        flRead = text.toFloat(&success);
    }

    if (success){
        realValue = flRead * unitMult;
        //qDebug("succes: %f",realValue);
        updateControls(2);
    }  else{
        // ui->lineEdit->setText(QString::number(labelValue,'f',2));
    }
}


/*
Set range fills the dial with options.
@parmas
min - minimum value
max - maximum value
baseUnit - base unit to be used e.g. 's' for seconds (don't use ms or mV!)
buttonStep - one click step. Linked with curent label value -> not dependent on unit (1 by default)
smalestUnitMult - smallest unit to be shown in comboBox unit selection (1 by default)
defaultValue - selected by default (0 by default)
log - bool type true=log scale, false=lin scale (false by default)
*/
void WidgetDialRange::setRange(float min, float max, QString baseUnit, float buttonStep, float smalestUnitMult, float defaultValue, bool log ){
    rangeMax = max;
    rangeMin = min;
    rangePrecision = smalestUnitMult;
    dialStep = (max-min)/dialMaxValue;
    this->buttonStep = buttonStep;

    logaritmic = log;

    //min cannot be 0 for log scale --> set 0.001
    if(log && min==0){
        rangeMin=0.001;
    }

    if(log){
        logGain = dialMaxValue/log2(rangeMax/rangeMin);
        logOffset = -logGain*log2(rangeMin);
    }


    dial->setMinimum(0);
    dial->setMaximum(dialMaxValue);

    if(defaultValue<min){
        realValue=rangeMin;
    }else if(defaultValue>max){
        realValue=rangeMax;
    }else{
        realValue= defaultValue;
    }


    if (smalestUnitMult<1e-3){
        addOption("u"+baseUnit,0.000001);
    }
    if (smalestUnitMult<1){
        addOption("m"+baseUnit,0.001);
        ui->comboBox->setCurrentIndex(units->length()-1);
        unitMult = units->last().mult;
    }

    if(max>9 && max<1000){
        addOption(baseUnit,1);
        ui->comboBox->setCurrentIndex(units->length()-1);
        // unitString = units->last().unit;
        unitMult = units->last().mult;
    }


    if(max>=1000){
        addOption("k"+baseUnit,1000);
    }if(max>1000000){
        addOption("M"+baseUnit,1000000);
    }


    connect(ui->pushButton_plus,SIGNAL(clicked()),this,SLOT(plusClicked()));
    connect(ui->pushButton_minus,SIGNAL(clicked()),this,SLOT(minusClicked()));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged(int)));
    connect(dial,SIGNAL(valueChanged(int)),this,SLOT(dialValueChanged(int)));
    connect(ui->lineEdit,SIGNAL(textEdited(const QString &)),this,SLOT(textEditChanged(const QString &)));
    connect(ui->lineEdit,SIGNAL(editingFinished()),this,SLOT(textEditFinished()));

    updateControls(0);

}

void WidgetDialRange::updateControls(int except){

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

            if(tempLabelVal>=1 && tempLabelVal<1000){
                break;
            }
        }

        ui->label_unit->setText(unitString);
        ui->lineEdit->setText(QString::number(labelValue,'f',2));
    }


    if(except!=1){
        // qDebug("Dial set : %f",(realValue-rangeMin)/dialStep);
        disconnect(dial,SIGNAL(valueChanged(int)),this,SLOT(dialValueChanged(int)));
        dial->setValue(getValueForDial(realValue));
        connect(dial,SIGNAL(valueChanged(int)),this,SLOT(dialValueChanged(int)));
    }
    emit valueChanged(realValue);
}

float WidgetDialRange::getRealValueFromDial(int in){
    if(!logaritmic){
       // qDebug("real lin %f",in*dialStep);
        return rangeMin+in*dialStep;
    }else{
        float ret = pow(2,(in-logOffset)/logGain);
        //qDebug("real log10 val:%f in:%d",ret,in);
        return ret;
    }
}

int WidgetDialRange::getValueForDial(float real){

    if(!logaritmic){
        //qDebug("dial lin %f",real);
        return (real-rangeMin)/dialStep;//rangeMin+in*dialStep;
    }else{
        float ret = logOffset + logGain*log2(real);
        //qDebug("dial log10 val:%f in:%f",ret,real);
        return ret;
    }

};








