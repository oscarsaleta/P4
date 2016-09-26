#include <qbrush.h>
#include <qcolor.h>
#include <qpen.h>
#include <qpainter.h>
#include <math.h>
#include "custom.h"
#include "table.h"
#include "win_p4.h"
#include "math_p4.h"

int PrintColorTable[NUMXFIGCOLORS] =

#ifdef PRINT_REVERSE_BLACK_AND_WHITE
{
    WHITE,          // black --> white when printing
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BLACK,          // yellow --> black when printing
    BLACK,          // white --> black when printing
    BLUE1,
    BLUE2,
    BLUE3,
    BLUE4,
    GREEN1,
    GREEN2,
    GREEN3,
    CYAN1,
    CYAN2,
    CYAN3,
    RED1,
    RED2,
    RED3,
    MAGENTA1,
    MAGENTA2,
    MAGENTA3,
    BROWN1,
    BROWN2,
    BROWN3,
    PINK1,
    PINK2,
    PINK3,
    PINK4,
    GOLD
};
#else
{
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    YELLOW,
    WHITE,
    BLUE1,
    BLUE2,
    BLUE3,
    BLUE4,
    GREEN1,
    GREEN2,
    GREEN3,
    CYAN1,
    CYAN2,
    CYAN3,
    RED1,
    RED2,
    RED3,
    MAGENTA1,
    MAGENTA2,
    MAGENTA3,
    BROWN1,
    BROWN2,
    BROWN3,
    PINK1,
    PINK2,
    PINK3,
    PINK4,
    GOLD
};
#endif
extern void (* print_saddle)( double, double );
extern void (* print_stablenode)( double, double );
extern void (* print_unstablenode)( double, double );
extern void (* print_stableweakfocus)( double, double );
extern void (* print_unstableweakfocus)( double, double );
extern void (* print_weakfocus)( double, double );
extern void (* print_stablestrongfocus)( double, double );
extern void (* print_unstablestrongfocus)( double, double );
extern void (* print_sesaddle)( double, double );
extern void (* print_sesaddlenode)( double, double );
extern void (* print_sestablenode)( double, double );
extern void (* print_seunstablenode)( double, double );
extern void (* print_degen)( double, double );
extern void (* print_center)( double, double );
extern void (* print_elips)( double, double, double, double, int, bool, struct P4POLYLINES * );
extern void (* print_line)( double, double, double, double, int );
extern void (* print_point)( double, double, int );
extern void (* print_comment)( QString );

extern void spherePlotLine( QWinSphere * sp, double * p1, double * p2, int color );
extern void spherePlotPoint( QWinSphere * sp, double * p, int color );
extern void spherePrintLine( QWinSphere * sp, double * p1, double * p2, int color );
extern void spherePrintPoint( QWinSphere * sp, double * p, int color );

static bool P4PrintBlackWhite = true;

static QPainter * P4PrintPainter = NULL;

static int P4PrintLineWidth = 0;
static int P4PrintSymbolWidth = 0;
static int LastP4PrintX0 = 0;
static int LastP4PrintY0 = 0;
static int LastP4Printcolor = 0;

// ---------------------------------------------------------------------------------------

extern void win_plot_saddle( QPainter * p, int x, int y );
extern void win_plot_stablenode( QPainter * p, int x, int y );
extern void win_plot_unstablenode( QPainter * p, int x, int y );
extern void win_plot_weakfocus( QPainter * p, int x, int y );
extern void win_plot_stableweakfocus( QPainter * p, int x, int y );
extern void win_plot_unstableweakfocus( QPainter * p, int x, int y );
extern void win_plot_center( QPainter * p, int x, int y );
extern void win_plot_stablestrongfocus( QPainter * p, int x, int y );
extern void win_plot_unstablestrongfocus( QPainter * p, int x, int y );
extern void win_plot_sesaddlenode( QPainter * p, int x, int y );
extern void win_plot_sestablenode( QPainter * p, int x, int y );
extern void win_plot_seunstablenode( QPainter * p, int x, int y );
extern void win_plot_sesaddle( QPainter * p, int x, int y );
extern void win_plot_degen( QPainter * p, int x, int y );

void P4Print_comment( QString s )
{
    UNUSED(s);
    // do nothing
}

void P4Print_print_saddle( double _x, double _y )
{
    int x;
    int y;
    int color;
    x=(int)_x;
    y=(int)_y;

    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CSADDLE];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );
    
    // print box:
    P4PrintPainter->drawRect( x-P4PrintSymbolWidth/2, y-P4PrintSymbolWidth/2, P4PrintSymbolWidth, P4PrintSymbolWidth );

}
void P4Print_print_stablenode( double _x, double _y )
{
    int x;
    int y;
    int color;

    x=(int)_x;
    y=(int)_y;

    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CNODE_S];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );

    // print box:
    P4PrintPainter->drawRect( x-P4PrintSymbolWidth/2, y-P4PrintSymbolWidth/2, P4PrintSymbolWidth, P4PrintSymbolWidth );
}
void P4Print_print_unstablenode( double _x, double _y )
{
    int x;
    int y;
    int color;
    x=(int)_x;
    y=(int)_y;

    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CNODE_U];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );

    // print box:
    P4PrintPainter->drawRect( x-P4PrintSymbolWidth/2, y-P4PrintSymbolWidth/2, P4PrintSymbolWidth, P4PrintSymbolWidth );
}
void P4Print_print_stableweakfocus( double _x, double _y )
{
    int x;
    int y;
    int color;
    
    x=(int)_x;
    y=(int)_y;
    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CWEAK_FOCUS_S];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );

    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x,y-P4PrintSymbolWidth*13/20,x+P4PrintSymbolWidth*13/20,y,
        x,y+P4PrintSymbolWidth*13/20,x-P4PrintSymbolWidth*13/20,y);

    P4PrintPainter->drawPolygon( qpa, Qt::OddEvenFill );
}
void P4Print_print_unstableweakfocus( double _x, double _y )
{
    int x;
    int y;
    int color;

    x=(int)_x;
    y=(int)_y;
    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CWEAK_FOCUS_U];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );

    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x,y-P4PrintSymbolWidth*13/20,x+P4PrintSymbolWidth*13/20,y,
        x,y+P4PrintSymbolWidth*13/20,x-P4PrintSymbolWidth*13/20,y);

    P4PrintPainter->drawPolygon( qpa, Qt::OddEvenFill );
}
void P4Print_print_weakfocus( double _x, double _y )
{
    int x;
    int y;
    int color;

    x=(int)_x;
    y=(int)_y;

    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CWEAK_FOCUS];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );

    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x,y-P4PrintSymbolWidth*13/20,x+P4PrintSymbolWidth*13/20,y,
        x,y+P4PrintSymbolWidth*13/20,x-P4PrintSymbolWidth*13/20,y);

    P4PrintPainter->drawPolygon( qpa, Qt::OddEvenFill );
}
void P4Print_print_center( double _x, double _y )
{
    int x;
    int y;
    int color;

    x=(int)_x;
    y=(int)_y;

    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CCENTER];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );
    
    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x,y-P4PrintSymbolWidth*13/20,x+P4PrintSymbolWidth*13/20,y,
        x,y+P4PrintSymbolWidth*13/20,x-P4PrintSymbolWidth*13/20,y);

    P4PrintPainter->drawPolygon( qpa, Qt::OddEvenFill );
}
void P4Print_print_stablestrongfocus( double _x, double _y )
{
    int x;
    int y;
    int color;

    x=(int)_x;
    y=(int)_y;
    
    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CSTRONG_FOCUS_S];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );
    
    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x,y-P4PrintSymbolWidth*13/20,x+P4PrintSymbolWidth*13/20,y,
        x,y+P4PrintSymbolWidth*13/20,x-P4PrintSymbolWidth*13/20,y);

    P4PrintPainter->drawPolygon( qpa, Qt::OddEvenFill );
}
void P4Print_print_unstablestrongfocus( double _x, double _y )
{
    int x, y;
    int color;

    x=(int)_x; // seems to be necessary 
    y=(int)_y;
    
    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CSTRONG_FOCUS_U];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );
    
    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x,y-P4PrintSymbolWidth*13/20,x+P4PrintSymbolWidth*13/20,y,
        x,y+P4PrintSymbolWidth*13/20,x-P4PrintSymbolWidth*13/20,y);

    P4PrintPainter->drawPolygon( qpa, Qt::OddEvenFill );
}
void P4Print_print_sesaddle( double _x, double _y )
{
    int x;
    int y;
    int color;

    x=(int)_x;
    y=(int)_y;

    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CSADDLE];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );

    // print triangle:

    QPolygon qpa(3);
    qpa.setPoints(3, x-P4PrintSymbolWidth*12/20,y+P4PrintSymbolWidth*12/20,
        x+P4PrintSymbolWidth*12/20,y+P4PrintSymbolWidth*12/20,
        x,y-P4PrintSymbolWidth*12/20);
    P4PrintPainter->drawPolygon( qpa, Qt::OddEvenFill );
}
void P4Print_print_sesaddlenode( double _x, double _y )
{
    int x, y;
    int color;

    x=(int)_x; // seems to be necessary 
    y=(int)_y;

    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CSADDLE_NODE];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );
    // print triangle:

    QPolygon qpa(3);
    qpa.setPoints(3, x-P4PrintSymbolWidth*12/20,y+P4PrintSymbolWidth*12/20,
        x+P4PrintSymbolWidth*12/20,y+P4PrintSymbolWidth*12/20,
        x,y-P4PrintSymbolWidth*12/20);
    P4PrintPainter->drawPolygon( qpa, Qt::OddEvenFill );
}
void P4Print_print_sestablenode( double _x, double _y )
{
    int x, y;
    int color;

    x=(int)_x; // seems to be necessary 
    y=(int)_y;

    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CNODE_S];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );

    // print triangle:

    QPolygon qpa(3);
    qpa.setPoints(3, x-P4PrintSymbolWidth*12/20,y+P4PrintSymbolWidth*12/20,
        x+P4PrintSymbolWidth*12/20,y+P4PrintSymbolWidth*12/20,
        x,y-P4PrintSymbolWidth*12/20);
    P4PrintPainter->drawPolygon( qpa, Qt::OddEvenFill );
}
void P4Print_print_seunstablenode( double _x, double _y )
{
    int x, y;
    int color;

    x=(int)_x; // seems to be necessary 
    y=(int)_y;

    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CNODE_U];

    P4PrintPainter->setPen( QXFIGCOLOR(color) );
    P4PrintPainter->setBrush( QXFIGCOLOR(color) );

    // print triangle:

    QPolygon qpa(3);
    qpa.setPoints(3, x-P4PrintSymbolWidth*12/20,y+P4PrintSymbolWidth*12/20,
        x+P4PrintSymbolWidth*12/20,y+P4PrintSymbolWidth*12/20,
        x,y-P4PrintSymbolWidth*12/20);
    P4PrintPainter->drawPolygon( qpa, Qt::OddEvenFill );
}
void P4Print_print_degen( double _x, double _y )
{
    int i,x,y;
    int color;

    i = P4PrintLineWidth*26/20;
//  if( i > P4PrintSymbolWidth/2 )
//      i = P4PrintSymbolWidth/4;

    x=(int)_x;
    y=(int)_y;

    // print cross:

    color = PrintColorTable[P4PrintBlackWhite ? CFOREGROUND : CDEGEN];

    QPen p = QPen( QXFIGCOLOR(color), i );
    P4PrintPainter->setPen( p );
    P4PrintPainter->drawLine( x-P4PrintSymbolWidth/2, y-P4PrintSymbolWidth/2, x+P4PrintSymbolWidth/2, y+P4PrintSymbolWidth/2 );
    P4PrintPainter->drawLine( x+P4PrintSymbolWidth/2, y-P4PrintSymbolWidth/2, x-P4PrintSymbolWidth/2, y+P4PrintSymbolWidth/2 );
}
void P4Print_print_elips( double x0, double y0, double a, double b, int color, bool dotted, struct P4POLYLINES * ellipse )
{
    color = PrintColorTable[color];
    if( P4PrintBlackWhite )
        color = PrintColorTable[CFOREGROUND];

    QPen p = QPen(QXFIGCOLOR(color), P4PrintLineWidth );
    p.setCapStyle( Qt::RoundCap );
    P4PrintPainter->setPen( p );

    // we do not use the (x0,y0,a,b,dotted) parameters.
    // Instead, we use the "precompiled" ellipse parameter.  Here, a list of lines is computed
    // that approximates the ellipse.

    UNUSED(x0);
    UNUSED(y0);
    UNUSED(a);
    UNUSED(b);
    UNUSED(dotted);
    while( ellipse != NULL )
    {
        P4PrintPainter->drawLine( (int)(ellipse->x1), (int)(ellipse->y1), (int)(ellipse->x2), (int)(ellipse->y2) );
        ellipse = ellipse->next;
    }
}
void P4Print_print_line( double _x0, double _y0, double _x1, double _y1, int color )
{
    int x0, y0, x1, y1;

    color = PrintColorTable[color];
    if( P4PrintBlackWhite )
        color = PrintColorTable[CFOREGROUND];

    x0 = (int)_x0;
    x1 = (int)_x1;
    y0 = (int)_y0;
    y1 = (int)_y1;

    if( x0==x1 && y0==y1 ) 
        return;

    QPen p = QPen( QXFIGCOLOR(color), P4PrintLineWidth );
    p.setCapStyle( Qt::RoundCap );

    P4PrintPainter->setPen( p );
    P4PrintPainter->drawLine( x0, y0, x1, y1 );
}
void P4Print_print_point( double _x0, double _y0, int color )
{
    int x0;
    int y0;

    if( P4PrintBlackWhite )
        color = CFOREGROUND;
    color = PrintColorTable[color];

    x0 = (int)_x0;
    y0 = (int)_y0;

    if( x0 == LastP4PrintX0 && y0 == LastP4PrintY0 && color == LastP4Printcolor )
        return;

    LastP4PrintX0 = x0;
    LastP4PrintY0 = y0;
    LastP4Printcolor = color;

    QPen p = QPen( QXFIGCOLOR(color), P4PrintLineWidth );
    p.setCapStyle( Qt::RoundCap );

    P4PrintPainter->setPen( p );
    if( P4PrintLineWidth > 1 )
    {
        P4PrintPainter->drawLine( x0, y0, x0, y0 );
    }
    else
    {
        P4PrintPainter->drawPoint( x0, y0 );
    }
}

// ---------------------------------------------------------------------------------------

void PrepareP4Printing( int w, int h, bool isblackwhite, QPainter * p4paint, int linewidth, int symbolwidth )
{
    QString s;

    P4PrintBlackWhite = isblackwhite;
    P4PrintPainter = p4paint;
    P4PrintLineWidth = linewidth;
    P4PrintSymbolWidth = symbolwidth;

    plot_l = spherePrintLine;
    plot_p = spherePrintPoint;

    print_saddle = P4Print_print_saddle;
    print_stablenode = P4Print_print_stablenode;
    print_unstablenode = P4Print_print_unstablenode;
    print_stableweakfocus = P4Print_print_stableweakfocus;
    print_unstableweakfocus = P4Print_print_unstableweakfocus;
    print_weakfocus = P4Print_print_weakfocus;
    print_stablestrongfocus = P4Print_print_stablestrongfocus;
    print_unstablestrongfocus = P4Print_print_unstablestrongfocus;
    print_sesaddle = P4Print_print_sesaddle;
    print_sesaddlenode = P4Print_print_sesaddlenode;
    print_sestablenode = P4Print_print_sestablenode;
    print_seunstablenode = P4Print_print_seunstablenode;
    print_degen = P4Print_print_degen;
    print_center = P4Print_print_center;
    print_elips = P4Print_print_elips;
    print_point = P4Print_print_point;
    print_line = P4Print_print_line;
    print_comment = P4Print_comment;

    LastP4Printcolor = -1;

    p4paint->fillRect(0,0,w,h,QBrush(QXFIGCOLOR(PrintColorTable[CBACKGROUND])));
}

void FinishP4Printing( void )
{
    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;
    P4PrintPainter = NULL;
}
