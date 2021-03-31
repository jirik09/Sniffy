#include "panelmath.h"

PanelMath::PanelMath(QVBoxLayout *destination, QWidget *parent ) : QObject(parent)
{
    destination->addWidget(new WidgetSeparator(parent,"Math"));

    mathType = new WidgetButtons(parent,4,ButtonTypes::RADIO,"");
    mathType->setObjectName("MathTypeBtnSelection");
    destination->addWidget(mathType);
    mathType->setText("None",0);
    mathType->setColor(QString::fromUtf8("background-color:")+COLOR_GREY,0);
    mathType->setText("Basic",1);
    mathType->setDisabledButton(true,1);
    mathType->setText("Symbolic",2);
    mathType->setText(" FFT ",3);

    destination->addWidget(new WidgetSeparator(parent,""));

    btnChannelASel = new WidgetButtons(parent,4,ButtonTypes::RADIO,"");
    btnChannelASel->setObjectName("MathbtnChannelASel");
    destination->addWidget(btnChannelASel);
    btnChannelASel->setText("CH1",0);
    btnChannelASel->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(0)),0);
    btnChannelASel->setText("CH2",1);
    btnChannelASel->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(1)),1);
    btnChannelASel->setText("CH3",2);
    btnChannelASel->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(2)),2);
    btnChannelASel->setText("CH4",3);
    btnChannelASel->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(3)),3);

    operatorSel = new WidgetSelection(parent,"");
    destination->addWidget(operatorSel);
    operatorSel->addOption("Add (+)",1);
    operatorSel->addOption("Subtract (-)",2);
    operatorSel->addOption("Multiply (*)",3);
    operatorSel->addOption("Divide (/)",4);
    operatorSel->setSelected(0);

    btnChannelBSel = new WidgetButtons(parent,4,ButtonTypes::RADIO,"");
    btnChannelBSel->setObjectName("MathbtnChannelBSel");
    destination->addWidget(btnChannelBSel);
    btnChannelBSel->setText("CH1",0);
    btnChannelBSel->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(0)),0);
    btnChannelBSel->setText("CH2",1);
    btnChannelBSel->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(1)),1);
    btnChannelBSel->setText("CH3",2);
    btnChannelBSel->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(2)),2);
    btnChannelBSel->setText("CH4",3);
    btnChannelBSel->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(3)),3);

    symbolicTitle = new WidgetLabel(parent,"Type an expression");
    destination->addWidget(symbolicTitle);
    symbolicExpression = new WidgetTextInput(parent,"","a+b");
    symbolicExpression->setObjectName("symbExpr");
    destination->addWidget(symbolicExpression);
    symbolicDesc = new WidgetLabel(parent,"Use:","t=time, a=CH1, b=CH2, ...");
    destination->addWidget(symbolicDesc);
    symbolicExample= new WidgetLabel(parent,"Example:","2.1*a+ln(b)+sin(pi*100*t)");
    destination->addWidget(symbolicExample);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);

    connect(mathType,&WidgetButtons::clicked,this,&PanelMath::typeChanged);
    connect(symbolicExpression,&WidgetTextInput::textChanged,this,&PanelMath::symbolicExpressionCallback);

}

void PanelMath::symbolicError(int errorPosition)
{
    if(errorPosition<=0){
        symbolicDesc->setStyleSheet("");
        symbolicDesc->setName("Use:");
        symbolicDesc->setValue("t=time, a=CH1, b=CH2, ...");

        symbolicExample->setStyleSheet("");
        symbolicExample->setName("Example:");
        symbolicExample->setValue("2.1*a+ln(b)+sin(pi*100*t)");
    }else{
        symbolicDesc->setStyleSheet("font: 10pt \"Courier New\";");
        symbolicDesc->setName("Error");
        symbolicDesc->setValue(errorExp);
        symbolicExample->setStyleSheet("font: 10pt \"Courier New\";");
        symbolicExample->setName("here ");
        QString out;// = QString("^%*s").arg(symbolicExpression->getText().length()-errorPosition).arg("");
        out = QString::asprintf("^%*s", errorExp.length()-errorPosition, "");
        symbolicExample->setValue(out);
    }
}

void PanelMath::typeChanged(int index)
{
    hideAll();

    switch (index) {
    case 0:
        break;
    case 1:
        btnChannelASel->show();
        operatorSel->show();
        btnChannelBSel->show();
        break;
    case 2:
        symbolicDesc->show();
        symbolicTitle->show();
        symbolicExpression->show();
        symbolicExample->show();
        symbolicExpressionCallback(symbolicExpression->getText());
        break;
    case 3:
        fftCallback();
        break;
    }

    if(previousMathType == 2){
        emit expressionChanged("");
    }else if(previousMathType == 3){
        emit fftChanged(0, FFTWindow::rectangular, FFTType::periodogram, 0);
    }

    previousMathType = index;
}

void PanelMath::symbolicExpressionCallback(QString exp)
{
    errorExp = symbolicExpression->getText();
    if(mathType->getSelectedIndex()==2) emit expressionChanged(exp);
}

void PanelMath::fftCallback()
{
    //TODO emit data from GUI
    emit fftChanged(1024, FFTWindow::rectangular, FFTType::periodogram, 0);
}

void PanelMath::hideAll()
{
    btnChannelASel->hide();
    operatorSel->hide();
    btnChannelBSel->hide();
    symbolicDesc->hide();
    symbolicTitle->hide();
    symbolicExpression->hide();
    symbolicExample->hide();
}

