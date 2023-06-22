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
#include <vector>
#include <string>
#include <random>
#include <thread>
#include <fstream>
#include <CSerialPort/SerialPort.h>
#include <CSerialPort/SerialPortInfo.h>
#include <fmt/core.h>
#include "VoltMeterSession.h"
using namespace itas109;


QT_BEGIN_NAMESPACE
namespace Ui { class QVoltMeter; }
QT_END_NAMESPACE

class QVoltMeter : public QWidget {
Q_OBJECT

public:
    explicit QVoltMeter(QWidget *parent = nullptr);

    ~QVoltMeter() override;

    void initMeter();

    signals:
    void notifyMaxRange(int newRange);

private slots:

    void setLCDValue(double val);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void addPointToChart(double val);

    void on_comboBox_activated(int index);

private:
    void  initChart();

    bool warnInvalidPort() const;

    Ui::QVoltMeter *ui;

    QChart *voltChart;

    QLineSeries series;

    QValueAxis *voltAxis = nullptr;

    QValueAxis *timeAxis = nullptr;

    CSerialPort* pMeterPort = nullptr;

    std::vector<SerialPortInfo> availableDevices;

    bool canUpdateChart = false;
};


#endif //QVOLTMETER_QVOLTMETER_H
