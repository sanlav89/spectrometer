#ifndef UARTCONNECTTOMKA_H
#define UARTCONNECTTOMKA_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QFile>
#include <QDateTime>
//#include <QTime>
//#include <QDate>

class UartConnectToMka : public QObject
{
    Q_OBJECT
public:
    explicit UartConnectToMka(QObject *parent = nullptr);
    bool openSerialPort();
    void closeSerialPort();
    void sendCmd(uint8_t cmd);
    QString logFileName;

private:
    QSerialPort* serialPort;
    QFile* logFile;
    static const uint32_t Baud600000 = 600000;
    bool findMkaDevice(QString* pName);
    void openLogFile();
    void closeLogFile();

private slots:
    void uartReadData();

signals:
    void dataPartReady(QByteArray);

public slots:
};

#endif // UARTCONNECTTOMKA_H
