#include <qpainter.h>
#include "custom.h"
#include "table.h"
#include "win_p4.h"
#include "win_legend.h"
#include "p4application.h"

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
extern void SetP4WindowTitle( QWidget *, QString );

// -----------------------------------------------------------------------
//                          Define Colors in RGB
// -----------------------------------------------------------------------

struct P4RGBITEM XFigToRGB[NUMXFIGCOLORS] =
{
    // 8 pure colours:

    {  0,  0,  0},      // BLACK
    {  0,  0,255},      // BLUE
    {  0,255,  0},      // GREEN
    {  0,255,255},      // CYAN
    {255,  0,  0},      // RED
    {255,  0,255},      // MAGENTA
    {255,255,  0},      // YELLOW
    {255,255,255},      // WHITE

    // shaded colours (ordered from dark to light):

    {  0,  0,143},      // BLUE1
    {  0,  0,176},      // BLUE2
    {  0,  0,209},      // BLUE3
    {135,207,255},      // BLUE4
    {  0,143,  0},      // GREEN1
    {  0,176,  0},      // GREEN2
    {  0,209,  0},      // GREEN3
    {  0,143,143},      // CYAN1
    {  0,176,176},      // CYAN2
    {  0,209,209},      // CYAN3
    {143,  0,  0},      // RED1
    {176,  0,  0},      // RED2
    {209,  0,  0},      // RED3
    {143,  0,143},      // MAGENTA1
    {176,  0,176},      // MAGENTA2
    {209, 32,209},      // MAGENTA3
    {128, 48,  0},      // BROWN1
    {161, 64,  0},      // BROWN2
    {191, 97,  0},      // BROWN3
    {255,128,128},      // PINK1
    {255,161,161},      // PINK2
    {255,191,191},      // PINK3
    {255,224,224},      // PINK4
    {255,214,  0}       // GOLD
};

// -----------------------------------------------------------------------
//                              LEGEND
// -----------------------------------------------------------------------

QLegendWnd::QLegendWnd()
    : QWidget(NULL,Qt::Tool | Qt::WindowStaysOnTopHint)
{
    int w, h;

    // general initialization

    calculateGeometry();
    resize( w = hmargin6+1, h = vmargin7+1 );
    setMinimumSize( w, h );
    setMaximumSize( w, h );
    SetP4WindowTitle( this, "P4 Legend" );
}

void QLegendWnd::paintEvent( QPaintEvent * p )
{
    QPainter paint( this );
    UNUSED(p);

    paint.setFont( *(p4app->LegendFont) );

    paint.setPen( QPen( QXFIGCOLOR(CFOREGROUND) ) );
    paint.drawText( hmargin1, vmargin1, "Non-Degenerate:" );
    paint.drawText( hmargin4, vmargin1, "Semi-hyperbolic:" );
    paint.drawText( hmargin1, vmargin3, "Separatrices:" );
    paint.drawText( hmargin4, vmargin3, "Orbits:" );
    paint.drawText( hmargin4, vmargin5, "Curve of Singularities:" );
    paint.drawText( hmargin4, vmargin6, "Limit Cycles:" );

    paint.setPen( QPen( QXFIGCOLOR(CSADDLE) ) );
    paint.drawText( hmargin2, vmargin2,                 "Saddle" );
    paint.setPen( QPen( QXFIGCOLOR(CNODE_S) ) );
    paint.drawText( hmargin2, vmargin2+1*interline,     "Stable Node" );
    paint.setPen( QPen( QXFIGCOLOR(CNODE_U) ) );
    paint.drawText( hmargin2, vmargin2+2*interline,     "Unstable Node" );
    paint.setPen( QPen( QXFIGCOLOR(CWEAK_FOCUS) ) );
    paint.drawText( hmargin2, vmargin2+3*interline,     "Weak Focus or center" );
    paint.setPen( QPen( QXFIGCOLOR(CWEAK_FOCUS_S) ) );
    paint.drawText( hmargin2, vmargin2+4*interline,     "Stable Weak Focus" );
    paint.setPen( QPen( QXFIGCOLOR(CWEAK_FOCUS_U) ) );
    paint.drawText( hmargin2, vmargin2+5*interline,     "Unstable Weak Focus" );
    paint.setPen( QPen( QXFIGCOLOR(CCENTER) ) );
    paint.drawText( hmargin2, vmargin2+6*interline,     "Center" );
    paint.setPen( QPen( QXFIGCOLOR(CSTRONG_FOCUS_S) ) );
    paint.drawText( hmargin2, vmargin2+7*interline,     "Stable Strong Focus" );
    paint.setPen( QPen( QXFIGCOLOR(CSTRONG_FOCUS_U) ) );
    paint.drawText( hmargin2, vmargin2+8*interline,     "Unstable Strong Focus" );

    win_plot_saddle( &paint, hmargin1+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 );
    win_plot_stablenode( &paint, hmargin1+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 1*interline );
    win_plot_unstablenode( &paint, hmargin1+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 2*interline );
    win_plot_weakfocus( &paint, hmargin1+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 3*interline );
    win_plot_stableweakfocus( &paint, hmargin1+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 4*interline );
    win_plot_unstableweakfocus( &paint, hmargin1+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 5*interline );
    win_plot_center( &paint, hmargin1+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 6*interline );
    win_plot_stablestrongfocus( &paint, hmargin1+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 7*interline );
    win_plot_unstablestrongfocus( &paint, hmargin1+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 8*interline );

    paint.setPen( QPen( QXFIGCOLOR(CSADDLE_NODE) ) );
    paint.drawText( hmargin5, vmargin2,                 "Saddle-Node" );
    paint.setPen( QPen( QXFIGCOLOR(CNODE_S) ) );
    paint.drawText( hmargin5, vmargin2+1*interline,     "Stable-Node" );
    paint.setPen( QPen( QXFIGCOLOR(CNODE_U) ) );
    paint.drawText( hmargin5, vmargin2+2*interline,     "Unstable-Node" );
    paint.setPen( QPen( QXFIGCOLOR(CSADDLE) ) );
    paint.drawText( hmargin5, vmargin2+3*interline,     "Saddle" );
    paint.setPen( QPen( QXFIGCOLOR(CDEGEN) ) );
    paint.drawText( hmargin5, vmargin2+5*interline,     "Non-elementary" );

    win_plot_sesaddlenode( &paint, hmargin4+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 );
    win_plot_sestablenode( &paint, hmargin4+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 1*interline );
    win_plot_seunstablenode( &paint, hmargin4+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 2*interline );
    win_plot_sesaddle( &paint, hmargin4+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 3*interline );
    win_plot_degen( &paint, hmargin4+SYMBOLWIDTH/2, vmargin2-SYMBOLHEIGHT/2 + 5*interline );

    paint.setPen( QPen( QXFIGCOLOR(CSTABLE) ) );
    paint.drawText( hmargin3, vmargin4,                 "Stable" );
    paint.drawLine( hmargin1, vmargin4-xheight, hmargin1+sepwidth-1, vmargin4-xheight );
    paint.setPen( QPen( QXFIGCOLOR(CUNSTABLE) ) );
    paint.drawText( hmargin3, vmargin4+1*interline,     "Unstable" );
    paint.drawLine( hmargin1, vmargin4+interline-xheight, hmargin1+sepwidth-1, vmargin4+interline-xheight );
    paint.setPen( QPen( QXFIGCOLOR(CCENT_STABLE) ) );
    paint.drawText( hmargin3, vmargin4+2*interline,     "Center-Stable" );
    paint.drawLine( hmargin1, vmargin4+2*interline-xheight, hmargin1+sepwidth-1, vmargin4+2*interline-xheight );
    paint.setPen( QPen( QXFIGCOLOR(CCENT_UNSTABLE) ) );
    paint.drawText( hmargin3, vmargin4+3*interline,     "Center-Unstable" );
    paint.drawLine( hmargin1, vmargin4+3*interline-xheight, hmargin1+sepwidth-1, vmargin4+3*interline-xheight );

    paint.setPen( QPen( QXFIGCOLOR(CORBIT) ) );
    paint.drawLine( hmargin4, vmargin3+interline-xheight, hmargin4+orbitwidth-1, vmargin3+interline-xheight );
    paint.setPen( QPen( QXFIGCOLOR(CSING) ) );
    paint.drawLine( hmargin4, vmargin5+interline-xheight, hmargin4+orbitwidth-1, vmargin5+interline-xheight );
    paint.setPen( QPen( QXFIGCOLOR(CLIMIT) ) );
    paint.drawLine( hmargin4, vmargin6+interline-xheight, hmargin4+orbitwidth-1, vmargin6+interline-xheight );

    SetP4WindowTitle( this, "P4 Legend" );
}


/*
    Non-degenerate                  Semi-hyperbolic

    X Saddle                        X Saddle-Node
    X Stable Node                   X Stable-Node
    X Unstable Node                 X Unstable-Node
    X Weak Focus                    X Saddle
    X Stable Weak Focus
    X Unstable Weak Focus           X Non-elementary
    X Center
    X Stable Strong Focus
    X Unstable Strong Focus

    Separatrices:                   Orbits:
                                    ------------------
    ------ Stable                   Line of Singularities:
    ------ Unstable                 -----------------
    ------ Center-Stable            Limit Cycles:
    ------ Center-unstable          -----------------

horizontal margins:
    1 2    3                        4 5                    6

vertical margins:
    1 non-deg / semi-hyp
    2 saddle / saddle-node
    3 sep/orbits
    4 stable
    5 line of sing
    6 limit cycl
    7 bottom
*/

void QLegendWnd::calculateGeometry( void )
{
    int e, me;

    QFontMetrics fm( *(p4app->LegendFont) );

    QPalette palette;
    palette.setColor(backgroundRole(), QXFIGCOLOR(CBACKGROUND) );
    setPalette(palette);
    
    hmargin1 = fm.maxWidth();
    interline = fm.lineSpacing();

    if( interline < SYMBOLHEIGHT + 2 )
        interline = SYMBOLHEIGHT + 2;

    hmargin2 = hmargin1 + SYMBOLWIDTH + fm.maxWidth();

    orbitwidth = fm.width("abcdefghk");
    sepwidth = fm.width("abc");

    hmargin3 = hmargin1 + sepwidth + fm.maxWidth();

    me= hmargin1 + fm.width( "Non-degenerate:" );
    
    e = hmargin2 + fm.width( "Saddle" );                    if( me < e ) me = e;
    e = hmargin2 + fm.width( "Stable Node" );               if( me < e ) me = e;
    e = hmargin2 + fm.width( "Unstable Node" );             if( me < e ) me = e;
    e = hmargin2 + fm.width( "Weak Focus" );                if( me < e ) me = e;
    e = hmargin2 + fm.width( "Stable Weak Focus" );         if( me < e ) me = e;
    e = hmargin2 + fm.width( "Unstable Weak Focus" );       if( me < e ) me = e;
    e = hmargin2 + fm.width( "Center" );                    if( me < e ) me = e;
    e = hmargin2 + fm.width( "Stable Strong Focus" );       if( me < e ) me = e;
    e = hmargin2 + fm.width( "Unstable Strong Focus" );     if( me < e ) me = e;
    
    e = hmargin1 + fm.width( "Separatrices:" );             if( me < e ) me = e;
    e = hmargin3 + fm.width( "Stable" );                    if( me < e ) me = e;
    e = hmargin3 + fm.width( "Unstable" );                  if( me < e ) me = e;
    e = hmargin3 + fm.width( "Center-Stable" );             if( me < e ) me = e;
    e = hmargin3 + fm.width( "Center-Unstable" );           if( me < e ) me = e;

    hmargin4 = me + fm.maxWidth() * 3;          // 3 chars intercolumn space

    hmargin5 = hmargin4 + SYMBOLWIDTH + fm.maxWidth();

    me= hmargin4 + fm.width( "Semi-hyperbolic" );
    e = hmargin5 + fm.width( "Saddle-Node" );               if( me < e ) me = e;
    e = hmargin5 + fm.width( "Stable Node" );               if( me < e ) me = e;
    e = hmargin5 + fm.width( "Unstable Node" );             if( me < e ) me = e;
    e = hmargin5 + fm.width( "Saddle" );                    if( me < e ) me = e;
    
    e = hmargin5 + fm.width( "Non-elementary:" );           if( me < e ) me = e;
    e = hmargin4 + fm.width( "Orbits:" );                   if( me < e ) me = e;
    e = hmargin4 + fm.width( "Curve of Singularities:" );   if( me < e ) me = e;
    e = hmargin4 + fm.width( "Limit Cycles:" );             if( me < e ) me = e;

    hmargin6 = me + fm.maxWidth();

    vmargin1 = fm.height() + fm.descent();
    vmargin2 = vmargin1 + 2*interline;
    vmargin3 = vmargin2 + 10*interline;
    vmargin4 = vmargin3 + 2*interline;
    
    vmargin5 = vmargin3 + (5*interline)/2;
    vmargin6 = vmargin5 + (5*interline)/2;
    
    me = vmargin6 + 2*interline;
    e = vmargin4 + 4*interline;     if( me < e ) me = e;

    vmargin7 = e+interline;

    xheight = fm.ascent()/2;
}
