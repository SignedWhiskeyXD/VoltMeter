#ifndef VOLTMETERSESSION_H
#define VOLTMETERSESSION_H

#include <CSerialPort/SerialPort.h>
#include <CSerialPort/SerialPortListener.h>
#include <string>
#include <QObject>
#include "qvoltmeter.h"
using namespace itas109;

class SessionSignalSender : public QObject{
    Q_OBJECT
public:
    signals:
    void notifyLCD(double val);

    //static void notifyLCD(uint16_t rawVal);
};

class VoltMeterSession : public CSerialPortListener{
public:
    VoltMeterSession(CSerialPort* sp) :
        pListenerPort(sp){}

    void onReadEvent(const char* portName, unsigned int readBufferLen) override;

    void convertAndSend(uint16_t rawValue);

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
