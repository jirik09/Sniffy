#include "movingaverage.h"

MovingAverage::MovingAverage(uint defaultBuffSize, QObject *parent) :
    QObject(parent), buffSize(defaultBuffSize) {
    if(buffSize < 1) buffSize = 1; // enforce minimum window size
}

/*
 * @param newValue A new element to be added at the beginning of the list
 * @retval Remaining sample count to fill the buffer
 */
uint MovingAverage::prepend(double newValue){
    uint count = buffer.count();
    if (count == buffSize){
        runningSum -= buffer.last();
        buffer.removeLast();
    }
    buffer.prepend(newValue);
    runningSum += newValue;
    count = buffer.count();
    return (buffSize > count) ? (buffSize - count) : 0;
}

/*
 * @brief Function returning average. Call only if buffer is full.
 * @param newValue A new element to be added in the list
 * @retval Returns average. If buffer not full -100 is returned.
 */
double MovingAverage::getAverage(){
    // Contract: only call when buffer is full.
    return runningSum / static_cast<double>(buffSize);
}

/*
 * @param buffSize A new buffer size to be set
 * @retval Remaining sample count to fill the buffer
 */
uint MovingAverage::setBufferSize(int newBuffSize){
    if(newBuffSize < 1) newBuffSize = 1;
    int count = buffer.count();
    if(newBuffSize < count){
        removeItems(static_cast<uint>(count - newBuffSize));
        count = buffer.count();
    }
    buffSize = static_cast<uint>(newBuffSize);
    return (buffSize > static_cast<uint>(count)) ? (buffSize - static_cast<uint>(count)) : 0;
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
    return ((uint)buffer.count() == buffSize) ? true : false;
}

/*
 * @retval Remaining sample count to fill the buffer
 */
uint MovingAverage::getSampleCountToFillBuff(){
    return buffSize - buffer.count();
}

/*
 * @rbrief Clear the entire buffer
 */
void MovingAverage::clear(){
    if(!buffer.isEmpty())
        buffer.clear();
    runningSum = 0.0;
}

/*
 * @brief Private function used internally by setBufferSize() function
 * @param sampleCount A number of elements at the end of the list to be removed
 * @retval None
 */
void MovingAverage::removeItems(uint sampleCount){
    while(sampleCount != 0 && !buffer.isEmpty()){
        runningSum -= buffer.last();
        buffer.removeLast();
        --sampleCount;
    }
}

/*
 * @brief Delete all elements from the buffer
 */
void MovingAverage::clearBuffer(){
    removeItems((uint)buffer.count());
}

MovingAverage::~MovingAverage(){

}