#ifndef VOLTMETERSESSION_H
#define VOLTMETERSESSION_H

#include <CSerialPort/SerialPort.h>
#include <CSerialPort/SerialPortListener.h>
#include <string>
using namespace itas109;


class VoltMeterSession : public CSerialPortListener {
public:
    VoltMeterSession(CSerialPort* sp) :
        pListenerPort(sp) { }

    void onReadEvent(const char* portName, unsigned int readBufferLen) override;

    int getRange() const {
        return meterMode;
    }

    uint16_t getRawValue() const {
        return rawValue;
    }

private:
    CSerialPort* pListenerPort;
    int meterMode = 4;
    uint16_t rawValue = 0;
    const std::string UART_Header = "SYN";
};


#endif // !VOLTMETERSESSION_H
