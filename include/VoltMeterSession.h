#ifndef VOLTMETERSESSION_H
#define VOLTMETERSESSION_H

#include <CSerialPort/SerialPort.h>
#include <CSerialPort/SerialPortListener.h>
#include <string>
#include <deque>
#include <QObject>
using namespace itas109;

class SessionSignalSender : public QObject{
    Q_OBJECT
public:
    int getMaxRange(){
        return maxRange;
    }

    signals:
    void notifyLCD(double val);

public slots:
    void setMaxRange(int newMaxRange){
        maxRange = newMaxRange;
    };
private:
    int maxRange = 5000;
};

class VoltMeterSession : public CSerialPortListener{
public:
    explicit VoltMeterSession(CSerialPort* sp) :
        pListenerPort(sp){}

    enum {
        BUFFER_SIZE = 20,
        REALLOC_BUFFER_SIZE = 5
    };

    void onReadEvent(const char* portName, unsigned int readBufferLen) override;

    SessionSignalSender* getSender(){
        return &sender;
    }

private:
    void processData(double newVal);

    std::deque<double> recentValBuffer;

    std::deque<double> badValBuffer;

    CSerialPort* pListenerPort;
    int gain = 0;
    double bufferAVG = 0.0;
    SessionSignalSender sender;
};


#endif // !VOLTMETERSESSION_H
