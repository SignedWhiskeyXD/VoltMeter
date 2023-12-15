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
    bool isEnableMistakeCtrl() const{
        return enableMistakeCtrl;
    };

    double getCaliberation() const {
        return this->caliberarion;
    }

signals:
    void notifyLCD(double val);

public slots:
    void setMaxRange(int newMaxRange){
        maxRange = newMaxRange;
    };

    void setCaliberation(double newVal) {
        this->caliberarion = newVal + caliberarion;
    }

    void setEnableMistakeCtrl(int newState){
        enableMistakeCtrl = newState;
        spdlog::warn("Mistake Control {}!",
                     enableMistakeCtrl ? "Enabled" : "Disabled");
    }

private:
    double caliberarion = 1000;

    int maxRange = 2000;

    bool enableMistakeCtrl = false;
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

    SessionSignalHandler* getSender(){
        return &sender;
    }

private:
    CSerialPort* pListenerPort;

    int gain = 0;

    SessionSignalHandler sender;
};


#endif // !VOLTMETERSESSION_H
