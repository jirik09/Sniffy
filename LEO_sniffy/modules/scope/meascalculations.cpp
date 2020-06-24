#include "meascalculations.h"

MeasCalculations::MeasCalculations(QObject *parent): QThread(parent)
{
    restart = false;
    abort = false;
}

MeasCalculations::~MeasCalculations(){
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void MeasCalculations::calculate(QVector<QVector<QPointF>> scopeData, QList<Measurement*> scopeMeas, qint32 samplingFreq){
    QMutexLocker locker(&mutex);

    data = scopeData;
    meas = scopeMeas;
    this->samplingFreq = samplingFreq;
    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}

void MeasCalculations::run()
{
    forever {
        mutex.lock();
        QVector<QVector<QPointF>> data = this->data;
        QList<Measurement *> meas = this->meas;
        qint32 samplingFreq = this->samplingFreq;
        for (int i = 0;i<8;i++) {
            isVoltCalculated[i] = 0;
            isTimeCalculated[i] = 0;
        }
        mutex.unlock();



        foreach(Measurement* m, meas){
            qint8 channelIndex = m->getChannel()-1;
            switch (m->getType()) {
            case MeasurementType::LOW:
                calculateTime(data[channelIndex],channelIndex,samplingFreq);
                m->setLabel("Duty Low");
                m->setValue(formatOutout(1-High[channelIndex],"%"));
                break;
            case MeasurementType::HIGH:
                break;
            case MeasurementType::MIN:
                break;
            case MeasurementType::MAX:
                break;
            case MeasurementType::DUTY:
                break;
            case MeasurementType::RMS:
                break;
            case MeasurementType::MEAN:
                break;
            case MeasurementType::PKPK:
                calculateVolt(data[channelIndex],channelIndex);
                m->setLabel("Pk Pk");
                m->setValue(formatOutout(Max[channelIndex] - Min[channelIndex] ,"V"));
                qDebug () << "pkpk vaue calculated";
                break;
            case MeasurementType::PHASE:
                break;
            case MeasurementType::PERIOD:
                break;
            case MeasurementType::FREQUENCY:
                break;
            }
        }

        if (restart)
            break;
        if (abort)
            return;

        if (!restart)
            emit measCalculated(meas);

        mutex.lock();
        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
    }
}

void MeasCalculations::calculateVolt(QVector<QPointF> data, qint32 ch){
    RMS[ch] = 0;
    Mean[ch] = 0;
    Max[ch] = std::numeric_limits<qreal>::min();
    Min[ch] = std::numeric_limits<qreal>::max();;
    foreach (QPointF pt, data){

        RMS[ch] += qPow(pt.y(),2);
        Mean[ch] += pt.y();
        if(pt.y()>Max[ch])
            Max[ch]= pt.y();
        if(pt.y()<Min[ch])
            Min[ch]= pt.y();
    }
    RMS[ch] = qSqrt(RMS[ch] / data.length());
    Mean[ch] = (Mean[ch] / data.length());
    isVoltCalculated[ch] = true;
}


void MeasCalculations::calculateTime(QVector<QPointF> data, qint32 ch, qint32 sampling){
    if(!isVoltCalculated[ch]){
        calculateVolt(data,ch);
    }


    if(!isTimeCalculated[ch]){
        double center = ((double)(Max[ch]) + Min[ch]) / 2;
        //double center = Mean[ch] / scale;

        int state = 0;
        int up = 0;
        int down = 0;
        bool below = false;
        int periods = 0;
        double frq = 0;
        qreal LP = data[0].y();
        int totalUp = 0;
        int totalDown = 0;
        int zerocrossindex = 0;

        if (data[0].y() < center) {
            below = true;
        }

        for (int i = 3; i < data.length()-4; i++){
            //LP = (samples[ch, i] + samples[ch, i + 1] + samples[ch, i - 1] + samples[ch, i + 2] + samples[ch, i - 2] + samples[ch, i + 3] + samples[ch, i - 3] + samples[ch, i +4]) / 8;
            LP = 0.9 * LP + 0.1 * data[0].y();
            if (state == 0){
                if ((below && LP >= center)){
                    state = 1; ;
                    if (zerocrossindex < MAX_ZERO_CROSS){
                        ZeroCrossingTimes[ch][zerocrossindex] = i;
                        zerocrossindex++;
                    }
                }else if ((!below && LP < center)){
                    state = 2;
                }
            }
            else if (state == 1){ //signal was below and now is above
                if (LP < center){
                    state = 3;
                }
                up++;
            }
            else if (state == 2){ //signal was above and now is below
                if (LP > center){
                    state = 4;
                    if (zerocrossindex<MAX_ZERO_CROSS){
                        ZeroCrossingTimes[ch][zerocrossindex] = i;
                        zerocrossindex++;
                    }
                }
                down++;
            }
            else if (state == 3){ //signal was above and now is below
                if (LP > center){
                    state = 5;
                    if (zerocrossindex<MAX_ZERO_CROSS){
                        ZeroCrossingTimes[ch][zerocrossindex] = i;
                        zerocrossindex++;
                    }
                }
                down++;
            }
            else if (state == 4){ //signal was below and now is above
                if (LP < center){
                    state = 6;
                }
                up++;
            }
            else if (state >= 5){
                if (up > 3 && down > 3){
                    periods++;
                    frq += (double)sampling / (up + down);
                }
                state -= 4;

                totalUp += up;
                totalDown += down;
                up = 0;
                down = 0;
            }
        }

        if (periods >= 1){
            Freq[ch] = frq / periods;
            Period[ch] = 1 / Freq[ch];
            High[ch] = qRound((double)totalUp*1000 / (totalUp + totalDown))/1000;
        }else{
            Freq[ch] = std::numeric_limits<qreal>::infinity();
            Period[ch] = std::numeric_limits<qreal>::infinity();
            High[ch] = qRound((double)totalUp*1000 / (totalUp + totalDown))/1000;
        }
        isTimeCalculated[ch] = true;
    }
}

// we can do this as a class because it will be use on many places
QString MeasCalculations::formatOutout(qreal value, QString unit){
    QString str;

    if (value>=100000000000){
        str=QString::number(value/1000000000,'f',0) + " G"+unit;
    }else if (value>=10000000000){
        str=QString::number(value/1000000000,'f',1) + " G"+unit;
    }else if (value>=1000000000){
        str=QString::number(value/1000000000,'f',2) + " G"+unit;
    }else if (value>=100000000){
        str=QString::number(value/1000000,'f',0) + " M"+unit;
    }else if (value>=10000000){
        str=QString::number(value/1000000,'f',1) + " M"+unit;
    }else if (value>=1000000){
        str=QString::number(value/1000000,'f',2) + " M"+unit;
    }else if (value>=100000){
        str=QString::number(value/1000,'f',0) + " k"+unit;
    }else if (value>=10000){
        str=QString::number(value/1000,'f',1) + " k"+unit;
    }else if (value>=1000){
        str=QString::number(value/1000,'f',2) + " k"+unit;
    }else if (value>=100){
        str=QString::number(value,'f',0) + " "+unit;
    }else if (value>=10){
        str=QString::number(value,'f',1) + " "+unit;
    }else if (value>=1){
        str=QString::number(value,'f',2) + " "+unit;
    }else if (value>=0.1){
        str=QString::number(value*1000,'f',0) + " m"+unit;
    }else if (value>=0.01){
        str=QString::number(value*1000,'f',1) + " m"+unit;
    }else if (value>=0.001){
        str=QString::number(value*1000,'f',2) + " m"+unit;
    }else if (value>=0.0001){
        str=QString::number(value*1000000,'f',0) + " u"+unit;
    }else if (value>=0.00001){
        str=QString::number(value*1000000,'f',1) + " u"+unit;
    }else if (value>=0.000001){
        str=QString::number(value*1000000,'f',2) + " u"+unit;
    }else if (value>=0.0000001){
        str=QString::number(value*1000000000,'f',0) + " n"+unit;
    }else if (value>=0.00000001){
        str=QString::number(value*1000000000,'f',1) + " n"+unit;
    }else if (value>=0.000000001){
        str=QString::number(value*1000000000,'f',2) + " n"+unit;
    }
    return str;
}


