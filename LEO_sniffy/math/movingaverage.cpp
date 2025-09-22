#include "movingaverage.h"
#include <limits>

MovingAverage::MovingAverage(uint defaultBuffSize, QObject *parent) :
    QObject(parent) {
    if(defaultBuffSize < 1) defaultBuffSize = 1;
    allocate(defaultBuffSize);
}

/*
 * @param newValue A new element to be added at the beginning of the list
 * @retval Remaining sample count to fill the buffer
 */
uint MovingAverage::prepend(double newValue){
    if(buffSize == 0) allocate(1);
    if(buffer.size() != static_cast<int>(buffSize)) allocate(buffSize);

    // If buffer full, we'll overwrite oldest -> adjust runningSum by subtracting outgoing sample
    if(count == buffSize){
        uint oldestPhysical = physicalIndex(count - 1);
        runningSum -= buffer[oldestPhysical];
    } else {
        ++count;
    }
    head = (head + 1) % buffer.size();
    buffer[head] = newValue;
    runningSum += newValue;
    return buffSize - count;
}

/*
 * @brief Function returning average. Call only if buffer is full.
 * @param newValue A new element to be added in the list
 * @retval Returns average. If buffer not full -100 is returned.
 */
double MovingAverage::getAverage(){
    if(count == 0) return std::numeric_limits<double>::quiet_NaN();
    // Partial average (backward compatible behavior expected by modules)
    return runningSum / static_cast<double>(isBufferFull() ? buffSize : count);
}

double MovingAverage::getAverageFull() const {
    if(!isBufferFull()) return std::numeric_limits<double>::quiet_NaN();
    return runningSum / static_cast<double>(buffSize);
}

double MovingAverage::getAveragePartial() const {
    if(count == 0) return std::numeric_limits<double>::quiet_NaN();
    return runningSum / static_cast<double>(isBufferFull() ? buffSize : count);
}

/*
 * @param buffSize A new buffer size to be set
 * @retval Remaining sample count to fill the buffer
 */
uint MovingAverage::setBufferSize(int newBuffSize){
    if(newBuffSize < 1) newBuffSize = 1;
    uint oldCount = count;
    buffSize = static_cast<uint>(newBuffSize);
    allocate(buffSize);
    // If shrinking and oldCount > buffSize we need to recompute runningSum from retained items
    if(oldCount > buffSize){
        runningSum = 0.0;
        count = buffSize;
        for(uint i=0;i<count;i++){
            runningSum += buffer[physicalIndex(i)];
        }
    }
    return buffSize > count ? (buffSize - count) : 0;
}

/*
 * @param None
 * @retval Actual buffer size
 */
uint MovingAverage::getBufferSize() const { return buffSize; }

/*
 * @brief Checks whether the buffer is full
 * @param None
 * @retval TRUE / FALSE
 */
bool MovingAverage::isBufferFull() const { return count == buffSize; }

/*
 * @retval Remaining sample count to fill the buffer
 */
uint MovingAverage::getSampleCountToFillBuff() const { return buffSize - count; }

/*
 * @rbrief Clear the entire buffer
 */
void MovingAverage::clear(){
    runningSum = 0.0;
    count = 0;
    head = buffer.isEmpty() ? 0 : (buffer.size() - 1); // so first prepend puts at index 0
}

/*
 * @brief Private function used internally by setBufferSize() function
 * @param sampleCount A number of elements at the end of the list to be removed
 * @retval None
 */
void MovingAverage::allocate(uint size){
    if(size < 1) size = 1;
    if(static_cast<uint>(buffer.size()) == size) return;
    QVector<double> newBuf;
    newBuf.resize(static_cast<int>(size));
    // Copy newest up to min(size, count) samples into new buffer
    uint toCopy = std::min<uint>(count, size);
    for(uint i=0;i<toCopy;i++){
        newBuf[i] = buffer.isEmpty() ? 0.0 : buffer[physicalIndex(i)]; // newest at index 0
    }
    buffer.swap(newBuf);
    head = toCopy == 0 ? 0 : 0; // head points to newest sample (index 0)
    count = toCopy;
    // Recompute runningSum
    runningSum = 0.0;
    for(uint i=0;i<count;i++) runningSum += buffer[i];
}

MovingAverage::~MovingAverage(){

}
