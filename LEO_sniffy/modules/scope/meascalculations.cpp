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

    if(scopeMeas.length()>0){
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
}

void MeasCalculations::run()
{
    qDebug()<< "thread vypoctu"<<this->thread(); //todo chceck if it is really done in separate thread
    forever {
        mutex.lock();
        QVector<QVector<QPointF>> data = this->data;
        QList<Measurement *> meas = this->meas;
        qint32 samplingFreq = this->samplingFreq;
        qint32 channA = -1;
        qint32 channB = -1;
        for (int i = 0;i<8;i++) {
            isVoltCalculated[i] = 0;
            isTimeCalculated[i] = 0;
        }
        mutex.unlock();

        foreach(Measurement* m, meas){
            qint8 channelIndex = m->getChannelIndex();

            if(channelIndex>=data.length() && m->getType()!=MeasurementType::PHASE){
                m->setLabel("N/A");
                continue;
            }

            switch (m->getType()) {
            case MeasurementType::LOW:
                calculateTime(data[channelIndex],channelIndex,samplingFreq);
                m->setLabel("Duty Low");
                m->setValue(LabelFormator::formatOutout(100-High[channelIndex]*100,"%"));
                break;
            case MeasurementType::HIGH:
                calculateTime(data[channelIndex],channelIndex,samplingFreq);
                m->setLabel("Duty High");
                m->setValue(LabelFormator::formatOutout(High[channelIndex]*100,"%"));
                break;
            case MeasurementType::MIN:
                calculateVolt(data[channelIndex],channelIndex);
                m->setLabel("Min");
                m->setValue(LabelFormator::formatOutout(Min[channelIndex],"V"));
                break;
            case MeasurementType::MAX:
                calculateVolt(data[channelIndex],channelIndex);
                m->setLabel("Max");
                m->setValue(LabelFormator::formatOutout(Max[channelIndex],"V"));
                break;
            case MeasurementType::DUTY:
                calculateTime(data[channelIndex],channelIndex,samplingFreq);
                m->setLabel("Duty");
                m->setValue(LabelFormator::formatOutout(High[channelIndex]*100,"%"));
                break;
            case MeasurementType::RMS:
                calculateVolt(data[channelIndex],channelIndex);
                m->setLabel("RMS");
                m->setValue(LabelFormator::formatOutout(RMS[channelIndex],"V"));
                break;
            case MeasurementType::MEAN:
                calculateVolt(data[channelIndex],channelIndex);
                m->setLabel("Mean");
                m->setValue(LabelFormator::formatOutout(Mean[channelIndex],"V"));
                break;
            case MeasurementType::PKPK:
                calculateVolt(data[channelIndex],channelIndex);
                m->setLabel("Pk Pk");
                m->setValue(LabelFormator::formatOutout(Max[channelIndex] - Min[channelIndex] ,"V"));
                break;
            case MeasurementType::PHASE:
                channA = m->getChannelIndex()/10;
                channB = m->getChannelIndex()%10;
                if(channA >= data.length() || channB >= data.length()){
                    m->setLabel("N/A");
                    break;
                }else{
                    m->setLabel("Phase ("+QString::number(channA+1)+"->"+QString::number(channB+1)+")");
                    m->setValue("TODO");
                }
                break;
            case MeasurementType::PERIOD:
                calculateTime(data[channelIndex],channelIndex,samplingFreq);
                m->setLabel("Period");
                m->setValue(LabelFormator::formatOutout(1.0/Freq[channelIndex],"s"));
                break;
            case MeasurementType::FREQUENCY:
                calculateTime(data[channelIndex],channelIndex,samplingFreq);
                m->setLabel("Freq.");
                m->setValue(LabelFormator::formatOutout(Freq[channelIndex],"Hz"));
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
        double middleValue = ((double)(Max[ch]) + Min[ch]) / 2;

        int state = 0;
        int up = 0;
        int down = 0;
        bool below = false;
        int periods = 0;
        double frq = 0;
        qreal lowPass = data[0].y();
        int totalUp = 0;
        int totalDown = 0;
        int zerocrossindex = 0;

        if (data[0].y() < middleValue) {
            below = true;
        }

        for (int i = 3; i < data.length()-4; i++){
            //LP = (samples[ch, i] + samples[ch, i + 1] + samples[ch, i - 1] + samples[ch, i + 2] + samples[ch, i - 2] + samples[ch, i + 3] + samples[ch, i - 3] + samples[ch, i +4]) / 8;
            lowPass = 0.9 * lowPass + 0.1 * data[i].y();
            if (state == 0){
                if ((below && lowPass >= middleValue)){
                    state = 1; ;
                    if (zerocrossindex < MAX_ZERO_CROSS){
                        ZeroCrossingTimes[ch][zerocrossindex] = i;
                        zerocrossindex++;
                    }
                }else if ((!below && lowPass < middleValue)){
                    state = 2;
                }
            }
            else if (state == 1){ //signal was below and now is above
                if (lowPass < middleValue){
                    state = 3;
                }
                up++;
            }
            else if (state == 2){ //signal was above and now is below
                if (lowPass > middleValue){
                    state = 4;
                    if (zerocrossindex<MAX_ZERO_CROSS){
                        ZeroCrossingTimes[ch][zerocrossindex] = i;
                        zerocrossindex++;
                    }
                }
                down++;
            }
            else if (state == 3){ //signal was above and now is below
                if (lowPass > middleValue){
                    state = 5;
                    if (zerocrossindex<MAX_ZERO_CROSS){
                        ZeroCrossingTimes[ch][zerocrossindex] = i;
                        zerocrossindex++;
                    }
                }
                down++;
            }
            else if (state == 4){ //signal was below and now is above
                if (lowPass < middleValue){
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



