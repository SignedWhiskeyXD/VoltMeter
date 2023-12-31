#ifndef VOLTMETERSESSION_H
#define VOLTMETERSESSION_H

#include <CSerialPort/SerialPort.h>
#include <CSerialPort/SerialPortListener.h>
#include <string>
#include <deque>
#include <QObject>
#include <spdlog/spdlog.h>
using namespace itas109;

class SessionSignalHandler : public QObject{
    Q_OBJECT
public:

    double getCaliberation() const {
        return this->caliberarion;
    }

signals:
    void notifyLCD(double accVal, double tempVal);

public slots:
    ;

    void setCaliberation(double newVal) {
        this->caliberarion = newVal + caliberarion;
    }

private:
    double caliberarion = 1000;
};

class MCUSession : public CSerialPortListener{
public:
    explicit MCUSession(CSerialPort* sp) :
        pListenerPort(sp){}

    void onReadEvent(const char* portName, unsigned int readBufferLen) override;

    SessionSignalHandler* getSender(){
        return &sender;
    }

private:
    CSerialPort* pListenerPort;

    SessionSignalHandler sender;
};


#endif // !VOLTMETERSESSION_H
