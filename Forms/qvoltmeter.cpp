//
// Created by WhiskeyXD on 2023/6/18.
//

#include "qvoltmeter.h"
#include "ui_QVoltMeter.h"


QVoltMeter::QVoltMeter(QWidget *parent) :
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

QVoltMeter::~QVoltMeter() {
    delete ui;
    delete voltChart;
    delete voltAxis;
    delete timeAxis;
    delete pTableModel;
    delete pMeterPort;
    delete pMeterSession;
}

void QVoltMeter::loadSQLTags() noexcept {
    auto tags = sqlHandler.SelectTags();
    ui->comboSQLTags->clear();
    for(const auto& tagStr : tags){
        ui->comboSQLTags->addItem(tagStr.c_str());
    }
    spdlog::info("Loaded {} Record Tags From Database", tags.size());
}

void QVoltMeter::initChart() noexcept {
    voltChart->addSeries(&series);
    voltChart->legend()->hide();

    voltAxis->setMax(ui->spinBox->value());
    voltAxis->setMin(0);
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
    SQLCanRecord = false;
    ui->editSQLTag->setReadOnly(false);
    ui->spinBoxWait->setReadOnly(false);
    ui->btnSQLRecord->setText("开始记录");
    loadSQLTags();
    disconnectPort();
    if(canUpdateChart)
        on_btnCtrlChart_clicked();
    ui->lcdNumber->display(0);
}

void QVoltMeter::addPointToChart(double val) {
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
    QObject::connect(this->ui->checkBox, SIGNAL(stateChanged(int)),
                     pMeterSession->getSender(), SLOT(setEnableMistakeCtrl(int)));

    pMeterSession->getSender()->setMaxRange(ui->spinBox->value());
    pMeterSession->getSender()->setEnableMistakeCtrl(ui->checkBox->checkState());
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

void QVoltMeter::on_btnSQLRecord_clicked() {
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
        std::thread threadSQLRecord(&QVoltMeter::taskSQLRecord, this);
        threadSQLRecord.detach();
    }
}

void QVoltMeter::on_btnSQLRecordNow_clicked(){
    if(ui->editSQLTag->text().length() == 0){
        QMessageBox::warning(nullptr, "警告", "数据标签不可为空！", QMessageBox::Ok);
        return;
    }
    if(warnInvalidPort()) return;

    SQLVoltRecord newRecord(
            ui->editSQLTag->text().toStdString(),
            SQLHandler::getCurrentTime(),
            ui->lcdNumber->value()
    );
    sqlHandler.InsertOneRecord(newRecord);
    loadSQLTags();
}

void QVoltMeter::taskSQLRecord() {
    bool isSQLTagsUpdated = false;

    while(SQLCanRecord){
        SQLVoltRecord newRecord(
                ui->editSQLTag->text().toStdString(),
                SQLHandler::getCurrentTime(),
                ui->lcdNumber->value()
                );
        sqlHandler.InsertOneRecord(newRecord);
        std::this_thread::sleep_for(std::chrono::seconds (ui->spinBoxWait->value()));

        if(!isSQLTagsUpdated){
            loadSQLTags();
            isSQLTagsUpdated = true;
        }
    }
}

void QVoltMeter::on_btnSQLQuery_clicked() {
    if(ui->comboSQLTags->currentText().length() == 0){
        QMessageBox::warning(nullptr, "警告", "数据标签不可为空！", QMessageBox::Ok);
        return;
    }
    std::thread threadSQLQuery(&QVoltMeter::taskSQLQueryByTag, this);
    threadSQLQuery.detach();
    ui->tabWidget->setCurrentIndex(1);
}

void QVoltMeter::on_btnSQLDelete_clicked() {
    if(ui->comboSQLTags->currentText().length() == 0){
        QMessageBox::warning(nullptr, "警告", "数据标签不可为空！", QMessageBox::Ok);
        return;
    }
    sqlHandler.RemoveRecordByTag(ui->comboSQLTags->currentText().toStdString());
    ui->tabWidget->setCurrentIndex(0);
    loadSQLTags();
}

void QVoltMeter::taskSQLQueryByTag() {
    queryResults = sqlHandler.SelectRecordByTag(ui->comboSQLTags->currentText().toStdString());
    spdlog::info("Query by tag compelte, recieve {} rows", queryResults.size());

    notifyUpdateSQLTable();
}

void QVoltMeter::UpdateSQLTable() {
    delete pTableModel;
    pTableModel = new QStandardItemModel;
    pTableModel->setRowCount((int)queryResults.size());
    pTableModel->setColumnCount(2);
    pTableModel->setHeaderData(0, Qt::Horizontal, "记录时间");
    pTableModel->setHeaderData(1, Qt::Horizontal, "测量值");

    for(int i = 0; i < queryResults.size(); ++i){
        pTableModel->setData(pTableModel->index(i, 0),
                             Poco::DateTimeFormatter::format(
                                     queryResults[i].recordTime, "%Y-%m-%d %H:%M:%S").c_str());
        pTableModel->setData(pTableModel->index(i, 1), queryResults[i].value);
    }

    ui->SQLtableView->setModel(pTableModel);
    ui->SQLtableView->resizeColumnsToContents();
    ui->SQLtableView->update();
}
