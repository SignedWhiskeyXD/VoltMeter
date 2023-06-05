#include "pch.h"
#include "VoltMeterSession.h"


void VoltMeterSession::onReadEvent(const char* portName, unsigned int readBufferLen)
{
    if (readBufferLen > 0)
    {
        char* data = new char[readBufferLen + 1]; // '\0'
        // read
        int recLen = pListenerPort->readData(data, (int)readBufferLen);
        std::string recvStr(data, recLen);

        if (recvStr.substr(0, 3) == UART_Header && recvStr.length() == 6) {
            uint16_t rawValue = ((unsigned char)data[4] << 8) | (unsigned char)data[5];
        }


        delete[] data;
    }
}