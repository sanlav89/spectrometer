//==============================================================================
// (C) Copyright 2019 MIET
// Moscow, Zelenograd, Russia
//
// Device:      DISS
// Module:      MPR
// Component:   AFC calibration utility
// File:        helpers.h
// Function:    Helper Classes
// Notes:
// Author:      A.Lavrinenko
//==============================================================================
#ifndef HELPERS_H
#define HELPERS_H

#include <qwt_plot_canvas.h>
#include <qwt_painter.h>
#include <qwt_plot_legenditem.h>

//==============================================================================
/*
 * Инициирует дизайн полотна для графика (Canvas)
 */
class Canvas: public QwtPlotCanvas
{
public:
    Canvas( QColor beginColor, QColor endColor, QwtPlot *plot = NULL ):
        QwtPlotCanvas( plot )
    {
        // The backing store is important, when working with widget
        // overlays ( f.e rubberbands for zooming ).
        // Here we don't have them and the internal
        // backing store of QWidget is good enough.

        setPaintAttribute( QwtPlotCanvas::BackingStore, false );
        setBorderRadius( 10 );

        if ( QwtPainter::isX11GraphicsSystem() )
        {
#if QT_VERSION < 0x050000
            // Even if not liked by the Qt development, Qt::WA_PaintOutsidePaintEvent
            // works on X11. This has a nice effect on the performance.

            setAttribute( Qt::WA_PaintOutsidePaintEvent, true );
#endif

            // Disabling the backing store of Qt improves the performance
            // for the direct painter even more, but the canvas becomes
            // a native window of the window system, receiving paint events
            // for resize and expose operations. Those might be expensive
            // when there are many points and the backing store of
            // the canvas is disabled. So in this application
            // we better don't disable both backing stores.

            if ( testPaintAttribute( QwtPlotCanvas::BackingStore ) )
            {
                setAttribute( Qt::WA_PaintOnScreen, true );
                setAttribute( Qt::WA_NoSystemBackground, true );
            }
        }

        setupPalette(beginColor, endColor);
    }

private:
    void setupPalette(QColor beginColor, QColor endColor)
    {
        QPalette pal = palette();

#if QT_VERSION >= 0x040400
        QLinearGradient gradient;
        gradient.setCoordinateMode( QGradient::StretchToDeviceMode );

//        gradient.setColorAt( 1.0, QColor(53, 125, 242) );
//        gradient.setColorAt( 0.0, QColor( 0, 87, 174 ) );

        gradient.setColorAt( 1.0, beginColor );
        gradient.setColorAt( 0.0, endColor );

        pal.setBrush( QPalette::Window, QBrush( gradient ) );
#else
        pal.setBrush( QPalette::Window, QBrush( color ) );
#endif

        // QPalette::WindowText is used for the curve color
        pal.setColor( QPalette::WindowText, Qt::green );

        setPalette( pal );
    }
};
//==============================================================================

//==============================================================================
/*
 * Инициирует поле информации о кривых на графике (легенду)
 */
class LegendItem: public QwtPlotLegendItem
{
public:
    LegendItem()
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );

        QColor color( Qt::white );

        setTextPen( color );
#if 1
        setBorderPen( color );

        QColor c( Qt::gray );
        c.setAlpha( 200 );

        setBackgroundBrush( c );
#endif
    }
};
//==============================================================================

#endif // HELPERS_H
