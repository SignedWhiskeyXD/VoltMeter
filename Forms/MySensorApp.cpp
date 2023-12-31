//
// Created by WhiskeyXD on 2023/6/18.
//

#include "MySensorApp.h"
#include "ui_QVoltMeter.h"


MySensorApp::MySensorApp(QWidget *parent) :
        QWidget(parent), ui(new Ui::QVoltMeter), voltChart(new QChart()),
        voltAxis(new QValueAxis()), timeAxis(new QValueAxis()) {
    ui->setupUi(this);
    on_btnScan_clicked();
    loadSQLTags();
    initChart();

    if(ui->comboSQLTags->count() != 0)
        ui->editSQLTag->setText(ui->comboSQLTags->currentText());
    else {
        ui->editSQLTag->setPlaceholderText("请输入备注");
        ui->comboSQLTags->setPlaceholderText("暂无保存记录");
    }

    QObject::connect(this, SIGNAL(notifyUpdateSQLTable()),
                     this, SLOT(UpdateSQLTable()));
}

MySensorApp::~MySensorApp() {
    delete ui;
    delete voltChart;
    delete voltAxis;
    delete timeAxis;
    delete pTableModel;
    delete pMeterPort;
    delete pMeterSession;
}

void MySensorApp::loadSQLTags() noexcept {
    auto tags = sqlHandler.SelectTags();
    ui->comboSQLTags->clear();
    for(const auto& tagStr : tags){
        ui->comboSQLTags->addItem(tagStr.c_str());
    }
    spdlog::info("Loaded {} Record Tags From Database", tags.size());
}

void MySensorApp::initChart() noexcept {
    voltChart->addSeries(&series);
    voltChart->legend()->hide();

    voltAxis->setMax(ui->spinBox->value());
    voltAxis->setMin(-ui->spinBox->value());
    voltAxis->setTickCount(6);
    voltAxis->setTickInterval(1000);
    voltChart->addAxis(voltAxis, Qt::AlignLeft);
    series.attachAxis(voltAxis);

    timeAxis->setMax(100);
    timeAxis->setMin(0);
    timeAxis->setTickCount(2);
    timeAxis->setTickInterval(100);
    timeAxis->hide();
    voltChart->addAxis(timeAxis, Qt::AlignBottom);
    series.attachAxis(timeAxis);

    ui->voltChartView->setChart(voltChart);
}

void MySensorApp::setLCDValue(double accVal, double tempVal)
{
    static char valStr[10];

    std::snprintf(valStr,10, "%.3lf", accVal);
    ui->lcdNumber->display(valStr);

    std::snprintf(valStr,10, "%.2lf", tempVal);
    ui->lcdTemp->display(valStr);
}

void MySensorApp::on_btnScan_clicked()
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

void MySensorApp::on_btnZeroCal_clicked() const {
    if(warnInvalidPort()) return;
    notifyCaliberation(ui->lcdNumber->value());

    spdlog::warn("Zero Calibration Instructed!");
}

void MySensorApp::on_btnFullCal_clicked() const {
    if(warnInvalidPort()) return;
    pMeterPort->writeData("1", 1);
    spdlog::warn("Full Calibration Instructed!");
}

void MySensorApp::on_btnChangeRange_clicked() {
    int newRange = ui->spinBox->value();
    voltAxis->setMax(newRange);
    voltAxis->setMin(-newRange);
    notifyMaxRange(newRange);
    spdlog::warn("New Max Range set: {}mG", newRange);
}

void MySensorApp::on_btnCtrlChart_clicked() {
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

void MySensorApp::on_btnPortClose_clicked() {
    SQLCanRecord = false;
    ui->editSQLTag->setReadOnly(false);
    ui->spinBoxWait->setReadOnly(false);
    ui->btnSQLRecord->setText("开始记录");
    loadSQLTags();
    disconnectPort();
    if(canUpdateChart)
        on_btnCtrlChart_clicked();
    ui->lcdNumber->display(0);
    ui->lcdTemp->display(0);
}

void MySensorApp::addPointToChart(double accVal, double tempVal) {
    if(!canUpdateChart) return;

    if(series.count() > timeAxis->max() - timeAxis->min()) {
        timeAxis->setRange(timeAxis->min() + 1, timeAxis->max() + 1);
        series.append(timeAxis->max(), ui->lcdNumber->value());
        series.remove(0);
    }else{
        series.append(series.count(), ui->lcdNumber->value());
    }

    ui->voltChartView->chart()->update();
    ui->voltChartView->update();
}

void MySensorApp::on_comboBox_activated(int index)
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

    pMeterSession = new MCUSession(pMeterPort);

    QObject::connect(pMeterSession->getSender(), SIGNAL(notifyLCD(double, double)),
                     this, SLOT(setLCDValue(double, double)));
    QObject::connect(pMeterSession->getSender(), SIGNAL(notifyLCD(double, double)),
                     this, SLOT(addPointToChart(double, double)));
    QObject::connect(this, SIGNAL(notifyCaliberation(double)),
                     pMeterSession->getSender(), SLOT(setCaliberation(double)));

    pMeterPort->connectReadEvent(pMeterSession);

    if(!canUpdateChart)
        on_btnCtrlChart_clicked();

    spdlog::info("{} Port status: {}",
                 pMeterPort->getPortName(),
                 pMeterPort->isOpen() ? "Opened" : "Closed");
    spdlog::info("{} Listener Status: {}",
                 pMeterPort->getPortName(),
                 pMeterSession == nullptr ? "Not Ready" : "Standing By");
}

void MySensorApp::disconnectPort() noexcept
{
    QObject::disconnect(this, SLOT(setLCDValue(double, double)));
    QObject::disconnect(this, SLOT(addPointToChart(double, double)));
    if(pMeterSession != nullptr){
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

bool MySensorApp::warnInvalidPort() const
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

void MySensorApp::on_btnSQLRecord_clicked() {
    if(ui->editSQLTag->text().length() == 0){
        QMessageBox::warning(nullptr, "警告", "数据标签不可为空！", QMessageBox::Ok);
        return;
    }
    if(warnInvalidPort()) return;

    if(SQLCanRecord){
        SQLCanRecord = false;
        ui->editSQLTag->setReadOnly(false);
        ui->spinBoxWait->setReadOnly(false);
        ui->btnSQLRecord->setText("开始记录");
        loadSQLTags();
    }else{
        ui->editSQLTag->setReadOnly(true);
        ui->spinBoxWait->setReadOnly(true);
        ui->btnSQLRecord->setText("停止记录");

        SQLCanRecord = true;
        std::thread threadSQLRecord(&MySensorApp::taskSQLRecord, this);
        threadSQLRecord.detach();
    }
}

void MySensorApp::on_btnSQLRecordNow_clicked(){
    if(ui->editSQLTag->text().length() == 0){
        QMessageBox::warning(nullptr, "警告", "数据标签不可为空！", QMessageBox::Ok);
        return;
    }
    if(warnInvalidPort()) return;

    SQLSensorRecord newRecord(
            ui->editSQLTag->text().toStdString(),
            SQLHandler::getCurrentTime(),
            ui->lcdNumber->value(),
            ui->lcdTemp->value()
    );
    sqlHandler.InsertOneRecord(newRecord);
    loadSQLTags();
}

void MySensorApp::taskSQLRecord() {
    bool isSQLTagsUpdated = false;

    while(SQLCanRecord){
        SQLSensorRecord newRecord(
                ui->editSQLTag->text().toStdString(),
                SQLHandler::getCurrentTime(),
                ui->lcdNumber->value(),
                ui->lcdTemp->value()
                );
        sqlHandler.InsertOneRecord(newRecord);
        std::this_thread::sleep_for(std::chrono::seconds (ui->spinBoxWait->value()));

        if(!isSQLTagsUpdated){
            loadSQLTags();
            isSQLTagsUpdated = true;
        }
    }
}

void MySensorApp::on_btnSQLQuery_clicked() {
    if(ui->comboSQLTags->currentText().length() == 0){
        QMessageBox::warning(nullptr, "警告", "数据标签不可为空！", QMessageBox::Ok);
        return;
    }
    std::thread threadSQLQuery(&MySensorApp::taskSQLQueryByTag, this);
    threadSQLQuery.detach();
    ui->tabWidget->setCurrentIndex(1);
}

void MySensorApp::on_btnSQLDelete_clicked() {
    if(ui->comboSQLTags->currentText().length() == 0){
        QMessageBox::warning(nullptr, "警告", "数据标签不可为空！", QMessageBox::Ok);
        return;
    }
    sqlHandler.RemoveRecordByTag(ui->comboSQLTags->currentText().toStdString());
    ui->tabWidget->setCurrentIndex(0);
    loadSQLTags();
}

void MySensorApp::taskSQLQueryByTag() {
    queryResults = sqlHandler.SelectRecordByTag(ui->comboSQLTags->currentText().toStdString());
    spdlog::info("Query by tag compelte, recieve {} rows", queryResults.size());

    notifyUpdateSQLTable();
}

void MySensorApp::UpdateSQLTable() {
    delete pTableModel;
    pTableModel = new QStandardItemModel;
    pTableModel->setRowCount((int)queryResults.size());
    pTableModel->setColumnCount(3);
    pTableModel->setHeaderData(0, Qt::Horizontal, "记录时间");
    pTableModel->setHeaderData(1, Qt::Horizontal, "瞬时加速度(mG)");
    pTableModel->setHeaderData(2, Qt::Horizontal, "温度(℃)");

    for(int i = 0; i < queryResults.size(); ++i){
        pTableModel->setData(pTableModel->index(i, 0),
                             Poco::DateTimeFormatter::format(
                                     queryResults[i].recordTime, "%Y-%m-%d %H:%M:%S").c_str());
        pTableModel->setData(pTableModel->index(i, 1), queryResults[i].value);
        pTableModel->setData(pTableModel->index(i, 2), queryResults[i].temp);
    }

    ui->SQLtableView->setModel(pTableModel);
    ui->SQLtableView->resizeColumnsToContents();
    ui->SQLtableView->update();
}
