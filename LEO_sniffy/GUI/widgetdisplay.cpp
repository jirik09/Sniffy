#include "widgetdisplay.h"
#include "ui_widgetdisplay.h"

WidgetDisplay::WidgetDisplay(QString firstLabelText, QString &unitsStyleSheet, bool showPrgrssBar, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetDisplay)
{
    ui->setupUi(this);

    palette = ui->lcdNumber_avg->palette();

    labelList.append(ui->label_0);
    labelList.append(ui->label_1);
    labelList.append(ui->label_2);
    labelList.append(ui->label_3);
    labelList.append(ui->label_4);

    foreach(QLabel* label, labelList)
        label->hide();

    ui->label_0->setText(firstLabelText);
    ui->label_0->show();

    setUnitsStyle(unitsStyleSheet);

    if(!showPrgrssBar)
        showBarDisplay(false);

    displayString("");
    displayAvgString("");
}

void WidgetDisplay::setUnitsStyle(QString &unitsStyleSheet){
    ui->styleUnits->setStyleSheet(unitsStyleSheet);
}

void WidgetDisplay::setAvgStyle(QString &avgStyleSheet){
    ui->styleAvg->setStyleSheet(avgStyleSheet);
}

void WidgetDisplay::setErrStyle(QString &errStyleSheet){
    ui->styleErr->setStyleSheet(errStyleSheet);
}

void WidgetDisplay::setQerrStyle(QString &qerrStyleSheet){
    ui->styleQerr->setStyleSheet(qerrStyleSheet);
}

void WidgetDisplay::setTerrStyle(QString &terrStyleSheet){
    ui->styleTerr->setStyleSheet(terrStyleSheet);
}

void WidgetDisplay::setBarStyle(QString &barStyleSheet){
    ui->progressBar->setStyleSheet(barStyleSheet);
}

void WidgetDisplay::showQerrStyle(bool visible){
    (visible) ? ui->styleQerr->show() : ui->styleQerr->hide();
}

void WidgetDisplay::showTerrStyle(bool visible){
    (visible) ? ui->styleTerr->show() : ui->styleQerr->hide();
}

void WidgetDisplay::showQerrTerrStyle(bool visible){
    if(visible){
        ui->styleQerr->show();
        ui->styleTerr->show();
    }else {
        ui->styleQerr->hide();
        ui->styleTerr->hide();
    }
}

void WidgetDisplay::showAvgDisplay(bool visible){
    (visible) ? ui->horizontalWidget_avg->show() : ui->horizontalWidget_avg->hide();
}

void WidgetDisplay::showErrDisplay(bool visible){
    (visible) ? ui->horizontalWidget_err->show() : ui->horizontalWidget_err->hide();
}

void WidgetDisplay::showBarDisplay(bool visible){
    (visible) ? ui->horizontalWidget_bar->show() : ui->horizontalWidget_bar->hide();
}

void WidgetDisplay::displayNumber(double number){
    ui->lcdNumber->display(number);
}

void WidgetDisplay::displayString(const QString &string){
    ui->lcdNumber->display(string);
}

void WidgetDisplay::displayAvgNumber(double number){
    ui->lcdNumber_avg->display(number);
}

void WidgetDisplay::displayAvgString(const QString &string){
    ui->lcdNumber_avg->display(string);
}

void WidgetDisplay::displayQerrNumber(double number){
    ui->lcdNumber_qerr->display(number);
}

void WidgetDisplay::displayQerrString(const QString &string){
    ui->lcdNumber_qerr->display(string);
}

void WidgetDisplay::displayTerrNumber(double number){
    ui->lcdNumber_terr->display(number);
}

void WidgetDisplay::displayTerrString(const QString &string){
    ui->lcdNumber_terr->display(string);
}

void WidgetDisplay::changeAvgColor(QColor color){
    palette.setColor(QPalette::WindowText, color);
    ui->lcdNumber_avg->setPalette(palette);
}

void WidgetDisplay::showProgressBar(bool visible){
    (visible) ? ui->progressBar->show() : ui->progressBar->hide();
}

void WidgetDisplay::setProgressBarRange(int min, int max){
    ui->progressBar->setRange(min, max);
}

void WidgetDisplay::updateProgressBar(int value){
    ui->progressBar->setValue(value);
    //  ui->progressBar->repaint();
    //  if progress bar not left with its default style
    //  the chunks are around 5% instead of at least 1%
}

void WidgetDisplay::configLabel(int labelNumber, QString text, QString colorStyle, bool isVisible){
    labelList.at(labelNumber)->setText(text);
    labelList.at(labelNumber)->setStyleSheet(colorStyle);
    isVisible ? labelList.at(labelNumber)->show() : labelList.at(labelNumber)->hide();
}

QString WidgetDisplay::getLabelText(int labelNumber){
    return labelList.at(labelNumber)->text();
}

void WidgetDisplay::setLabelText(int labelNumber, const QString text){
    labelList.at(labelNumber)->setText(text);
    labelList.at(labelNumber)->show();
}

void WidgetDisplay::setLabelColor(int labelNumber, const QString &textStyleSheet){
    labelList.at(labelNumber)->setStyleSheet(textStyleSheet);
}

void WidgetDisplay::hideLabel(int labelNumber){
    labelList.at(labelNumber)->hide();
}

void WidgetDisplay::showLabel(int labelNumber){
    labelList.at(labelNumber)->show();
}

void WidgetDisplay::drawIndicationFlag(int labelNumber, QString color){
    if(color == "blue"){
        if(drawFlag == 0){
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(24,154,224);color:rgb(24,154,224);">--<span style="background-color:rgb(29,115,162);color:rgb(29,115,162);">--<span style="background-color:rgb(33,77,100);color:rgb(33,77,100);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 1) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(29,115,162);color:rgb(29,115,162);">--<span style="background-color:rgb(24,154,224);color:rgb(24,154,224);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 2) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(33,77,100);color:rgb(33,77,100);">--<span style="background-color:rgb(29,115,162);color:rgb(29,115,162);">--<span style="background-color:rgb(24,154,224);color:rgb(24,154,224);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 3) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(33,77,100);color:rgb(33,77,100);">--<span style="background-color:rgb(29,115,162);color:rgb(29,115,162);">--<span style="background-color:rgb(24,154,224);color:rgb(24,154,224);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 4) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(33,77,100);color:rgb(33,77,100);">--<span style="background-color:rgb(29,115,162);color:rgb(29,115,162);">--<span style="background-color:rgb(24,154,224);color:rgb(24,154,224);">--)**");
            drawFlag++;
        }else if (drawFlag == 5) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(24,154,224);color:rgb(24,154,224);">--<span style="background-color:rgb(29,115,162);color:rgb(29,115,162);">--)**");
            drawFlag++;
        }else if (drawFlag == 6) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(24,154,224);color:rgb(24,154,224);">--<span style="background-color:rgb(29,115,162);color:rgb(29,115,162);">--<span style="background-color:rgb(33,77,100);color:rgb(33,77,100);">--)**");
            drawFlag++;
        }else if (drawFlag == 7) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(24,154,224);color:rgb(24,154,224);">--<span style="background-color:rgb(29,115,162);color:rgb(29,115,162);">--<span style="background-color:rgb(33,77,100);color:rgb(33,77,100);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag = 0;
        }
    }else if (color == "grey") {
        if(drawFlag == 0){
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(124,124,124);color:rgb(124,124,124);">--<span style="background-color:rgb(95,95,95);color:rgb(95,95,95);">--<span style="background-color:rgb(67,67,67);color:rgb(67,67,67);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 1) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(95,95,95);color:rgb(95,95,95);">--<span style="background-color:rgb(124,124,124);color:rgb(124,124,124);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 2) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(67,67,67);color:rgb(67,67,67);">--<span style="background-color:rgb(95,95,95);color:rgb(95,95,95);">--<span style="background-color:rgb(124,124,124);color:rgb(124,124,124);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 3) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(67,67,67);color:rgb(67,67,67);">--<span style="background-color:rgb(95,95,95);color:rgb(95,95,95);">--<span style="background-color:rgb(124,124,124);color:rgb(124,124,124);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 4) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(67,67,67);color:rgb(67,67,67);">--<span style="background-color:rgb(95,95,95);color:rgb(95,95,95);">--<span style="background-color:rgb(124,124,124);color:rgb(124,124,124);">--)**");
            drawFlag++;
        }else if (drawFlag == 5) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(124,124,124);color:rgb(124,124,124);">--<span style="background-color:rgb(95,95,95);color:rgb(95,95,95);">--)**");
            drawFlag++;
        }else if (drawFlag == 6) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(124,124,124);color:rgb(124,124,124);">--<span style="background-color:rgb(95,95,95);color:rgb(95,95,95);">--<span style="background-color:rgb(67,67,67);color:rgb(67,67,67);">--)**");
            drawFlag++;
        }else if (drawFlag == 7) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(124,124,124);color:rgb(124,124,124);">--<span style="background-color:rgb(95,95,95);color:rgb(95,95,95);">--<span style="background-color:rgb(67,67,67);color:rgb(67,67,67);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag = 0;
        }
    }else if (color == "green") {
        if(drawFlag == 0){
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(54,154,24);color:rgb(54,154,24);">--<span style="background-color:rgb(49,115,29);color:rgb(49,115,29);">--<span style="background-color:rgb(43,77,33);color:rgb(43,77,33);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 1) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(49,115,29);color:rgb(49,115,29);">--<span style="background-color:rgb(54,154,24);color:rgb(54,154,24);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 2) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(43,77,33);color:rgb(43,77,33);">--<span style="background-color:rgb(49,115,29);color:rgb(49,115,29);">--<span style="background-color:rgb(54,154,24);color:rgb(54,154,24);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 3) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(43,77,33);color:rgb(43,77,33);">--<span style="background-color:rgb(49,115,29);color:rgb(49,115,29);">--<span style="background-color:rgb(54,154,24);color:rgb(54,154,24);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag++;
        }else if (drawFlag == 4) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(43,77,33);color:rgb(43,77,33);">--<span style="background-color:rgb(49,115,29);color:rgb(49,115,29);">--<span style="background-color:rgb(54,154,24);color:rgb(54,154,24);">--)**");
            drawFlag++;
        }else if (drawFlag == 5) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(54,154,24);color:rgb(54,154,24);">--<span style="background-color:rgb(49,115,29);color:rgb(49,115,29);">--)**");
            drawFlag++;
        }else if (drawFlag == 6) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(54,154,24);color:rgb(54,154,24);">--<span style="background-color:rgb(49,115,29);color:rgb(49,115,29);">--<span style="background-color:rgb(43,77,33);color:rgb(43,77,33);">--)**");
            drawFlag++;
        }else if (drawFlag == 7) {
            labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(54,154,24);color:rgb(54,154,24);">--<span style="background-color:rgb(49,115,29);color:rgb(49,115,29);">--<span style="background-color:rgb(43,77,33);color:rgb(43,77,33);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
            drawFlag = 0;
        }
    }else if (color == "clear") {
        labelList.at(labelNumber)->setText(R"**(<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--<span style="background-color:rgb(38, 38, 38);color:rgb(38, 38, 38);">--)**");
    }

    /* BLUE   - rgb(24,154,224)
     * Darker - rgb(29,115,162)
     * Dark   - rgb(33,77,100)
     * */

    /* GREY   - rgb(124,124,124)
     * Darker - rgb(49,115,29)
     * Dark   - rgb(43,77,33)
     * */

    /* GREEN  - rgb(54,154,24)
     * Darker - rgb(95,95,95)
     * Dark   - rgb(67,67,67)
     * */
}

WidgetDisplay::~WidgetDisplay()
{
    delete ui;
}
