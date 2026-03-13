/*
Widget to be shown on the left selection in centralWidget
- This widget controls the functions / features to be anabled. It acts as a button
- Can have icon on the left and text desription
- On the right side is shown stauts of the feature (Play, Stop, Idle, Error)
*/
#include "widgetcontrolmodule.h"
#include "ui_widgetmodule.h"
#include "stylehelper.h"

// ── SpinnerOverlay ──────────────────────────────────────────

SpinnerOverlay::SpinnerOverlay(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFixedSize(parent->minimumSize());
    connect(&timer, &QTimer::timeout, this, [this](){
        angle = (angle - 10 + 360) % 360;
        update();
    });
    hide();
}

void SpinnerOverlay::start() {
    angle = 0;
    show();
    timer.start(33);
}

void SpinnerOverlay::stop() {
    timer.stop();
    hide();
}

void SpinnerOverlay::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor color(Graphics::palette().moduleActiveText);
    QColor transparent = color;
    transparent.setAlpha(0);

    qreal pw = 3.25;
    QRectF r(pw, pw, width() - 2 * pw, height() - 2 * pw);

    QConicalGradient grad(r.center(), angle);
    grad.setColorAt(0.0, color);
    grad.setColorAt(0.8, transparent);
    grad.setColorAt(1.0, transparent);

    p.setPen(QPen(QBrush(grad), pw, Qt::SolidLine, Qt::RoundCap));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(r);
}

// ── WidgetControlModule ─────────────────────────────────────

WidgetControlModule::WidgetControlModule(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetFeature)
{
    ui->setupUi(this);

    if (Graphics::palette().isEmberTheme) {
        setMinimumHeight(40);
        setMaximumHeight(40);
        layout()->setContentsMargins(4, 2, 4, 2);
    }

    ui->pushButton_name->setStyleSheet(Graphics::palette().styleModuleButton);
    ui->pushButton_name->setText(name);

    ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::tintedPath(Graphics::getCommonPath()+"status_stop.png")+")");
    connect(ui->pushButton_name,SIGNAL(clicked()),this,SLOT(clickedInternal()));

    status = ModuleStatus::STOP;
}

WidgetControlModule::~WidgetControlModule()
{
    delete ui;
}

void WidgetControlModule::setStatus(ModuleStatus stat){
    // Stop spinner when leaving WAIT_EVENT
    if(stat != ModuleStatus::WAIT_EVENT && spinner) spinner->stop();

    if(stat==ModuleStatus::PLAY){
        ui->pushButton_name->setChecked(true);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getCommonPath()+"status_play.png)");
    }
    if(stat==ModuleStatus::STOP){        
        ui->pushButton_name->setEnabled(true);
        ui->pushButton_name->setChecked(false);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::tintedPath(Graphics::getCommonPath()+"status_stop.png")+")");        
    }
    if(stat==ModuleStatus::HIDDEN_PLAY){
        ui->pushButton_name->setChecked(true);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getCommonPath()+"status_play_hidden.png)");        
    }
    if(stat==ModuleStatus::PAUSE){
        ui->pushButton_name->setChecked(true);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getCommonPath()+"status_pause.png)");
    }
    if(stat==ModuleStatus::HIDDEN_PAUSE){
        ui->pushButton_name->setChecked(true);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getCommonPath()+"status_pause_hidden.png)");
    }
    if(stat==ModuleStatus::LOCKED){
        ui->pushButton_name->setEnabled(false);
        QColor lockTint = Graphics::palette().moduleDisabledTint.isEmpty()
                            ? QColor(Graphics::palette().componentDisabled)
                            : QColor(Graphics::palette().moduleDisabledTint);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::tintedPath(Graphics::getCommonPath()+"status_locked.png", lockTint)+")");
    }
    if(stat==ModuleStatus::WAIT_EVENT){
        ui->pushButton_name->setChecked(true);
        ui->widget_status->setStyleSheet("margin: 2px;image: none;");
        if(!spinner){
            spinner = new SpinnerOverlay(ui->widget_status);
        }
        spinner->start();
    }
    if(stat==ModuleStatus::HIDDEN_WAIT_EVENT){
        ui->pushButton_name->setChecked(true);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getCommonPath()+"status_play_hidden.png)");
    }
    status = stat;
    updateIcon();
}

ModuleStatus WidgetControlModule::getStatus(void){
    return status;
}

void WidgetControlModule::hideStatus(){
    ui->widget_status->hide();
}

void WidgetControlModule::mousePressEvent(QMouseEvent *event){
    emit mouseWheelPressEvent(event);
}

void WidgetControlModule::clickedInternal(){
    emit clicked(status);
}

void WidgetControlModule::setIcon (QString ImageURI){
    iconURI = ImageURI;
    updateIcon();
}

void WidgetControlModule::updateIcon(){
    if(iconURI.isEmpty()) return;

    bool isActive = (status == ModuleStatus::PLAY  ||
                     status == ModuleStatus::PAUSE ||
                     status == ModuleStatus::HIDDEN_PLAY ||
                     status == ModuleStatus::HIDDEN_PAUSE ||
                     status == ModuleStatus::WAIT_EVENT ||
                     status == ModuleStatus::HIDDEN_WAIT_EVENT);

    QColor normalColor = isActive ? QColor(Graphics::palette().moduleActiveText)
                                  : QColor(Graphics::palette().textAll);
    QColor disabledColor = Graphics::palette().moduleDisabledTint.isEmpty()
                             ? QColor(Graphics::palette().componentDisabled)
                             : QColor(Graphics::palette().moduleDisabledTint);

    // Hover color: active modules keep moduleActiveText; inactive modules
    // use moduleHoverText if the theme defines it, otherwise stay at normalColor.
    QColor hoverColor = isActive ? QColor(Graphics::palette().moduleActiveText)
                                 : (Graphics::palette().moduleHoverText.isEmpty()
                                        ? normalColor
                                        : QColor(Graphics::palette().moduleHoverText));

    icon = QIcon();
    icon.addPixmap(Graphics::tintedPixmap(iconURI, normalColor), QIcon::Normal);
    icon.addPixmap(Graphics::tintedPixmap(iconURI, disabledColor), QIcon::Disabled);
    icon.addPixmap(Graphics::tintedPixmap(iconURI, hoverColor), QIcon::Active);
    ui->pushButton_name->setIcon(icon);

    if (isActive) {
        StyleHelper::applyGlowEffect(ui->pushButton_name, normalColor.name(), StyleHelper::GLOW_BLUR_RADIUS_CONTROL_MODULE);
    } else {
        if (ui->pushButton_name->graphicsEffect()) {
            ui->pushButton_name->setGraphicsEffect(nullptr);
        }
    }
}

void WidgetControlModule::setName (QString name){
    ui->pushButton_name->setText(name);
}


