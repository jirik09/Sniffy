/*
Widget with combobox.
- Optional label on the laef ant comboBox on the right
- Options has to be added by addOption (params shownValue(QString) and realValue(float))
*/
#include "widgetselection.h"
#include "ui_widgetselection.h"
#include "../diagnostics.h"
#include "../statehelper.h"

WidgetSelection::WidgetSelection(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetSelection)
{
    ui->setupUi(this);
    ui->label->setText(name);
    setStyleSheet(Graphics::STYLE_COMBO_BOX);
    // options list starts empty
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));
}

WidgetSelection::~WidgetSelection()
{
    delete ui;
}

QByteArray WidgetSelection::saveGeometry()
{
    return StateHelper::pack(getSelectedIndex());
}

void WidgetSelection::restoreGeometry(QByteArray geom)
{
    int idx=0; bool ok = StateHelper::unpack(geom, idx);
    if(ok && idx >=0 && idx < options.size()) {
        setSelected(idx,true);
    } else {
        SNIFFY_LOG_INFO("WidgetSelection restoreGeometry ignored invalid index:" << geom);
    }
}

void WidgetSelection::addOption (QString shownValue,float realValue){
    params_sel par { shownValue, realValue };
    bool first = options.isEmpty();
    options.append(par);
    ui->comboBox->addItem(shownValue);
    if(first){
        setSelected(0,true); // silent initial select
    }
}

void WidgetSelection::setSelected(int index, bool silent){
    if(index < 0 || index >= ui->comboBox->count()){
        SNIFFY_LOG_WARN("setSelected invalid index:" << index);
        return;
    }
    if(silent)
        disconnect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));
    ui->comboBox->setCurrentIndex(index);
    selectedIndex = index;
    if(silent)
        connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));
}

int WidgetSelection::getSelectedIndex(){
    return ui->comboBox->currentIndex();
}

qreal WidgetSelection::getSelectedValue()
{
    int idx = getSelectedIndex();
    if(idx < 0 || idx >= options.size()) return 0;
    return options.at(idx).realValue;
}

int WidgetSelection::count(){
    return ui->comboBox->count();
}

void WidgetSelection::indexChanged(int index)
{
    if(index >= 0 && index < options.size())
        emit selectedIndexChanged(index, options.at(index).realValue);
    else
        SNIFFY_LOG_WARN("indexChanged received invalid index:" << index);
}

void WidgetSelection::clear(){
    ui->comboBox->clear();
    options.clear();
}
