#include <QApplication>
#include <Poco/Data/SQLite/Connector.h>
#include "qvoltmeter.h"
#include "VoltMeterSession.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QApplication::setStyle("fusion");
    Poco::Data::SQLite::Connector::registerConnector();

    QVoltMeter qVoltMeter;
    qVoltMeter.setMinimumSize(640, 360);
    qVoltMeter.setMaximumSize(640, 360);

    qVoltMeter.show();
    return QApplication::exec();
}
