//
// Created by WhiskeyXD on 2023/6/18.
//

#include "qvoltmeter.h"
#include "ui_QVoltMeter.h"


QVoltMeter::QVoltMeter(QWidget *parent) :
        QWidget(parent), ui(new Ui::QVoltMeter), voltChart(new QChart()) {
    ui->setupUi(this);
    initMeter();
    initChart();
}

QVoltMeter::~QVoltMeter() {
    delete ui;
}

void QVoltMeter::initMeter() noexcept {
    std::ifstream ifs("log.txt", std::ios::in);
    ui->listWidget->clear();
    std::string record;
    while(std::getline(ifs, record)){
        ui->listWidget->addItem(record.c_str());
    }
    ifs.close();
    on_btnScan_clicked();
}

void QVoltMeter::initChart() noexcept {
    voltChart->addSeries(&series);
    voltChart->legend()->hide();

    voltAxis = new QValueAxis();
    voltAxis->setMax(ui->spinBox->value());
    voltAxis->setMin(0);
    voltAxis->setTickCount(6);
    voltAxis->setTickInterval(1000);
    voltChart->addAxis(voltAxis, Qt::AlignLeft);
    series.attachAxis(voltAxis);

    timeAxis = new QValueAxis();
    timeAxis->setMax(100);
    timeAxis->setMin(0);
    timeAxis->setTickCount(2);
    timeAxis->setTickInterval(100);
    timeAxis->hide();
    voltChart->addAxis(timeAxis, Qt::AlignBottom);
    series.attachAxis(timeAxis);

    ui->voltChartView->setChart(voltChart);
}

void QVoltMeter::setLCDValue(double val)
{
    static char valStr[10];
    std::snprintf(valStr,10, "%.3lf", val);
    ui->lcdNumber->display(valStr);
}

void QVoltMeter::on_btnScan_clicked()
{
    spdlog::info("Scanning for Serial Devices...");
    availableDevices = CSerialPortInfo::availablePortInfos();
    ui->comboBox->clear();
    for (const auto& dev : availableDevices){
        QString deviceInfo(dev.portName);
        deviceInfo.append(' ');
        deviceInfo.append(dev.description);
        ui->comboBox->addItem(deviceInfo);
    }
    spdlog::info("Found {} available", availableDevices.size());
}

void QVoltMeter::on_btnZeroCal_clicked() const {
    if(warnInvalidPort()) return;
    pMeterPort->writeData("0", 1);
    spdlog::warn("Zero Calibration Instructed!");
}

void QVoltMeter::on_btnFullCal_clicked() const {
    if(warnInvalidPort()) return;
    pMeterPort->writeData("1", 1);
    spdlog::warn("Full Calibration Instructed!");
}

void QVoltMeter::on_btnSaveRecord_clicked() {
    if(warnInvalidPort()) return;
    char newRecord[30];
    snprintf(newRecord, 30, "%d\t%.3lfmV\t%s",
             ui->listWidget->count() + 1,
             ui->lcdNumber->value(),
             pMeterPort->getPortName());

    ui->listWidget->addItem(newRecord);

    std::ofstream ofs("log.txt", std::ios::out | std::ios::app);
    ofs << newRecord << std::endl;
    ofs.flush();
    ofs.close();
    spdlog::info("Current Record Saved");
}

void QVoltMeter::on_btnClearRecord_clicked() {
    ui->listWidget->clear();
    std::ofstream ofs("log.txt", std::ios::out);
    ofs.close();
    spdlog::warn("Removed All Records");
}

void QVoltMeter::on_btnChangeRange_clicked() {
    int newRange = ui->spinBox->value();
    voltAxis->setMax(newRange);
    notifyMaxRange(newRange);
    spdlog::warn("New Max Range set: {}mV", newRange);
}

void QVoltMeter::on_btnCtrlChart_clicked() {
    //停止示波器
    if(canUpdateChart){
        canUpdateChart = false;
        series.clear();
        timeAxis->setRange(0, 100);
        ui->voltChartView->update();
        ui->btnCtrlChart->setText("启用示波");
        spdlog::info("Chart record disabled");
    }else{      //启用示波器
        canUpdateChart = true;
        ui->btnCtrlChart->setText("禁用示波");
        spdlog::info("Chart record enabled");
    }
}

void QVoltMeter::on_btnPortClose_clicked() {
    disconnectPort();
}

void QVoltMeter::addPointToChart(double val) {
    if(!canUpdateChart) return;
    series.append(series.count(), val);
    if(series.count() > timeAxis->max() - timeAxis->min())
        timeAxis->setRange(timeAxis->min() + 1, timeAxis->max() + 1);

    ui->voltChartView->chart()->update();
    ui->voltChartView->update();

    if(series.count() % 100 == 0)
        spdlog::info("Chart Data count: {}", series.count());
    if(series.count() > 10000){
        series.clear();
        timeAxis->setRange(0, 100);
        ui->voltChartView->update();
    }
}

void QVoltMeter::on_comboBox_activated(int index)
{
    if(pMeterPort != nullptr && strcmp(pMeterPort->getPortName(), availableDevices[index].portName) == 0)
        return;

    disconnectPort();

    pMeterPort = new CSerialPort();
    pMeterPort->init(
            availableDevices[index].portName,
            BaudRate9600,	//波特率9600
            ParityNone,		//无校验位
            DataBits8,		//8位数据传输
            StopOne,		//停止位1
            FlowNone,		//无流控制
            4096
    );
    pMeterPort->setReadIntervalTimeout(0);
    if(!pMeterPort->open()) {
        spdlog::warn("Failed to open {}", pMeterPort->getPortName());
        QMessageBox::warning(nullptr, "警告", "串口正在使用！", QMessageBox::Ok);
    }

    pMeterSession = new VoltMeterSession(pMeterPort);

    QObject::connect(pMeterSession->getSender(), SIGNAL(notifyLCD(double)),
                     this, SLOT(setLCDValue(double)));
    QObject::connect(pMeterSession->getSender(), SIGNAL(notifyLCD(double)),
                     this, SLOT(addPointToChart(double)));
    QObject::connect(this, SIGNAL(notifyMaxRange(int)),
                     pMeterSession->getSender(), SLOT(setMaxRange(int)));
    pMeterSession->getSender()->setMaxRange(ui->spinBox->value());
    pMeterPort->connectReadEvent(pMeterSession);

    spdlog::info("{} Port status: {}",
                 pMeterPort->getPortName(),
                 pMeterPort->isOpen() ? "Opened" : "Closed");
    spdlog::info("{} Listener Status: {}",
                 pMeterPort->getPortName(),
                 pMeterSession == nullptr ? "Not Ready" : "Standing By");
}

void QVoltMeter::disconnectPort() noexcept
{
    QObject::disconnect(this, SLOT(setLCDValue(double)));
    QObject::disconnect(this, SLOT(addPointToChart(double)));
    if(pMeterSession != nullptr){
        QObject::disconnect(pMeterSession->getSender(), SLOT(setMaxRange(int)));
    }

    if(pMeterPort != nullptr){
        pMeterPort->disconnectReadEvent();
        pMeterPort->close();
        spdlog::warn("Disconnected from {}", pMeterPort->getPortName());
    }
    delete pMeterSession;
    delete pMeterPort;
    pMeterSession = nullptr;
    pMeterPort = nullptr;
}

bool QVoltMeter::warnInvalidPort() const
{
    if(pMeterPort == nullptr){
        spdlog::warn("Serial Port NOT initialized");
        QMessageBox::warning(nullptr, "警告", "未选择串口设备！", QMessageBox::Ok);
        return true;
    }
    else if(!pMeterPort->isOpen()){
        spdlog::warn("Serial Port NOT opened");
        QMessageBox::warning(nullptr, "警告", "未打开此串口！", QMessageBox::Ok);
        return true;
    }
    return false;
}

