#include "moduledockwidget.h"

ModuleDockWidget::ModuleDockWidget(QWidget *parent, QString title) : QDockWidget(parent)
{
    setMinimumSize(QSize(250, 150));
    setAllowedAreas(Qt::RightDockWidgetArea);
    setObjectName(title);
    hide();

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
    btnHold->setStyleSheet(Graphics::STYLE_HOLD_BUTTON);
    btnHold->hide();
    titleBarLayout->addWidget(btnHold);
    titleBarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QPushButton *down = new QPushButton();
    down->resize(10,10);
    down->setStyleSheet(Graphics::STYLE_CONTROL_BUTTON+"QPushButton{image: url("+Graphics::getGraphicsPath()+"dock.png);}"
                                                       "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}");
    titleBarLayout->addWidget(down);

    QPushButton *up = new QPushButton();
    up->resize(10,10);
    up->setStyleSheet(Graphics::STYLE_CONTROL_BUTTON+"QPushButton{image: url("+Graphics::getGraphicsPath()+"unDock.png);}"
                                                     "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}");
    titleBarLayout->addWidget(up);

    QPushButton *exit = new QPushButton();
    exit->resize(10,10);
    exit->setStyleSheet(Graphics::STYLE_CONTROL_BUTTON+"QPushButton{image: url("+Graphics::getGraphicsPath()+"exit.png);}"
                                                       "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_EXIT_HOVER+";}");
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
