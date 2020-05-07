/*
Main window of the application.

Left - centralWidget:
- top: logo from ui file.
- Features widgets are added here and act as a buttons
- separator
- widgetFooter with settings

Right - area for dock widgets
- already has DockWidged_device which is will show connection to board and its parameters on the righ
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setup left menu with features separators etc
    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_features->addWidget(sep);

    WidgetFeature_scope = new WidgetFeature(ui->centralwidget,FeatureIcon::SCOPE,"Oscilloscope");
    ui->verticalLayout_features->addWidget(WidgetFeature_scope);
    WidgetFeature_scope->setStatus(FeatureStatus::STOP);

    WidgetFeature_gen = new WidgetFeature(ui->centralwidget,FeatureIcon::SCOPE,"Signal generator");
    ui->verticalLayout_features->addWidget(WidgetFeature_gen);
    WidgetFeature_gen->setStatus(FeatureStatus::STOP);


    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_features->addItem(verticalSpacer);

    WidgetSeparator *sepa = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_features->addWidget(sepa);
    //footer widget
    WidgetFooter *footer = new WidgetFooter();
    ui->verticalLayout_features->addWidget(footer);

    connect(footer->getPushButtonSize(),SIGNAL(clicked()),this,SLOT(setMenuSize()));

    ui->dockWidget_device->setWindowTitle("Device");


    dockWidget_scope=new QDockWidget(this);
    dockWidget_scope->setObjectName("dockWidget_scope");
    dockWidget_scope->setWindowTitle("Oscilloscope");
    dockWidget_scope->setMinimumSize(QSize(250, 250));
    dockWidget_scope->setFloating(false);
    dockWidget_scope->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_scope);
    scp = new WindowScope(dockWidget_scope);






    QVBoxLayout *horizontalLayout;
    horizontalLayout = new QVBoxLayout(dockWidget_scope);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->addWidget(scp);
    dockWidget_scope->setWidget(scp);
    dockWidget_scope->raise();


    connect(WidgetFeature_scope->getPushButton(),SIGNAL(clicked()),this,SLOT(openScope()));





    QSerialPortInfo *portInfo = new QSerialPortInfo();
    QSerialPort *sPort;
    sPort=new QSerialPort(this);
    QList<device_descriptor> list = *new QList<device_descriptor>;

    QList<QSerialPortInfo> ports;
    ports = portInfo->availablePorts();

    QSerialPortInfo tmpPort;
    int numberOfDevices = 0;
    QByteArray received;
    device_descriptor desc;

    foreach (tmpPort, ports){
        sPort = new QSerialPort(this);
        sPort->setPort(tmpPort);
        sPort->setPortName(tmpPort.portName());
        sPort->setBaudRate(460800);

        sPort->setDataBits(QSerialPort::DataBits::Data8);
        sPort->setParity(QSerialPort::Parity::NoParity);
        sPort->setStopBits(QSerialPort::StopBits::OneStop);
        sPort->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

        if(sPort->open(QIODevice::ReadWrite)){
            sPort->write("IDN?;");
            QThread::msleep(150);
            received = sPort->readAll();
            if (received.length()>1){
                desc.port = tmpPort.portName();
                desc.speed = 460800;
                desc.connType = Connection::SERIAL;
                desc.index = 0 + numberOfDevices;
                desc.deviceName = received;
                list.append(desc);
                numberOfDevices++;
            }
            sPort->close();
        }

    }



    device_descriptor devStr;
    foreach(devStr, list){
        ui->comboBox->addItem(devStr.deviceName + "(" + devStr.port+")");
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMenuSize(){
    if(ui->centralwidget->geometry().width()>150){
       ui->centralwidget->setMinimumSize(100,200);
       ui->centralwidget->setMaximumSize(100,20000);
    }else{
       ui->centralwidget->setMinimumSize(250,200);
       ui->centralwidget->setMaximumSize(250,20000);
    }
}

void MainWindow::openScope(){
    if(dockWidget_scope->isHidden()){
        dockWidget_scope->show();
    }else{
        dockWidget_scope->raise();
    }

}

