#include "labelformator.h"

LabelFormator::LabelFormator(QObject *parent) : QObject(parent)
{

}

QString LabelFormator::formatOutout(qreal value, QString unit){
    QString str;

    if (value == std::numeric_limits<qreal>::infinity()){
        str = "N/A " + unit;
    }else if (abs(value)>=100000000000){
        str=QString::number(value/1000000000,'f',0) + " G"+unit;
    }else if (abs(value)>=10000000000){
        str=QString::number(value/1000000000,'f',1) + " G"+unit;
    }else if (abs(value)>=1000000000){
        str=QString::number(value/1000000000,'f',2) + " G"+unit;
    }else if (abs(value)>=100000000){
        str=QString::number(value/1000000,'f',0) + " M"+unit;
    }else if (abs(value)>=10000000){
        str=QString::number(value/1000000,'f',1) + " M"+unit;
    }else if (abs(value)>=1000000){
        str=QString::number(value/1000000,'f',2) + " M"+unit;
    }else if (abs(value)>=100000){
        str=QString::number(value/1000,'f',0) + " k"+unit;
    }else if (abs(value)>=10000){
        str=QString::number(value/1000,'f',1) + " k"+unit;
    }else if (abs(value)>=1000){
        str=QString::number(value/1000,'f',2) + " k"+unit;
    }else if (abs(value)>=100){
        str=QString::number(value,'f',0) + " "+unit;
    }else if (abs(value)>=10){
        str=QString::number(value,'f',1) + " "+unit;
    }else if (abs(value)>=1){
        str=QString::number(value,'f',2) + " "+unit;
    }else if (abs(value)>=0.1){
        str=QString::number(value*1000,'f',0) + " m"+unit;
    }else if (abs(value)>=0.01){
        str=QString::number(value*1000,'f',1) + " m"+unit;
    }else if (abs(value)>=0.001){
        str=QString::number(value*1000,'f',2) + " m"+unit;
    }else if (abs(value)>=0.0001){
        str=QString::number(value*1000000,'f',0) + " u"+unit;
    }else if (abs(value)>=0.0000099){
        str=QString::number(value*1000000,'f',1) + " u"+unit;
    }else if (abs(value)>=0.00000099){
        str=QString::number(value*1000000,'f',2) + " u"+unit;
    }else if (abs(value)>=0.000000099){
        str=QString::number(value*1000000000,'f',0) + " n"+unit;
    }else if (abs(value)>=0.0000000099){
        str=QString::number(value*1000000000,'f',1) + " n"+unit;
    }else if (abs(value)>=0.00000000099){
        str=QString::number(value*1000000000,'f',2) + " n"+unit;
    }else {
        str = "0.00 " + unit;
    }
    return str;
}
