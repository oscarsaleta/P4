#include <math.h>
#include <float.h>
#include <qpainter.h>
#include "custom.h"
#include "table.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_sphere.h"

#define p4_isnan       isnan
#define p4_finite      finite

struct P4POLYLINES * QWinSphere::produceEllipse( double cx, double cy, double a, double b, bool dotted, double resa, double resb )
{
    // this is an exact copy of the plotEllipse routine, except that output is stored
    // in a list of points that is dynamically allocated.

    double theta, t1, t2, e, R, x, y, c, prevx, prevy;
    bool d;
    bool doton;
    int dotcount;
    struct P4POLYLINES * first;
    struct P4POLYLINES * last;

    prevx = prevy = 0;
    dotcount=0;
    first = NULL;
    last = NULL;
    
    R = (resa < resb) ? resa : resb;
    if( R < 1.0 ) R = 1.0; // protection
    e = 2*acos(1.0-0.5/R);
    if( R*sin(e) > 1.0 )
        e = asin(1.0/R);

    theta = 0;
    
    d = false;
    doton = true;

//  FILE * fp;
//  fp = fopen( "C:\\test.txt", "wt" );

    while( theta < TWOPI )
    {
//        fprintf( fp, "%f\n", (float)theta );
//        fflush(fp);
        c = (x0-cx)/a;
        if( c > -1.0 && c < 1.0 )
        {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if( theta >= t1 && theta < t2 )
            {
//                fprintf( fp, "A EXCL [%f %f]\n", (float)t1, (float)t2 );
                theta = t2+e/4; d = false; continue;
            }
        }
        c = (x1-cx)/a;
        if( c > -1.0 && c < 1.0 )
        {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if( theta < t1 )
            {
//                fprintf( fp, "B EXCL [-infinity %f]\n", (float)t1 );
                theta = t1+e/4; d = false; continue;
            }
            if( theta >= t2 )
            {
//                fprintf( fp, "C EXCL [%f, infinity]\n",  (float)t2 );
                theta = TWOPI+e/4; d = false; break;
            }
        }
        c = (y0-cy)/b;
        if( c > -1.0 && c < 1.0 )
        {
            t1 = asin(c);
            t2 = PI-t1;
            if( t1 < 0 )
            {
                t2 = t1+TWOPI;
                t1 = PI-t1;
                if( theta >= t1 && theta < t2 )
                {
//                    fprintf( fp, "D EXCL [%f %f]\n", (float)t1, (float)t2 );
                    theta = t2+e/4; d = false; continue;
                }
            }
            else
            {
                if( theta < t1 )
                {
//                    fprintf( fp, "E EXCL [-infinity %f]\n", (float)t1 );
                    theta = t1+e/4; d = false; continue;
                }
                if( theta >= t2 )
                {
//                    fprintf( fp, "F EXCL [%f, infinity]\n",  (float)t2 );
                    theta = TWOPI+e/4; d = false; break;
                }
            }
        }
        c = (y1-cy)/b;
        if( c > -1.0 && c < 1.0 )
        {
            t1 = asin(c);
            t2 = PI-t1;
            if( t1 < 0 )
            {
                t2 = t1+TWOPI;
                t1 = PI-t1;
                if( theta < t1 )
                {
//                    fprintf( fp, "G EXCL [-infinity %f]\n", (float)t1 );
                    theta = t1+e/4; d = false; continue;
                }
                if( theta >= t2 )
                {
//                    fprintf( fp, "H EXCL [%f, infinity]\n",  (float)t2 );
                    theta = TWOPI; d = false; break;
                }
            }
            else
            {
                if( theta >= t1 && theta < t2 )
                {
//                    fprintf( fp, "I EXCL [%f %f]\n", (float)t1, (float)t2 );
                    theta = t2+e/4; d = false; continue;
                }
            }
        }

        x = cx + a*cos(theta);
        y = cy + b*sin(theta);
        theta += e;
        
        // (x,y) is inside view

        if( !d )
        {
            if( doton )
            {
                d = true;
                prevx = x;
                prevy = y;
                dotcount = 0;
                doton = true;
            }
            else
            {
                if( ++dotcount > 7 && dotted )
                {
                    d = false;
                    doton = (doton) ? false:true;
                    dotcount = 0;
                }
            }
        }
        else
        {
            if( doton )
            {
                if( first == NULL )
                {
                    last = first = (struct P4POLYLINES *)malloc( sizeof(struct P4POLYLINES) );
                    last->next = NULL;
                }
                else
                {
                    last->next = (struct P4POLYLINES *)malloc( sizeof(struct P4POLYLINES) );
                    last = last->next;
                    last->next = NULL;
                }

                last->x1 = prevx;
                last->y1 = prevy;
                last->x2 = x;
                last->y2 = y;

                prevx = x;
                prevy = y;
            }
            if( ++dotcount > 7 && dotted )
            {
                d = false;
                doton = (doton) ? false:true;
                dotcount = 0;
            }
        }
    }
//  fclose(fp);
    return first;
}

/*
void plotEllipse( QPainter * p, int cx, int cy, int a, int b, int color, bool dotted,
                    int clipx0, int clipy0, int clipx1, int clipy1 )
{
//  THIS IS THE QT CODE: BUT FOR SOME REASON IT DOES NOT WORK WELL ALL THE TIME WHEN ZOOMING
//  (SOMETIMES, WHEN A PART OF THE SPHERE IS VISIBLE, NOTHING IS DRAWN)

    QPen pen( (QColor)(QXFIGCOLOR(color)) );
    if( dotted )
        pen.setStyle( Qt::DashLine );
    else
        pen.setStyle( Qt::SolidLine );
    
    p->setPen( pen );
    p->drawEllipse( cx-a, cy-b, a+a, b+b );

    if( dotted )
    {
        pen.setStyle( Qt::SolidLine );
        p->setPen(pen);
    }
}
*/

void QWinSphere::plotPoincareSphere( void )
{
    int color;
    struct P4POLYLINES * p;

    p = CircleAtInfinity;
    color = VFResults.singinf ? CSING : CLINEATINFINITY;

    staticPainter->setPen( QXFIGCOLOR(color) );
    while( p != NULL )
    {
        staticPainter->drawLine( coWinX(p->x1), coWinY(p->y1), coWinX(p->x2), coWinY(p->y2) );
        p = p->next;
    }
}

void QWinSphere::plotPoincareLyapunovSphere( void )
{
    int color;
    struct P4POLYLINES * p;

    p = CircleAtInfinity;
    color = VFResults.singinf ? CSING : CLINEATINFINITY;

    staticPainter->setPen( QXFIGCOLOR(color) );
    while( p != NULL )
    {
        staticPainter->drawLine( coWinX(p->x1), coWinY(p->y1), coWinX(p->x2), coWinY(p->y2) );
        p = p->next;
    }

    p = PLCircle;
    color = CLINEATINFINITY;

    staticPainter->setPen( QXFIGCOLOR(color) );
    while( p != NULL )
    {
        staticPainter->drawLine( coWinX(p->x1), coWinY(p->y1), coWinX(p->x2), coWinY(p->y2) );
        p = p->next;
    }
    return;
}

void QWinSphere::plotLineAtInfinity( void )
{
    switch( VFResults.typeofview )
    {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if( x0 < 0.0 && x1 > 0.0 )
        {
            staticPainter->setPen( QXFIGCOLOR(CLINEATINFINITY) );
            staticPainter->drawLine( coWinX(0.0), 0, coWinX(0.0), h-1 );
        }
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if( y0 < 0.0 && y1 > 0.0 )
        {
            staticPainter->setPen( QXFIGCOLOR(CLINEATINFINITY) );
            staticPainter->drawLine( 0, coWinY(0.0), w-1, coWinY(0.0) );
        }

        break;
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_SPHERE:
        // should not appear
        break;
    }
}

void spherePlotLine( QWinSphere * sp, double * p1, double * p2, int color )
{
    double ucoord1[2];
    double ucoord2[2];
    double ucoord3[2];
    double ucoord4[2];

    if( MATHFUNC(sphere_to_viewcoordpair)(p1,p2,ucoord1,ucoord2,ucoord3,ucoord4) )
    {
        while( sp != NULL )
        {
            sp->drawLine( ucoord1[0], ucoord1[1], ucoord2[0], ucoord2[1], color );
            sp = sp->next;
        }
    }
    else
    {
        while( sp != NULL )
        {
            sp->drawLine( ucoord1[0], ucoord1[1], ucoord2[0], ucoord2[1], color );
            sp->drawLine( ucoord3[0], ucoord3[1], ucoord4[0], ucoord4[1], color );
            sp = sp->next;
        }
    }
}

void spherePlotPoint( QWinSphere * sp, double * p, int color )
{
    double ucoord[2];

    MATHFUNC(sphere_to_viewcoord)(p[0],p[1],p[2],ucoord);

    while( sp != NULL )
    {
        sp->drawPoint( ucoord[0], ucoord[1], color );
        sp = sp->next;
    }
}

void spherePrintLine( QWinSphere * sp, double * p1, double * p2, int color )
{
    double ucoord1[2];
    double ucoord2[2];
    double ucoord3[2];
    double ucoord4[2];

    if( MATHFUNC(sphere_to_viewcoordpair)(p1,p2,ucoord1,ucoord2,ucoord3,ucoord4) )
    {
        sp->printLine( ucoord1[0], ucoord1[1], ucoord2[0], ucoord2[1], color );
    }
    else
    {
        sp->printLine( ucoord1[0], ucoord1[1], ucoord2[0], ucoord2[1], color );
        sp->printLine( ucoord3[0], ucoord3[1], ucoord4[0], ucoord4[1], color );
    }
}

void spherePrintPoint( QWinSphere * sp, double * p, int color )
{
    double ucoord[2];

    MATHFUNC(sphere_to_viewcoord)(p[0],p[1],p[2],ucoord);

    sp->printPoint( ucoord[0], ucoord[1], color );
}



// Intersects a line with a rectangle.  Changes the coordinates so that both endpoints
// are the endpoints of the visible part of the line.  Returns false if there is no visible
// part.
//
// The rectangle is given by [xmin,xmax] and [ymin,ymax], whereas the line is given by
// the two end points (x1,y1), (x2,y2).

bool LineRectangleIntersect( double & x1, double & y1, double & x2, double & y2,
                                        double xmin, double xmax, double ymin, double ymax )
{
    double dx, dy;

    if( p4_isnan(x1) || p4_isnan(x2) || p4_isnan(y1) || p4_isnan(y2) ||
        !p4_finite(x1) || !p4_finite(x2) || !p4_finite(y1) || !p4_finite(y2) )
    {
        return false;
    }

    if( (x1 < xmin && x2 < xmin) || (x1 > xmax && x2 > xmax) ||
        (y1 < ymin && y2 < ymin) || (y1 > ymax && y2 > ymax) )
    {
        // early out when it is easily seen that the line does not cut the window:

        return false;
    }

    dx = x2-x1;
    dy = y2-y1;

    if( fabs(dx) < fabs(dy) )
    {
        if( fabs(dy)==0 )
            return false;

        return LineRectangleIntersect( y1, x1, y2, x2, ymin, ymax, xmin, xmax );
    }

    if( dx < 0 )
    {
        return LineRectangleIntersect( x2, y2, x1, y1, xmin, xmax, ymin, ymax );
    }

    // here, we are sure that dx >= |dy| > 0.

    // Since dx > 0, and since we have done the early-out test,
    //   we know that x2 >= xmin and x1 <= xmax.

    if( x1 < xmin )
    {
        x1 = xmin;
        y1 = y2 + dy/dx*(xmin-x2);

        // now, we know that xmin <= x1 <= xmax.
    }

    // now check y1:

    if( dy > 0 )
    {
        if( y1 > ymax )
            return false;       // y will be increasing, and is already too large
        if( y1 < ymin )
        {
            if( y2 + (dy/dx)/(xmax-x2) < ymin )
                return false;   // y is increasing, but will never reach ymin quickly enough for x in [xmin,xmax].

            y1 = ymin;
            x1 = x2 + dx/dy*(ymin-y2);      // calculate intersection point.
        }
    }
    else
    {
        if( y1 < ymin )
            return false;
        if( y1 > ymax )
        {
            if( y2 + (dy/dx)/(xmax-x2) > ymax )
                return false;

            y1 = ymax;
            x1 = x2 + dx/dy*(ymax-y2);
        }
    }

    // here: (x1,y1) is inside the rectangle.
    //   we hence know that xmin <= x1 < x2  (since dx>0).

    if( x2 > xmax )
    {
        x2 = xmax;
        y2 = y1 + dy/dx*(xmax-x1);

        // now, we know that xmin <= x1 < x2 <= xmax.
    }

    if( dy < 0 )
    {
        if( y2 < ymin )
        {
            y2 = ymin;
            x2 = x1 + dx/dy*(ymin-y1);      // calculate intersection point.
        }
    }
    else
    {
        if( y2 > ymax )
        {
            y2 = ymax;
            x2 = x1 + dx/dy*(ymax-y1);
        }
    }
    return true;
}
