#include "voltmeterwindow.h"
#include "ui_voltmeterwindow.h"

VoltmeterWindow::VoltmeterWindow(VoltmeterConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoltmeterWindow),
    config(config)
{
    ui->setupUi(this);

    //setup displays
    QString styleSheet = IMAGE_UNITS_VOLT;
    WidgetDisplay *display;

    display  = new WidgetDisplay("Voltmeter_CH1", "Channel 1", styleSheet, true, 1, HISTORY_SIZE, this);
    display->configLabel(0,"CH1","color:"+Colors::getChannelColorString(0),true);
    display->setProgressBarColor(Colors::getChannelColorString(0));
    displays.append(display);
    display  = new WidgetDisplay("Voltmeter_CH2", "Channel 2", styleSheet, true, 1, HISTORY_SIZE, this);
    display->configLabel(0,"CH2","color:"+Colors::getChannelColorString(1),true);
    display->setProgressBarColor(Colors::getChannelColorString(1));
    displays.append(display);
    display  = new WidgetDisplay("Voltmeter_CH3", "Channel 3", styleSheet, true, 1, HISTORY_SIZE, this);
    display->configLabel(0,"CH3","color:"+Colors::getChannelColorString(2),true);
    display->setProgressBarColor(Colors::getChannelColorString(2));
    displays.append(display);
    display  = new WidgetDisplay("Voltmeter_CH4", "Channel 4", styleSheet, true, 1, HISTORY_SIZE, this);
    displays.append(display);
    display->configLabel(0,"CH4","color:"+Colors::getChannelColorString(3),true);
    display->setProgressBarColor(Colors::getChannelColorString(3));

    foreach(WidgetDisplay * dis, displays){
        dis->setContentsMargins(5, 5, 5, 5);
        dis->showAvgDisplay(false);
        dis->configLabel(2,"VOLTAGE","color:"+QString::fromUtf8(COLOR_GREY),true);
        ui->verticalLayout_display->addWidget(dis);
    }

    //setup control
    tabs = new widgetTab(ui->widget_settings, 2);
    tabs->setText("Control", 0);
    tabs->setText("Data log", 1);
    ui->verticalLayout_settings->addWidget(tabs);

    WidgetSeparator *separatorChannelEnable = new WidgetSeparator(this,"Channel enable");
    tabs->getLayout(0)->addWidget(separatorChannelEnable);

    buttonsChannelEnable = new WidgetButtons(this,4,ButtonTypes::CHECKABLE);
    buttonsChannelEnable->setObjectName("ChannelvoltmeterEnable");
    tabs->getLayout(0)->addWidget(buttonsChannelEnable);
    buttonsChannelEnable->setText("CH1",0);
    buttonsChannelEnable->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(0)),0);
    buttonsChannelEnable->setText("CH2",1);
    buttonsChannelEnable->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(1)),1);
    buttonsChannelEnable->setText("CH3",2);
    buttonsChannelEnable->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(2)),2);
    buttonsChannelEnable->setText("CH4",3);
    buttonsChannelEnable->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(3)),3);

    buttonsMode = new WidgetButtons(this,2,ButtonTypes::RADIO,"Speed");
    buttonsMode->setObjectName("ChannelVoltmode");
    tabs->getLayout(0)->addWidget(buttonsMode);
    buttonsMode->setText("Normal",0);
    buttonsMode->setText("Fast",1);

    labelVdd = new WidgetLabel(this,"Measured Vcc","--V");
    tabs->getLayout(0)->addWidget(labelVdd);

    WidgetSeparator *separatorAvg = new WidgetSeparator(this,"Averaging");
    tabs->getLayout(0)->addWidget(separatorAvg);

    dialAveraging = new WidgetDialRange(tabs ,"Samples");
    dialAveraging->setObjectName("voltAvgSamples");
    dialAveraging->setRange(1,64,"x",1,1,4,false);
    dialAveraging->hideUnitSelection();
    tabs->getLayout(0)->addWidget(dialAveraging);

    labelProgress = new WidgetLabel(this,"Progress","-/-");
    tabs->getLayout(0)->addWidget(labelProgress);

    WidgetSeparator *separatorSet = new WidgetSeparator(this,"Display");
    tabs->getLayout(0)->addWidget(separatorSet);

    buttonsCalc = new WidgetButtons(this,3,ButtonTypes::RADIO,"",0);
    buttonsCalc->setObjectName("buttonscalc");
    tabs->getLayout(0)->addWidget(buttonsCalc);
    buttonsCalc->setText("Min/Max",0);
    buttonsCalc->setText("Ripple",1);
    buttonsCalc->setText("None",2);
    buttonsCalc->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(12)),2);

    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    tabs->getLayout(0)->addItem(verticalSpacer);


    WidgetSeparator *separatorLogger = new WidgetSeparator(this,"Data Logger");
    tabs->getLayout(1)->addWidget(separatorLogger);

    buttonSelectFile = new WidgetButtons(this,1,ButtonTypes::NORMAL,"File");
    buttonSelectFile->setObjectName("buttonselectfile");
    tabs->getLayout(1)->addWidget(buttonSelectFile);
    buttonSelectFile->setText("   Select   ");

    labelFile = new WidgetLabel(this,"","No File selected");
    tabs->getLayout(1)->addWidget(labelFile);

    buttonStartLog = new WidgetButtons(this,1,ButtonTypes::NORMAL);
    buttonStartLog->setObjectName("buttonlogcontrol");
    tabs->getLayout(1)->addWidget(buttonStartLog);
    buttonStartLog->setText("Start",0);
    buttonStartLog->setColor("background-color:"+QString::fromUtf8(COLOR_GREEN),0);
    buttonStartLog->enableAll(false);

    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacerLog;
    verticalSpacerLog = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    tabs->getLayout(1)->addItem(verticalSpacerLog);

    connect(buttonsChannelEnable,&WidgetButtons::statusChanged,this,&VoltmeterWindow::channelEnableCallback);
    connect(dialAveraging,&WidgetDialRange::valueChanged,this,&VoltmeterWindow::averagingCallback);
    connect(buttonsCalc,&WidgetButtons::statusChanged,this,&VoltmeterWindow::buttonsCalcsCallback);
    connect(buttonSelectFile,&WidgetButtons::clicked,this,&VoltmeterWindow::selectFileCallback);
    connect(buttonStartLog,&WidgetButtons::clicked,this,&VoltmeterWindow::datalogCallback);
    connect(buttonsMode,&WidgetButtons::clicked,this,&VoltmeterWindow::modeCallback);

}

VoltmeterWindow::~VoltmeterWindow()
{
    delete ui;
}

void VoltmeterWindow::restoreGUIAfterStartup()
{
    channelEnableCallback(buttonsChannelEnable->getStatus());
    averagingCallback(dialAveraging->getRealValue());
    modeCallback(buttonsMode->getSelectedIndex());
    showEmptyCalcs();
    //buttonStartLog->setChecked(true,1);
    //TODO GUI is loaded to previous state
    //validate the GUI appereance (colors according to selected channel
    //disabled/enabled buttons accordign to selected function
}

void VoltmeterWindow::showData(ChannelData data[], int numChannels){
    for(int i = 0;i<numChannels;i++){
        displays.at(i)->displayString(displays.at(i)->formatNumber(data[i].voltage,'f',4));
        displays.at(i)->updateProgressBar(data[i].percent);
        displays.at(i)->appendNewHistorySample("",data[i].voltage , "V", 1);

        if(buttonsCalc->getSelectedIndex()==0){
            displays.at(i)->displayQerrString(displays.at(i)->formatNumber(data[i].min,'f',4));
            displays.at(i)->displayTerrString(displays.at(i)->formatNumber(data[i].max,'f',4));
            QString styleSheet = IMAGE_UNITS_VMAX;
            displays[i]->setErrStyle(styleSheet);
            styleSheet = IMAGE_UNITS_VMIN;
            displays[i]->setTerrStyle(styleSheet);
            displays[i]->showErrDisplay(true);
            displays[i]->showTerrStyle(true);
        }else if(buttonsCalc->getSelectedIndex()==1){
            displays.at(i)->displayQerrString(displays.at(i)->formatNumber(data[i].frequency,'f',4));
            displays.at(i)->displayTerrString(displays.at(i)->formatNumber(data[i].ripple,'f',4));
            QString styleSheet = IMAGE_UNITS_VRIPPLE;
            displays[i]->setErrStyle(styleSheet);
            styleSheet = IMAGE_UNITS_HZ;
            displays[i]->setTerrStyle(styleSheet);
            displays[i]->showErrDisplay(true);
            displays[i]->showTerrStyle(true);
        }else{
            displays[i]->showErrDisplay(false);
            displays[i]->showTerrStyle(false);
            displays.at(i)->displayQerrString("");
            displays.at(i)->displayTerrString("");
        }
    }
    if(isDataLogRunning){
        appendDatalog(data, numChannels);
    }
}

void VoltmeterWindow::showEmptyCalcs(){
    for(int i = 0;i<MAX_VOLTMETER_CHANNELS;i++){
        if(buttonsCalc->getSelectedIndex()==0){
            displays.at(i)->displayQerrString("--");
            displays.at(i)->displayTerrString("--");
            QString styleSheet = IMAGE_UNITS_VMAX;
            displays[i]->setErrStyle(styleSheet);
            styleSheet = IMAGE_UNITS_VMIN;
            displays[i]->setTerrStyle(styleSheet);
            displays[i]->showErrDisplay(true);
            displays[i]->showTerrStyle(true);
        }else if(buttonsCalc->getSelectedIndex()==1){
            displays.at(i)->displayQerrString("--");
            displays.at(i)->displayTerrString("--");
            QString styleSheet = IMAGE_UNITS_VRIPPLE;
            displays[i]->setErrStyle(styleSheet);
            styleSheet = IMAGE_UNITS_HZ;
            displays[i]->setTerrStyle(styleSheet);
            displays[i]->showErrDisplay(true);
            displays[i]->showTerrStyle(true);
        }else{
            displays[i]->showErrDisplay(false);
            displays[i]->showTerrStyle(false);
            displays.at(i)->displayQerrString("");
            displays.at(i)->displayTerrString("");
        }
    }
}

void VoltmeterWindow::showVddValue(qreal value){
    labelVdd->setValue(QString::number(value)+" V");
    currentVdd = value;
}

void VoltmeterWindow::showProgress(int current, int max){
    labelProgress->setValue(QString::number(current)+"/"+QString::number(max));
}

void VoltmeterWindow::setPins(QString pins[], int numOfCh)
{
    for(int i = 0;i<numOfCh;i++){
        displays.at(i)->configLabel(1,"pin "+pins[i],"color:"+QString::fromUtf8(COLOR_GREY),true);
    }
}

void VoltmeterWindow::appendDatalog(ChannelData data[], int numChannels)
{
    QDateTime now;
    *logStream << QString::number(logSampleIndex)+","+now.currentDateTime().toString("dd.MM.yyyy") + "," +now.currentDateTime().toString("hh:mm:ss.zzz") + ","+ QString::number(currentVdd) ;

    for(int i = 0; i < numChannels; i++){
        *logStream << ","+QString::number(data[i].voltage);
       // *logStream << ","+QString::number(data[i].min);
       // *logStream << ","+QString::number(data[i].max);
       // *logStream << ","+QString::number(data[i].ripple);
       // *logStream << ","+QString::number(data[i].frequency);
    }
    *logStream << "\n";

    logSampleIndex++;
    labelFile->setValue("Logging active (" + QString::number(logSampleIndex) + " smpl)");
}

void VoltmeterWindow::stopDatalog()
{
    if(isDataLogRunning){
        logFile->close();
        isDataLogRunning = false;
        buttonStartLog->setText("Start",0);
        buttonStartLog->setColor("background-color:"+QString::fromUtf8(COLOR_GREEN),0);
        buttonSelectFile->enableAll(true);
        labelFile->setValue("Log stopped (" + QString::number(logSampleIndex) + " smpl)");
    }
}

void VoltmeterWindow::startDatalog()
{
    if(!isDataLogRunning){
        logSampleIndex = 0;
        logFile = new QFile(config->datalogFileName);
        if(logFile->exists()){
            logFile->remove();
        }
        if(logFile->open(QFile::WriteOnly | QFile::Truncate)){
            logStream = new QTextStream(logFile);
            *logStream << "sample ID,date,time,Vdd";
            //logStream->operator<<("sample,date,time,");
            for(int i = 0; i < MAX_VOLTMETER_CHANNELS; i++){
                *logStream << ",Voltage CH"+QString::number(i+1);
               // *logStream << ",Min CH"+QString::number(i+1);
               // *logStream << ",Max CH"+QString::number(i+1);
               // *logStream << ",Ripple CH"+QString::number(i+1);
               // *logStream << ",Frequency CH"+QString::number(i+1);
            }
            *logStream << "\n";
            isDataLogRunning = true;
            buttonStartLog->setText("Stop",0);
            buttonStartLog->setColor("background-color:"+QString::fromUtf8(COLOR_RED),0);
            buttonSelectFile->enableAll(false);
        }else{
            labelFile->setValue("Error opening file");
            qDebug ()<< "ERROR log file could not be opened";
        }
    }
}

void VoltmeterWindow::channelEnableCallback(int status)
{
    int numChEnabled=0;
    for(int i =0;i<4;i++){
        if((int)(pow(2,i))&(status)){
            displays.at(i)->show();
            numChEnabled = i+1;
        }else{
            displays.at(i)->hide();
        }
    }
    emit numChannelsEnabledChanged(numChEnabled);
}

void VoltmeterWindow::averagingCallback(int value)
{
    emit averagingChanged(value);
}

void VoltmeterWindow::buttonsCalcsCallback(int clicked)
{
    showEmptyCalcs();
    if(clicked!=0){
        emit resetMinMax();
    }
}

void VoltmeterWindow::selectFileCallback()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,"Select output file","","Text files (*.csv *.txt)");
    config->datalogFileName = fileName;
    qDebug() << fileName;
    if(fileName.length()>=25){
        fileName = fileName.left(8)+" ... "+fileName.right(12);
    }
    if(fileName.length()>3){
        labelFile->setValue(fileName);
        buttonStartLog->enableAll(true);
    }

}

void VoltmeterWindow::datalogCallback(int index)
{
    Q_UNUSED(index);
    if(buttonStartLog->getText(0)=="Start"){
        startDatalog();
    }else if(buttonStartLog->getText(0)=="Stop"){
        stopDatalog();
    }
}

void VoltmeterWindow::modeCallback(int index)
{
    emit modeChanged(index);
}
