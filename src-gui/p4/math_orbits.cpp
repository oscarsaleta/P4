/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
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

#include "math_orbits.h"

#include "custom.h"
#include "math_charts.h"
#include "math_numerics.h"
#include "math_p4.h"
#include "math_polynom.h"
#include "plot_tools.h"

#include <cmath>

// -----------------------------------------------------------------------
//                      INTEGRATEORBIT
// -----------------------------------------------------------------------
//
// dir = -1: backwards, dir=0: continue, dir=+1: forwards

void integrateOrbit(QWinSphere *sphere, int dir)
{
    struct orbits_points *sep;
    double pcoord[3], ucoord[2];

    if (dir == 0) {
        // continue orbit button has been pressed

        dir = VFResults.current_orbit->current_f_orbits->dir;

        copy_x_into_y(VFResults.current_orbit->current_f_orbits->pcoord,
                      pcoord);
        VFResults.current_orbit->current_f_orbits->next_point =
            integrate_orbit(sphere, pcoord, VFResults.config_currentstep, dir,
                            CORBIT, VFResults.config_intpoints, &sep);

        VFResults.current_orbit->current_f_orbits = sep;
        return;
    }

    copy_x_into_y(VFResults.current_orbit->pcoord, pcoord);
    MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
    if (VFResults.config_kindvf == INTCONFIG_ORIGINAL)
        if (eval_term2(VFResults.gcf, ucoord) < 0)
            dir = -dir;

    if (VFResults.current_orbit->f_orbits == nullptr) {
        VFResults.current_orbit->f_orbits =
            integrate_orbit(sphere, pcoord, VFResults.config_step, dir, CORBIT,
                            VFResults.config_intpoints, &sep);
    } else {
        VFResults.current_orbit->current_f_orbits->next_point =
            new orbits_points;
        VFResults.current_orbit->current_f_orbits =
            VFResults.current_orbit->current_f_orbits->next_point;
        copy_x_into_y(pcoord,
                      VFResults.current_orbit->current_f_orbits->pcoord);
        VFResults.current_orbit->current_f_orbits->dashes = 0;
        VFResults.current_orbit->current_f_orbits->color = CORBIT;
        VFResults.current_orbit->current_f_orbits->dir = dir;
        VFResults.current_orbit->current_f_orbits->next_point =
            integrate_orbit(sphere, pcoord, VFResults.config_step, dir, CORBIT,
                            VFResults.config_intpoints, &sep);
    }
    VFResults.current_orbit->current_f_orbits = sep;
}

// -----------------------------------------------------------------------
//                      STARTORBIT
// -----------------------------------------------------------------------
/* R=0 then point selected in the drawing canvas else in the orbit window */

bool startOrbit(QWinSphere *sphere, double x, double y, bool R)
{
    double pcoord[3];
    double ucoord[2];

    if (VFResults.first_orbit == nullptr) {
        VFResults.first_orbit = new orbits;
        VFResults.current_orbit = VFResults.first_orbit;
    } else {
        VFResults.current_orbit->next_orbit = new orbits;
        VFResults.current_orbit = VFResults.current_orbit->next_orbit;
    }
    if (R)
        MATHFUNC(R2_to_sphere)(x, y, pcoord);
    else
        MATHFUNC(viewcoord_to_sphere)(x, y, pcoord);

    copy_x_into_y(pcoord, VFResults.current_orbit->pcoord);
    VFResults.current_orbit->color = CORBIT;
    VFResults.current_orbit->f_orbits = nullptr;
    VFResults.current_orbit->next_orbit = nullptr;

    MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], ucoord);
    sphere->drawPoint(ucoord[0], ucoord[1], CORBIT);

    return true;
}

// -----------------------------------------------------------------------
//                      DRAWORBIT
// -----------------------------------------------------------------------

void drawOrbit(QWinSphere *spherewnd, double *pcoord,
               struct orbits_points *points, int color)
{
    double pcoord1[3];

    copy_x_into_y(pcoord, pcoord1);
    (*plot_p)(spherewnd, pcoord, color);

    while (points != nullptr) {
        if (points->dashes) {
            (*plot_l)(spherewnd, pcoord1, points->pcoord, color);
        } else {
            (*plot_p)(spherewnd, points->pcoord, color);
        }

        copy_x_into_y(points->pcoord, pcoord1);

        points = points->next_point;
    }
}

// -----------------------------------------------------------------------
//                      DRAWORBITS
// -----------------------------------------------------------------------

void drawOrbits(QWinSphere *spherewnd)
{
    struct orbits *orbit;

    for (orbit = VFResults.first_orbit; orbit != nullptr;
         orbit = orbit->next_orbit) {
        drawOrbit(spherewnd, orbit->pcoord, orbit->f_orbits, orbit->color);
    }
}

// -----------------------------------------------------------------------
//                      DELETELASTORBIT
// -----------------------------------------------------------------------

void deleteLastOrbit(QWinSphere *spherewnd)
{
    struct orbits *orbit1, *orbit2;

    if (VFResults.current_orbit == nullptr)
        return;

    orbit2 = VFResults.current_orbit;
    drawOrbit(spherewnd, orbit2->pcoord, orbit2->f_orbits,
              spherewnd->spherebgcolor);

    if (VFResults.first_orbit == VFResults.current_orbit) {
        VFResults.first_orbit = nullptr;
        VFResults.current_orbit = nullptr;
    } else {
        orbit1 = VFResults.first_orbit;

        do {
            VFResults.current_orbit = orbit1;
            orbit1 = orbit1->next_orbit;
        } while (orbit1 != orbit2);

        VFResults.current_orbit->next_orbit = nullptr;
    }
    VFResults.deleteOrbitPoint(orbit2->f_orbits);
    delete orbit2; // free( orbit2 );
    orbit2 = nullptr;
}

/*integrate poincare sphere case p=q=1 */
void integrate_poincare_orbit(double p0, double p1, double p2, double *pcoord,
                              double *hhi, int *dashes, int *dir, double h_min,
                              double h_max)
{
    double y[2], theta;

    *dashes = true;
    *dir = 1;
    if (pcoord[2] > ZCOORD) {
        psphere_to_R2(p0, p1, p2, y);
        rk78(eval_r_vec_field, y, hhi, h_min, h_max,
             VFResults.config_tolerance);
        R2_to_psphere(y[0], y[1], pcoord);
    } else {
        theta = atan2(fabs(p1), fabs(p0));
        if ((theta < PI_DIV4) && (theta > -PI_DIV4)) {
            if (p0 > 0) {
                psphere_to_U1(p0, p1, p2, y);
                rk78(eval_U1_vec_field, y, hhi, h_min, h_max,
                     VFResults.config_tolerance);
                if (y[1] >= 0 || !VFResults.singinf)
                    U1_to_psphere(y[0], y[1], pcoord);
                else {
                    VV1_to_psphere(y[0], y[1], pcoord);
                    if (VFResults.dir_vec_field == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                    }
                    psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], y);
                    *dashes = false;
                }
            } else {
                psphere_to_V1(p0, p1, p2, y);
                rk78(eval_V1_vec_field, y, hhi, h_min, h_max,
                     VFResults.config_tolerance);
                if (y[1] >= 0 || !VFResults.singinf)
                    V1_to_psphere(y[0], y[1], pcoord);
                else {
                    UU1_to_psphere(y[0], y[1], pcoord);
                    if (VFResults.dir_vec_field == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                    }
                    psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], y);
                    *dashes = false;
                }
            }
        } else {
            if (p1 > 0) {
                psphere_to_U2(p0, p1, p2, y);
                rk78(eval_U2_vec_field, y, hhi, h_min, h_max,
                     VFResults.config_tolerance);
                if (y[1] >= 0 || !VFResults.singinf)
                    U2_to_psphere(y[0], y[1], pcoord);
                else {
                    VV2_to_psphere(y[0], y[1], pcoord);
                    if (VFResults.dir_vec_field == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                    }
                    psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], y);
                    *dashes = false;
                }
            } else {
                psphere_to_V2(p0, p1, p2, y);
                rk78(eval_V2_vec_field, y, hhi, h_min, h_max,
                     VFResults.config_tolerance);
                if (y[1] >= 0 || !VFResults.singinf)
                    V2_to_psphere(y[0], y[1], pcoord);
                else {
                    UU2_to_psphere(y[0], y[1], pcoord);
                    if (VFResults.dir_vec_field == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                    }
                    psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], y);
                    *dashes = false;
                }
            }
        }
    }
}

/* integrate on the Poincare-Lyapunov sphere */
void integrate_lyapunov_orbit(double p0, double p1, double p2, double *pcoord,
                              double *hhi, int *dashes, int *dir, double h_min,
                              double h_max)
{
    double y[2];

    *dashes = true;
    *dir = 1;
    if (p0 == 0) {
        y[0] = p1;
        y[1] = p2;
        rk78(eval_r_vec_field, y, hhi, h_min, h_max,
             VFResults.config_tolerance);
        R2_to_plsphere(y[0], y[1], pcoord);
    } else {
        y[0] = p1;
        y[1] = p2;
        rk78(eval_vec_field_cyl, y, hhi, h_min, h_max,
             VFResults.config_tolerance);
        if (y[1] >= TWOPI)
            y[1] -= TWOPI;
        cylinder_to_plsphere(y[0], y[1], pcoord);
    }
}

struct orbits_points *integrate_orbit(QWinSphere *spherewnd, double pcoord[3],
                                      double step, int dir, int color,
                                      int points_to_int,
                                      struct orbits_points **orbit)
{
    int i, d, h;
    int dashes;
    double hhi;
    double pcoord2[3], h_min, h_max;
    struct orbits_points *first_orbit = nullptr, *last_orbit = nullptr;

    hhi = (double)dir * step;
    h_min = VFResults.config_hmi;
    h_max = VFResults.config_hma;
    copy_x_into_y(pcoord, pcoord2);
    for (i = 1; i <= points_to_int; ++i) {
        MATHFUNC(integrate_sphere_orbit)
        (pcoord[0], pcoord[1], pcoord[2], pcoord, &hhi, &dashes, &d, h_min,
         h_max);

        if ((i % UPDATEFREQ_STEPSIZE) == 0)
            set_current_step(fabs(hhi));

        if (last_orbit == nullptr) {
            first_orbit = new orbits_points;
            last_orbit = first_orbit;
            h = dir;
        } else {
            last_orbit->next_point = new orbits_points;
            h = last_orbit->dir;
            last_orbit = last_orbit->next_point;
        }

        copy_x_into_y(pcoord, last_orbit->pcoord);
        last_orbit->color = color;
        last_orbit->dashes = dashes * VFResults.config_dashes;
        last_orbit->dir = d * h;
        last_orbit->next_point = nullptr;
        if (dashes * VFResults.config_dashes)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);
    }
    set_current_step(fabs(hhi));
    *orbit = last_orbit;
    return (first_orbit);
}
