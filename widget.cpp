#include "widget.h"
#include "shproto.h"
#include <QDebug>
#include <QGridLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
//    // Сборка пакета и передача:
//    static unsigned char packet_buff_tx[1024];
//    static shproto_struct packet_tx = {
//        packet_buff_tx,
//        sizeof(packet_buff_tx),
//        0,
//        0,
//        0,
//        0,
//        0,
//        0
//    };
//    shproto_packet_start(&packet_tx, 0x03);     // 0x03 - код команды, для примера
//    shproto_packet_add_data(&packet_tx, 0x00);  // Добавляем один байт данных, для примера он равен нулю
//    //...или не один...
//    shproto_packet_complete(&packet_tx);    // Добавляем маркер конца пакета и контрольную сумму
//    output(packet_tx.data, packet_tx.len);  // Передаём пакет через UART

//    // Прием пакета из потока байт:
//    static unsigned char packet_buff_rx[256];
//    static shproto_struct packet = {
//        packet_buff_rx,
//        sizeof(packet_buff_rx),
//        0,
//        0,
//        0,
//        0,
//        0,
//        0
//    };
//    while (!fifo_is_empty(&usbRxBuff)) {    // Считываем байты из FIFO или еще откуда-нибудь
//        shproto_byte_received(&packet, fifo_get(&usbRxBuff)); // Передаем 1 байт парсеру пакетов
//        if (!packet.ready)
//            continue;       // Пакет еще не найден или контрольная сумма некорректна
//        packet.ready = 0;   // Обязательно сбрасываем флаг приёма пакета
//        if (packet.cmd == 0x03) {
//            // Обрабатываем принятый пакет с кодом команды равным 0x03
//            // Поле packet.len содержит длину payload
//        }
//    }

    // Objects
    plot = new PlotCalibr(
                "АЧХ",
                "Частота [Гц]",
                "Амплитуда [дБ]",
                QColor(75, 75, 75),
                QColor(25, 25, 25));
    // Init Object's Properties
    plot->setMinimumWidth(800);

    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addWidget(plot, 0, 0);
    setLayout(mainLayout);
    setWindowTitle("Спектрометр");

    for (int i = 0; i < 8192; i++) {
        dataX[i] = i;
    }
    spectrum = {0};
//    readTestAndSaveToUartTest();
    parsePacketsFromTestFile("uart_test_data.bin");

}

Widget::~Widget()
{

}

void Widget::readTestAndSaveToUartTest()
{
    QFile testFile("test_data.bin");
    QDataStream testFileDs(&testFile);
    QFile uartTestFile("uart_test_data.bin");
//    QDataStream uartTestFileDs(&uartTestFile);

    uint8_t testFileByte;
    uint16_t packetNum = 0;

    static unsigned char packet_buff_tx[1024];
    static shproto_struct packet_tx;

    if (!testFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Can't open file" << testFile.fileName();
    } else if (!uartTestFile.open(QIODevice::Append)) {
        qDebug() << "Can't open file" << uartTestFile.fileName();
    } else {
        uartTestFile.resize(0);
        while (!testFileDs.atEnd()) {
            packet_tx = {
               packet_buff_tx,
               sizeof(packet_buff_tx),
               0,
               0,
               0,
               0,
               0,
               0
            };
            shproto_packet_start(&packet_tx, 0x01);     // 0x03 - код команды, для примера
            shproto_packet_add_data(&packet_tx, 0x64);  // Добавляем один байт данных, для примера он равен нулю
            shproto_packet_add_data(&packet_tx, 0x0);
            shproto_packet_add_data(&packet_tx, (uint8_t)((packetNum >> 0) & 0xFF));
            shproto_packet_add_data(&packet_tx, (uint8_t)((packetNum >> 8) & 0xFF));
            for (int j = 0; j < 128; j++) {
                for (int k = 0; k < 4; k++) {
                    testFileDs >> testFileByte;
                    if (k < 3) {
                        shproto_packet_add_data(&packet_tx, testFileByte);
                    }
                }
            }
            shproto_packet_complete(&packet_tx);    // Добавляем маркер конца пакета и контрольную сумму

            // output(packet_tx.data, packet_tx.len);  // Передаём пакет через UART
            uartTestFile.write((char*)packet_tx.data, packet_tx.len);
            packetNum += 128;
        }
        qDebug() << "Uart test file had written";
    }

}

void Widget::parsePacketsFromTestFile(QString filename)
{
    QFile uartTestFile(filename);
    QDataStream uartTestFileDs(&uartTestFile);
    uint8_t rx_byte;
    static unsigned char packet_buff_rx[512];
    static shproto_struct packet;
    uint16_t packet_num;

    if (!uartTestFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Can't open file" << uartTestFile.fileName();
    } else {
        packet = {packet_buff_rx, sizeof(packet_buff_rx), 0, 0, 0, 0, 0, 0};
        while (!uartTestFileDs.atEnd()) {    // Считываем байты из FIFO или еще откуда-нибудь
            uartTestFileDs >> rx_byte;
            shproto_byte_received(&packet, rx_byte); // Передаем 1 байт парсеру пакетов
            if (!packet.ready)
               continue;       // Пакет еще не найден или контрольная сумма некорректна
            packet.ready = 0;   // Обязательно сбрасываем флаг приёма пакета
            if (packet.cmd == 0x01) {
                // Обрабатываем принятый пакет с кодом команды равным 0x03
                // Поле packet.len содержит длину payload
                memcpy(&packet_num, &packet.data[2], sizeof(uint16_t));
//                qDebug() << packet_num << packet.len;
                updateSpectrum(&packet.data[4], packet_num, (packet.len - 4) / 3);
                packet = {packet_buff_rx, sizeof(packet_buff_rx), 0, 0, 0, 0, 0, 0};
            }
        }
    }

}

void Widget::updateSpectrum(uint8_t* data, uint16_t begin, uint16_t count)
{
    uint32_t bin;
    for (int i = 0; i < count; i++) {
        bin = (data[3 * i + 2] << 16) |
              (data[3 * i + 1] << 8) |
              (data[3 * i + 0] << 0);
        spectrum.bins_sum[begin + i] = bin;
        spectrum.flags[begin + i] = true;
    }
    if (checkSpectrumFlags()) {
        spectrum.spectrum_cnt++;
        for (int i = 0; i < 8192; i++) {
            spectrum.bins_accum[i] = spectrum.bins_sum[i] * 1.0 / spectrum.spectrum_cnt;
//            qDebug() << spectrum.bins_accum[i];
            plot->UpdateCurves(dataX, spectrum.bins_accum);
        }
    }
}

bool Widget::checkSpectrumFlags()
{
    for (int i = 0; i < 8192; i++) {
        if (!spectrum.flags[i])
            return false;
    }
    return true;
}
