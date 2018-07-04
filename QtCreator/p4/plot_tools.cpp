/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  P4 is free software: you can redistribute it and/or modify
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

#include "plot_tools.hpp"

#include <cmath>

#include "P4ParentStudy.hpp"
#include "P4Sphere.hpp"
#include "math_p4.hpp"
#include "tables.hpp"

void (*plot_l)(P4Sphere *, const double *, const double *, int) = nullptr;
void (*plot_p)(P4Sphere *, const double *, int) = nullptr;

/*
void plotEllipse( QPainter * p, int cx, int cy, int a, int b, int color, bool
dotted,
                    int clipx0, int clipy0, int clipx1, int clipy1 )
{
//  THIS IS THE QT CODE: BUT FOR SOME REASON IT DOES NOT WORK WELL ALL THE TIME
WHEN ZOOMING
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

// FIXME: com fer que ho faci per cada sphere del vector?
void spherePlotLine(P4Sphere *sp, const double *p1, const double *p2,
                    int color)
{
    double ucoord1[2], ucoord2[2], ucoord3[2], ucoord4[2];

    if (MATHFUNC(sphere_to_viewcoordpair)(p1, p2, ucoord1, ucoord2, ucoord3,
                                          ucoord4)) {
        for (auto const &it : sp->sM_sphereList) {
            it->drawLine(ucoord1[0], ucoord1[1], ucoord2[0], ucoord2[1], color);
        }
    } else {
        for (auto const &it : sp->sM_sphereList) {
            it->drawLine(ucoord1[0], ucoord1[1], ucoord2[0], ucoord2[1], color);
            it->drawLine(ucoord3[0], ucoord3[1], ucoord4[0], ucoord4[1], color);
        }
    }
}

void spherePlotPoint(P4Sphere *sp, const double *p, int color)
{
    double ucoord[2];

    MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], ucoord);

    for (auto const &it : sp->sM_sphereList) {
        it->drawPoint(ucoord[0], ucoord[1], color);
    }
}

void spherePrintLine(P4Sphere *sp, const double *p1, const double *p2,
                     int color)
{
    double ucoord1[2];
    double ucoord2[2];
    double ucoord3[2];
    double ucoord4[2];

    if (MATHFUNC(sphere_to_viewcoordpair)(p1, p2, ucoord1, ucoord2, ucoord3,
                                          ucoord4)) {
        sp->printLine(ucoord1[0], ucoord1[1], ucoord2[0], ucoord2[1], color);
    } else {
        sp->printLine(ucoord1[0], ucoord1[1], ucoord2[0], ucoord2[1], color);
        sp->printLine(ucoord3[0], ucoord3[1], ucoord4[0], ucoord4[1], color);
    }
}

void spherePrintPoint(P4Sphere *sp, const double *p, int color)
{
    double ucoord[2];

    MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], ucoord);

    sp->printPoint(ucoord[0], ucoord[1], color);
}

// Intersects a line with a rectangle.  Changes the coordinates so that both
// endpoints are the endpoints of the visible part of the line.  Returns false
// if there is no visible part.
//
// The rectangle is given by [xmin,xmax] and [ymin,ymax], whereas the line is
// given by the two end points (x1,y1), (x2,y2).

bool lineRectangleIntersect(double &x1, double &y1, double &x2, double &y2,
                            double xmin, double xmax, double ymin, double ymax)
{
    double dx, dy;

    if (std::isnan(x1) || std::isnan(x2) || std::isnan(y1) || std::isnan(y2) ||
        !std::isfinite(x1) || !std::isfinite(x2) || !std::isfinite(y1) || !std::isfinite(y2)) {
        return false;
    }

    if ((x1 < xmin && x2 < xmin) || (x1 > xmax && x2 > xmax) ||
        (y1 < ymin && y2 < ymin) || (y1 > ymax && y2 > ymax)) {
        // early out when it is easily seen that the line does not cut the
        // window:
        return false;
    }

    dx = x2 - x1;
    dy = y2 - y1;

    if (fabs(dx) < fabs(dy)) {
        if (fabs(dy) == 0)
            return false;
        return lineRectangleIntersect(y1, x1, y2, x2, ymin, ymax, xmin, xmax);
    }

    if (dx < 0) {
        return lineRectangleIntersect(x2, y2, x1, y1, xmin, xmax, ymin, ymax);
    }

    // here, we are sure that dx >= |dy| > 0.

    // Since dx > 0, and since we have done the early-out test,
    //   we know that x2 >= xmin and x1 <= xmax.

    if (x1 < xmin) {
        x1 = xmin;
        y1 = y2 + dy / dx * (xmin - x2);

        // now, we know that xmin <= x1 <= xmax.
    }

    // now check y1:

    if (dy > 0) {
        if (y1 > ymax)
            return false; // y will be increasing, and is already too large
        if (y1 < ymin) {
            if (y2 + (dy / dx) / (xmax - x2) < ymin)
                return false; // y is increasing, but will never reach ymin
                              // quickly enough for x in [xmin,xmax].

            y1 = ymin;
            x1 = x2 + dx / dy * (ymin - y2); // calculate intersection point.
        }
    } else {
        if (y1 < ymin)
            return false;
        if (y1 > ymax) {
            if (y2 + (dy / dx) / (xmax - x2) > ymax)
                return false;

            y1 = ymax;
            x1 = x2 + dx / dy * (ymax - y2);
        }
    }

    // here: (x1,y1) is inside the rectangle.
    //   we hence know that xmin <= x1 < x2  (since dx>0).

    if (x2 > xmax) {
        x2 = xmax;
        y2 = y1 + dy / dx * (xmax - x1);

        // now, we know that xmin <= x1 < x2 <= xmax.
    }

    if (dy < 0) {
        if (y2 < ymin) {
            y2 = ymin;
            x2 = x1 + dx / dy * (ymin - y1); // calculate intersection point.
        }
    } else {
        if (y2 > ymax) {
            y2 = ymax;
            x2 = x1 + dx / dy * (ymax - y1);
        }
    }
    return true;
}
