#include <math.h>
#include <qpaintengine.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <qapplication.h>
#include <qstatusbar.h>
#include <qevent.h>
#include "custom.h"
#include "table.h"
#include "file_vf.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_sphere.h"
#include "win_main.h"
#include "p4application.h"
#include "win_print.h"
#include "win_event.h"

#define SELECTINGPOINTSTEPS         5
#define SELECTINGPOINTSPEED         150

extern int PrintColorTable[NUMXFIGCOLORS];

extern void SetP4WindowTitle( QWidget *, QString );
extern void PreparePostscriptPrinting( int, int, int, int, bool, bool, int, int, int );
extern void FinishPostscriptPrinting( void );
extern void PrepareXFigPrinting( int, int, bool, bool, int, int, int );
extern void FinishXFigPrinting( void );
extern void PrepareP4Printing( int, int, bool, QPainter *, int, int );
extern void FinishP4Printing( void );

extern int find_critical_point( QWinSphere *, double, double );
extern QPrinter * p4printer;
QPixmap * p4pixmap = NULL;
static  double p4pixmap_dpm = 0;

extern void (* print_line)( double, double, double, double, int );
extern void (* print_point)( double, double, int );

extern bool LineRectangleIntersect( double &, double &, double &, double &, double, double, double, double );

int QWinSphere::numSpheres = 0;
QWinSphere * * QWinSphere::SphereList = NULL;

extern void DrawOrbits( QWinSphere * );
extern void DrawLimitCycles( QWinSphere * );

/*
    Coordinates on the sphere:

    - in a local study: the coordinates are cartesian
    - in a normal window: the coordinates are mapped to a square of width/height 2.2
      Inside this square, the circle at infinity is displayed as a circle with radius 1.

    - in a zoom window: the coordinates are a part of the coordinates from a normal window.
*/

// parameters _x1,... are irrelevant if isZoom is false

QWinSphere::QWinSphere( QWidget * parent, QStatusBar * bar, bool isZoom,
                       double _x1, double _y1, double _x2, double _y2 )
    : QWidget(parent)
{
    ReverseYaxis = false;
    PainterCache = NULL;
    isPainterCacheDirty = true;
    AnchorMap = NULL;
    refreshTimeout = NULL;
    SelectingTimer = NULL;

//    setAttribute( Qt::WA_PaintOnScreen );

    SphereList = (QWinSphere * *)realloc( SphereList, sizeof(QWinSphere *) * (numSpheres+1) );
    SphereList[numSpheres++] = this;
    if( numSpheres > 1 )
    {
        SphereList[numSpheres-2]->next = this;
    }

    parentWnd = parent;
    setMinimumSize(MINWIDTHPLOTWINDOW,MINHEIGHTPLOTWINDOW);         // THIS IS THE MINIMUM SIZE
    w = width();
    h = height();
    idealh = w;
    SelectingPointStep =0;

    horPixelsPerMM = ((double)w) / ((double)widthMM());
    verPixelsPerMM = ((double)h) / ((double)heightMM());

    setMouseTracking(true);
    msgBar = bar;
    selectingZoom = false;
    selectingLCSection = false;
    setFocusPolicy( Qt::ClickFocus );
    setWindowFlags( windowFlags() );
    next = NULL;

    iszoom = isZoom;
    if( isZoom )
    {
        x0 = _x1;
        y0 = _y1;
        
        x1 = _x2;
        y1 = _y2;
    }

    CircleAtInfinity = NULL;
    PLCircle = NULL;
}

/*
    Keyboard codes:

    F       forward integration of orbit
    C       continue integration of orbit
    B       backward integration of orbit
    D       delete orbit
    A       delete all orbits

    Shift+C continue integrate separatrice
    Shift+N next separatrice
    Shift+I integrate next separatrice
    Shift+S start integrate separatrice
    Shift+A delete all separatrices???
*/

void QWinSphere::keyPressEvent ( QKeyEvent * e )
{
    int id;
    int * data1;
    Qt::KeyboardModifiers bs;
    
    id = e->key();
    bs = e->modifiers();
    if( id == Qt::Key_F && (bs == Qt::NoModifier || bs == Qt::AltModifier) )
    {
        // F: integrate orbit forwards in time

        data1 = new int;//(int *)malloc(sizeof(int));
        *data1 = 1;
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_ORBIT_EVENT, data1 );
        p4app->postEvent( parentWnd, e1 );
    }
    if( id == Qt::Key_C && (bs == Qt::NoModifier || bs == Qt::AltModifier) )
    {
        // C: continue integrate orbit

        data1 = new int;//(int *)malloc(sizeof(int));
        *data1 = 0;
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_ORBIT_EVENT, data1 );
        p4app->postEvent( parentWnd, e1 );
    }
    if( id == Qt::Key_B && (bs == Qt::NoModifier || bs == Qt::AltModifier) )
    {
        // B: integrate orbit backwards in time

        data1 = new int;//(int *)malloc(sizeof(int));
        *data1 = -1;
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_ORBIT_EVENT, data1 );
        p4app->postEvent( parentWnd, e1 );
    }
    if( id == Qt::Key_D && (bs == Qt::NoModifier || bs == Qt::AltModifier) )
    {
        // D: delete orbit

        data1 = new int;//(int *)malloc(sizeof(int));
        *data1 = 2;
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_ORBIT_EVENT, data1 );
        p4app->postEvent( parentWnd, e1 );
    }

    if( id == Qt::Key_A && (bs == Qt::NoModifier || bs == Qt::AltModifier) )
    {
        // A: delete all orbits

        data1 = new int;//(int *)malloc(sizeof(int));
        *data1 = 3;
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_ORBIT_EVENT, data1 );
        p4app->postEvent( parentWnd, e1 );
    }

    if( id == Qt::Key_C && (bs == Qt::ShiftModifier || bs == Qt::AltModifier+Qt::ShiftModifier ) )
    {
        // SHIFT+C:  continue integrating separatrice

        data1 = new int;//(int *)malloc(sizeof(int));
        *data1 = 0;
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_SEP_EVENT, data1 );
        p4app->postEvent( parentWnd, e1 );
    }

    if( id == Qt::Key_N && (bs == Qt::ShiftModifier || bs == Qt::AltModifier+Qt::ShiftModifier ) )
    {
        // SHIFT+N: select next separatrice

        data1 = new int;//(int *)malloc(sizeof(int));
        *data1 = 3;
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_SEP_EVENT, data1 );
        p4app->postEvent( parentWnd, e1 );
    }

    if( id == Qt::Key_I && (bs == Qt::ShiftModifier || bs == Qt::AltModifier+Qt::ShiftModifier ) )
    {
        // SHIFT+I: integrate next separatrice

        data1 = new int;//(int *)malloc(sizeof(int));
        *data1 = 2;
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_SEP_EVENT, data1 );
        p4app->postEvent( parentWnd, e1 );
    }

    if( id == Qt::Key_S && (bs == Qt::ShiftModifier || bs == Qt::AltModifier+Qt::ShiftModifier ) )
    {
        // SHIFT+S: start integrate separatrice

        data1 = new int;//(int *)malloc(sizeof(int));
        *data1 = 1;
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_SEP_EVENT, data1 );
        p4app->postEvent( parentWnd, e1 );
    }

    e->ignore();
}

static QString makechartstring( int p, int q, bool isu1v1chart, bool negchart )
{
    QString buf;

    if( isu1v1chart )
    {
        // make { x = +/- 1/z2^p, y = z1/z2^q }

        if( p != 1 && q != 1 )
            buf.sprintf( "{x=%d/z2^%d,y=z1/z2^%d}", (int)(negchart ? -1 : 1), p, q );
        else if( p == 1 && q != 1 )
            buf.sprintf( "{x=%d/z2,y=z1/z2^%d}", (int)(negchart ? -1 : 1), q );
        else if( p != 1 && q == 1 )
            buf.sprintf( "{x=%d/z2^%d,y=z1/z2}", (int)(negchart ? -1 : 1), p );
        else
            buf.sprintf( "{x=%d/z2,y=z1/z2}", (int)(negchart ? -1 : 1) );
    }
    else
    {
        // make { x = 1/z2^p, y = +/- z1/z2^q }

        if( p != 1 && q != 1 )
            buf.sprintf( "{x=z1/z2^%d,y=%d/z2^%d}", p, (int)(negchart ? -1 : 1), q );
        else if( p == 1 && q != 1 )
            buf.sprintf( "{x=z1/z2,y=%d/z2^%d}", (int)(negchart ? -1 : 1), q );
        else if( p != 1 && q == 1 )
            buf.sprintf( "{x=z1/z2^%d,y=%d/z2}", p, (int)(negchart ? -1 : 1) );
        else
            buf.sprintf( "{x=z1/z2,y=%d/z2}", (int)(negchart ? -1 : 1) );
    }

    return (QString)buf;
}

void QWinSphere::SetupPlot( void )
{
    struct P4POLYLINES * t;
    QPalette palette;

    spherebgcolor=CBACKGROUND;
    palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor) );
    setPalette(palette);

    while( CircleAtInfinity != NULL )
    {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        free( t );
    }
    while( PLCircle != NULL )
    {
        t = PLCircle;
        PLCircle = t->next;
        free( t );
    }

    if( !iszoom )
    {
        switch( VFResults.typeofview )
        {
        case TYPEOFVIEW_PLANE:
        case TYPEOFVIEW_U1:
        case TYPEOFVIEW_U2:
        case TYPEOFVIEW_V1:
        case TYPEOFVIEW_V2:
            x0 = VFResults.xmin;
            y0 = VFResults.ymin;
            x1 = VFResults.xmax;
            y1 = VFResults.ymax;
            break;
        case TYPEOFVIEW_SPHERE:
            x0 = -1.1;
            y0 = -1.1;
            x1 = 1.1;
            y1 = 1.1;
            break;
        }
    }

    dx = x1-x0;
    dy = y1-y0;

    double idealhd;
    
    idealhd = w;
    idealhd = (idealhd/dx)*dy;

    idealh = (int)(idealhd+.5);

    switch( VFResults.typeofview )
    {
    case TYPEOFVIEW_PLANE:  chartstring = "";   break;
    case TYPEOFVIEW_SPHERE: chartstring = "";   break;
    case TYPEOFVIEW_U1:     chartstring = makechartstring( VFResults.p, VFResults.q, true, false ); break;
    case TYPEOFVIEW_U2:     chartstring = makechartstring( VFResults.p, VFResults.q, false, false );break;
    case TYPEOFVIEW_V1:     chartstring = makechartstring( VFResults.p, VFResults.q, true, true );  break;
    case TYPEOFVIEW_V2:     chartstring = makechartstring( VFResults.p, VFResults.q, false, true ); break;
    }

    if( VFResults.typeofview == TYPEOFVIEW_SPHERE )
    {
        CircleAtInfinity = produceEllipse( 0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0) );
        if( VFResults.plweights )
            PLCircle = produceEllipse( 0.0, 0.0, RADIUS, RADIUS, true, coWinH(RADIUS), coWinV(RADIUS) );
    }
    
    isPainterCacheDirty = true;
}

QWinSphere::~QWinSphere()
{
    int i;

    struct P4POLYLINES * t;
    while( CircleAtInfinity != NULL )
    {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        free( t );
    }
    while( PLCircle != NULL )
    {
        t = PLCircle;
        PLCircle = t->next;
        free( t );
    }

    for( i = 0; i < numSpheres; i++ )
    {
        if( SphereList[i] == this )
            break;
    }
    if( i == numSpheres )
        return;         // error: sphere not found?

    if( i > 0 )
        SphereList[i-1]->next = next;

    if( i < numSpheres-1 )
        memmove( SphereList+i, SphereList+i+1, sizeof(QWinSphere *) * (numSpheres-i-1) );

    numSpheres--;
    
    if( PainterCache != NULL )
    {
        delete PainterCache;
        PainterCache = NULL;
    }
    
    if( AnchorMap != NULL )
    {
        delete AnchorMap;
        AnchorMap = NULL;
    }
    
    if( refreshTimeout != NULL )
    {
        delete refreshTimeout;
        refreshTimeout = NULL;
    }
    if( SelectingTimer != NULL )
    {
        delete SelectingTimer;
        SelectingTimer = NULL;
    }
}

void QWinSphere::LoadAnchorMap( void )
{
     int x1,y1;
     int x2,y2;
     int aw,ah;         
     int s;
     if( selectingZoom )
     {
         x1 = zoomAnchor1.x();
         y1 = zoomAnchor1.y();
         x2 = zoomAnchor2.x();
         y2 = zoomAnchor2.y();
     }
     else if( selectingLCSection )
     {
         x1 = lcAnchor1.x();
         y1 = lcAnchor1.y();
         x2 = lcAnchor2.x();
         y2 = lcAnchor2.y();
     }
     else
         return;
     
     if( x1 > x2 ) { s=x1; x1=x2; x2=s; }
     if( y1 > y2 ) { s=y1; y1=y2; y2=s; }
     if( x1 < 0 ) x1 = 0;
     if( y1 < 0 ) y1 = 0;
     if( x2 >= width() ) x2 = width()-1;
     if( y2 >= height() ) y2 = height()-1;
     
     aw = x2-x1+1;
     ah = y2-y1+1;
     
         
/*     QString ms = msgBar->currentMessage();
     QString as;
     as.sprintf( " Load: (%d,%d,%d,%d)", x1,y1,aw,ah );
     msgBar->showMessage(as+ms);
*/     
     if( AnchorMap != NULL )
     {
         if( AnchorMap->width() < aw || AnchorMap->height() < ah )
         {
             delete AnchorMap;
             AnchorMap = new QPixmap( aw,ah );
         }
     }
     else
     {
         AnchorMap = new QPixmap( aw,ah );
     }
     
     if( PainterCache == NULL )
     {
         delete AnchorMap;
         AnchorMap = NULL;
         return;
     }   

     QPainter paint( AnchorMap );
     if( selectingZoom )
     {
         // only copy rectangular edges, not inside
      //   paint.drawPixmap (    0,    0, aw,  1, *PainterCache, x1, y1, aw, 1 );
    //     paint.drawPixmap (    0, ah-1, aw,  1, *PainterCache, x1, y2, aw, 1 );
  //       paint.drawPixmap (    0,    0,  1, ah, *PainterCache, x1, y1, 1, ah );
//         paint.drawPixmap ( aw-1, 0,     1, ah, *PainterCache, x2, y1, 1, ah );

         paint.drawPixmap ( 0, 0, aw, ah, *PainterCache, x1, y1, aw, ah );

     }
     else
     {
         paint.drawPixmap ( 0, 0, aw, ah, *PainterCache, x1, y1, aw, ah );
     }
}

void QWinSphere::SaveAnchorMap( void )
{
     int x1,y1;
     int x2,y2;
     int aw,ah;         
     int s;

     if( AnchorMap == NULL || PainterCache == NULL || (!selectingZoom && !selectingLCSection) )
         return;

     if( selectingZoom )
     {
         x1 = zoomAnchor1.x();
         y1 = zoomAnchor1.y();
         x2 = zoomAnchor2.x();
         y2 = zoomAnchor2.y();
     }
     if( selectingLCSection )
     {
         x1 = lcAnchor1.x();
         y1 = lcAnchor1.y();
         x2 = lcAnchor2.x();
         y2 = lcAnchor2.y();
     }
     
     if( x1 > x2 ) { s=x1; x1=x2; x2=s; }
     if( y1 > y2 ) { s=y1; y1=y2; y2=s; }
     if( x1 < 0 ) x1 = 0;
     if( y1 < 0 ) y1 = 0;
     if( x2 >= width() ) x2 = width()-1;
     if( y2 >= height() ) y2 = height()-1;
     
     aw = x2-x1+1;
     ah = y2-y1+1;
/*         
     QString ms = msgBar->currentMessage();
     QString as;
     as.sprintf( " Save: (%d,%d,%d,%d)", x1,y1,aw,ah );
     msgBar->showMessage(as+ms);
*/
     QPainter paint( PainterCache );

     if( selectingZoom )
     {
         // only copy rectangular edges, not inside
//         paint.drawPixmap ( x1, y1, aw,  1, *AnchorMap,    0,    0, aw, 1 );
  //       paint.drawPixmap ( x1, y2, aw,  1, *AnchorMap,    0, ah-1, aw, 1 );
    //     paint.drawPixmap ( x1, y1,  1, ah, *AnchorMap,    0,    0, 1, ah );
      //   paint.drawPixmap ( x2, y1,  1, ah, *AnchorMap, aw-1,    0, 1, ah );
         paint.drawPixmap ( x1, y1, aw, ah, *AnchorMap, 0, 0, aw, ah );
     }
     else
     {
         paint.drawPixmap ( x1, y1, aw, ah, *AnchorMap, 0, 0, aw, ah );
     }
     
     update( x1,y1,aw,ah );
}

void QWinSphere::adjustToNewSize(void)
{
    double idealhd;
    double reqratio;
    double ratio;
    QString buf;
    struct P4POLYLINES * t;

    w=width();
    h=height();

    idealhd = w;
    idealhd = (idealhd/dx)*dy;

    idealh = (int)(idealhd+.5);

    reqratio = ( ((double)w)/horPixelsPerMM ) / ( idealh/verPixelsPerMM );
    ratio = ( ((double)w)/horPixelsPerMM ) / ( ((double)h)/verPixelsPerMM );

    buf.sprintf( "Aspect Ratio = %f\n", (float)(ratio/reqratio) );
    msgBar->showMessage(buf);

    while( CircleAtInfinity != NULL )
    {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        free( t );
    }
    while( PLCircle != NULL )
    {
        t = PLCircle;
        PLCircle = t->next;
        free( t );
    }
    if( VFResults.typeofview == TYPEOFVIEW_SPHERE )
    {
        CircleAtInfinity = produceEllipse( 0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0) );
        if( VFResults.plweights )
            PLCircle = produceEllipse( 0.0, 0.0, RADIUS, RADIUS, true, coWinH(RADIUS), coWinV(RADIUS) );
    }

    if( PainterCache != NULL )
    {
        delete PainterCache;
        PainterCache = new QPixmap( size() );
        isPainterCacheDirty = false;

        QPainter paint( PainterCache );
        paint.fillRect(0,0,width(),height(), QColor( QXFIGCOLOR(CBACKGROUND) ) );
        
        if( VFResults.singinf )
            paint.setPen( QXFIGCOLOR(CSING) );
        else
            paint.setPen( QXFIGCOLOR(CLINEATINFINITY) );

        staticPainter = &paint;

        // Mental note: do not use prepareDrawing/FinishDrawing here,
        // since it is not good to do drawing for all spheres every time we
        // get a paint event from windows

        if( VFResults.typeofview != TYPEOFVIEW_PLANE )
        {
            if( VFResults.typeofview == TYPEOFVIEW_SPHERE )
            {
                if( VFResults.plweights )
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            }
            else
                plotLineAtInfinity();
        }
        // during resizing : only plot essential information
//      plotSeparatrices();
//      plotGcf();
//      DrawOrbits(this);
//      DrawLimitCycles(this);
        plotPoints();

        QColor c  = QXFIGCOLOR(WHITE);
        c.setAlpha(128);
        paint.setPen(c);
        paint.drawText( 0, 0, width(), height(),
                           Qt::AlignHCenter | Qt::AlignVCenter, "Resizing ...  " );

        staticPainter = NULL;

        if( refreshTimeout != NULL )
            refreshTimeout->stop();
        else
        {
            refreshTimeout = new QTimer();
            connect( refreshTimeout, SIGNAL(timeout()), this, SLOT(refreshAfterResize()) );
        }
        refreshTimeout->start( 500 );
    }
}

void QWinSphere::refreshAfterResize( void )
{
     if( refreshTimeout != NULL )
     {
         delete refreshTimeout;
         refreshTimeout = NULL;
     }
    refresh();
}

void QWinSphere::resizeEvent( QResizeEvent * e )
{
    if( e->size() == e->oldSize() )
        return;

    adjustToNewSize( );
}

void QWinSphere::paintEvent( QPaintEvent * p )
{
    UNUSED(p);
    if( ThisVF->evaluating )
        return;

    if( PainterCache == NULL || isPainterCacheDirty )
    {
        if( PainterCache == NULL )
            PainterCache = new QPixmap( size() );
        isPainterCacheDirty = false;

        QPainter paint( PainterCache );
        paint.fillRect(0,0,width(),height(), QColor( QXFIGCOLOR(CBACKGROUND) ) );

        if( VFResults.singinf )
            paint.setPen( QXFIGCOLOR(CSING) );
        else
            paint.setPen( QXFIGCOLOR(CLINEATINFINITY) );

        staticPainter = &paint;

        // Mental note: do not use prepareDrawing/FinishDrawing here,
        // since it is not good to do drawing for all spheres every time we
        // get a paint event from windows

        if( VFResults.typeofview != TYPEOFVIEW_PLANE )
        {
            if( VFResults.typeofview == TYPEOFVIEW_SPHERE )
            {
                if( VFResults.plweights )
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            }
            else
                plotLineAtInfinity();
        }
        plotSeparatrices();
        plotGcf();
        DrawOrbits(this);
        DrawLimitCycles(this);
        plotPoints();
        staticPainter = NULL;
    }
    
    QPainter widgetpaint( this );
    widgetpaint.drawPixmap( 0, 0, *PainterCache );
    
    if( SelectingPointStep != 0 )
    {
        widgetpaint.setPen( QXFIGCOLOR(WHITE) );
        widgetpaint.setBrush( Qt::NoBrush );
        widgetpaint.drawEllipse( SelectingX-SelectingPointRadius,
                                 SelectingY-SelectingPointRadius,
                                 SelectingPointRadius+SelectingPointRadius,
                                 SelectingPointRadius+SelectingPointRadius );
    }
}

void QWinSphere::MarkSelection( int x1, int y1, int x2, int y2, int selectiontype )
{
    int bx1, by1, bx2, by2;
     
    if( PainterCache == NULL )
          return;
    
    bx1 = (x1<x2) ? x1 : x2;
    bx2 = (x1<x2) ? x2 : x1;
    by1 = (y1<y2) ? y1 : y2;
    by2 = (y1<y2) ? y2 : y1;
    
    if( bx1 < 0 ) bx1 = 0;
    if( by1 < 0 ) by1 = 0;
    if( bx2 >= width() ) bx2 = width()-1;
    if( by2 >= height() ) by2 = height()-1;

    QPainter p(PainterCache); 
    QColor c;

    switch(selectiontype )
    {
    case 0:
        c = QXFIGCOLOR(WHITE);
        c.setAlpha(32);
        p.setPen(QXFIGCOLOR(WHITE));
        p.setBrush(c);
        if( bx1 == bx2 || by1 == by2 )
           p.drawLine( bx1, by1, bx2, by2 );
        else
           p.drawRect( bx1, by1, bx2-bx1, by2-by1 );
        break;
    
    case 1:
        p.setPen( QXFIGCOLOR(WHITE) );
        p.drawLine( x1, y1, x2, y2 );
        break;
    }
    update( bx1, by1, bx2-bx1+1, by2-by1+1 );
}

void QWinSphere::mouseMoveEvent( QMouseEvent * e )
{
    int x, y;
    double wx, wy;
    double ucoord[2];
    QString buf;

    x = e->x();
    y = e->y();
    
    wx = coWorldX(x);
    wy = coWorldY(y);

/*  double ratio;
    ratio = ( ((double)w)/horPixelsPerMM ) / ( ((double)h)/verPixelsPerMM );
    sprintf( buf, "Aspect Ratio = %f\n", (float)ratio );
    msgBar->showMessage(buf);
    return;
*/
    double pcoord[3];
    if( MATHFUNC(is_valid_viewcoord)( wx, wy, pcoord ) )
    {
        switch( VFResults.typeofview )
        {
        case TYPEOFVIEW_PLANE:
            if( VFResults.typeofstudy == TYPEOFSTUDY_ONE )
                buf.sprintf( "Local study   (x,y) = (%f,%f)", (float)wx, (float)wy );
            else
                buf.sprintf( "Planar view   (x,y) = (%f,%f)", (float)wx, (float)wy );
            break;
        case TYPEOFVIEW_SPHERE:
            MATHFUNC(sphere_to_R2)( pcoord[0],pcoord[1],pcoord[2], ucoord );

            if( VFResults.p == 1 && VFResults.q == 1 )
                buf.sprintf( "The Poincare sphere  (x,y) = (%f,%f)",
                    (float)ucoord[0], (float)ucoord[1] );
            else
                buf.sprintf( "The P-L sphere of type (%d,%d)  (x,y) = (%f,%f)",
                    VFResults.p, VFResults.q, (float)ucoord[0], (float)ucoord[1] );
            break;
        case TYPEOFVIEW_U1:
            MATHFUNC(sphere_to_U1)( pcoord[0],pcoord[1],pcoord[2], ucoord );
            if( ucoord[1] >= 0 )
                buf.sprintf( "The U1 chart  (z2,z1) = (%f,%f) ", (float)ucoord[1], (float)ucoord[0] );
            else
                buf.sprintf( "The V1' chart (z2,z1) = (%f,%f) ", (float)ucoord[1], (float)ucoord[0] );
            buf.append( chartstring );
            break;
        case TYPEOFVIEW_V1:
            MATHFUNC(sphere_to_V1)( pcoord[0],pcoord[1],pcoord[2], ucoord );
            if( !VFResults.plweights ) { ucoord[0] = -ucoord[0]; ucoord[1] = -ucoord[1]; }
            if( ucoord[1] >= 0 )
                buf.sprintf( "The V1 chart  (z2,z1) = (%f,%f) ", (float)ucoord[1], (float)ucoord[0] );
            else
                buf.sprintf( "The U1' chart (z2,z1) = (%f,%f) ", (float)ucoord[1], (float)ucoord[0] );
            buf.append( chartstring );
            break;
        case TYPEOFVIEW_U2:
            MATHFUNC(sphere_to_U2)( pcoord[0],pcoord[1],pcoord[2], ucoord );
            if( ucoord[1] >= 0 )
                buf.sprintf( "The U2 chart  (z1,z2) = (%f,%f) ", (float)ucoord[0], (float)ucoord[1] );
            else
                buf.sprintf( "The V2' chart (z1,z2) = (%f,%f) ", (float)ucoord[0], (float)ucoord[1] );
            buf.append( chartstring );
            break;
        case TYPEOFVIEW_V2:
            MATHFUNC(sphere_to_V2)( pcoord[0],pcoord[1],pcoord[2], ucoord );
            if( !VFResults.plweights ) { ucoord[0] = -ucoord[0]; ucoord[1] = -ucoord[1]; }
            if( ucoord[1] >= 0 )
                buf.sprintf( "The V2 chart  (z1,z2) = (%f,%f) ", (float)ucoord[0], (float)ucoord[1] );
            else
                buf.sprintf( "The U2' chart (z1,z2) = (%f,%f) ", (float)ucoord[0], (float)ucoord[1] );
            buf.append( chartstring );
            break;
        }
    }
    else
    {
        switch( VFResults.typeofview )
        {
        case TYPEOFVIEW_PLANE:
            if( VFResults.typeofstudy == TYPEOFSTUDY_ONE )
                buf.sprintf( "Local study" );
            else
                buf.sprintf( "Planar view" );
            break;
        case TYPEOFVIEW_SPHERE:
            if( VFResults.p == 1 && VFResults.q == 1 )
                buf.sprintf( "The Poincare sphere" );
            else
                buf.sprintf( "The P-L sphere of type (%d,%d)", VFResults.p, VFResults.q );
            break;
        case TYPEOFVIEW_U1:     buf.sprintf( "The U1 chart" );  break;
        case TYPEOFVIEW_V1:     buf.sprintf( "The V1 chart" );  break;
        case TYPEOFVIEW_U2:     buf.sprintf( "The U2 chart" );  break;
        case TYPEOFVIEW_V2:     buf.sprintf( "The V2 chart" );  break;
        }
    }

    msgBar->showMessage( buf );

    if( selectingZoom )
    {
        SaveAnchorMap();
        zoomAnchor2 = e->pos();
        LoadAnchorMap();
        MarkSelection( zoomAnchor1.x(), zoomAnchor1.y(), zoomAnchor2.x(), zoomAnchor2.y(), 0 );
    }

    if( selectingLCSection )
    {
        SaveAnchorMap();
        lcAnchor2 = e->pos();
        LoadAnchorMap();
        MarkSelection( lcAnchor1.x(), lcAnchor1.y(), lcAnchor2.x(), lcAnchor2.y(), 1 );
    }
}


double QWinSphere::coWorldX( int x )
{
    double wx;

    wx = (double)x;
    wx /= (w-1);
    return (wx*dx + x0);
}

double QWinSphere::coWorldY( int y )
{
    double wy;

    wy = (double)(h-1-y);
    wy /= (h-1);
    return (wy*dy + y0);
}

int QWinSphere::coWinX( double x )
{
    double wx;
    int iwx;

    wx = (x-x0)/dx;
    wx *= w-1;

    iwx = (int)(wx+0.5);        // +0.5 to round upwards
    if( iwx >= w )
        iwx = w-1;

    return iwx;
}

int QWinSphere::coWinH( double deltax )
{
    double wx;

    wx = deltax/dx;
    wx *= w-1;
    return (int)(wx+0.5);
}

int QWinSphere::coWinV( double deltay )
{
    double wy;

    wy = deltay/dy;
    wy *= h-1;
    return (int)(wy+0.5);
}

int QWinSphere::coWinY( double y )
{
    double wy;
    int iwy;

    wy = (y-y0)/dy;
    wy *= h-1;

    iwy = (int)(wy+0.5);        // +0.5 to round upwards
    if( iwy >= h )
        iwy = h-1;

    return ( ReverseYaxis ) ? iwy : h-1-iwy;        // on screen: vertical axis orientation is reversed
}

void QWinSphere::mousePressEvent( QMouseEvent * e )
{
    if( e->modifiers() == Qt::ControlModifier && e->button() == Qt::LeftButton )
    {
        if( !selectingZoom )
        {
            selectingZoom = true;
            zoomAnchor1 = e->pos();
            zoomAnchor2 = zoomAnchor1;
            LoadAnchorMap();
            MarkSelection( zoomAnchor1.x(), zoomAnchor1.y(), zoomAnchor2.x(), zoomAnchor2.y(), 0 );
        }
        QWidget::mousePressEvent(e);
        return;
    }

    if( e->modifiers() == Qt::ShiftModifier && e->button() == Qt::LeftButton )
    {
        // select nearest singularity having separatrices, and open the
        // separatrices window.

        SelectNearestSingularity( e->pos() );

        QWidget::mousePressEvent(e);
        return;
    }

    if( e->button() == Qt::LeftButton && (lcWindowIsUp || e->modifiers() == Qt::AltModifier) )
    {
        if( selectingLCSection == false )
        {
            selectingLCSection = true;
            lcAnchor1 = e->pos();
            lcAnchor2 = lcAnchor1;

            LoadAnchorMap();
            MarkSelection( lcAnchor1.x(), lcAnchor1.y(), lcAnchor2.x(), lcAnchor2.y(), 0 );
        }
        QWidget::mousePressEvent(e);
        return;
    }

    if( e->button() == Qt::LeftButton )
    {
        // normally, start integrating new orbit at the chosen point.
        //
        // However, when the limit cycle window is open, select the first
        // and second point of a transverse section.

        struct DOUBLEPOINT * data1 = (struct DOUBLEPOINT *)malloc( sizeof(struct DOUBLEPOINT) + sizeof( void * ) );
        data1->x = coWorldX( e->x() );
        data1->y = coWorldY( e->y() );
        *((void **)(data1+1)) = this;

        double pcoord[3];
        if( MATHFUNC(is_valid_viewcoord)( data1->x, data1->y, pcoord ) )
        {
            QP4Event * e1;
            e1 = new QP4Event( (QEvent::Type)TYPE_SELECT_ORBIT, data1 );
            p4app->postEvent( parentWnd, e1 );
        }
        else
        {
            free(data1);
        }

        QWidget::mousePressEvent(e);
        return;
    }

    if( e->button() == Qt::RightButton )
    {
        // cancel zoom window with right mouse button

        if( selectingZoom )
        {
            SaveAnchorMap();
            selectingZoom = false;
        }

        if( selectingLCSection )
        {
            SaveAnchorMap();
            selectingLCSection = false;
        }

        QWidget::mousePressEvent(e);
        return;
    }

    QWidget::mousePressEvent(e);
}

void QWinSphere::mouseReleaseEvent( QMouseEvent * e )
{
    if( e->button() == Qt::LeftButton )
    {
        // finish zoom window between zoomAnchor1 and zoomAnchor2

        if( selectingZoom )
        {
            SaveAnchorMap();
            selectingZoom = false;

            double * data1 = (double *)malloc( sizeof(double) * 4 );
            data1[0] = coWorldX( zoomAnchor1.x() );
            data1[1] = coWorldY( zoomAnchor1.y() );
            data1[2] = coWorldX( zoomAnchor2.x() );
            data1[3] = coWorldY( zoomAnchor2.y() );
            QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_OPENZOOMWINDOW, data1 );
            p4app->postEvent( parentWnd, e1 );
        }

        if( selectingLCSection )
        {
            SaveAnchorMap();
            selectingLCSection = false;

            double * data1 = (double *)malloc( sizeof(double) * 4 );
            data1[0] = coWorldX( lcAnchor1.x() );
            data1[1] = coWorldY( lcAnchor1.y() );
            data1[2] = coWorldX( lcAnchor2.x() );
            data1[3] = coWorldY( lcAnchor2.y() );
            QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_SELECT_LCSECTION, data1 );
            p4app->postEvent( parentWnd, e1 );
        }

        QWidget::mouseReleaseEvent(e);
        return;
    }

    QWidget::mouseReleaseEvent(e);
}

bool QWinSphere::getChartPos( int chart, double x0, double y0, double * pos )
{
    double pcoord[3];

    switch( chart )
    {
    case CHART_R2:
        MATHFUNC(finite_to_viewcoord)( x0, y0, pos );
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)( x0, 0, pcoord );
        MATHFUNC(sphere_to_viewcoord)( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)( x0, 0, pcoord );
        MATHFUNC(sphere_to_viewcoord)( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)( x0, 0, pcoord );
        MATHFUNC(sphere_to_viewcoord)( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)( x0, 0, pcoord );
        MATHFUNC(sphere_to_viewcoord)( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    }

    return true;
}

void QWinSphere::updatePointSelection( void )
{
     if( SelectingPointStep == 0 )
     {
         SelectingPointRadius = 0;
         update( SelectingX - SELECTINGPOINTSTEPS*4, SelectingY - SELECTINGPOINTSTEPS*4,
                              SELECTINGPOINTSTEPS*4*2+1, SELECTINGPOINTSTEPS*4*2+1 );
         return;
     }
     
     SelectingPointStep--;
     SelectingPointRadius = (SELECTINGPOINTSTEPS-SelectingPointStep)*4;
     update( SelectingX - SelectingPointRadius, SelectingY - SelectingPointRadius,
             SelectingPointRadius + SelectingPointRadius + 1,
             SelectingPointRadius + SelectingPointRadius + 1 );
}

void QWinSphere::SelectNearestSingularity( QPoint winpos )
{
    bool result;
    int x, y;
    int px, py;

    x = winpos.x();
    y = winpos.y();

    (*SphereList)->prepareDrawing();
    result = find_critical_point( *SphereList, coWorldX( x ), coWorldY( y ) );
    (*SphereList)->finishDrawing();

    if( result == false )
    {
        msgBar->showMessage( "Search nearest critical point: None with separatrices found." );
    }
    else
    {
        px = coWinX(VFResults.selected_ucoord[0]);
        py = coWinY(VFResults.selected_ucoord[1]);

        if( SelectingTimer != NULL )
        {
            delete SelectingTimer;
            SelectingTimer = NULL;
            SelectingPointStep=0;
            updatePointSelection();
        }

        SelectingPointStep=SELECTINGPOINTSTEPS-1;
        SelectingX=px;
        SelectingY=py;
        
        SelectingTimer = new QTimer();
        connect( SelectingTimer, SIGNAL(timeout()), this, SLOT(updatePointSelection()) );
        SelectingTimer->start(SELECTINGPOINTSPEED);
        msgBar->showMessage( "Search nearest critical point: Found" );

        int * data1;
        data1 = (int *)malloc( sizeof(int) );
        *data1 = -1;
        QP4Event * e1 = new QP4Event( (QEvent::Type)TYPE_SEP_EVENT, data1 );
        p4app->postEvent( parentWnd, e1 );
    }
}

void QWinSphere::drawLine( double _x1, double _y1, double _x2, double _y2, int color )
{
    int wx1, wy1, wx2, wy2;

    if( staticPainter != NULL )
    {
        if( _x1 >= x0 && _x1 <= x1 && _y1 >= y0 && _y1 <= y1 )
        {
            wx1 = coWinX(_x1);
            wy1 = coWinY(_y1);

            if( _x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1 )
            {
                // both points are visible in the window

                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);

                if( paintedXMin > wx1 ) paintedXMin = wx1;
                if( paintedXMax < wx1 ) paintedXMax = wx1;
                if( paintedYMin > wy1 ) paintedYMin = wy1;
                if( paintedYMax < wy1 ) paintedYMax = wy1;
                if( paintedXMin > wx2 ) paintedXMin = wx2;
                if( paintedXMax < wx2 ) paintedXMax = wx2;
                if( paintedYMin > wy2 ) paintedYMin = wy2;
                if( paintedYMax < wy2 ) paintedYMax = wy2;

                staticPainter->setPen( QXFIGCOLOR(color) );
                staticPainter->drawLine( wx1, wy1, wx2, wy2 );
            }
            else
            {
                // only (_x2,_y2) is not visible

                if( LineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) )
                {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen( QXFIGCOLOR(color) );

                    if( paintedXMin > wx1 ) paintedXMin = wx1;
                    if( paintedXMax < wx1 ) paintedXMax = wx1;
                    if( paintedYMin > wy1 ) paintedYMin = wy1;
                    if( paintedYMax < wy1 ) paintedYMax = wy1;
                    if( paintedXMin > wx2 ) paintedXMin = wx2;
                    if( paintedXMax < wx2 ) paintedXMax = wx2;
                    if( paintedYMin > wy2 ) paintedYMin = wy2;
                    if( paintedYMax < wy2 ) paintedYMax = wy2;

                    staticPainter->drawLine( wx1, wy1, wx2, wy2 );
                }
            }
        }
        else
        {
            if( _x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1 )
            {
                // only (_x2,_y2) is visible

                if( LineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) )
                {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen( QXFIGCOLOR(color) );

                    if( paintedXMin > wx1 ) paintedXMin = wx1;
                    if( paintedXMax < wx1 ) paintedXMax = wx1;
                    if( paintedYMin > wy1 ) paintedYMin = wy1;
                    if( paintedYMax < wy1 ) paintedYMax = wy1;
                    if( paintedXMin > wx2 ) paintedXMin = wx2;
                    if( paintedXMax < wx2 ) paintedXMax = wx2;
                    if( paintedYMin > wy2 ) paintedYMin = wy2;
                    if( paintedYMax < wy2 ) paintedYMax = wy2;

                    staticPainter->drawLine( wx1, wy1, wx2, wy2 );
                }
            }
            else
            {
                // both end points are invisible

                if( LineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) )
                {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen( QXFIGCOLOR(color) );

                    if( paintedXMin > wx1 ) paintedXMin = wx1;
                    if( paintedXMax < wx1 ) paintedXMax = wx1;
                    if( paintedYMin > wy1 ) paintedYMin = wy1;
                    if( paintedYMax < wy1 ) paintedYMax = wy1;
                    if( paintedXMin > wx2 ) paintedXMin = wx2;
                    if( paintedXMax < wx2 ) paintedXMax = wx2;
                    if( paintedYMin > wy2 ) paintedYMin = wy2;
                    if( paintedYMax < wy2 ) paintedYMax = wy2;
                    
                    staticPainter->drawLine( wx1, wy1, wx2, wy2 );
                }
            }
        }
    }
}

void QWinSphere::drawPoint( double x, double y, int color )
{
    int _x, _y;
    if( staticPainter != NULL )
    {
        if( x < x0 || x > x1 || y < y0 || y > y1 )
            return;
        staticPainter->setPen( QXFIGCOLOR(color) );
        _x=coWinX(x);
        _y=coWinY(y);

        if( paintedXMin > _x ) paintedXMin = _x;
        if( paintedXMax < _x ) paintedXMax = _x;
        if( paintedYMin > _y ) paintedYMin = _y;
        if( paintedYMax < _y ) paintedYMax = _y;

        staticPainter->drawPoint( _x, _y );
    }
}

void QWinSphere::printLine( double _x1, double _y1, double _x2, double _y2, int color )
{
    int wx1, wy1, wx2, wy2;

    if( _x1 >= x0 && _x1 <= x1 && _y1 >= y0 && _y1 <= y1 )
    {
        wx1 = coWinX(_x1);
        wy1 = coWinY(_y1);

        if( _x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1 )
        {
            // both points are visible in the window

            wx2 = coWinX(_x2);
            wy2 = coWinY(_y2);

            print_line( wx1, wy1, wx2, wy2, color );
        }
        else
        {
            // only (_x2,_y2) is not visible

            if( LineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) )
            {
                wx1 = coWinX(_x1);
                wy1 = coWinY(_y1);
                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);
                print_line( wx1, wy1, wx2, wy2, color );
            }
        }
    }
    else
    {
        if( _x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1 )
        {
            // only (_x2,_y2) is visible

            if( LineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) )
            {
                wx1 = coWinX(_x1);
                wy1 = coWinY(_y1);
                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);
                print_line( wx1, wy1, wx2, wy2, color );
            }
        }
        else
        {
            // both end points are invisible

            if( LineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) )
            {
                wx1 = coWinX(_x1);
                wy1 = coWinY(_y1);
                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);
                print_line( wx1, wy1, wx2, wy2, color );
            }
        }
    }
}

void QWinSphere::printPoint( double x, double y, int color )
{
    if( x < x0 || x > x1 || y < y0 || y > y1 )
        return;
    
    print_point( coWinX(x), coWinY(y), color );
}

void QWinSphere::refresh( void )
{
    isPainterCacheDirty = true;
    update();
}

void QWinSphere::CalculateHeightFromWidth( int * width, int * height, int maxheight = -1, double aspectratio = 1  )
{
    // given an optimal width in *width, this procedure calculates the
    // corresponding height in order to maintain the given aspectratio
    // If however the maximum height is violated, then we choose to
    // have the maximum height and calculate the corresponding width.

    // aspect ratio is 

    double w, h;
    
    w = (double)(*width);
    h = w * dy / dx;
    h *= aspectratio;

    if( (int)(h+0.5) <= maxheight || maxheight == -1 )
    {
        *height = (int)(h+0.5);
    }
    else
    {
        *height = maxheight;
        
        h = (double)maxheight;
        w = h * dx /dy;
        w /= aspectratio;

        *width = (int)(w+0.5);
    }
}

void QWinSphere::preparePrinting( int printmethod, bool isblackwhite, int myresolution, double mylinewidth, double mysymbolsize )
{
    double aspectratio, lw, ss, hpixels, maxvpixels, pagewidth, pageheight, tx, ty;

    PrintMethod = printmethod;

    aspectratio = 1;        // assume aspect ratio 1

    if( printmethod == P4PRINT_DEFAULT )
    {
        // p4printer->setResolution(myresolution); we have moved this.
        // according to documentation: set of resolution must be done BEFORE setup!
    
        //  aspectratio = metrics.logicalDpiX()/metrics.logicalDpiY();
        aspectratio = 1;

        pagewidth = p4printer->width();
        pageheight = p4printer->height();
    }
    else
        pagewidth = pageheight = -1;  // will be redefined in a minut

    p4pixmap_dpm = lw = ss = hpixels = myresolution;
    p4pixmap_dpm /= 2.54;
    hpixels *= 15;
    hpixels /= 2.54;

    maxvpixels = myresolution;
    maxvpixels *= aspectratio;
    maxvpixels *= 20;
    maxvpixels /= 2.54;
    maxvpixels += 0.5;

    oldw = w;
    oldh = h;
    w = (int)(hpixels+0.5);

    switch( printmethod )
    {
    case P4PRINT_DEFAULT:   /* pagewidth and height already set */ break;
    case P4PRINT_JPEGIMAGE: pagewidth = -1; pageheight = -1;    break;
    case P4PRINT_XFIGIMAGE: pagewidth = -1; pageheight = -1;    break;
    case P4PRINT_EPSIMAGE:
        pagewidth = POSTSCRIPTPAGEWIDTH;    // see custom.cpp
        pageheight = POSTSCRIPTPAGEHEIGHT;
        pagewidth *= myresolution;
        pageheight *= myresolution;
        break;
    }

    if( w > pagewidth && pagewidth != -1 )
        w = (int)pagewidth;
    if( maxvpixels > pageheight && pageheight != -1 )
        maxvpixels = pageheight;

    CalculateHeightFromWidth( &w, &h, (int)maxvpixels, aspectratio );

    lw /= 25.4;         // dots per mm
    lw *= mylinewidth;  // linewidth in pixels
    lw += 0.5;          // prepare round above
    if( lw < 1.0 )
        lw = 1.0;

    ss /= 25.4;         // dots per mm
    ss *= mysymbolsize; // symbolsize in pixels
    ss += 0.5;          // prepare round above for units of 2
    ss /= 2.0;
    if( ss < 1.0 )
        ss = 1.0;

    if( pagewidth == -1 || pageheight == -1 )
    {
        tx = 0;
        ty = 0;
    }
    else
    {
        tx = pagewidth;
        tx -= w;
        tx /= 2;
        ty = pageheight;
        ty -= h;
        ty /= 2;
    }

    switch( printmethod )
    {
    case P4PRINT_EPSIMAGE:
        ReverseYaxis = true;
        PreparePostscriptPrinting( (int)(tx+0.5), (int)(ty+0.5), w, h, iszoom, isblackwhite,
                                        myresolution, (int)lw, 2*(int)ss );
        break;
    case P4PRINT_XFIGIMAGE:
        ReverseYaxis = false;
        PrepareXFigPrinting( w, h, iszoom, isblackwhite, myresolution, (int)lw, 2*(int)ss );
        break;
    case P4PRINT_DEFAULT:
        staticPainter = new QPainter();

        if( !staticPainter->begin( p4printer ) )
        {
            delete staticPainter;
            staticPainter = NULL;
            return;
        }

        staticPainter->translate( tx, ty );
        if( iszoom || VFResults.typeofview == TYPEOFVIEW_PLANE )
        {
            QPen p = QPen( QXFIGCOLOR(PrintColorTable[CFOREGROUND]), (int)lw );
            staticPainter->setPen( p );
            staticPainter->drawRect( 0, 0, w, h );
        }
//      staticPainter->setClipRect( (int)tx, (int)ty, w, h );
        ReverseYaxis = false;  // no need for reversing axes in this case
        PrepareP4Printing( w, h, isblackwhite, staticPainter, (int)lw, 2*(int)ss );
        break;

    case P4PRINT_JPEGIMAGE:
        staticPainter = new QPainter();
        p4pixmap = new QPixmap( w, h );
        ReverseYaxis = false;  // no need for reversing axes in this case
        if( p4pixmap->isNull() )
        {
            msgBar->showMessage( "Print failure (try to choose a lower resolution)." );
            delete p4pixmap;
            p4pixmap = NULL;
            delete staticPainter;
            staticPainter = NULL;
            return;
        }
        if( !staticPainter->begin( p4pixmap ) )
        {
            delete p4pixmap;
            p4pixmap = NULL;
            delete staticPainter;
            staticPainter = NULL;
            return;
        }

        PrepareP4Printing( w, h, isblackwhite, staticPainter, (int)lw, 2*(int)ss );
        break;
    }

    msgBar->showMessage( "Printing ..." );
}

void QWinSphere::finishPrinting( void )
{
    if( PrintMethod == P4PRINT_EPSIMAGE )
    {
        FinishPostscriptPrinting();
        ReverseYaxis = false;
        w = oldw;
        h = oldh;
    }
    else if( PrintMethod == P4PRINT_XFIGIMAGE )
    {
        FinishXFigPrinting();
        ReverseYaxis = false;
        w = oldw;
        h = oldh;
    }
    else if( PrintMethod == P4PRINT_DEFAULT )
    {
        FinishP4Printing();
        staticPainter->end();
        delete staticPainter;
        staticPainter = NULL;
        w = oldw;
        h = oldh;
        ReverseYaxis = false;
    }
    else if( PrintMethod == P4PRINT_JPEGIMAGE )
    {
        if( p4pixmap == NULL )
        {
            FinishP4Printing();
            w = oldw;
            h = oldh;
            ReverseYaxis = false;
            return;
        }

        FinishP4Printing();
        staticPainter->end();
        delete staticPainter;
        staticPainter = NULL;

        if( p4pixmap->save( ThisVF->getbarefilename() + ".jpg", "JPEG", 100 ) == false )
        {
            QMessageBox::critical( this, "P4",
                    "For some reason, P4 is unable to save the resulting JPEG image to disc." );

        }

        delete p4pixmap;
        p4pixmap = NULL;
        ReverseYaxis = false;
        w = oldw;
        h = oldh;
    }
    msgBar->showMessage( "Printing has finished." );
}

void QWinSphere::print( void )
{
    if( PrintMethod == P4PRINT_JPEGIMAGE && p4pixmap == NULL )
        return;

    if( VFResults.typeofview != TYPEOFVIEW_PLANE )
    {
        if( VFResults.typeofview == TYPEOFVIEW_SPHERE )
        {
            if( VFResults.plweights )
                printPoincareLyapunovSphere();
            else
                printPoincareSphere();
        }
        else
            printLineAtInfinity();
    }
    printOrbits();
    printSeparatrices();
    printGcf();
    printLimitCycles();
    printPoints();
}

void QWinSphere::prepareDrawing()
{
    if( PainterCache == NULL )
    {
        isPainterCacheDirty = true;
        PainterCache = new QPixmap( size() );
    }
    staticPainter = new QPainter(PainterCache);
    
    paintedXMin = width()-1;
    paintedYMin = height()-1;
    paintedXMax = 0;
    paintedYMax = 0;
    
    if( next != NULL )
        next->prepareDrawing();
}

void QWinSphere::finishDrawing()
{
    if( next != NULL )
        next->finishDrawing();

    if( staticPainter != NULL )
    {
        staticPainter->end();
        delete staticPainter;
        staticPainter = NULL;

        if( paintedXMin < 0 ) paintedXMin = 0;
        if( paintedXMax >= width() ) paintedXMax = width()-1;
        if( paintedYMin < 0 ) paintedYMin = 0;
        if( paintedYMax >= height() ) paintedYMax = height()-1;

        if( paintedYMax >= paintedYMin && paintedXMax >= paintedXMin )
            update( paintedXMin, paintedYMin, paintedXMax-paintedXMin+1, paintedYMax-paintedYMin+1);
    }
}

void QWinSphere::Signal_Evaluating( void )
{
/*
    QPalette palette;
    palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor = CBACKGROUND) );
    setPalette(palette);
*/
}

void QWinSphere::Signal_Changed( void )
{
/*
    QPalette palette;
    palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor = DARKGRAY) );
    setPalette(palette);
*/
}
