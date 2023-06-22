#include "VoltMeterSession.h"


void VoltMeterSession::onReadEvent(const char* portName, unsigned int readBufferLen)
{
    if (readBufferLen > 0)
    {
        // 堆上分配接受缓冲区，并将串口数据读入缓冲区
        char* data = new char[readBufferLen + 1];
        int recLen = pListenerPort->readData(data, (int)readBufferLen);

        //校验数据帧
        if (recLen == 6 && data[0] == 'W' && data[1] == 'S' && data[2] == 'K' && data[3]) {
            boost = (uint8_t)data[3];
            // 从两个字节恢复为16位无符号整数
            uint16_t tempVal = ((unsigned char)data[4] << 8) | (unsigned char)data[5];
            convertAndSend(tempVal);
        }

        delete[] data;  //释放堆上缓冲区
    }
}

void VoltMeterSession::convertAndSend(uint16_t rawVal)
{
    double convertVal = (double)sender.getMaxRange() * rawVal / (65535 * boost);

    sender.notifyLCD(convertVal);
}
