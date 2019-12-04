#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>
#include <QDataStream>
#include "plot.h"
#include "uartconnecttomka.h"
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    void readTestAndSaveToUartTest();
    void parsePacketsFromTestFile(QString filename);

private:
    QPushButton* getSpectrBtn;
    QPushButton* showSpectrBtn;
    QProgressBar* prBar;
    UartConnectToMka* uartConnect;
    QTimer* timer;
    Plot* plot;
    struct {
        int spectrum_cnt[8192];
        uint32_t bins_sum[8192];
        bool flags[8192];
        double bins_accum[8192];
    } spectrum;
    void updateSpectrum(uint8_t* data, uint16_t begin, uint16_t count);
    bool checkSpectrumFlags();
    double dataX[8192];

    void closeEvent(QCloseEvent *event);
    const int seconds = 6;

private slots:
    void onGetSpectrBtn();
    void onShowSpectrBtn();
    void onTimeout();
    void parsePacketsFromUart(QByteArray ba);
};

#endif // WIDGET_H
