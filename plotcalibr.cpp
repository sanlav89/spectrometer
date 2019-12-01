//==============================================================================
// (C) Copyright 2019 MIET
// Moscow, Zelenograd, Russia
//
// Device:      DISS
// Module:      MPR
// Component:   AFC calibration utility
// File:        plotcalibr.c
// Function:    Object for displaying calibration information on a graph
// Notes:
// Author:      A.Lavrinenko
//==============================================================================
#include "plotcalibr.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_symbol.h>

//==============================================================================
/*
 * Объект для отображения информации о калибровке на графике
 */
PlotCalibr::PlotCalibr(
        QString title,
        QString xAxisTitle,
        QString yAxisTitle,
        QColor beginColor,
        QColor endColor, QWidget * parent):
    QwtPlot( parent )
{
    initCanvasDesign(
            title,
            xAxisTitle,
            yAxisTitle,
            beginColor,
            endColor);  // Инициализация элементов графика
    initCurves();       // Инициализация кривых
    initLegendItem();   // Инициализация легенды
    initMarkers();      // Инициализация маркеров
}

/*
 * Инициализация элементов графика
 */
void PlotCalibr::initCanvasDesign(
        QString title,
        QString xAxisTitle,
        QString yAxisTitle,
        QColor beginColor,
        QColor endColor
        )
{
    // Полотно
    setCanvas( new Canvas(beginColor, endColor) );
    plotLayout()->setAlignCanvasToScales( true );
    // Шрифт осей
    QFont axisFont( "Consolas", 8 );
    axisFont.setBold(false);
    setAxisFont(QwtPlot::xBottom, axisFont);
    setAxisFont(QwtPlot::yLeft, axisFont);
    QwtText t;
    if (!title.isEmpty()) {
        t.setText(title);
        t.setFont(QFont("Consolas", 10));
        setTitle(t);
    }
    if (!xAxisTitle.isEmpty()) {
        t.setText(xAxisTitle);
        t.setFont(QFont("Courier", 10, QFont::Bold));
        setAxisTitle(QwtPlot::xBottom, t);
    }
    if (!yAxisTitle.isEmpty()) {
        t.setText(yAxisTitle);
        t.setFont(QFont("Courier", 10, QFont::Bold));
        setAxisTitle(QwtPlot::yLeft, t);
    }
    // Сетка и зуммер
    QFont zoomerFont( "Consolas", 9 );
    zoomerFont.setBold(false);
    grid = new QwtPlotGrid;
    grid->enableXMin(1);
    grid->enableYMin(1);
    grid->setPen(Qt::gray, (qreal)0.0, Qt::DotLine);
    grid->attach(this);
    zoomer = new QwtPlotZoomer(this->canvas());
    zoomer->setRubberBandPen(QPen(Qt::white));
    zoomer->setTrackerPen(QPen(Qt::white));
    zoomer->setTrackerFont(zoomerFont);
}

/*
 * Инициализация кривых
 */
void PlotCalibr::initCurves()
{
    QPen curvePen[2] = {
        QPen(QColor(128, 255, 128)),
        QPen(QColor(128, 128, 255))
    };
    curvePen[1].setWidth(2);
//    curvePen[0].setB
    for (int i = 0; i < 2; i++) {
        curves[i] = new QwtPlotCurve();
        curves[i]->setStyle(QwtPlotCurve::Lines);
        curves[i]->attach(this);
        curves[i]->setPen(curvePen[i]);
//        curves[i]->setB
    }
//    curves[0]->setStyle(QwtPlotCurve::Sticks);
    curves[0]->setTitle(" Спектр ");
    curves[1]->setTitle(" Сглаженный спектр ");
}

/*
 * Установить "Легенду"
 */
void PlotCalibr::initLegendItem()
{
    LegendItem* legend = new LegendItem;
    legend->attach(this);
    legend->setMaxColumns(1);
    legend->setAlignment(Qt::AlignRight | Qt::AlignTop);
    legend->setBackgroundMode(QwtPlotLegendItem::BackgroundMode(0));
    legend->setBorderRadius(4);
    legend->setMargin(0);
    legend->setSpacing(4);
    legend->setItemMargin(2);
    QFont legItemFont("Consolas", 10);
    legItemFont.setBold(false);
    legend->setFont(legItemFont);
}

/*
 * Инициализация маркеров
 */
void PlotCalibr::initMarkers()
{
    for (int i = 0; i < 2; i++) {
        peakMarker[i] = new QwtPlotMarker();
        peakMarker[i]->setValue( 0.0, 0.0 );
        peakMarker[i]->setLineStyle( QwtPlotMarker::Cross );
        peakMarker[i]->setLinePen( QColor( 200, 150, 0 ), 0, Qt::DashDotLine );
        peakMarker[i]->setSymbol( new QwtSymbol( QwtSymbol::Diamond,
            QColor( Qt::yellow ), QColor( Qt::green ), QSize( 8, 8 ) ) );
    }
    peakMarker[0]->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    peakMarker[1]->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
    peakMarker[0]->attach( this );
    peakMarker[1]->attach( this );
    // Инициализация стиля текста маркеров
    peakText.setFont(QFont("Consolas", 9, QFont::Bold));
    peakText.setColor(QColor(200, 150, 0));
    // Инициализация текстового сообщения о разности мин. и макс. значения
    diffPeaksText.setRenderFlags(Qt::AlignCenter | Qt::AlignBottom);
    diffPeaksText.setFont(QFont("Consolas", 9, QFont::Bold));
    diffPeaksText.setColor(QColor(200, 150, 0));
    diffPeaksLabel = new QwtPlotTextLabel();
    diffPeaksLabel->setText( diffPeaksText );
    diffPeaksLabel->attach( this );
}

/*
 * Обновить данные на графике
 */
void PlotCalibr::UpdateCurves(
        double* dataX,
        double* dataY,
        bool lgScale
        )
{
    double peakX_min; // Координата X первого маркера (минимума)
    double peakY_min; // Координата Y первого маркера (минимума)
    double peakX_max; // Координата X первого маркера (максимума)
    double peakY_max; // Координата Y первого маркера (максимума)
    // Пики для 2-й кривой
    QString peakLabel;
    // Обновление кривых
    smoothCurve0(dataY, N_SM_WIN);
    memcpy(y1, dataY, sizeof(double) * N);
    if (lgScale)
        dataToLgScale();
    curves[0]->setSamples(dataX, y1, N);
    curves[1]->setSamples(dataX, y2, N);
    double delta = (curves[0]->maxYValue() - curves[0]->minYValue()) * 0.1;
    peakX_min = curves[0]->minXValue();
    peakY_min = curves[0]->minYValue();
    peakX_max = curves[0]->maxXValue();
    peakY_max = curves[0]->maxYValue();
    SetScale(peakX_min, peakX_max, peakY_min - delta, peakY_max + delta);
    peakLabel.sprintf("Min: %.3g дБ", peakY_min);
    peakText.setText(peakLabel);
    peakMarker[0]->setValue(peakX_min, peakY_min);
    peakMarker[0]->setLabel(peakText);
    peakLabel.sprintf("Max: %.3g дБ", peakY_max);
    peakText.setText(peakLabel);
    peakMarker[1]->setValue(peakX_min, peakY_max);
    peakMarker[1]->setLabel(peakText);
    replot();
}

/*
 * Установить масштаб осей координат
 */
void PlotCalibr::SetScale(double Xmin, double Xmax, double Ymin, double Ymax)
{
    setAxisScale(QwtPlot::xBottom, Xmin, Xmax);
    setAxisScale(QwtPlot::yLeft, Ymin, Ymax);
    zoomer->setZoomBase();
    replot();
}

/*
 * Сгладить скользящим средним массив dataY[]
 */
void PlotCalibr::smoothCurve0(double* dataY, int nWin)
{
    int k1, k2, dk;
    double cur_value;
    dk = nWin / 2;
    for (int i = 0; i < N; i++) {
        cur_value = 0;
        if (i < dk) {
            k1 = 0;
            k2 = i + dk;
        } else if (i >= N - dk) {
            k1 = i - dk;
            k2 = N;
        } else {
            k1 = i - dk;
            k2 = i + dk;
        }
        for (int j = k1; j < k2; j++) {
            cur_value += dataY[j];
        }
        cur_value /= (2 * dk);
        y2[i] = cur_value;
    }
}

/*
 * Перевод в логарифмическую шкалу
 */
void PlotCalibr::dataToLgScale()
{
    for (int i = 0; i < 8192; i++) {
        y1[i] = 20 * log10(y1[i]);
        y2[i] = 20 * log10(y2[i]);
    }
}
//==============================================================================
