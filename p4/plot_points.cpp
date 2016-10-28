#include <qpainter.h>
#include "custom.h"
#include "file_tab.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_sphere.h"
#include "math_gcf.h"
#include "math_separatrice.h"

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------
//
// This uses the pixel-coordinate system, i.e. any transformations should
// be done beforehand.

void win_plot_saddle( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSADDLE) );
    p->setBrush( QXFIGCOLOR(CSADDLE) );
    p->drawRect( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, SYMBOLWIDTH, SYMBOLHEIGHT );
}

void win_plot_stablenode( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CNODE_S) );
    p->setBrush( QXFIGCOLOR(CNODE_S) );
    p->drawRect( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, SYMBOLWIDTH, SYMBOLHEIGHT );
}

void win_plot_unstablenode( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CNODE_U) );
    p->setBrush( QXFIGCOLOR(CNODE_U) );
    p->drawRect( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, SYMBOLWIDTH, SYMBOLHEIGHT );
}

void win_plot_weakfocus( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CWEAK_FOCUS) );
    p->setBrush( QXFIGCOLOR(CWEAK_FOCUS) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_stableweakfocus( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CWEAK_FOCUS_S) );
    p->setBrush( QXFIGCOLOR(CWEAK_FOCUS_S) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_unstableweakfocus( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CWEAK_FOCUS_U) );
    p->setBrush( QXFIGCOLOR(CWEAK_FOCUS_U) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_center( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CCENTER) );
    p->setBrush( QXFIGCOLOR(CCENTER) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_stablestrongfocus( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSTRONG_FOCUS_S) );
    p->setBrush( QXFIGCOLOR(CSTRONG_FOCUS_S) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_unstablestrongfocus( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSTRONG_FOCUS_U) );
    p->setBrush( QXFIGCOLOR(CSTRONG_FOCUS_U) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_sesaddlenode( QPainter * p, int x, int y )
{
    QPolygon qpa(3);
    p->setPen( QXFIGCOLOR(CSADDLE_NODE) );
    p->setBrush( QXFIGCOLOR(CSADDLE_NODE) );
    qpa.setPoints(3, x-SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,
        x,y-SYMBOLHEIGHT/2);

    p->drawPolygon( qpa );
}

void win_plot_sestablenode( QPainter * p, int x, int y )
{
    QPolygon qpa(3);
    p->setPen( QXFIGCOLOR(CNODE_S) );
    p->setBrush( QXFIGCOLOR(CNODE_S) );
    qpa.setPoints(3, x-SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,
        x,y-SYMBOLHEIGHT/2);

    p->drawPolygon( qpa );
}

void win_plot_seunstablenode( QPainter * p, int x, int y )
{
    QPolygon qpa(3);
    p->setPen( QXFIGCOLOR(CNODE_U) );
    p->setBrush( QXFIGCOLOR(CNODE_U) );
    qpa.setPoints(3, x-SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,
        x,y-SYMBOLHEIGHT/2);

    p->drawPolygon( qpa );
}

void win_plot_sesaddle( QPainter * p, int x, int y )
{
    QPolygon qpa(3);
    p->setPen( QXFIGCOLOR(CSADDLE) );
    p->setBrush( QXFIGCOLOR(CSADDLE) );
    qpa.setPoints(3, x-SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,
        x,y-SYMBOLHEIGHT/2);

    p->drawPolygon( qpa );
}

void win_plot_degen( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CDEGEN) );
    p->drawLine( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, x+SYMBOLWIDTH/2, y+SYMBOLHEIGHT/2 );
    p->drawLine( x+SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, x-SYMBOLWIDTH/2, y+SYMBOLHEIGHT/2 );
}

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------

void QWinSphere::plotPoint( struct saddle * p )
{
    double pos[2];
    int x, y;
    
    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );
        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;
            
        x = coWinX(pos[0]);
        y = coWinY(pos[1]); 

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;
        
        win_plot_saddle( staticPainter, x, y );
    }
}

void QWinSphere::plotPoint( struct node * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );
        
        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]); 

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        if( p->stable == -1 )
            win_plot_stablenode( staticPainter, x, y );
        else
            win_plot_unstablenode( staticPainter, x, y );
    }
}

void QWinSphere::plotPoint( struct weak_focus * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]); 

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        switch( p->type )
        {
        case FOCUSTYPE_STABLE:
            win_plot_stableweakfocus( staticPainter, x, y );
            break;
        case FOCUSTYPE_UNSTABLE:
            win_plot_unstableweakfocus( staticPainter, x, y );
            break;
        case FOCUSTYPE_CENTER:
            win_plot_center( staticPainter, x, y );
            break;
        default:
            win_plot_weakfocus( staticPainter, x, y );
            break;
        }
    }
}

void QWinSphere::plotPoint( struct strong_focus * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]); 

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        if( p->stable == -1 )
            win_plot_stablestrongfocus( staticPainter, x, y );
        else
            win_plot_unstablestrongfocus( staticPainter, x, y );
    }
}

void QWinSphere::plotPoint( struct degenerate * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]); 

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        win_plot_degen( staticPainter, x, y );
    }
}

void QWinSphere::plotPoint( struct semi_elementary * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]); 

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        switch(p->type)
        {
        case 1: win_plot_sesaddlenode( staticPainter, x, y ); break;
        case 2: win_plot_sesaddlenode( staticPainter, x, y ); break;
        case 3: win_plot_sesaddlenode( staticPainter, x, y ); break;
        case 4: win_plot_sesaddlenode( staticPainter, x, y ); break;
        case 5: win_plot_seunstablenode( staticPainter, x, y ); break;
        case 6: win_plot_sesaddle( staticPainter, x, y ); break;
        case 7: win_plot_sesaddle( staticPainter, x, y ); break;
        case 8: win_plot_sestablenode( staticPainter, x, y ); break;
        }
    }
}

void QWinSphere::plotPoints( void )
{
    struct saddle * sp;
    struct node * np;
    struct weak_focus * wfp;
    struct strong_focus * sfp;
    struct semi_elementary * sep;
    struct degenerate * dp;

    for( sp = VFResults.first_saddle_point; sp != nullptr; sp = sp->next_saddle )
        plotPoint( sp );
    for( np = VFResults.first_node_point; np != nullptr; np = np->next_node )
        plotPoint( np );
    for( wfp = VFResults.first_wf_point; wfp != nullptr; wfp = wfp->next_wf )
        plotPoint( wfp );
    for( sfp = VFResults.first_sf_point; sfp != nullptr; sfp = sfp->next_sf )
        plotPoint( sfp );
    for( sep = VFResults.first_se_point; sep != nullptr; sep = sep->next_se )
        plotPoint( sep );
    for( dp = VFResults.first_de_point; dp != nullptr; dp = dp->next_de )
        plotPoint( dp );
}

void QWinSphere::plotPointSeparatrices( struct semi_elementary * p )
{
    struct sep * separatrice;
    
    for( separatrice = p->separatrices; separatrice != nullptr; separatrice = separatrice->next_sep )
        draw_sep( this, separatrice->first_sep_point );
}

void QWinSphere::plotPointSeparatrices( struct saddle * p )
{
    struct sep * separatrice;
    
    for( separatrice = p->separatrices; separatrice != nullptr; separatrice = separatrice->next_sep )
        draw_sep( this, separatrice->first_sep_point );
}

void QWinSphere::plotPointSeparatrices( struct degenerate * p )
{
    struct blow_up_points *blow_up;

    for( blow_up = p->blow_up; blow_up != nullptr; blow_up = blow_up->next_blow_up_point )
    {
        draw_sep( this, blow_up->first_sep_point );
    }
}

void QWinSphere::plotSeparatrices( void )
{
    struct saddle * sp;
    struct semi_elementary * sep;
    struct degenerate * dp;

    for( sp = VFResults.first_saddle_point; sp != nullptr; sp = sp->next_saddle )
        plotPointSeparatrices( sp );
    for( sep = VFResults.first_se_point; sep != nullptr; sep = sep->next_se )
        plotPointSeparatrices( sep );
    for( dp = VFResults.first_de_point; dp != nullptr; dp = dp->next_de )
        plotPointSeparatrices( dp );
}

void QWinSphere::plotGcf(void )
{
    draw_gcf( this, VFResults.gcf_points, CSING, 1 );
}
