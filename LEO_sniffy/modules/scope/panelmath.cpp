#include "panelmath.h"

PanelMath::PanelMath(QVBoxLayout *destination, QWidget *parent ) : QObject(parent)
{
    destination->addWidget(new WidgetSeparator(parent,"Math"));

    mathType = new WidgetButtons(parent,4,ButtonTypes::RADIO,"");
    mathType->setObjectName("MathTypeBtnSelection");
    destination->addWidget(mathType);
    mathType->setText("None",0);
    mathType->setColor(Graphics::COLOR_UNUSED,0);
    mathType->setText("Basic",1);
    mathType->setDisabledButton(true,1);
    mathType->setText("Symbolic",2);
    mathType->setText(" FFT ",3);

    destination->addWidget(new WidgetSeparator(parent,""));

    btnChannelASel = new WidgetButtons(parent,4,ButtonTypes::RADIO,"");
    btnChannelASel->setObjectName("MathbtnChannelASel");
    destination->addWidget(btnChannelASel);
    btnChannelASel->setText("CH1",0);
    btnChannelASel->setColor(Graphics::getChannelColor(0),0);
    btnChannelASel->setText("CH2",1);
    btnChannelASel->setColor(Graphics::getChannelColor(1),1);
    btnChannelASel->setText("CH3",2);
    btnChannelASel->setColor(Graphics::getChannelColor(2),2);
    btnChannelASel->setText("CH4",3);
    btnChannelASel->setColor(Graphics::getChannelColor(3),3);

    operatorSel = new WidgetSelection(parent,"");
    destination->addWidget(operatorSel);
    operatorSel->addOption("Add (+)",1);
    operatorSel->addOption("Subtract (-)",2);
    operatorSel->addOption("Multiply (*)",3);
    operatorSel->addOption("Divide (/)",4);

    btnChannelBSel = new WidgetButtons(parent,4,ButtonTypes::RADIO,"");
    btnChannelBSel->setObjectName("MathbtnChannelBSel");
    destination->addWidget(btnChannelBSel);
    btnChannelBSel->setText("CH1",0);
    btnChannelBSel->setColor(Graphics::getChannelColor(0),0);
    btnChannelBSel->setText("CH2",1);
    btnChannelBSel->setColor(Graphics::getChannelColor(1),1);
    btnChannelBSel->setText("CH3",2);
    btnChannelBSel->setColor(Graphics::getChannelColor(2),2);
    btnChannelBSel->setText("CH4",3);
    btnChannelBSel->setColor(Graphics::getChannelColor(3),3);

    symbolicTitle = new WidgetLabel(parent,"Type an expression");
    destination->addWidget(symbolicTitle);
    symbolicExpression = new WidgetTextInput(parent,"","a+b");
    symbolicExpression->setObjectName("symbExpr");
    destination->addWidget(symbolicExpression);
    symbolicDesc = new WidgetLabel(parent,"Use:","t=time, a=CH1, b=CH2, ...");
    destination->addWidget(symbolicDesc);
    symbolicExample= new WidgetLabel(parent,"Example:","2.1*a+ln(b)+sin(pi*100*t)");
    destination->addWidget(symbolicExample);


    btnChannelFFTSel = new WidgetButtons(parent,4,ButtonTypes::RADIO,"");
    btnChannelFFTSel->setObjectName("FFTbtnChannelBSel");
    destination->addWidget(btnChannelFFTSel);
    btnChannelFFTSel->setText("CH1",0);
    btnChannelFFTSel->setColor(Graphics::getChannelColor(0),0);
    btnChannelFFTSel->setText("CH2",1);
    btnChannelFFTSel->setColor(Graphics::getChannelColor(1),1);
    btnChannelFFTSel->setText("CH3",2);
    btnChannelFFTSel->setColor(Graphics::getChannelColor(2),2);
    btnChannelFFTSel->setText("CH4",3);
    btnChannelFFTSel->setColor(Graphics::getChannelColor(3),3);
    destination->addWidget(btnChannelFFTSel);

    selFFTWindow = new WidgetSelection(parent,"Window");
    selFFTWindow->setObjectName("FFTWindowSel");
    selFFTWindow->addOption("Rectangular",FFTWindow::rectangular);
    selFFTWindow->addOption("Hamming",FFTWindow::hamming);
    selFFTWindow->addOption("Hann",FFTWindow::hann);
    selFFTWindow->addOption("Blackman",FFTWindow::blackman);
    selFFTWindow->addOption("Flat top",FFTWindow::flatTop);
    destination->addWidget(selFFTWindow);
    selFFTWindow->setSelected(3,true);

    selFFTLength = new WidgetSelection(parent,"Length");
    selFFTLength->setObjectName("selFFTLength");
    selFFTLength->addOption("1024",1024);
    selFFTLength->addOption("2048",2048);
    selFFTLength->addOption("4096",4096);
    selFFTLength->addOption("8192",8192);
    selFFTLength->addOption("16384",16384);
    destination->addWidget(selFFTLength);
    selFFTWindow->setSelected(1,true);

    swFFTType = new WidgetButtons(parent,2,ButtonTypes::RADIO,"Type");
    swFFTType->setObjectName("FFTTypeSelection");
    swFFTType->setText("Spectrum",0);
    swFFTType->setText("Periodogram",1);
    destination->addWidget(swFFTType);

    connect(btnChannelFFTSel,&WidgetButtons::clicked,this,&PanelMath::fftCallback);
    connect(swFFTType,&WidgetButtons::clicked,this,&PanelMath::fftTypeCallback);
    connect(selFFTWindow,&WidgetSelection::selectedIndexChanged,this,&PanelMath::fftCallback);
    connect(selFFTLength,&WidgetSelection::selectedIndexChanged,this,&PanelMath::fftCallback);



    dialFFTVertical = new WidgetDial(parent,"Vertical Scale");
    dialFFTVertical->setObjectName("FFTdialVer");
    destination->addWidget(dialFFTVertical);
    /* dialFFTHorizontal = new WidgetDial(parent,"Horizontal Scale");
    dialFFTHorizontal->setObjectName("FFTdialHor");
    destination->addWidget(dialFFTHorizontal);*/
    dialFFTShift = new WidgetDialRange(parent,"Shift");
    dialFFTShift->setObjectName("FFTshift");
    dialFFTShift->hideUnitSelection();
    destination->addWidget(dialFFTShift);
    fillVerticalDials();
    // fillHorizontalDials();

    connect(dialFFTVertical,&WidgetDial::valueChanged,this,&PanelMath::fftChartCallback);
    connect(dialFFTShift,&WidgetDialRange::valueChanged,this,&PanelMath::fftChartCallback);
    connect(swFFTType,&WidgetButtons::clicked,this,&PanelMath::fftChartCallback);

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
        btnChannelFFTSel->show();
        selFFTWindow->show();
        swFFTType->show();
        selFFTLength->show();
        dialFFTVertical->show();
        //dialFFTHorizontal->show();
        dialFFTShift->show();
        fftTypeCallback();
        fftChartCallback();
        break;
    }

    if(previousMathType == 2){
        emit expressionChanged("");
    }else if(previousMathType == 3){
        emit fftChanged(0, FFTWindow::rectangular, FFTType::periodogram,0);
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
    emit fftChanged(selFFTLength->getSelectedValue(), (FFTWindow)selFFTWindow->getSelectedValue(), FFTType(swFFTType->getSelectedIndex()),btnChannelFFTSel->getSelectedIndex());
}

void PanelMath::fftTypeCallback()
{
    fillVerticalDials();
    fftCallback();
}

void PanelMath::fftChartCallback()
{
    emit fftchartChanged(dialFFTVertical->getRealValue(), dialFFTShift->getRealValue(), swFFTType->getSelectedIndex());
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
    btnChannelFFTSel->hide();
    selFFTWindow->hide();
    swFFTType->hide();
    selFFTLength->hide();
    dialFFTVertical->hide();
    //dialFFTHorizontal->hide();
    dialFFTShift->hide();
}

void PanelMath::fillVerticalDials()
{
    dialFFTVertical->clearOptions();

    if(swFFTType->getSelectedIndex()==0){ //spectrum
        dialFFTVertical->addOption("10","VRms/div",10);
        dialFFTVertical->addOption("5","VRms/div",5);
        dialFFTVertical->addOption("2","VRms/div",2);
        dialFFTVertical->addOption("1","VRms/div",1);
        dialFFTVertical->addOption("500","mVRms/div",0.5);
        dialFFTVertical->addOption("200","mVRms/div",0.2);
        dialFFTVertical->addOption("100","mVRms/div",0.1);
        dialFFTVertical->addOption("50","mVRms/div",0.05);
        dialFFTVertical->addOption("20","mVRms/div",0.02);
        dialFFTVertical->addOption("10","mVRms/div",0.01);
        dialFFTVertical->setDefaultIndex(5);
        dialFFTShift->setRange(-3.3,6.6,"VRms",5,0.01,0);
    }else{  //periodogram
        dialFFTVertical->addOption("50","dB/div",50);
        dialFFTVertical->addOption("20","dB/div",20);
        dialFFTVertical->addOption("10","dB/div",10);
        dialFFTVertical->addOption("5","dB/div",5);
        dialFFTVertical->addOption("2","dB/div",2);
        dialFFTVertical->addOption("1","dB/div",1);
        dialFFTVertical->setDefaultIndex(1);
        dialFFTShift->setRange(-100,100,"dB",10,0.1,20);
    }
}

void PanelMath::fillHorizontalDials()
{
    /*  dialFFTHorizontal->addOption("10","Hz/div",10);
    dialFFTHorizontal->addOption("20","Hz/div",20);
    dialFFTHorizontal->addOption("50","Hz/div",50);
    dialFFTHorizontal->addOption("100","Hz/div",100);
    dialFFTHorizontal->addOption("200","Hz/div",200);
    dialFFTHorizontal->addOption("500","Hz/div",500);
    dialFFTHorizontal->addOption("1","kHz/div",1000);
    dialFFTHorizontal->addOption("2","kHz/div",2000);
    dialFFTHorizontal->addOption("5","kHz/div",5000);
    dialFFTHorizontal->addOption("10","kHz/div",10000);
    dialFFTHorizontal->addOption("20","kHz/div",20000);
    dialFFTHorizontal->addOption("50","kHz/div",50000);
    dialFFTHorizontal->addOption("100","kHz/div",100000);
    dialFFTHorizontal->addOption("200","kHz/div",200000);
    dialFFTHorizontal->addOption("500","kHz/div",500000);
    dialFFTHorizontal->setDefaultIndex(6);*/
}

