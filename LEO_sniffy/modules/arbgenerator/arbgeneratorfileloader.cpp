#include "arbgeneratorfileloader.h"

ArbGeneratorFileLoader::ArbGeneratorFileLoader(QObject *parent) :
    QObject(parent)
{
    data = new QVector<QVector<qreal>>;
    defaultData.clear();

    for(int j = 0; j<ARBITRARY_DEFAULT_SIGNAL_LENGTH; j++)
        defaultData.append(ARBITRARY_DEFAULT_SIGNAL[j]);

    for(int i = 0; i<MAX_ARB_CHANNELS_NUM; i++){
        data->append(defaultData);
    }
}

int ArbGeneratorFileLoader::parseFile(QString file)
{
    if(file.length()<4){
        return -3; //file name is incorrect
    }

    QFile *loadData = new QFile(file);

    if(!loadData->open(QFile::ReadOnly)) return -1; //Could not open the file
    QTextStream *fileStream = new QTextStream(loadData);

    QString tmpStr;
    QString tmpStr2;
    QString splitter = ";";
    QStringList inList;
    qreal sample;
    QVector<qreal> data1;
    QVector<qreal> data2;
    QVector<qreal> data3;
    QVector<qreal> data4;
    QVector<qreal> data5;

    QVector<QVector<qreal>> parsedData;
    parsedData.append(data1);
    parsedData.append(data2);
    parsedData.append(data3);
    parsedData.append(data4);
    parsedData.append(data5);

    for(int i=0;i<MAX_ARB_CHANNELS_NUM;i++){
        max[i] = -100000000;
        min[i] = 100000000;
    }
    bool run = true;
    isSampleRateDefined = false;
    parsingErrors = 0;
    bool parsed;
    int channelIndex = 0;

    tmpStr = fileStream->readLine();
    tmpStr2 = fileStream->readLine();

    if(parse(tmpStr,sample) || parse(tmpStr2,sample)){  //data can be directly passed (one channel)
        if(parse(tmpStr,sample)){
            parsedData[0].append(sample);
            updateMinMax(sample,0);
        }
        tmpStr = tmpStr2;
        while (run){
            if(!parse(tmpStr,sample)) parsingErrors++;
            parsedData[0].append(sample);
            updateMinMax(sample,0);
            tmpStr = fileStream->readLine();
            if(tmpStr.isEmpty()) run = false;
        }

    }else{  //more channels (or one with time)
        inList = tmpStr2.split(splitter);
        if(inList.length()==1){
            splitter = ", ";
            inList = tmpStr2.split(splitter);
            if(inList.length()==1){
                splitter = ",";
                inList = tmpStr2.split(splitter);
                if(inList.length()==1)return -2; //File seems not to have valid data (No separator found)
            }
        }
        inList = tmpStr.split(splitter);

        //parse first line
        channelIndex = 0;
        foreach(QString str, inList){
            parsed = parse(str,sample);
            if(!parsed && channelIndex==0) break;  //first line first column is not parsed  --> it is probably lable --> skip first line
            if(parsed){
                parsedData[channelIndex].append(sample);
                updateMinMax(sample,channelIndex);
            }
            channelIndex++;
        }

        //parse second line and follwowing for more channels
        tmpStr = tmpStr2;
        while (run){
            inList = tmpStr.split(splitter);
            channelIndex = 0;
            foreach(QString str, inList){
                parsed = parse(str,sample);
                if(parsed){
                    parsedData[channelIndex].append(sample);
                    updateMinMax(sample,channelIndex);
                }else{
                    parsingErrors++;
                }
                channelIndex++;
            }

            tmpStr = fileStream->readLine();
            if(tmpStr.isEmpty())run = false;
        }
    }
    loadData->close();

    //check if the first column is time (equidistant samples)
    if(parsedData.at(1).length()>1){ //skip if there is only one column
        qreal diff = 0;
        qreal prevSmpl = 0;
        isSampleRateDefined = true;
        foreach(sample, parsedData[0]){
            if(diff!=0){
                if(diff - (sample - prevSmpl)> 0.000001){
                    isSampleRateDefined = false;
                    break;
                }
            }
            if(prevSmpl!=0) diff = sample - prevSmpl;
            prevSmpl = sample;
        }
        if(isSampleRateDefined) sampleRate = 1/diff;
    }

    //chceck if data are above 127 --> data are probably represented in DAC values not in voltage
    qreal maxTotal = -100000000;
    for(int i=0;i<MAX_ARB_CHANNELS_NUM+1;i++){
        maxTotal = fmax(max[i],maxTotal);
    }
    if(maxTotal>127){
        for(int i = 0;i<parsedData.length();i++){
            max[i] = -100000000;
            min[i] = 100000000;
            for(int j=0;j<parsedData.at(i).length();j++){
                parsedData[i][j] = (parsedData[i][j] * (rangeMax-rangeMin)+rangeMin)/pow(2,resolution);
                updateMinMax(parsedData[i][j],i);
            }
        }
    }

    //copy data into private variable to be accesible by getter
    numOfChannels = 0;
    data->clear();
    for(int i=0;i<5;i++){
        if(isSampleRateDefined && i==0){i++;} //skip the first channel if it is time
        if(parsedData.at(i).length()>0){
            numOfChannels++;
            data->append(parsedData.at(i));
        }
    }
    if(isSampleRateDefined) { //skip the first channel if it is time
        for(int i=0;i<MAX_ARB_CHANNELS_NUM;i++){
            max[i] = max[i+1];
            min[i] = min[i+1];
        }
    }


    //append default data to channels which are not used
    int i = 0;
    while(data->length()<4){
        data->append(defaultData);
        max[numOfChannels+i] = ARBITRARY_DEFAULT_SIGNAL_MAX;
        min[numOfChannels+i] = ARBITRARY_DEFAULT_SIGNAL_MIN;
        i++;
    }
    return numOfChannels;// data was prsed and ok
}


int ArbGeneratorFileLoader::getNumChannels()
{
    return numOfChannels;
}

int ArbGeneratorFileLoader::getSignalLength(int channelIndex)
{
    return data->at(channelIndex).length();
}

QVector<qreal> ArbGeneratorFileLoader::getSignal(int channelIndex)
{
    return data->at(channelIndex);
}

qreal ArbGeneratorFileLoader::getSamplerateFrequency()
{
    return sampleRate;
}

bool ArbGeneratorFileLoader::isSamplerateDefined()
{
    return isSampleRateDefined;
}

void ArbGeneratorFileLoader::setSignalRangeParameters(int resolution, qreal rangeMin, qreal rangeMax)
{
    this->resolution = resolution;
    this->rangeMin = rangeMin;
    this->rangeMax = rangeMax;
}

QString ArbGeneratorFileLoader::getInfoString()
{
    QString out;
    if(isSampleRateDefined){
        out.append("time - "+ QString::number(round(sampleRate)) + " smps");
    }
    if(numOfChannels==1){
        out.append(", CH1 - " + QString::number(data->at(0).length())+" smpls ");
    }else if(numOfChannels>1){
        for (int i = 0;i<numOfChannels;i++){
            out.append(", CH"+QString::number(i+1)+" - " + QString::number(data->at(i).length())+" smpls ");
        }
    }
    return out;
}

qreal ArbGeneratorFileLoader::getAmplitude(int channelIndex)
{
    return max[channelIndex]-min[channelIndex];
}

qreal ArbGeneratorFileLoader::getOffset(int channelIndex)
{
    return (max[channelIndex]+min[channelIndex])/2;
}

int ArbGeneratorFileLoader::getParsingErrors() const
{
    return parsingErrors;
}

bool ArbGeneratorFileLoader::parse(QString in, qreal &output)
{
    bool success = false ;
    qreal flRead = in.toFloat(&success);//try . as a separator
    if(!success){
        flRead = QLocale().toFloat(in,&success); //try local decimal separator
    }
    if(success){
        output = flRead;
    }
    return success;
}

void ArbGeneratorFileLoader::updateMinMax(qreal sample, int channelIndex)
{
    min[channelIndex] = fmin(sample,min[channelIndex]);
    max[channelIndex] = fmax(sample,max[channelIndex]);
}


