#include <qobject.h>
#include "custom.h"
#include "file_tab.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_sphere.h"
#include "math_gcf.h"
#include "math_separatrice.h"

extern int PrintColorTable[NUMXFIGCOLORS];

void (* print_saddle)( double, double ) = nullptr;
void (* print_stablenode)( double, double ) = nullptr;
void (* print_unstablenode)( double, double ) = nullptr;
void (* print_stableweakfocus)( double, double ) = nullptr;
void (* print_unstableweakfocus)( double, double ) = nullptr;
void (* print_weakfocus)( double, double ) = nullptr;
void (* print_stablestrongfocus)( double, double ) = nullptr;
void (* print_unstablestrongfocus)( double, double ) = nullptr;
void (* print_sesaddle)( double, double ) = nullptr;
void (* print_sesaddlenode)( double, double ) = nullptr;
void (* print_sestablenode)( double, double ) = nullptr;
void (* print_seunstablenode)( double, double ) = nullptr;
void (* print_degen)( double, double ) = nullptr;
void (* print_center)( double, double ) = nullptr;
void (* print_elips)( double, double, double, double, int, bool, struct P4POLYLINES * ) = nullptr;
void (* print_line)( double, double, double, double, int ) = nullptr;
void (* print_point)( double, double, int ) = nullptr;
void (* print_comment)( QString ) = nullptr;

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------

void QWinSphere::printPoint( struct saddle * p )
{
    double pos[2];
    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );
        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;
        print_saddle( coWinX(pos[0]), coWinY(pos[1]) );
    }
}

void QWinSphere::printPoint( struct node * p )
{
    double pos[2];

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );
        
        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        if( p->stable == -1 )
            print_stablenode( coWinX( pos[0] ), coWinY( pos[1] ) );
        else
            print_unstablenode( coWinX( pos[0] ), coWinY( pos[1] ) );
    }
}

void QWinSphere::printPoint( struct weak_focus * p )
{
    double pos[2];

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        switch( p->type )
        {
        case FOCUSTYPE_STABLE:
            print_stableweakfocus( coWinX( pos[0] ), coWinY( pos[1] ) );
            break;
        case FOCUSTYPE_UNSTABLE:
            print_unstableweakfocus( coWinX( pos[0] ), coWinY( pos[1] ) );
            break;
        case FOCUSTYPE_CENTER:
            print_center( coWinX( pos[0] ), coWinY( pos[1] ) );
            break;
        default:
            print_weakfocus( coWinX( pos[0] ), coWinY( pos[1] ) );
            break;
        }
    }
}

void QWinSphere::printPoint( struct strong_focus * p )
{
    double pos[2];

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        if( p->stable == -1 )
            print_stablestrongfocus( coWinX( pos[0] ), coWinY( pos[1] ) );
        else
            print_unstablestrongfocus( coWinX( pos[0] ), coWinY( pos[1] ) );
    }
}

void QWinSphere::printPoint( struct degenerate * p )
{
    double pos[2];

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        print_degen( coWinX( pos[0] ), coWinY( pos[1] ) );
    }
}

void QWinSphere::printPoint( struct semi_elementary * p )
{
    double pos[2];

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        switch(p->type)
        {
        case 1: print_sesaddlenode( coWinX( pos[0] ), coWinY( pos[1] ) ); break;
        case 2: print_sesaddlenode( coWinX( pos[0] ), coWinY( pos[1] ) ); break;
        case 3: print_sesaddlenode( coWinX( pos[0] ), coWinY( pos[1] ) ); break;
        case 4: print_sesaddlenode( coWinX( pos[0] ), coWinY( pos[1] ) ); break;
        case 5: print_seunstablenode( coWinX( pos[0] ), coWinY( pos[1] ) ); break;
        case 6: print_sesaddle( coWinX( pos[0] ), coWinY( pos[1] ) ); break;
        case 7: print_sesaddle( coWinX( pos[0] ), coWinY( pos[1] ) ); break;
        case 8: print_sestablenode( coWinX( pos[0] ), coWinY( pos[1] ) ); break;
        }
    }
}

void QWinSphere::printPoints( void )
{
    struct saddle * sp;
    struct node * np;
    struct weak_focus * wfp;
    struct strong_focus * sfp;
    struct semi_elementary * sep;
    struct degenerate * dp;

    print_comment( "Printing symbols at all singular points:" );

    for( sp = VFResults.first_saddle_point; sp != nullptr; sp = sp->next_saddle )
        printPoint( sp );
    for( np = VFResults.first_node_point; np != nullptr; np = np->next_node )
        printPoint( np );
    for( wfp = VFResults.first_wf_point; wfp != nullptr; wfp = wfp->next_wf )
        printPoint( wfp );
    for( sfp = VFResults.first_sf_point; sfp != nullptr; sfp = sfp->next_sf )
        printPoint( sfp );
    for( sep = VFResults.first_se_point; sep != nullptr; sep = sep->next_se )
        printPoint( sep );
    for( dp = VFResults.first_de_point; dp != nullptr; dp = dp->next_de )
        printPoint( dp );
}

void QWinSphere::printPointSeparatrices( struct semi_elementary * p )
{
    struct sep * separatrice;
    
    for( separatrice = p->separatrices; separatrice != nullptr; separatrice = separatrice->next_sep )
    {
        draw_sep( this, separatrice->first_sep_point );
        if( separatrice->next_sep != nullptr )
        {
            print_comment( "Next separatrix of degenerate point:" );
        }
    }
}

void QWinSphere::printPointSeparatrices( struct saddle * p )
{
    struct sep * separatrice;
    
    for( separatrice = p->separatrices; separatrice != nullptr; separatrice = separatrice->next_sep )
    {
        draw_sep( this, separatrice->first_sep_point );
        if( separatrice->next_sep != nullptr )
        {
            print_comment( "Next separatrix of saddle point:" );
        }
    }
}

void QWinSphere::printPointSeparatrices( struct degenerate * p )
{
    struct blow_up_points *blow_up;

    for( blow_up = p->blow_up; blow_up != nullptr; blow_up = blow_up->next_blow_up_point )
    {
        draw_sep( this, blow_up->first_sep_point );
        if( blow_up->next_blow_up_point != nullptr )
        {
            print_comment( "Next separatrix of degenerate point:" );
        }
    }
}

void QWinSphere::printSeparatrices( void )
{
    QString comment;
    struct saddle * sp;
    struct semi_elementary * sep;
    struct degenerate * dp;

    for( sp = VFResults.first_saddle_point; sp != nullptr; sp = sp->next_saddle )
    {
        comment = "Printing separatrice for saddle singularity:";
        print_comment( comment );
        printPointSeparatrices( sp );
    }
    for( sep = VFResults.first_se_point; sep != nullptr; sep = sep->next_se )
    {
        comment = "Printing separatrices for semi-hyperbolic singularity:";
        print_comment( comment );
        printPointSeparatrices( sep );
    }
    for( dp = VFResults.first_de_point; dp != nullptr; dp = dp->next_de )
    {
        comment = "Printing separatrices for degenerate singularity:";
        print_comment( comment );
        printPointSeparatrices( dp );
    }
}

void QWinSphere::printGcf( void )
{
    QString comment;

    if( VFResults.gcf_points != nullptr )
    {
        comment = "Printing Greatest common factor:";
        print_comment( comment );
        draw_gcf( this, VFResults.gcf_points, CSING, 1 );
    }
}

void QWinSphere::printPoincareSphere( void )
{
    QString comment;
    struct P4POLYLINES * p;
    struct P4POLYLINES * q;

    comment = "Printing Poincare Sphere:";
    print_comment( comment );
    p = produceEllipse( 0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0) );
    for( q = p; q != nullptr; q = q->next )
    {
        q->x1 = coWinX(q->x1);
        q->y1 = coWinY(q->y1);
        q->x2 = coWinX(q->x2);
        q->y2 = coWinY(q->y2);
    }
    print_elips( coWinX(0), coWinY(0), coWinH(1), coWinV(1), VFResults.singinf ? CSING:CLINEATINFINITY, false, p );

    while( p != nullptr )
    {
        q = p;
        p = p->next;
        delete q;//free( q );
        q = nullptr;
    }
}

void QWinSphere::printPoincareLyapunovSphere( void )
{
    QString comment;
    struct P4POLYLINES * p;
    struct P4POLYLINES * q;

    comment = "Printing Poincare Lyapunov-Sphere (circle at infinity):";
    print_comment( comment );

    p = produceEllipse( 0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0) );
    for( q = p; q != nullptr; q = q->next )
    {
        q->x1 = coWinX(q->x1);
        q->y1 = coWinY(q->y1);
        q->x2 = coWinX(q->x2);
        q->y2 = coWinY(q->y2);
    }

    print_elips( coWinX(0.0), coWinY(0.0), coWinH(1.0), coWinV(1.0), CLINEATINFINITY, false, p );

    while( p != nullptr )
    {
        q = p;
        p = p->next;
        delete q;//free( q );
        q = nullptr;
    }

    comment = "Printing Poincare Lyapunov-Sphere (circle of finite radius):";
    print_comment( comment );

    p = produceEllipse( 0.0, 0.0, RADIUS, RADIUS, true, coWinH(RADIUS), coWinV(RADIUS) );
    for( q = p; q != nullptr; q = q->next )
    {
        q->x1 = coWinX(q->x1);
        q->y1 = coWinY(q->y1);
        q->x2 = coWinX(q->x2);
        q->y2 = coWinY(q->y2);
    }

    print_elips( coWinX(0.0), coWinY(0.0), coWinH(RADIUS), coWinV(RADIUS), CLINEATINFINITY, true, p );

    while( p != nullptr )
    {
        q = p;
        p = p->next;
        delete q;//free( q );
        q = nullptr;
    }
}

void QWinSphere::printLineAtInfinity( void )
{
    switch( VFResults.typeofview )
    {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if( x0 < 0.0 && x1 > 0.0 )
            print_line( coWinX(0.0), coWinY(y0), coWinX(0.0), coWinY(y1), CLINEATINFINITY );
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if( y0 < 0.0 && y1 > 0.0 )
            print_line( coWinX(x0), coWinY(0.0), coWinX(x1), coWinY(0.0), CLINEATINFINITY );
        break;
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_SPHERE:
        // should not appear
        break;
    }
}

// -----------------------------------------------------------------------
//                      DRAWORBITS
// -----------------------------------------------------------------------

extern void DrawOrbit( QWinSphere *, double *, struct orbits_points *, int );

void QWinSphere::printOrbits( void )
{
    // inspired by DrawOrbits, except that we put comments between

    struct orbits * orbit;
    QString s;
    int i;
    i = 1;

    for( orbit = VFResults.first_orbit; orbit != nullptr; orbit = orbit->next_orbit )
    {
        s.sprintf( "Starting orbit %d", i++ );
        print_comment( s );
        DrawOrbit( this, orbit->pcoord, orbit->f_orbits, orbit->color );
    }
}

void QWinSphere::printLimitCycles( void )
{
    // inspired by DrawOrbits, except that we put comments between

    struct orbits * orbit;
    QString s;
    int i;
    i = 1;

    for( orbit = VFResults.first_lim_cycle; orbit != nullptr; orbit = orbit->next_orbit )
    {
        s.sprintf( "Starting Limit Cycle %d", i++ );
        print_comment( s );
        DrawOrbit( this, orbit->pcoord, orbit->f_orbits, orbit->color );
    }
}
