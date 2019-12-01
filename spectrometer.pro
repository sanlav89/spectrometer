#-------------------------------------------------
#
# Project created by QtCreator 2019-11-29T18:30:45
#
#-------------------------------------------------

QT       += core gui
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = spectrometer
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    shproto.cpp \
    plot.cpp \
    uartconnecttomka.cpp

HEADERS  += widget.h \
    shproto.h \
    helpers.h \
    plotcalibr.h \
    plot.h \
    plot_helpers.h \
    uartconnecttomka.h

INCLUDEPATH += C:/Qt/qwt-6.1.3/include
LIBS += -LC:/Qt/qwt-6.1.3/lib -lqwt
