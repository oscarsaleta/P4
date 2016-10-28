#ifndef WIN_SPHERE_H
#define WIN_SPHERE_H


#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPoint>
#include <QResizeEvent>
#include <QStatusBar>
#include <QString>
#include <QTimer>
#include <QWidget>

#include "file_tab.h"
#include "win_p4.h"


class QWinSphere : public QWidget
{
    Q_OBJECT

    static int numSpheres;
    static QWinSphere * * SphereList; //this could be implemented as a c++ vector

public:
    QWinSphere( QWidget *, QStatusBar *, bool, double, double, double, double );
    ~QWinSphere();
    void paintEvent( QPaintEvent * );

    int coWinX( double x );     // coordinate changes: from world to windows coordinates
    int coWinY( double y );
    double coWorldX( int x );   // from windows to world coordinates
    double coWorldY( int y );
    int coWinV( double );
    int coWinH( double );

    double horPixelsPerMM;
    double verPixelsPerMM;

    double x0, y0;              // world-coordinates of upper-left corner
    double x1, y1;              // world-coordinates of upper-right corner
    double dx;                  // x1-x0
    double dy;                  // y1-y0

    QPixmap * PainterCache;
    bool isPainterCacheDirty;
    int paintedXMin;            // to know the update rectangle after painting
    int paintedXMax;            // we keep to smallest rectangle enclosing
    int paintedYMin;            // all painted objects.
    int paintedYMax;

    QString chartstring;

private:
    QPainter * staticPainter;
    QWidget * parentWnd;
    bool iszoom;
    bool ReverseYaxis;          // when calculating coordinates: this determines orientation
                                // of horizontal axis.  Normally false, only true when printing.
    
    P4POLYLINES * CircleAtInfinity;
    P4POLYLINES * PLCircle;
    QTimer * refreshTimeout;

public:
    int spherebgcolor;
    QWinSphere * next;          // visible to PlotWnd
    int SelectingX, SelectingY, SelectingPointStep, SelectingPointRadius;
    QTimer * SelectingTimer;

    bool getChartPos( int, double, double, double * );
    void adjustToNewSize( void );

    void Signal_Evaluating( void );
    void Signal_Changed( void );
    void plotPoint( struct saddle * );
    void plotPoint( struct node * );
    void plotPoint( struct semi_elementary * );
    void plotPoint( struct weak_focus * );
    void plotPoint( struct strong_focus * );
    void plotPoint( struct degenerate * );
    void plotPointSeparatrices( struct semi_elementary * p );
    void plotPointSeparatrices( struct saddle * p );
    void plotPointSeparatrices( struct degenerate * p );
    void plotPoints( void );
    void plotSeparatrices( void );
    void plotGcf( void );
    void plotPoincareSphere( void );
    void plotPoincareLyapunovSphere( void );
    void plotLineAtInfinity( void );
    void MarkSelection( int x1, int y1, int x2, int y2, int selectiontype );

    void printPoint( struct saddle * );
    void printPoint( struct node * );
    void printPoint( struct semi_elementary * );
    void printPoint( struct weak_focus * );
    void printPoint( struct strong_focus * );
    void printPoint( struct degenerate * );
    void printPointSeparatrices( struct semi_elementary * p );
    void printPointSeparatrices( struct saddle * p );
    void printPointSeparatrices( struct degenerate * p );
    void printPoints( void );
    void printSeparatrices( void );
    void printGcf( void );
    void printPoincareSphere( void );
    void printPoincareLyapunovSphere( void );
    void printLineAtInfinity( void );

    struct P4POLYLINES * produceEllipse( double cx, double cy, double a, double b, bool dotted, double resa, double resb );

    void SelectNearestSingularity( QPoint winpos );

    void prepareDrawing( void );
    void drawPoint( double x, double y, int color );
    void drawLine( double x1, double y1, double x2, double y2, int color );
    void finishDrawing( void );
    void printOrbits( void );
    void printLimitCycles( void );

    void preparePrinting( int, bool, int, double, double );
    void printPoint( double x, double y, int color );
    void print( void );
    void printLine( double x1, double y1, double x2, double y2, int color );
    void finishPrinting( void );
    
    void SaveAnchorMap( void );
    void LoadAnchorMap( void );

public slots:
    void resizeEvent( QResizeEvent * e );
    void mouseMoveEvent( QMouseEvent * e );
    void mousePressEvent( QMouseEvent * e );
    void mouseReleaseEvent( QMouseEvent * e );
    void SetupPlot( void );
    void refresh( void );
    void keyPressEvent ( QKeyEvent * e );
    void CalculateHeightFromWidth( int * width, int * height, int maxheight, double aspectratio );
    void refreshAfterResize( void );
    void updatePointSelection( void );

private:
    bool selectingZoom;
    bool selectingLCSection;
    QPoint zoomAnchor1;
    QPoint zoomAnchor2;
    QPoint lcAnchor1;
    QPoint lcAnchor2;
    QPixmap * AnchorMap;

    QStatusBar * msgBar;
    int PrintMethod;

public:
    int oldw;                       // used while printing
    int oldh;
    int w;                          // width of window
    int h;                          // height of window
    int idealh;                     // ideal height of window to get good aspect ratio
};


#endif /* WIN_SPHERE_H */
