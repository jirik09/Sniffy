#include "moduledockwidget.h"

ModuleDockWidget::ModuleDockWidget(QWidget *parent, QString title) : QDockWidget(parent)
{
    setMinimumSize(QSize(250, 150));
    setAllowedAreas(Qt::RightDockWidgetArea);
    setObjectName(title);
    hide();

    setStyleSheet(QString::fromUtf8("QDockWidget{border: 3px solid gray; background-color: ")+QString::fromUtf8(COLOR_GREY)+";}");

    // ***************** create title bar and window contorl icons *************
    QWidget *titleBar = new QWidget();

    QHBoxLayout* titleBarLayout = new QHBoxLayout();
    titleBarLayout->setContentsMargins(5, 5, 5, 5);
    titleBar->setLayout(titleBarLayout);
    titleBarLayout->addWidget(new QLabel(title));

    titleBarLayout->addItem(new QSpacerItem(6, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));

    btnHold = new QPushButton();
    btnHold->setCheckable(true);
    btnHold->setChecked(false);
    btnHold->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btnHold->resize(38, 12);
    btnHold->setMaximumSize(36, 13);
    btnHold->setText("HOLD");
    btnHold->setStyleSheet(QString::fromUtf8("QPushButton{padding-top: 0px; padding-bottom: 1px; padding-left: 0px; padding-right: 0px; font-size: 10px;}"
                           "QPushButton{background-color: ")+BACKGROUND_COLOR_CONTROL+" color: "+COLOR_GREY+
                           "border-style: solid; border-color: "+COLOR_GREY+" border-width: 1px; border-radius: 1px;}"
                           "QPushButton:checked{background-color: "+BACKGROUND_COLOR_CONTROL+" color: "+COLOR_ORANGE+
                           "border-style: solid; border-color: "+COLOR_ORANGE+"; border-width: 1px; border-radius: 1px;}"
                           "QPushButton:hover{border-width: 2px;}");
    btnHold->hide();
    titleBarLayout->addWidget(btnHold);

    titleBarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));


    QPushButton *down = new QPushButton();
    down->resize(10,10);
    down->setStyleSheet(QString::fromUtf8("QPushButton{image: url(:/graphics/graphics/dock.png);}"
                                          "QPushButton:hover{background-color: ")+COLOR_HOVER+"}");
    titleBarLayout->addWidget(down);

    QPushButton *up = new QPushButton();
    up->resize(10,10);
    up->setStyleSheet(QString::fromUtf8("QPushButton{image: url(:/graphics/graphics/unDock.png);}"
                                        "QPushButton:hover{background-color: ")+COLOR_HOVER+"}");
    titleBarLayout->addWidget(up);

    QPushButton *exit = new QPushButton();
    exit->resize(10,10);
    exit->setStyleSheet(QString::fromUtf8("QPushButton{image: url(:/graphics/graphics/exit.png);}"
                                          "QPushButton:hover{background-color: rgb(120, 50, 50);}"));
    titleBarLayout->addWidget(exit);

    connect(up,SIGNAL(clicked()),this,SLOT(unDockOrMaximize()));
    connect(down,SIGNAL(clicked()),this,SLOT(dockOrMinimize()));
    connect(exit,SIGNAL(clicked()),this,SLOT(close()));
    connect(btnHold,SIGNAL(clicked()),this,SLOT(hold()));
    setTitleBarWidget(titleBar);
}

ModuleDockWidget::~ModuleDockWidget()
{
}

void ModuleDockWidget::showHoldButton(){
    btnHold->show();
}

void ModuleDockWidget::unDockOrMaximize(){
    if(isFloating() && !fullScreen){

        winFlags = windowFlags();
        winStyleSheet = styleSheet();
        winGeometry = geometry();
        fullScreen = true;
        showFullScreen();

    }else if(isFloating() && fullScreen){
        setFloating(false);
        //setGeometry(winGeometry);
        //setStyleSheet(winStyleSheet);
        // setWindowFlags(winFlags);
        fullScreen = false;

    }else if (!isFloating()){
        setFloating(true);
    }
}

void ModuleDockWidget::dockOrMinimize(){
    setFloating(false);
}

void ModuleDockWidget::close(){
    emit moduleWindowClosing();
    hide();
}

void ModuleDockWidget::hold(){
    emit holdClicked(btnHold->isChecked());
}
