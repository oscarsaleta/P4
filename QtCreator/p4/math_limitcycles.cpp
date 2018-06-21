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

#include "math_limitcycles.hpp"

#include <cmath>

#include "P4LimitCyclesDlg.hpp"
#include "P4ParentStudy.hpp"
#include "P4Sphere.hpp"
#include "custom.hpp"
#include "math_charts.hpp"
#include "math_orbits.hpp"
#include "math_p4.hpp"
#include "plot_tools.hpp"
#include "tables.hpp"

// static functions

static bool less(double x0, double y0, double x1, double y1)
{
    if (x0 < x1)
        return (1);
    if ((x0 == x1) && (y0 < y1))
        return (1);

    return (0);
}

// -----------------------------------------------------------------------------
//          eval_orbit
// -----------------------------------------------------------------------------
// Given a point qp and a transverse section ax+by+cz = 0, the orbit through qp
// is integrated for a maximum number of integration points.  If meanwhile the
// section is crossed, this is detected.  If not, false is returned.
//
// We need to detect two intersections (a periodic orbit needs to intersect any
// line an even number of times).  At the second intersection it tries to locate
// as precisely as possible the intersection point of the orbit with the
// transverse section and stores it in pp.
static bool eval_orbit(double qp[3], double a, double b, double c, double pp[3],
                       int dir)
{
    double p1[3], p2[3];
    double hhi, h_max, h_min, hhi2, i;
    int dashes, d;
    bool ok;

    ok = false;

    hhi = (double)dir * gVFResults.config_step_;
    h_max = gVFResults.config_hma_;
    h_min = gVFResults.config_hmi_;
    copy_x_into_y(qp, p1);

    if (!prepareVfForIntegration(p1))
        return false;

    MATHFUNC(integrate_sphere_orbit)
    (p1[0], p1[1], p1[2], p2, hhi, dashes, d, h_min, h_max);
    for (i = 0; i <= gVFResults.config_lc_numpoints_; i++) {
        copy_x_into_y(p2, p1);
        if (!prepareVfForIntegration(p1))
            return false;
        MATHFUNC(integrate_sphere_orbit)
        (p1[0], p1[1], p1[2], p2, hhi, dashes, d, h_min, h_max);
        if ((MATHFUNC(eval_lc)(p1, a, b, c) * MATHFUNC(eval_lc)(p2, a, b, c)) <=
            0) {
            // we have crossed the line on which the transverse section is
            // located (a*X+b*Y+c*Z has changed sign)
            ok = true;
            break;
        }
    }

    if (!ok)
        return false;

    ok = false;
    copy_x_into_y(p2, p1);

    if (!prepareVfForIntegration(p1))
        return false;

    MATHFUNC(integrate_sphere_orbit)
    (p1[0], p1[1], p1[2], p2, hhi, dashes, d, h_min, h_max);
    for (i = 0; i <= gVFResults.config_lc_numpoints_; i++) {
        copy_x_into_y(p2, p1);
        hhi2 = hhi;
        if (!prepareVfForIntegration(p1))
            return false;
        MATHFUNC(integrate_sphere_orbit)
        (p1[0], p1[1], p1[2], p2, hhi, dashes, d, h_min, h_max);
        if ((MATHFUNC(eval_lc)(p1, a, b, c) * MATHFUNC(eval_lc)(p2, a, b, c)) <=
            0) {
            ok = true;
            break;
        }
    }

    if (!ok)
        return false;

    if (fabs(hhi2) < fabs(hhi))
        hhi = hhi2;

    /* find the intersection point p3 with the poincare section with a precision
     * of 1e-8 */
    if ((fabs(MATHFUNC(eval_lc)(p2, a, b, c)) <= 1e-8) || fabs(hhi2) <= h_min) {
        copy_x_into_y(p2, pp);
    } else {
        while (1) {
            hhi2 = hhi / 2.0;
            if (!prepareVfForIntegration(p1))
                return false;
            MATHFUNC(integrate_sphere_orbit)
            (p1[0], p1[1], p1[2], pp, hhi2, dashes, d, h_min, fabs(hhi2));
            if ((fabs(MATHFUNC(eval_lc)(pp, a, b, c)) <= 1e-8) ||
                (fabs(hhi2) <= h_min))
                break;
            if ((MATHFUNC(eval_lc)(p1, a, b, c) *
                 MATHFUNC(eval_lc)(pp, a, b, c)) > 0) {
                copy_x_into_y(pp, p1);
                hhi = (double)dir * (fabs(hhi) - fabs(hhi2));
            } else
                hhi = hhi2;
        }
    }
    return ok;
}

// -----------------------------------------------------------------------------
//          searchLimitCycle
// -----------------------------------------------------------------------------
//
// Loop to find limit cycles cutting some transverse section determined by two
// end points.
void searchLimitCycle(P4Sphere *spherewnd, double x0, double y0, double x1,
                      double y1, double grid)
{
    double p1[3], pf1[3], pb1[3], rf1[2], rb1[2];
    double p2[3], pf2[3], pb2[3], rf2[2], rb2[2], p3[3];
    double grid_dx, grid_dy;
    double a, b, c; // ax+by+c=0 defines the equation of the line of the
                    // transverse section
    double x, y, z;
    bool okf1, okb1, found;
    int counter{0}; // counter to keep steps: after so many steps, the progress
                    // window needs to be updated

    // first make sure x0 < x1:
    if (x1 < x0) {
        z = x0;
        x0 = x1;
        x1 = z;
        z = y0;
        y0 = y1;
        y1 = z;
    } else if ((x0 == x1) && (y1 < y0)) {
        z = y0;
        y0 = y1;
        y1 = z;
    }

    z = atan2(y1 - y0, x1 - x0);
    grid_dx = grid * cos(z);
    grid_dy = grid * sin(z);
    if ((z <= PI_DIV4) && (z >= -PI_DIV4)) {
        a = (y1 - y0) / (x1 - x0); // abs(a) <= 1
        b = -1.0;
        c = y0 - a * x0;
    } else {
        a = -1.0;
        b = (x1 - x0) / (y1 - y0); // abs(b) <= 1
        c = x0 - b * y0;
    }

    // transverse section is located on the line a*X+b*Y+c*Z=0

    okf1 = 0;
    okb1 = 0;

    MATHFUNC(R2_to_sphere)(x1, y1, p2);

    found = true;
    while (found) {
        found = false; // assume not found
        while (1) {
            if (++counter == gVFResults.config_lc_value_) {
                counter = 0;
                if (stop_search_limit()) {
                    okf1 = false;
                    okb1 = false;
                    break;
                }
            }

            MATHFUNC(R2_to_sphere)(x0, y0, p1);
            if (okf1) {
                // the flag is still set from the previous time, where we found
                // an intersection through forward integration.  Now try
                // backwards
                okf1 = false;
            } else {
                // check if forward integration intersects the segment
                okf1 = eval_orbit(p1, a, b, c, pf1, 1);
                if (okf1 && MATHFUNC(less2)(p1, pf1) &&
                    MATHFUNC(less2)(pf1, p2))
                    break;
            }
            if (okb1) {
                // the flag is still set from the previous time, where we found
                // an intersection through backward integration.
                okb1 = false;
            } else {
                // check if backward integration intersects the segment
                okb1 = eval_orbit(p1, a, b, c, pb1, -1);
                if (okb1 && MATHFUNC(less2)(p1, pb1) &&
                    MATHFUNC(less2)(pb1, p2))
                    break;
            }
            // if both orbits intersect the line, but not on the segment, then
            // we have spiraling behaviour, so stop early
            if (okf1 && okb1 && MATHFUNC(less2)(pf1, p1) &&
                MATHFUNC(less2)(p2, pb1)) {
                okf1 = false;
                okb1 = false;
                break;
            }
            if (okf1 && okb1 && MATHFUNC(less2)(p2, pf1) &&
                MATHFUNC(less2)(pb1, p1)) {
                okf1 = false;
                okb1 = false;
                break;
            }

            // advance on the segment
            x0 += grid_dx;
            y0 += grid_dy;

            if (less(x1, y1, x0, y0)) {
                // stop at the end of the segment
                okf1 = false;
                okb1 = false;
                break;
            }
            write_to_limit_window();
        }
        if (okf1 || okb1) { // note: they cannot be both true here
            while (1) {
                x = x0;
                y = y0;
                // write_to_limit_window();
                if (++counter == gVFResults.config_lc_value_) {
                    counter = 0;
                    if (stop_search_limit()) {
                        okf1 = false;
                        okb1 = false;
                        break;
                    }
                }

                if (okf1) {
                    MATHFUNC(sphere_to_R2)(pf1[0], pf1[1], pf1[2], rf1);
                    while (1) {
                        x = x + grid_dx;
                        y = y + grid_dy;
                        if (less(rf1[0], rf1[1], x, y))
                            break;
                    }
                    if (less(x1, y1, x, y))
                        break;
                    MATHFUNC(R2_to_sphere)(x, y, p3);
                    if (eval_orbit(p3, a, b, c, pf2, 1)) {
                        if (MATHFUNC(less2)(pf2, p3) &&
                            MATHFUNC(less2)(p1, pf2)) {
                            MATHFUNC(sphere_to_R2)(pf2[0], pf2[1], pf2[2], rf2);
                            spherewnd->prepareDrawing();
                            storeLimitCycle(spherewnd, (rf1[0] + rf2[0]) / 2,
                                            (rf1[1] + rf2[1]) / 2, a, b, c);
                            spherewnd->finishDrawing();
                            found = true;
                            p1[0] = p3[0];
                            p1[1] = p3[1];
                            p1[2] = p3[2];
                            x0 = x;
                            y0 = y;
                            break;
                        }
                        if (MATHFUNC(less2)(pf2, p2)) {
                            p1[0] = p3[0];
                            p1[1] = p3[1];
                            p1[2] = p3[2];
                            pf1[0] = pf2[0];
                            pf1[1] = pf2[1];
                            pf1[2] = pf2[2];
                            x0 = x;
                            y0 = y;
                        }
                    } else {
                        found = true;
                        x0 = x;
                        y0 = y;
                        break;
                    }
                } else { // okb1=true
                    MATHFUNC(sphere_to_R2)(pb1[0], pb1[1], pb1[2], rb1);
                    while (1) {
                        x = x + grid_dx;
                        y = y + grid_dy;
                        if (less(rb1[0], rb1[1], x, y))
                            break;
                    }
                    if (less(x1, y1, x, y))
                        break;
                    MATHFUNC(R2_to_sphere)(x, y, p3);
                    if (eval_orbit(p3, a, b, c, pb2, -1)) {
                        if (MATHFUNC(less2)(pb2, p3) &&
                            MATHFUNC(less2)(p1, pb2)) {
                            MATHFUNC(sphere_to_R2)(pb2[0], pb2[1], pb2[2], rb2);
                            spherewnd->prepareDrawing();
                            storeLimitCycle(spherewnd, (rb1[0] + rb2[0]) / 2,
                                            (rb1[1] + rb2[1]) / 2, a, b, c);
                            spherewnd->finishDrawing();
                            found = true;
                            p1[0] = p3[0];
                            p1[1] = p3[1];
                            p1[2] = p3[2];
                            x0 = x;
                            y0 = y;
                            break;
                        }
                        if (MATHFUNC(less2)(pb2, p2)) {
                            p1[0] = p3[0];
                            p1[1] = p3[1];
                            p1[2] = p3[2];
                            pb1[0] = pb2[0];
                            pb1[1] = pb2[1];
                            pb1[2] = pb2[2];
                            x0 = x;
                            y0 = y;
                        }
                    } else {
                        found = true;
                        x0 = x;
                        y0 = y;
                        break;
                    }
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------
//          storeLimitCycle
// -----------------------------------------------------------------------------
// The found limit cycle is re-integrated, and stored in memory.
// It is meanwhile drawn on the screen.
// The limit cycle is found through forward integration.
void storeLimitCycle(P4Sphere *spherewnd, double x, double y, double a,
                     double b, double c)
{
    double p1[3], p2[3];
    double hhi, h_max, h_min;
    int dashes, d;
    p4orbits::orbits LC;
    p4orbits::orbits_points LCpoints;

    MATHFUNC(R2_to_sphere)(x, y, p1);
    copy_x_into_y(p1, LC.pcoord);
    LC.color = CLIMIT;
    (*plot_p)(spherewnd, p1, CLIMIT);

    hhi = gVFResults.config_step_;
    h_max = gVFResults.config_hma_;
    h_min = gVFResults.config_hmi_;
    if (!prepareVfForIntegration(p1))
        return;
    MATHFUNC(integrate_sphere_orbit)
    (p1[0], p1[1], p1[2], p2, hhi, dashes, d, h_min, h_max);

    copy_x_into_y(p2, LCpoints.pcoord);
    LCpoints.color = CLIMIT;
    LCpoints.dashes = gVFResults.config_dashes_;
    LC.points.push_back(LCpoints);
    // LC.current_point_index = 0;
    if (gVFResults.config_dashes_)
        (*plot_l)(spherewnd, p1, p2, CLIMIT);
    else
        (*plot_p)(spherewnd, p2, CLIMIT);

    while (1) {
        copy_x_into_y(p2, p1);
        MATHFUNC(integrate_sphere_orbit)
        (p1[0], p1[1], p1[2], p2, hhi, dashes, d, h_min, h_max);

        copy_x_into_y(p2, LCpoints.pcoord);
        LCpoints.color = CLIMIT;
        LCpoints.dashes = gVFResults.config_dashes_;
        LC.points.push_back(LCpoints);
        // LC.current_point_index++;
        if (gVFResults.config_dashes_)
            (*plot_l)(spherewnd, p1, p2, CLIMIT);
        else
            (*plot_p)(spherewnd, p2, CLIMIT);

        if ((MATHFUNC(eval_lc)(p1, a, b, c) * MATHFUNC(eval_lc)(p2, a, b, c)) <=
            0)
            break;
    }
    copy_x_into_y(p2, p1);
    if (!prepareVfForIntegration(p1))
        return;
    MATHFUNC(integrate_sphere_orbit)
    (p1[0], p1[1], p1[2], p2, hhi, dashes, d, h_min, h_max);

    while (1) {
        copy_x_into_y(p2, p1);
        if (!prepareVfForIntegration(p1))
            return;
        MATHFUNC(integrate_sphere_orbit)
        (p1[0], p1[1], p1[2], p2, hhi, dashes, d, h_min, h_max);

        copy_x_into_y(p2, LCpoints.pcoord);
        LCpoints.color = CLIMIT;
        LCpoints.dashes = gVFResults.config_dashes_;
        LC.points.push_back(LCpoints);

        if ((MATHFUNC(eval_lc)(p1, a, b, c) * MATHFUNC(eval_lc)(p2, a, b, c)) <=
            0)
            break;
        if (gVFResults.config_dashes_)
            (*plot_l)(spherewnd, p1, p2, CLIMIT);
        else
            (*plot_p)(spherewnd, p2, CLIMIT);
    }
    MATHFUNC(R2_to_sphere)(x, y, p2);
    copy_x_into_y(p2, gVFResults.limCycles_.back().pcoord);
    if (gVFResults.config_dashes_)
        (*plot_l)(spherewnd, p1, p2, CLIMIT);
    else
        (*plot_p)(spherewnd, p2, CLIMIT);

    gVFResults.limCycles_.push_back(LC);
}

// -----------------------------------------------------------------------
//          drawLimitCycles
// -----------------------------------------------------------------------
// Draw limit cycles that were calculated earlier.  This is called during
// a repaint (but also during a print command).
void drawLimitCycles(P4Sphere *spherewnd)
{
    for (auto const &it : gVFResults.limCycles_)
        drawOrbit(spherewnd, it.pcoord, it.points, it.color);
}

// -----------------------------------------------------------------------
//          deleteLastLimitCycle
// -----------------------------------------------------------------------
void deleteLastLimitCycle(P4Sphere *spherewnd)
{
    if (gVFResults.limCycles_.empty())
        return;

    p4orbits::orbits &orbit1 = gVFResults.limCycles_.back();
    drawOrbit(spherewnd, orbit1.pcoord, orbit1.points,
              spherewnd->spherebgcolor_);

    gVFResults.limCycles_.pop_back();
}
