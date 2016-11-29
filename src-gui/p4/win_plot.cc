/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "win_plot.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "math_separatrice.h"
#include "p4application.h"
#include "plot_tools.h"
#include "win_event.h"
#include "win_gcf.h"
#include "win_intparams.h"
#include "win_limitcycles.h"
#include "win_orbits.h"
#include "win_print.h"
#include "win_separatrice.h"
#include "win_zoom.h"

#include <QToolBar>
#include <QPrintDialog>


QPlotWnd::QPlotWnd( QStartDlg * main )
    : QMainWindow()
{
    QToolBar * toolBar1;
    QToolBar * toolBar2;
    parent = main;

    //setAttribute( Qt::WA_PaintOnScreen, true );
    //setAttribute( Qt::WA_PaintOutsidePaintEvent, true );

    if( p4smallicon != nullptr )
        setWindowIcon( *p4smallicon );

    numZooms = 0;
    lastZoomIdentifier = 0;
    ZoomWindows = nullptr;

//    QPalette palette;
//    palette.setColor(backgroundRole(), QXFIGCOLOR(CBACKGROUND) );
//    setPalette(palette);

    toolBar1 = new QToolBar( "PlotBar1", this );
    toolBar2 = new QToolBar( "PlotBar2", this );
    toolBar1->setMovable(false);
    toolBar2->setMovable(false);

    ActClose = new QAction("Clos&e", this);
    ActClose->setShortcut( Qt::ALT + Qt::Key_E );
    connect(ActClose, SIGNAL(triggered()), this, SLOT(OnBtnClose()));
    toolBar1->addAction( ActClose );

    ActRefresh = new QAction("&Refresh", this);
    ActRefresh->setShortcut( Qt::ALT + Qt::Key_R );
    connect(ActRefresh, SIGNAL(triggered()), this, SLOT(OnBtnRefresh()));
    toolBar1->addAction( ActRefresh );

    ActLegend = new QAction("&Legend", this);
    ActLegend->setShortcut( Qt::ALT + Qt::Key_L );
    connect(ActLegend, SIGNAL(triggered()), this, SLOT(OnBtnLegend()));
    toolBar1->addAction( ActLegend );

    ActOrbits = new QAction("&Orbits", this);
    ActOrbits->setShortcut( Qt::ALT + Qt::Key_O );
    connect(ActOrbits, SIGNAL(triggered()), this, SLOT(OnBtnOrbits()));
    toolBar1->addAction( ActOrbits );

    ActIntParams = new QAction("&Integration Parameters", this);
    ActIntParams->setShortcut( Qt::ALT + Qt::Key_I );
    connect(ActIntParams, SIGNAL(triggered()), this, SLOT(OnBtnIntParams()));
    toolBar1->addAction( ActIntParams );

    ActGCF = new QAction("&GCF", this);
    ActGCF->setShortcut( Qt::ALT + Qt::Key_G );
    connect(ActGCF, SIGNAL(triggered()), this, SLOT(OnBtnGCF()));
    toolBar1->addAction( ActGCF );

    ActPlotSep = new QAction("Plot &Separatrice", this);
    ActPlotSep->setShortcut( Qt::ALT + Qt::Key_S );
    connect(ActPlotSep, SIGNAL(triggered()), this, SLOT(OnBtnPlotSep()));
    toolBar2->addAction( ActPlotSep );

    ActPlotAllSeps = new QAction("Plot All Separa&trices", this);
    ActPlotAllSeps->setShortcut( Qt::ALT + Qt::Key_T );
    connect(ActPlotAllSeps, SIGNAL(triggered()), this, SLOT(OnBtnPlotAllSeps()));
    toolBar2->addAction( ActPlotAllSeps );

    ActLimitCycles = new QAction("Limit C&ycles", this);
    ActLimitCycles->setShortcut( Qt::ALT + Qt::Key_Y );
    connect(ActLimitCycles, SIGNAL(triggered()), this, SLOT(OnBtnLimitCycles()));
    toolBar2->addAction( ActLimitCycles );

    ActView = new QAction("&View", this);
    ActView->setShortcut( Qt::ALT + Qt::Key_V );
    connect(ActView, SIGNAL(triggered()), this, SLOT(OnBtnView()));
    toolBar2->addAction( ActView );

    ActPrint = new QAction("&Print", this);
    ActPrint->setShortcut( Qt::ALT + Qt::Key_P );
    connect(ActPrint, SIGNAL(triggered()), this, SLOT(OnBtnPrint()));
    toolBar2->addAction( ActPrint );
    
    addToolBar( Qt::TopToolBarArea, toolBar1 );
    addToolBarBreak( Qt::TopToolBarArea );
    addToolBar( Qt::TopToolBarArea, toolBar2 );

#ifdef TOOLTIPS

    ActClose->setToolTip( "Closes the plot window, all subwindows and zoom window"  );
    ActRefresh->setToolTip( "Redraw the plot window" );
    ActLegend->setToolTip( "Show legend" );
    ActOrbits->setToolTip( "Opens \"Integrate Orbits\" window" );
    ActIntParams->setToolTip( "Opens \"Integration Parameters\" window" );
    ActGCF->setToolTip( "Opens Greatest-Common-Factor window.\n"
                            "Disabled if there is no GCF" );
    ActPlotSep->setToolTip( "Opens \"Plot separatrices\" window" );
    ActPlotAllSeps->setToolTip( "Plots all separatrices of all singular points with default integration parameters.\n"
                                    "Change integration parameters if the effect is too small to be visible." );
    ActLimitCycles->setToolTip( "Opens limit cycle window" );
    ActView->setToolTip( "Opens the \"View parameter\" window" );
    ActPrint->setToolTip( "Opens the print window" );
#endif

    statusBar()->showMessage( "Ready" );

    sphere = new QWinSphere( this, statusBar(), false, 0,0,0,0 );
    Legend_Window = new QLegendWnd();
    Orbits_Window = new QOrbitsDlg( this, sphere );
    Sep_Window = new QSepDlg( this, sphere );
    IntParams_Window = new QIntParamsDlg();
    ViewParams_Window = new QViewDlg(this);
    LC_Window = new QLimitCyclesDlg( this, sphere );
    GCF_Window = new QGcfDlg( this, sphere );
    lcWindowIsUp = false;       // Limit cycles: initially hidden

    sphere->show();
    setCentralWidget(sphere );
    resize( NOMINALWIDTHPLOTWINDOW, NOMINALHEIGHTPLOTWINDOW );

    IntParams_Window->UpdateDlgData();
    ViewParams_Window->UpdateDlgData();

//  if( ThisVF->evaluated )
        setP4WindowTitle( this, "Phase Portrait" );
//  else
//      SetP4WindowTitle( this, "Phase Portrait (*)" );

}

QPlotWnd::~QPlotWnd()
{
    if( numZooms != 0 )
    {
        for( int i = 0; i < numZooms; i++ )
        {
            delete ZoomWindows[i];
            ZoomWindows[i] = nullptr;
        }
        delete ZoomWindows;
        ZoomWindows = nullptr;
        numZooms = 0;
    }

    delete Legend_Window;
    Legend_Window = nullptr;
    delete Orbits_Window;
    Orbits_Window = nullptr;
    delete IntParams_Window;
    IntParams_Window = nullptr;
    delete ViewParams_Window;
    ViewParams_Window = nullptr;
    delete Sep_Window;
    Sep_Window = nullptr;
    delete LC_Window;
    LC_Window = nullptr;
    delete GCF_Window;
    GCF_Window = nullptr;
    ThisVF->gcfDlg = nullptr;
}

void QPlotWnd::AdjustHeight( void )
{
    sphere->adjustToNewSize();
    resize( width(), height() + sphere->idealh - sphere->h );
    sphere->refresh();
    statusBar()->showMessage( "Ready." );
}

void QPlotWnd::Signal_Changed( void )
{
//  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere->Signal_Changed();
    for( int i = 0; i < numZooms; i++ )
        ZoomWindows[i]->Signal_Changed();
}

void QPlotWnd::Signal_Evaluating( void )
{
//  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere->Signal_Evaluating();
    for( int i = 0; i < numZooms; i++ )
        ZoomWindows[i]->Signal_Evaluating();
}

void QPlotWnd::Signal_Evaluated( void )
{
//  SetP4WindowTitle( this, "Phase Portrait" );

    configure();
    for( int i = 0; i < numZooms; i++ )
        ZoomWindows[i]->Signal_Evaluated();
}

void QPlotWnd::OnBtnClose( void )
{
    QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr );
    p4app->postEvent( parent, e1 );
}

bool QPlotWnd::close( void )
{
    QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr );
    p4app->postEvent( parent, e1 );

    return QMainWindow::close();
}

void QPlotWnd::OnBtnRefresh( void )
{
    getDlgData();
    sphere->refresh();
}

void QPlotWnd::OnBtnLegend( void )
{
    if( Legend_Window == nullptr )
        Legend_Window = new QLegendWnd();

    Legend_Window->show();
    Legend_Window->raise();
}

void QPlotWnd::OnBtnOrbits( void )
{
    Orbits_Window->show();
    Orbits_Window->raise();
}

void QPlotWnd::OnBtnIntParams( void )
{
    IntParams_Window->show();
    IntParams_Window->raise();
}

void QPlotWnd::OnBtnView( void )
{
    ViewParams_Window->show();
    ViewParams_Window->raise();
}

void QPlotWnd::OnBtnGCF( void )
{
    GCF_Window->show();
    GCF_Window->raise();
}

void QPlotWnd::OnBtnPlotSep( void )
{
    getDlgData();
    Sep_Window->show();
    Sep_Window->raise();
}

void QPlotWnd::OnBtnPlotAllSeps( void )
{
    getDlgData();
    sphere->prepareDrawing();
    plot_all_sep( sphere );
    sphere->finishDrawing();
}

void QPlotWnd::OnBtnLimitCycles( void )
{
    LC_Window->show();
    LC_Window->raise();
}

void QPlotWnd::OnBtnPrint( void )
{
    int res;
    double lw;
    double ss;

    QPrintDlg * pdlg;
    pdlg = new QPrintDlg( this, 0 );
    int result = pdlg->exec();

    res = pdlg->GetChosenResolution();
    lw = pdlg->GetChosenLineWidth();
    ss = pdlg->GetChosenSymbolSize();

    delete pdlg;
    pdlg = nullptr;

    if( result != P4PRINT_NONE )
    {
        if( result == P4PRINT_DEFAULT || result == -P4PRINT_DEFAULT )
        {
            p4printer->setResolution(res);

             QPrintDialog dialog(p4printer, this);
             if( !dialog.exec() )
                 return;

            res = p4printer->resolution();
        }

        if( result < 0 )
            sphere->preparePrinting( -result, true, res, lw, ss );
        else
            sphere->preparePrinting( result, false, res, lw, ss );
        sphere->print();
        sphere->finishPrinting();
    }
}

void QPlotWnd::configure( void )
{
    statusBar()->showMessage( "Ready" );        // reset status bar
    plot_l = spherePlotLine;                // setup line/plot pointing to routines of the sphere window
    plot_p = spherePlotPoint;
    sphere->SetupPlot();                    // setup sphere window (define pixel transformations)
    IntParams_Window->UpdateDlgData();      // update data of integration parameters
    ViewParams_Window->UpdateDlgData();
    Orbits_Window->Reset();                 // reset forward/backward buttons to initial state
    Sep_Window->Reset();
    LC_Window->Reset();
    GCF_Window->Reset();

    sphere->update();
    if( VFResults.gcf == nullptr )             // reconfigure GCF button
        ActGCF->setEnabled(false);
    else
        ActGCF->setEnabled(true);
}

void QPlotWnd::OpenZoomWindow( double x1, double y1, double x2, double y2 )
{
    double swap;
    if( x1 > x2 )
    {
        swap = x1;
        x1 = x2;
        x2 = swap;
    }
    if( y1 > y2 )
    {
        swap = y1;
        y1 = y2;
        y2 = swap;
    }
    if( x1 == x2 || y1 == y2 )
        return;

    ZoomWindows = (QZoomWnd * *)realloc( ZoomWindows, sizeof(QZoomWnd *) * (numZooms+1) );
    ZoomWindows[numZooms] = new QZoomWnd( this, ++lastZoomIdentifier, x1, y1, x2, y2 );
    ZoomWindows[numZooms]->show();
    ZoomWindows[numZooms]->raise();
    ZoomWindows[numZooms]->AdjustHeight();
    numZooms++;
}

void QPlotWnd::CloseZoomWindow( int id )
{
    int i;
    for( i = 0; i < numZooms-1; i++ )
    {
        if( ZoomWindows[i]->zoomid == id )
            break;
    }
    if( i == numZooms )
        return;     // error: zoom window not found???

    delete ZoomWindows[i];
    ZoomWindows[i] = nullptr;
    if( i != numZooms-1 )
        memmove( ZoomWindows + i, ZoomWindows + i+ 1, sizeof(QZoomWnd*) * (numZooms-1-i) );

    numZooms--;
    if( numZooms == 0 )
    {
        delete ZoomWindows;//free( ZoomWindows );
        ZoomWindows = nullptr;
    }
}

void QPlotWnd::customEvent( QEvent * _e )
{
    QP4Event * e;
    double pcoord[3];
    double ucoord[2];
    double ucoord0[2];
    double ucoord1[2];
    struct DOUBLEPOINT * p;
    double x, y, x0, y0, x1, y1;
    int * oet;

    e = (QP4Event *)_e;

    if( e->type() == TYPE_OPENZOOMWINDOW )
    {
        double * data1;
        data1 = (double *)(e->data());
        
        OpenZoomWindow( data1[0], data1[1], data1[2], data1[3] );
        delete data1;//free( data1 );
        data1 = nullptr;
        return;
    }

    if( e->type() == TYPE_CLOSE_ZOOMWINDOW )
    {
        int * data2;
        data2 = (int *)(e->data());
        CloseZoomWindow( *data2 );
        delete data2;//free( data2 );
        data2 = nullptr;
        return;
    }

    if( e->type() == TYPE_ORBIT_EVENT )
    {
        oet = (int *)(e->data());
        Orbits_Window->OrbitEvent( *oet );
        delete oet;//free(oet);
        oet = nullptr;
        return;
    }

    if( e->type() == TYPE_SELECT_ORBIT )
    {
        p = (struct DOUBLEPOINT *)(e->data());
        x = p->x;
        y = p->y;
        void * win = *((void **)(p+1));
        delete p;//free( p );
        p = nullptr;

        // mouse clicked in position (x,y)  (world coordinates)

        if( MATHFUNC(is_valid_viewcoord)(x,y,pcoord) )
        {
            MATHFUNC(sphere_to_R2)(pcoord[0],pcoord[1],pcoord[2],ucoord);

            Orbits_Window->show();
            Orbits_Window->setInitialPoint(ucoord[0],ucoord[1]);
            if( win != nullptr )
            {
                ((QWidget *)win)->activateWindow();
            }
        }
        return;
    }

    if( e->type() == TYPE_SELECT_LCSECTION )
    {
        p = (struct DOUBLEPOINT *)(e->data());
        x0 = p->x;
        y0 = p->y;
        p++;
        x1 = p->x;
        y1 = p->y;
        p--;
        delete p;//free( p );
        p = nullptr;

        MATHFUNC(viewcoord_to_sphere)(x0,y0,pcoord);
        MATHFUNC(sphere_to_R2)(pcoord[0],pcoord[1],pcoord[2],ucoord0);

        MATHFUNC(viewcoord_to_sphere)(x1,y1,pcoord);
        MATHFUNC(sphere_to_R2)(pcoord[0],pcoord[1],pcoord[2],ucoord1);

        if( x0==x1 && y0 == y1 )
        {
            Orbits_Window->show();
            Orbits_Window->setInitialPoint(ucoord0[0],ucoord0[1]);
            return;
        }

        // mouse clicked in position (x,y)  (world coordinates)

        LC_Window->setSection(ucoord0[0],ucoord0[1], ucoord1[0], ucoord1[1] );
        LC_Window->show();
        LC_Window->raise();
        return;
    }

    if( e->type() == TYPE_SEP_EVENT )
    {
        oet = (int *)(e->data());
        Sep_Window->SepEvent( *oet );
        delete oet;//free(oet);
        oet = nullptr;
        return;
    }

    QMainWindow::customEvent(e);
}

void QPlotWnd::hideEvent ( QHideEvent * h )
{
    UNUSED(h);
    if( !isMinimized() )
    {
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr );
        p4app->postEvent( parent, e1 );
    }
}

void QPlotWnd::getDlgData( void )
{
    IntParams_Window->GetDataFromDlg();
    if( ViewParams_Window->GetDataFromDlg() )
    {
        // true when a big change occured in the view

        VFResults.setupCoordinateTransformations();
        configure();
    }
}
