#include "uartconnecttomka.h"
#include <QDebug>
#include "shproto.h"

UartConnectToMka::UartConnectToMka(QObject *parent) : QObject(parent)
{
    serialPort = new QSerialPort();
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(uartReadData()));
    logFile = new QFile;
}

bool UartConnectToMka::openSerialPort()
{
    QString portName;
    if (findMkaDevice(&portName)) {
        serialPort->setPortName(portName);
        serialPort->setBaudRate(Baud600000);
//        serialPort->setBaudRate(QSerialPort::Baud115200);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        if (serialPort->open(QIODevice::ReadWrite)) {
            openLogFile();
            qDebug() << "Open success";
            return true;
        } else {
            qDebug() << "Open error";
            return false;
        }
    } else {
        qDebug() << "MKA Device didn't find";
        return false;
    }
}

void UartConnectToMka::closeSerialPort()
{
    if (serialPort->isOpen())
        serialPort->close();
    if (logFile->isOpen())
        closeLogFile();
    qDebug() << "Port Closed";
}

void UartConnectToMka::sendCmd(uint8_t cmd)
{
    // Сборка пакета и передача:
    static unsigned char packet_buff_tx[1024];
    static shproto_struct packet_tx = {packet_buff_tx, sizeof(packet_buff_tx),
                                       0, 0, 0, 0, 0, 0};
    shproto_packet_start(&packet_tx, cmd);     // cmd - код команды, для примера
//    shproto_packet_add_data(&packet_tx, 0x00);  // Добавляем один байт данных, для примера он равен нулю
//    //...или не один...
    shproto_packet_complete(&packet_tx);    // Добавляем маркер конца пакета и контрольную сумму
//    output(packet_tx.data, packet_tx.len);  // Передаём пакет через UART
    serialPort->write((char*)packet_tx.data, packet_tx.len);
}

// TODO: Узнать ID прибора, по кототому его можно опознать
bool UartConnectToMka::findMkaDevice(QString* pName)
{
    QString description;
    QString manufacturer;
    QString serialNumber;
    bool isFinded = false;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
//        if (manufacturer == "Prolific"
//                && description == "Prolific USB-to-Serial Comm Port"
//                && serialNumber.isEmpty()) {
            qDebug() << info.portName()
                     << description
                     << manufacturer
                     << serialNumber;
            *pName = info.portName();
            isFinded = true;
//        }
    }
    return isFinded;
}

void UartConnectToMka::openLogFile()
{
    // Начало записи в файл телеметрии (TELEMETRIA.TXT)
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString date = curDateTime.toString("yyyy_MM_dd__hh_mm_ss");
//    QTime curTime = QTime::currentTime();
//    QString time = curTime.toString("hh:mm:ss.zzz\n");
//    QTextStream stream(logFile);
    logFileName.append("mka_log_");
    logFileName.append(date);
    logFileName.append(".dat");
    logFile->setFileName(logFileName);
    if (logFile->open(QIODevice::ReadWrite)) {
        qDebug() << logFileName;
//        stream << time;
    }
    else {
        qDebug() << " ОШИБКА: файл не может быть открыт!";
    }
}

void UartConnectToMka::closeLogFile()
{
//    QTime curTime = QTime::currentTime();
//    QString time = curTime.toString("hh:mm:ss.zzz\n");
//    QTextStream stream(logFile);
    if (logFile->isOpen()) {
//        stream << time;
        logFile->close();
    }
}

void UartConnectToMka::uartReadData()
{
//    qDebug() << serialPort->readAll();
    QByteArray data = serialPort->readAll();
    logFile->write(data);
//    emit dataPartReady(data);
//    qDebug() << data.toHex();
}
