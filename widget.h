#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>
#include <QDataStream>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    void readTestAndSaveToUartTest();
    void parsePacketsFromTestFile(QString filename);

private:
    struct {
        int spectrum_cnt;
        uint32_t bins_sum[8192];
        bool flags[8192];
        double bins_accum[8192];
    } spectrum;
    void updateSpectrum(uint8_t* data, uint16_t begin, uint16_t count);
    bool checkSpectrumFlags();
};

#endif // WIDGET_H
