#include "moduledockwidget.h"

ModuleDockWidget::ModuleDockWidget(QWidget *parent, QString title) : QDockWidget(parent)
{
    // Keep minimum width modest to avoid squeezing the left menu
    setMinimumSize(QSize(220, 130));
    setAllowedAreas(Qt::RightDockWidgetArea);
    setObjectName(title);
    hide();
    setStyleSheet(Graphics::palette().styleDockWidget);

    // ***************** create title bar and window contorl icons *************
    QWidget *titleBar = new QWidget();

    QHBoxLayout* titleBarLayout = new QHBoxLayout();
    titleBarLayout->setContentsMargins(5,0,5,0);
    titleBar->setLayout(titleBarLayout);

    titleBar->setStyleSheet(Graphics::palette().styleDockWindow);
    titleBarLayout->addWidget(new QLabel(title));

    titleBarLayout->addItem(new QSpacerItem(6, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));

    btnHold = new QPushButton();
    btnHold->setCheckable(true);
    btnHold->setChecked(false);
    btnHold->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btnHold->resize(38, 12);
    btnHold->setMaximumSize(36, 13);
    btnHold->setText("HOLD");
    btnHold->setStyleSheet(Graphics::palette().styleHoldButton);
    btnHold->hide();
    titleBarLayout->addWidget(btnHold);
    titleBarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));


    QPushButton *down = new QPushButton();
    down->resize(10,10);
    down->setContentsMargins(5,0,5,0);
    down->setStyleSheet(Graphics::palette().styleControlButton+"QPushButton:hover{background-color:"+Graphics::palette().windowControlHover+";}");
    down->setIcon(QIcon(Graphics::getGraphicsPath()+"dock.png"));
    titleBarLayout->addWidget(down);

    QPushButton *up = new QPushButton();
    up->resize(10,10);
    up->setContentsMargins(5,0,5,0);
    up->setStyleSheet(Graphics::palette().styleControlButton+"QPushButton:hover{background-color:"+Graphics::palette().windowControlHover+";}");
    up->setIcon(QIcon(Graphics::getGraphicsPath()+"unDock.png"));
    titleBarLayout->addWidget(up);

    QPushButton *exit = new QPushButton();
    exit->resize(10,10);
    exit->setContentsMargins(5,0,5,0);
    exit->setStyleSheet(Graphics::palette().styleControlButton+"QPushButton:hover{background-color:"+Graphics::palette().windowExitHover+";}");
    exit->setIcon(QIcon(Graphics::getGraphicsPath()+"exit.png"));
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

void ModuleDockWidget::showHoldButton(bool show){
    (show) ? btnHold->show() : btnHold->hide();
}

void ModuleDockWidget::enableHoldButton(bool enable){
    btnHold->setChecked(enable);
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
    if(isFloating()){
        setFloating(false);
    }else{
        emit hideModuleWindowClicked();
    }
}

void ModuleDockWidget::close(){
    emit moduleWindowClosing();
    hide();
}

void ModuleDockWidget::hold(){
    emit holdClicked(btnHold->isChecked());
}
