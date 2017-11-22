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

#include "math_intblowup.h"

#include "file_tab.h"
#include "math_charts.h"
#include "math_numerics.h"
#include "math_p4.h"
#include "math_polynom.h"
#include "math_separatrice.h"
#include "plot_tools.h"

// static global variables
static term2 *s_vec_field[2];

// function definitions
void eval_blow_vec_field(double *y, double *f)
{
    f[0] = eval_term2(s_vec_field[0], y);
    f[1] = eval_term2(s_vec_field[1], y);
}

orbits_points *integrate_blow_up(QWinSphere *spherewnd, // double x0, double y0,
                                 double *pcoord2, blow_up_points *de_sep,
                                 double step, int dir, int type,
                                 orbits_points **orbit, int chart)
{
    int i;
    double hhi, point[2];
    double pcoord[3];
    double y[2];
    int color, dashes, ok = true;
    orbits_points *first_orbit = nullptr, *last_orbit = nullptr;

    s_vec_field[0] = de_sep->vector_field[0];
    s_vec_field[1] = de_sep->vector_field[1];
    if (g_VFResults.plweights_ == false &&
        (chart == CHART_V1 || chart == CHART_V2))
        dir = g_VFResults.dir_vec_field_ * dir;

    hhi = (double)dir * step;
    y[0] = de_sep->point[0];
    y[1] = de_sep->point[1];
    for (i = 1; i <= g_VFResults.config_intpoints_; ++i) {
        rk78(eval_blow_vec_field, y, &hhi, g_VFResults.config_hmi_,
             g_VFResults.config_hma_, g_VFResults.config_tolerance_);
        make_transformations(
            de_sep->trans, de_sep->x0 + de_sep->a11 * y[0] + de_sep->a12 * y[1],
            de_sep->y0 + de_sep->a21 * y[0] + de_sep->a22 * y[1], point);
        dashes = true;
        switch (chart) {
        case CHART_R2:
            MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord);
            color = findSepColor2(g_VFResults.gcf_, type, point);
            break;

        case CHART_U1:
            if (point[1] >= 0 || !g_VFResults.singinf_) {
                MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (g_VFResults.dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = de_sep->type;
                color = findSepColor2(g_VFResults.gcf_U1_, type, point);
            } else {
                VV1_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (g_VFResults.dir_vec_field_ == 1)
                        dir *= -1;
                }
                if (g_VFResults.dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;

                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
                color = findSepColor2(g_VFResults.gcf_V1_, type, point);
            }
            break;

        case CHART_V1:
            MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
            if (g_VFResults.plweights_ == false)
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(g_VFResults.gcf_V1_, type, point);
            break;

        case CHART_U2:
            if (point[1] >= 0 || !g_VFResults.singinf_) {
                MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (g_VFResults.dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = de_sep->type;
                color = findSepColor2(g_VFResults.gcf_U2_, type, point);
            } else {
                VV2_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (g_VFResults.dir_vec_field_ == 1)
                        dir *= -1;
                }
                if (g_VFResults.dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
                color = findSepColor2(g_VFResults.gcf_V2_, type, point);
            }
            break;

        case CHART_V2:
            MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
            if (g_VFResults.plweights_ == false)
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(g_VFResults.gcf_V2_, type, point);
            break;

        default:
            color = 0;
            break;
        }
        if (last_orbit == nullptr) {
            first_orbit = new orbits_points;
            last_orbit = first_orbit;
        } else {
            last_orbit->next_point = new orbits_points;
            last_orbit = last_orbit->next_point;
        }
        last_orbit->pcoord[0] = pcoord[0];
        last_orbit->pcoord[1] = pcoord[1];
        last_orbit->pcoord[2] = pcoord[2];
        last_orbit->color = color;
        last_orbit->dashes = dashes * g_VFResults.config_dashes_;
        last_orbit->dir = ((g_VFResults.plweights_ == false) &&
                           (chart == CHART_V1 || chart == CHART_V2)) ?
                              g_VFResults.dir_vec_field_ * dir :
                              dir;
        last_orbit->type = type;
        last_orbit->next_point = nullptr;
        if (last_orbit->dashes)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);

        if (y[0] * y[0] + y[1] * y[1] >= 1.0) {
            de_sep->blow_up_vec_field = false;
            break;
        }
        copy_x_into_y(pcoord, pcoord2);
    }
    de_sep->point[0] = y[0];
    de_sep->point[1] = y[1];
    *orbit = last_orbit;

    return (first_orbit);
}
