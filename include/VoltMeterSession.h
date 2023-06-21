#ifndef VOLTMETERSESSION_H
#define VOLTMETERSESSION_H

#include <CSerialPort/SerialPort.h>
#include <CSerialPort/SerialPortListener.h>
#include <string>
#include <QObject>
#include <string_view>
#include "qvoltmeter.h"
using namespace itas109;

class SessionSignalSender : public QObject{
    Q_OBJECT
public:
    signals:
    void notifyLCD(double val);

    void notifyLCD(char* val);
};

class VoltMeterSession : public CSerialPortListener{
public:
    VoltMeterSession(CSerialPort* sp) :
        pListenerPort(sp){}

    void onReadEvent(const char* portName, unsigned int readBufferLen) override;

    void convertAndSend(uint16_t rawVal);

    SessionSignalSender* getSender(){
        return &sender;
    }

private:
    CSerialPort* pListenerPort;
    int meterMode = 4;
    uint16_t rawValue = 0;
    SessionSignalSender sender;
};


#endif // !VOLTMETERSESSION_H
