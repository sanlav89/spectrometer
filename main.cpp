#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}


// TODO:
// 1) Включить прибор, обновить драйвер
// 2) Узнать manufacturer, description, serialNumber (findMkaDevice())
// 3) Изменить findMkaDevice() с учетом параметров п. 2)
// 4) Задать вопросы по протоколу, если нужно внести соответствующие правки в код
