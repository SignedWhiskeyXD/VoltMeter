#include "pch.h"
#include "VoltMeterSession.h"


void VoltMeterSession::onReadEvent(const char* portName, unsigned int readBufferLen)
{
    if (readBufferLen > 0)
    {
        char* data = new char[readBufferLen + 1]; // '\0'
        // read
        int recLen = pListenerPort->readData(data, (int)readBufferLen);

        if (recLen == 6 /*&& data[0] == 'S' && data[1] == 'Y' && data[0] == 'N'*/) {
            meterMode = data[3] - '0';
            uint16_t tempVal = ((unsigned char)data[4] << 8) | (unsigned char)data[5];
            rawValue = tempVal;
        }
        else {
            meterMode = 4;
            rawValue = 0;
        }


        delete[] data;
    }
}