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

class uartConnectToMka : public QObject
{
    Q_OBJECT
public:
    explicit uartConnectToMka(QObject *parent = nullptr);
    bool openSerialPort();
    void closeSerialPort();

private:
    QSerialPort* serialPort;
    QTimer* timer;
    QFile* logFile;
    static const uint32_t Baud600000 = 600000;
    bool findMkaDevice(QString* pName);
    void openLogFile();
    void closeLogFile();

private slots:
    void uartReadData();
    void uartConnectTimeout();

signals:
    void connectionFailed();

public slots:
};

#endif // UARTCONNECTTOMKA_H
