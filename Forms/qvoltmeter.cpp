//
// Created by WhiskeyXD on 2023/6/18.
//

#include "qvoltmeter.h"
#include "ui_QVoltMeter.h"


QVoltMeter::QVoltMeter(QWidget *parent) :
        QWidget(parent), ui(new Ui::QVoltMeter) {
    ui->setupUi(this);
}

QVoltMeter::~QVoltMeter() {
    delete ui;
}

void QVoltMeter::initMeter() {
    std::ifstream ifs("log.txt", std::ios::in);
    ui->listWidget->clear();
    std::string record;
    while(std::getline(ifs, record)){
        ui->listWidget->addItem(record.c_str());
    }
    ifs.close();

    on_pushButton_clicked();
}

void QVoltMeter::setLCDValue(double val)
{
    static char valStr[10];
    std::snprintf(valStr,10, "%.3lf", val);
    ui->lcdNumber->display(valStr);
}

void QVoltMeter::on_pushButton_clicked()
{
    availableDevices = CSerialPortInfo::availablePortInfos();
    ui->comboBox->clear();
    for (const auto& dev : availableDevices){
        QString deviceInfo(dev.portName);
        deviceInfo.append(' ');
        deviceInfo.append(dev.description);
        ui->comboBox->addItem(deviceInfo);
    }
}

void QVoltMeter::on_pushButton_2_clicked()
{
    if(warnInvalidPort()) return;
    pMeterPort->writeData("0", 1);
}

void QVoltMeter::on_pushButton_3_clicked()
{
    if(warnInvalidPort()) return;
    pMeterPort->writeData("1", 1);
}

void QVoltMeter::on_pushButton_4_clicked() {
    if(warnInvalidPort()) return;
    std::string newRecord = fmt::format("{}\t{}mv\t{}",
                                 ui->listWidget->count() + 1,
                                 ui->lcdNumber->value(),
                                 pMeterPort->getPortName());
    ui->listWidget->addItem(newRecord.c_str());

    std::ofstream ofs("log.txt", std::ios::out | std::ios::app);
    ofs << newRecord << std::endl;
    ofs.flush();
    ofs.close();
}

void QVoltMeter::on_pushButton_5_clicked() {
    ui->listWidget->clear();
    std::ofstream ofs("log.txt", std::ios::out);
    ofs.close();
}

void QVoltMeter::on_comboBox_activated(int index)
{
    static VoltMeterSession* pMeterSession = nullptr;

    QObject::disconnect(this, SLOT(setLCDValue(double)));
    if(pMeterPort != nullptr){
        pMeterPort->disconnectReadEvent();
        pMeterPort->close();
    }
    delete pMeterSession;
    delete pMeterPort;

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
    pMeterPort->open();

    pMeterSession = new VoltMeterSession(pMeterPort);

    QObject::connect(pMeterSession->getSender(), SIGNAL(notifyLCD(double)),
                     this, SLOT(setLCDValue(double)));
    QObject::connect(this->ui->spinBox, SIGNAL(valueChanged(int)),
                     pMeterSession->getSender(), SLOT(setMaxRange(int)));
    pMeterSession->getSender()->setMaxRange(ui->spinBox->value());
    pMeterPort->connectReadEvent(pMeterSession);
}

bool QVoltMeter::warnInvalidPort() const
{
    if(pMeterPort == nullptr){
        QMessageBox::warning(nullptr, "警告", "未选择串口设备！", QMessageBox::Ok);
        return true;
    }
    else if(!pMeterPort->isOpen()){
        QMessageBox::warning(nullptr, "警告", "未打开此串口！", QMessageBox::Ok);
        return true;
    }
    return false;
}

