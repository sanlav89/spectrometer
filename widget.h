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
};

#endif // WIDGET_H
