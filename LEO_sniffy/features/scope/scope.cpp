#include "scope.h"

Scope::Scope(QObject *parent) : QObject(parent)
{

}

void Scope::parseData(QByteArray data){
    qDebug() << "data are in scope parser" << data;

    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.right(data.length()-4);

    if(dataHeader=="CFG_"){

        emit scopeSpecified();
    }else if(dataHeader=="SMPL"){

    }else if(dataHeader=="TRIG"){

    }else if(dataHeader=="OSC_"){
        qint8 tmpByte;
        qint16 tmpShort;
        int resolution;
        int length;
        int samplingFreq;
        int currentChannel;
        int numChannels;

        data = data.right(data.length()-4);


        QDataStream streamBuffLeng(data);
        streamBuffLeng>>samplingFreq;

        streamBuffLeng>>tmpByte;
        resolution = tmpByte;

        streamBuffLeng>>length;

        streamBuffLeng>>tmpByte;
        streamBuffLeng>>tmpByte;
        currentChannel = tmpByte;

        streamBuffLeng>>tmpByte;
        numChannels = tmpByte;



        QVector<QPointF> points;
        if(length<500000){
            points.reserve(length);
        }else{
            qDebug() << "Too long signal to be alocated" << length;
        }

        if(resolution>8){
            for (int j(0); j < length/2; j++){
                streamBuffLeng>>tmpByte;
                tmpShort = tmpByte;
                streamBuffLeng>>tmpByte;
                tmpShort += tmpByte*256;
                qreal x(0);
                qreal y(0);
                x=j*1.0/samplingFreq;
                y=tmpShort*3.3/4095.0;
                points.append(QPointF(x,y));
            }
        }else{
            qDebug() << "TODO <8bit data parser";

        }

        if(scopeData.length()>=currentChannel){
            scopeData.replace(currentChannel-1,points);
        }else{
            scopeData.append(points);
        }


        if(currentChannel==numChannels){
            emit scopeDataReceived(scopeData);

        }





    }else{
        qDebug() << "ERROR such data cannot be parsed in Scope";
    }


}

void Scope::stopScope(){
    sendData("OSCP:STOP;");
}
void Scope::startScope(){
    sendData("OSCP:STRT;");
}


void Scope::restartScope(){
    sendData("OSCP:NEXT;");
}


void Scope::sendData(QByteArray data){
    emit send(data);
}
