//
// Created by WhiskeyXD on 2023/6/18.
//

#ifndef QVOLTMETER_QVOLTMETER_H
#define QVOLTMETER_QVOLTMETER_H

#include <QWidget>
#include <QMessageBox>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QTableView>
#include <QStandardItemModel>
#include <vector>
#include <string>
#include <random>
#include <thread>
#include <fstream>
#include <CSerialPort/SerialPort.h>
#include <CSerialPort/SerialPortInfo.h>
#include <spdlog/spdlog.h>

#include "MCUSession.h"
#include "SQLHandler.h"
using namespace itas109;


QT_BEGIN_NAMESPACE
namespace Ui { class QVoltMeter; }
QT_END_NAMESPACE

class MySensorApp : public QWidget {
Q_OBJECT

public:
    explicit MySensorApp(QWidget *parent = nullptr);

    ~MySensorApp() override;

    signals:
    void notifyMaxRange(int newRange);

    void notifyCaliberation(double newVal) const;

    void notifyUpdateSQLTable();

private slots:

    void setLCDValue(double accVal, double tempVal);

    void on_btnScan_clicked();

    void on_btnZeroCal_clicked() const;

    void on_btnFullCal_clicked() const;

    void on_btnChangeRange_clicked();

    void on_btnCtrlChart_clicked();

    void on_btnPortClose_clicked();

    void on_btnSQLRecord_clicked();

    void on_btnSQLRecordNow_clicked();

    void on_btnSQLQuery_clicked();

    void on_btnSQLDelete_clicked();

    void addPointToChart(double accVal, double tempVal);

    void on_comboBox_activated(int index);

    void UpdateSQLTable();

private:

    void initChart() noexcept;

    void loadSQLTags() noexcept;

    void disconnectPort() noexcept;

    bool warnInvalidPort() const;

    void taskSQLRecord();

    Ui::QVoltMeter *ui;

    QChart *voltChart;

    QLineSeries series;

    QValueAxis *voltAxis;

    QValueAxis *timeAxis;

    QStandardItemModel *pTableModel = nullptr;

    CSerialPort *pMeterPort = nullptr;

    MCUSession *pMeterSession = nullptr;

    std::vector<SerialPortInfo> availableDevices;

    std::vector<SQLSensorRecord> queryResults;

    SQLHandler sqlHandler;

    bool canUpdateChart = false;

    bool SQLCanRecord = false;

    void taskSQLQueryByTag();
};


#endif //QVOLTMETER_QVOLTMETER_H