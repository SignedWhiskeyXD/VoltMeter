#include "MCUSession.h"
#include <spdlog/spdlog.h>

void MCUSession::onReadEvent(const char* portName, unsigned int readBufferLen)
{
    if (readBufferLen > 0)
    {
        // 堆上分配接受缓冲区，并将串口数据读入缓冲区
        unsigned char* data = new unsigned char[readBufferLen + 1];
        int recLen = pListenerPort->readData(data, (int)readBufferLen);

        //校验数据帧
        if (recLen == 6 && data[0] == 'W' && data[1] == 'S') {
            // 从两个字节恢复为16位无符号整数
            int16_t rawVal = (data[4] << 8) + data[5];
            // spdlog::info("raw val: {}, {:#x}, {:#x}", rawVal, data[4], data[5]);
            uint16_t rawTemp = (data[2] << 8) + data[3];
            const double convertVal = rawVal * (1000.0 / 256) - sender.getCaliberation();

            sender.notifyLCD(convertVal, rawTemp / 4.0);
        }

        delete[] data;  //释放堆上缓冲区
    }
}