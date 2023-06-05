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

private:
    CSerialPort* pListenerPort;
    const std::string UART_Header = "SYN";
};


#endif // !VOLTMETERSESSION_H
