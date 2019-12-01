#include "uartconnecttomka.h"
#include <QDebug>

uartConnectToMka::uartConnectToMka(QObject *parent) : QObject(parent)
{
    serialPort = new QSerialPort();
    timer = new QTimer;
    timer->start(3000);

    connect(serialPort, SIGNAL(readyRead()), this, SLOT(uartReadData()));
    connect(timer, SIGNAL(timeout()), this, SLOT(uartConnectTimeout()));

    logFile = new QFile;
}

bool uartConnectToMka::openSerialPort()
{
    QString portName;
    if (findMkaDevice(&portName)) {
        serialPort->setPortName(portName);
        serialPort->setBaudRate(Baud600000);
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
        qDebug() << "Gps Device didn't find";
        return false;
    }
}

void uartConnectToMka::closeSerialPort()
{
    if (serialPort->isOpen())
        serialPort->close();
    if (logFile->isOpen())
        closeLogFile();
    timer->stop();
    qDebug() << "Port Closed";
}

// TODO: Узнать ID прибора, по кототому его можно опознать
bool uartConnectToMka::findMkaDevice(QString* pName)
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

void uartConnectToMka::openLogFile()
{
    // Начало записи в файл телеметрии (TELEMETRIA.TXT)
    QString filename;
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString date = curDateTime.toString("yyyy_MM_dd__hh_mm_ss");
//    QTime curTime = QTime::currentTime();
//    QString time = curTime.toString("hh:mm:ss.zzz\n");
//    QTextStream stream(logFile);
    filename.append("LOG/gps_log_");
    filename.append(date);
    filename.append(".dat");
    logFile->setFileName(filename);
    if (logFile->open(QIODevice::ReadWrite)) {
        qDebug() << filename;
//        stream << time;
    }
    else {
        qDebug() << " ОШИБКА: файл не может быть открыт!";
    }
}

void uartConnectToMka::closeLogFile()
{
//    QTime curTime = QTime::currentTime();
//    QString time = curTime.toString("hh:mm:ss.zzz\n");
//    QTextStream stream(logFile);
    if (logFile->isOpen()) {
//        stream << time;
        logFile->close();
    }
}

void uartConnectToMka::uartReadData()
{
//    qDebug() << serialPort->readAll();
    QByteArray data = serialPort->readAll();
    logFile->write(data);
}

void uartConnectToMka::uartConnectTimeout()
{
    emit connectionFailed();
    if (serialPort->isOpen())
        serialPort->close();
    openSerialPort();
}
