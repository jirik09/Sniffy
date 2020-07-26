#include "movingaverage.h"

MovingAverage::MovingAverage(uint defaultBuffSize, QObject *parent) :
    QObject(parent), buffSize(defaultBuffSize)
{
    /* Default buffer size is set to 2 if not specified higher */
    if(defaultBuffSize < 2){ buffSize = 2; }
}

/*
 * @param newValue A new element to be added at the beginning of the list
 * @retval Remaining sample count to fill the buffer
 */
uint MovingAverage::prepend(double newValue){
    uint count = buffer.count();
    if (count == buffSize){
        buffer.removeLast();
    }
    buffer.prepend(newValue);
    return (buffSize - count);
}

/*
 * @brief Function returning average. Call only if buffer is full.
 * @param newValue A new element to be added in the list
 * @retval Returns average. If buffer not full -100 is returned.
 */
double MovingAverage::getAverage(){
    if(isBufferFull() == true){
        QListIterator<double> i(buffer);
        double sum = 0;
        while (i.hasNext()){
            sum += buffer.at(i.next());
        }
        double average = sum / buffSize;
        return average;
    }else {
        return -100;
    }
}

/*
 * @param buffSize A new buffer size to be set
 * @retval Remaining sample count to fill the buffer
 */
uint MovingAverage::setBufferSize(uint newBuffSize){
    if(newBuffSize < buffSize){
        removeItems(buffSize - newBuffSize);
    }
    buffSize = newBuffSize;
    return (newBuffSize - buffer.count());
}

/*
 * @param None
 * @retval Actual buffer size
 */
uint MovingAverage::getBufferSize(){
    return buffSize;
}

/*
 * @brief Checks whether the buffer is full
 * @param None
 * @retval TRUE / FALSE
 */
bool MovingAverage::isBufferFull(){
    if((uint)buffer.count() == buffSize){
        return true;
    }else {
        return false;
    }
}

/*
 * @brief Private function used internally by setBufferSize() function
 * @param sampleCount A number of elements at the end of the list to be removed
 * @retval None
 */
void MovingAverage::removeItems(uint sampleCount){
    while(sampleCount != 0){
        buffer.removeLast();
        sampleCount--;
    }
}

