#include "VoltMeterSession.h"
#include <spdlog/spdlog.h>

void VoltMeterSession::onReadEvent(const char* portName, unsigned int readBufferLen)
{
    if (readBufferLen > 0)
    {
        // 堆上分配接受缓冲区，并将串口数据读入缓冲区
        char* data = new char[readBufferLen + 1];
        int recLen = pListenerPort->readData(data, (int)readBufferLen);

        //校验数据帧
        if (recLen == 6 && data[0] == 'W' && data[1] == 'S' && data[2] == 'K' && data[3]) {
            if((uint8_t)data[3] != gain){
                gain = (uint8_t)data[3];
                spdlog::info("Gain Factor Changed: {}", gain);
            }
            // 从两个字节恢复为16位无符号整数
            uint16_t rawVal = ((unsigned char)data[4] << 8) | (unsigned char)data[5];
            double convertVal = (double)sender.getMaxRange() * rawVal / (65535 * gain);

            if(sender.isEnableMistakeCtrl())
                processData(convertVal);
            else{
                recentValBuffer.clear();
                badValBuffer.clear();
                recentBufferSum = 0.0;
                sender.notifyLCD(convertVal);
            }
        }

        delete[] data;  //释放堆上缓冲区
    }
}

void VoltMeterSession::processData(double newVal) {
    if(recentValBuffer.empty()){
        recentValBuffer.push_back(newVal);
        recentBufferSum = newVal;
        sender.notifyLCD(newVal);
        return;
    }

    double bufferAVG = recentBufferSum / (double)recentValBuffer.size();
    bool isGoodValue = abs(newVal - bufferAVG) < bufferAVG * 0.2;

    if(isGoodValue){
        recentValBuffer.push_back(newVal);
        recentBufferSum += newVal;
        if(!badValBuffer.empty()) {
            spdlog::warn("Caught {} bad {}!", badValBuffer.size(),
                         badValBuffer.size() > 1 ? "values" : "value");
            badValBuffer.clear();
        }
        if(recentValBuffer.size() > BUFFER_SIZE) {
            recentBufferSum -= recentValBuffer.front();
            recentValBuffer.pop_front();
        }
    }else if(badValBuffer.size() < REALLOC_BUFFER_SIZE){
        badValBuffer.push_back(newVal);
    }else{
        recentValBuffer = std::move(badValBuffer);
        recentValBuffer.push_back(newVal);
        recentBufferSum = std::accumulate(recentValBuffer.cbegin(), recentValBuffer.cend(), 0.0);
        badValBuffer.clear();
        spdlog::warn("Bad value buffer reallocated!");
    }

    if(recentValBuffer.empty())
        sender.notifyLCD(0.0);
    else
        sender.notifyLCD(recentValBuffer.back());
}

