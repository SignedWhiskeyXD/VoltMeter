#include <QApplication>
#include <QPushButton>
#include "qvoltmeter.h"
#include "VoltMeterSession.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QApplication::setStyle("fusion");
    QVoltMeter qVoltMeter;
    qVoltMeter.setMinimumSize(280, 360);
    qVoltMeter.setMaximumSize(280, 360);

    qVoltMeter.loadRecord();
    qVoltMeter.show();
    return QApplication::exec();
}
