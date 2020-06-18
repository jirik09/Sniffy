#include "scope.h"

Scope::Scope(QObject *parent) : QObject(parent)
{
    config = new ScopeConfig();
    cmd = new Commands();
}

void Scope::parseData(QByteArray data){
 //   qDebug() << "data are in scope parser" << data;

    QByteArray dataHeader = data.left(4);
   // QByteArray dataToPass = data.right(data.length()-4);

    if(dataHeader=="CFG_"){

        emit scopeSpecificationLoaded();
        scpModuleWidget->show();
        //todo pass specification into scopeSpec.cpp and parse it

    }else if(dataHeader=="SMPL"){

    }else if(dataHeader=="TRIG"){

    }else if(dataHeader=="OSC_"){
        qint8 tmpByte;
        qint16 tmpShort;
        int resolution;
        int length;
        int samplingFreq;
        int currentChannel=0;
        int numChannels=1;

        //data = data.right(data.length()-4);
        QDataStream streamBuffLeng(data);

        while (numChannels>currentChannel){


            streamBuffLeng>>samplingFreq; //thow away first 4 bytes because it contains "OSC_"

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
                qDebug() << "ERROR Too long signal to be alocated" << length;
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

            while(numChannels<scopeData.length()){ //remove signals which will not be filled
                scopeData.removeAt(scopeData.length()-1);
            }

            if(scopeData.length()>=currentChannel){
                scopeData.replace(currentChannel-1,points);
            }else{
                scopeData.append(points);
            }
        }


        if(currentChannel==numChannels){
            //emit scopeDataReceived(scopeData);
            scpWindow->dataReceived(scopeData,config->timeBase);

            if(config->triggerMode!=ScopeTriggerMode::TRIG_SINGLE){
                scopeNextData();
            }
        }
    }else{
        qDebug() << "ERROR such data cannot be parsed in Scope";
    }


}

void Scope::initDefault(){
    config->samplingRate = 10000;
    comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_100K+";");

    config->dataLength = 1000;
    comm->write(cmd->SCOPE+":"+cmd->DATA_LENGTH+":"+cmd->SAMPLES_1K+";");

    config->timeBase = 0.001;
    config->longMemory = 0;


    config->enabledChannels[0]=1;
    config->enabledChannels[1]=0;
    config->enabledChannels[2]=0;
    config->enabledChannels[3]=0;
    config->numberOfChannels=1;
    comm->write(cmd->SCOPE+":"+cmd->CHANNELS+":"+cmd->CHANNELS_1+";");

    //setTriggerLevel(50);
   // setPretrigger(50);

    config->triggerMode = ScopeTriggerMode::TRIG_AUTO;
    comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_MODE+":"+cmd->MODE_AUTO+";");

    config->triggerChannel = 1;
    comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_CHANNEL+":"+cmd->CHANNELS_1+";");

    config->triggerEdge = ScopeTriggerEdge::EDGE_RISING;
    comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_EDGE+":"+cmd->EDGE_RISING+";");
}

void Scope::setTimebase(float div){

    config->timeBase = div;

    config->samplingRate = float(config->dataLength)/(10.0*div);

    if(config->samplingRate<=1000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_1K+";");
    }else if(config->samplingRate<=2000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_2K+";");
    }else if(config->samplingRate<=5000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_5K+";");
    }else if(config->samplingRate<=10000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_10K+";");
    }else if(config->samplingRate<=20000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_20K+";");
    }else if(config->samplingRate<=50000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_50K+";");
    }else if(config->samplingRate<=100000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_100K+";");
    }else if(config->samplingRate<=200000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_200K+";");
    }else if(config->samplingRate<=500000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_500K+";");
    }else if(config->samplingRate<=1000000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_1M+";");
    }else if(config->samplingRate<=2000000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_2M+";");
    }
}

void Scope::setPretrigger(float percentage){
    config->pretriggerPercent=percentage;
    config->pretrigger = 65536*percentage/100;

    quint32 tmpTrig = config->pretrigger;
    QByteArray toSend;
    QDataStream out(&toSend, QIODevice::WriteOnly);
    out << cmd->SCOPE + ":" + cmd->SCOPE_PRETRIGGER + " " << tmpTrig << ";";
    comm->write(toSend);
}

void Scope::setTriggerLevel(float percentage){
    config->triggerLevelPercent=percentage;
    config->triggerLevel = 65536*percentage/100;

    comm->write(cmd->SCOPE, cmd->SCOPE_TRIG_LEVEL, config->triggerLevel);
}

void Scope::setNumberOfChannels(int num){
    switch (num){
    case 1:
        comm->write(cmd->SCOPE, cmd->CHANNELS, cmd->CHANNELS_1);
        break;
    case 2:
        comm->write(cmd->SCOPE+":"+cmd->CHANNELS+":"+cmd->CHANNELS_2+";");
        break;
    case 3:
        comm->write(cmd->SCOPE+":"+cmd->CHANNELS+":"+cmd->CHANNELS_3+";");
        break;
    case 4:
        comm->write(cmd->SCOPE+":"+cmd->CHANNELS+":"+cmd->CHANNELS_4+";");
        break;
    }
}



void Scope::channelEnableCallback(int buttonStatus){
    if(buttonStatus & 0x01){
        config->enabledChannels[0]=1;
        config->numberOfChannels = 1;
    }
    if(buttonStatus & 0x02){
        config->enabledChannels[1]=1;
        config->numberOfChannels = 2;
    }
    if(buttonStatus & 0x04){
        config->enabledChannels[2]=1;
        config->numberOfChannels = 3;
    }
    if(buttonStatus & 0x08){
        config->enabledChannels[3]=1;
        config->numberOfChannels = 4;
    }
    setNumberOfChannels(config->numberOfChannels);
}

void Scope::triggerModeCallback(int buttonIndex){
    if(buttonIndex==0){
        config->triggerMode=ScopeTriggerMode::TRIG_SINGLE;
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_MODE+":"+cmd->MODE_SINGLE+";");
    }
    if(buttonIndex==1){
        config->triggerMode=ScopeTriggerMode::TRIG_NORMAL;
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_MODE+":"+cmd->MODE_NORMAL+";");
    }
    if(buttonIndex==2){
        config->triggerMode=ScopeTriggerMode::TRIG_AUTO;
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_MODE+":"+cmd->MODE_AUTO+";");
    }
}

void Scope::triggerEdgeCallback(int buttonIndex){
    if(buttonIndex==0){
        config->triggerEdge=ScopeTriggerEdge::EDGE_RISING;
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_EDGE+":"+cmd->EDGE_RISING+";");
    }
    if(buttonIndex==1){
        config->triggerEdge=ScopeTriggerEdge::EDGE_FALLING;
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_EDGE+":"+cmd->EDGE_FALLING+";");
    }
}

void Scope::triggerChannelCallback(int index){
    if(index==0){
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_CHANNEL+":"+cmd->CHANNELS_1+";");
    }
    if(index==1){
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_CHANNEL+":"+cmd->CHANNELS_2+";");
    }
    if(index==2){
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_CHANNEL+":"+cmd->CHANNELS_3+";");
    }
    if(index==3){
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_CHANNEL+":"+cmd->CHANNELS_4+";");
    }
}

void Scope::scopeOpened(){
    //initDefault();
   // startScope();
}

void Scope::scopeClosed(){
    stopScope();
    scpModuleWidget->setStatus(ModuleStatus::STOP);

}


void Scope::stopScope(){
    comm->write(cmd->SCOPE+":"+cmd->STOP+";");
}
void Scope::startScope(){
    comm->write(cmd->SCOPE+":"+cmd->START+";");
}


void Scope::scopeNextData(){
    comm->write(cmd->SCOPE+":"+cmd->NEXT+";");
}

float Scope::getTimebase(){
    return config->timeBase;
}

ScopeTriggerMode Scope::getTriggerMode(){
    return config->triggerMode;
}

void Scope::setModuleWindow(WindowScope *scpWin){
    scpWindow = scpWin;

    //connect signals from scope to GUI --> can be actually done by function call


    //connect signals from GUI into scope
    connect(scpWin,SIGNAL(scopeWindowOpened()),this,SLOT(scopeOpened()));
    connect(scpWin,SIGNAL(scopeWindowClosed()),this,SLOT(scopeClosed()));
    connect(scpWin,SIGNAL(timeBaseChanged(float)),this,SLOT(setTimebase(float)));
}

void Scope::setModuleWidget(WidgetModule *scpWidget){
    scpModuleWidget = scpWidget;
    connect(scpModuleWidget,SIGNAL(clicked(ModuleStatus)),this,SLOT(widgetClicked(ModuleStatus)));
}

void Scope::widgetClicked(ModuleStatus status){
    if(status == ModuleStatus::STOP){
        initDefault();
        startScope();
    }
}



void Scope::setComms(Comms *communication){
    comm = communication;
    comm->write(cmd->SCOPE+":"+cmd->CONFIG_REQUEST+";");
}
