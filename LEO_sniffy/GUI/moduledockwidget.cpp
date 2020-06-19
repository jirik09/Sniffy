#include "moduledockwidget.h"

ModuleDockWidget::ModuleDockWidget(QWidget *parent, QString title) : QDockWidget(parent)
{
    setMinimumSize(QSize(250, 150));
    setAllowedAreas(Qt::RightDockWidgetArea);
    hide();

    setStyleSheet(QString::fromUtf8("QDockWidget{border: 3px solid gray; background-color: rgb(108, 108, 108);}"));

    // ***************** create title bar and window contorl icons *************
    QWidget *titleBar = new QWidget();

    QHBoxLayout* titleBarLayout = new QHBoxLayout();
    titleBarLayout->setContentsMargins(5, 5, 5, 5);
    titleBar->setLayout(titleBarLayout);
    titleBarLayout->addWidget(new QLabel(title));
    titleBarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));


    QPushButton *down = new QPushButton();
    down->resize(10,10);
    down->setStyleSheet(QString::fromUtf8("QPushButton{image: url(:/graphics/graphics/dock.png);}QPushButton:hover{background-color: rgb(71, 76, 94);}"));
    titleBarLayout->addWidget(down);
    QPushButton *up = new QPushButton();
    up->resize(10,10);
    up->setStyleSheet(QString::fromUtf8("QPushButton{image: url(:/graphics/graphics/unDock.png);}QPushButton:hover{background-color: rgb(71, 76, 94);}"));
    titleBarLayout->addWidget(up);
    QPushButton *exit = new QPushButton();
    exit->resize(10,10);
    exit->setStyleSheet(QString::fromUtf8("QPushButton{image: url(:/graphics/graphics/exit.png);}QPushButton:hover{background-color: rgb(120, 50, 50);}"));
    titleBarLayout->addWidget(exit);

    connect(up,SIGNAL(clicked()),this,SLOT(unDockOrMaximize()));
    connect(down,SIGNAL(clicked()),this,SLOT(dockOrMinimize()));
    connect(exit,SIGNAL(clicked()),this,SLOT(close()));
    setTitleBarWidget(titleBar);
}

ModuleDockWidget::~ModuleDockWidget()
{
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
