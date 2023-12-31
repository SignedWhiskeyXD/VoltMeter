#include <QApplication>
#include <Poco/Data/SQLite/Connector.h>
#include "MySensorApp.h"
#include "MCUSession.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QApplication::setStyle("fusion");
    Poco::Data::SQLite::Connector::registerConnector();

    MySensorApp qVoltMeter;
    qVoltMeter.setMinimumSize(640, 360);
    qVoltMeter.setMaximumSize(640, 360);

    qVoltMeter.show();
    return QApplication::exec();
}
