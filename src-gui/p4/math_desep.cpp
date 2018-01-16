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

#include "math_desep.h"

#include "file_tab.h"
#include "math_separatrice.h"

static std::vector<p4polynom::term2> vec_field_0;
static std::vector<p4polynom::term2> vec_field_1;

// ---------------------------------------------------------------------------
//          eval_blow_vec_field
// ---------------------------------------------------------------------------
void eval_blow_vec_field(const double *y, double *f)
{
    f[0] = eval_term2(vec_field_0, y);
    f[1] = eval_term2(vec_field_1, y);
}

// ---------------------------------------------------------------------------
//          power
// ---------------------------------------------------------------------------
// Calculates a^b with the convention that a^0=1  (hence also 0^0=1)
static double power(double a, int b)
{
    if (b == 0)
        return 1.0;
    else
        return pow(a, (double)b);
}

// ---------------------------------------------------------------------------
//          make_transformations
// ---------------------------------------------------------------------------
void make_transformations(std::vector<p4blowup::transformations> trans,
                          double x0, double y0, double *point)
{
    double x{x0}, y{y0};
    for (auto t : trans) {
        point[0] = t.x0 + ((double)t.c1) * power(x, t.d1) * power(y, t.d2);
        point[1] = t.y0 + ((double)t.c2) * power(x, t.d3) * power(y, t.d4);
        x = point[0];
        y = point[1];
    }
}

// ---------------------------------------------------------------------------
//          integrate_blow_up
// ---------------------------------------------------------------------------
std::vector<p4orbits::orbits_points> integrate_blow_up(
    QWinSphere *spherewnd, double *pcoord2, p4blowup::blow_up_points &de_sep,
    double step, int dir, int type, p4orbits::orbits_points &orbit, int chart)
{
    int i;
    double hhi, hhi0;
    double y0[2], y[2];
    double h_min{g_VFResults.config_hmi_}, h_max{g_VFResults.config_hma_};
    double point[2], pcoord[3];
    int color;
    bool dashes, ok{true};
    p4orbits::orbits_points last_orbit;
    std::vector<p4orbits::orbits_points> orbit_result;

    vec_field_0 = de_sep.vector_field_0;
    vec_field_1 = de_sep.vector_field_1;

    y[0] = de_sep.point[0];
    y[1] = de_sep.point[1];

    make_transformations(
        de_sep.trans, de_sep.x0 + de_sep.a11 * y[0] + de_sep.a12 * y[1],
        de_sep.y0 + de_sep.a21 * y[0] + de_sep.a22 * y[1], point);

    switch (chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
        break;
    }

    if (!prepareForIntegration(pcoord))
        return std::vector<p4orbits::orbits_points>();

    if (g_VFResults.plweights_ == false &&
        (chart == CHART_V1 || chart == CHART_V2))
        dir *= g_VFResults.vf_->back().dir_vec_field_;

    hhi = (double)dir * step;
    y[0] = de_sep.point[0];
    y[1] = de_sep.point[1];

    for (i = 1; i <= g_VFResults.config_intpoints_; ++i) {
        hhi0 = hhi;
        y0[0] = y[0];
        y0[1] = y[1];
        while (1) {
            y[0] = y0[0];
            y[1] = y0[1];
            rk78(eval_blow_vec_field, y, &hhi0, g_VFResults.config_hmi_,
                 g_VFResults.config_hma_, g_VFResults.config_tolerance_);
            make_transformations(
                de_sep.trans, de_sep.x0 + de_sep.a11 * y[0] + de_sep.a12 * y[1],
                de_sep.y0 + de_sep.a21 * y[0] + de_sep.a22 * y[1], point);
            switch (chart) {
            case CHART_R2:
                MATHFUNC(R2_to_sphere)
                (point[0], point[1], point[2], pcoord);
                break;
            case CHART_U1:
                if (point[1] >= 0 || !g_VFResults.vf_->back().singinf_)
                    MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                else
                    VV1_to_psphere(point[0], point[1], pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
                break;
            case CHART_U2:
                if (point[1] >= 0 || !g_VFResults.vf_->back().singinf_)
                    MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
                else
                    VV2_to_psphere(point[0], point[1], pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
                break;
            }

            if (g_ThisVF->getVFIndex_sphere(pcoord) == g_VFResults.K_)
                break;
            h_min = g_VFResults.config_branchhmi_;
            h_max /= 2;
            hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
            if (fabs(hhi0) < h_min || h_max < g_VFResults.config_branchhmi_) {
                hhi0 = h_min;
                if (hhi < 0)
                    hhi0 = -hhi0;
                de_sep.integrating_in_local_chart = false;
                break;
            }
        }
        hhi = hhi0;

        dashes = true;
        switch (chart) {
        case CHART_R2:
            MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord);
            color = findSepColor(g_VFResults.vf_->back().gcf_, type, point);
            break;

        case CHART_U1:
            if (point[1] >= 0 || !g_VFResults.vf_->back().singinf_) {
                MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (g_VFResults.vf_->back().dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = de_sep.type;
                color =
                    findSepColor(g_VFResults.vf_->back().gcf_U1_, type, point);
            } else {
                VV1_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (g_VFResults.vf_->back().dir_vec_field_ == 1)
                        dir *= -1;
                }
                if (g_VFResults.vf_->back().dir_vec_field_ == 1)
                    type = change_type(de_sep.type);
                else
                    type = de_sep.type;
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
                color =
                    findSepColor(g_VFResults.vf_->back().gcf_V1_, type, point);
            }
            break;

        case CHART_V1:
            MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
            if (!g_VFResults.plweights_)
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor(g_VFResults.vf_->back().gcf_V1_, type, point);
            break;

        case CHART_U2:
            if (point[1] >= 0 || !g_VFResults.vf_->back().singinf_) {
                MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (g_VFResults.vf_->back().dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = de_sep.type;
                color =
                    findSepColor(g_VFResults.vf_->back().gcf_U2_, type, point);
            } else {
                VV2_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (g_VFResults.vf_->back().dir_vec_field_ == 1)
                        dir *= -1;
                }
                if (g_VFResults.vf_->back().dir_vec_field_ == 1)
                    type = change_type(de_sep.type);
                else
                    type = de_sep.type;
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
                color =
                    findSepColor(g_VFResults.vf_->back().gcf_V2_, type, point);
            }
            break;

        case CHART_V2:
            MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
            if (!g_VFResults.plweights_)
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor(g_VFResults.vf_->back().gcf_V2_, type, point);
            break;

        default:
            color = 0;
            break;
        }

        last_orbit.pcoord[0] = pcoord[0];
        last_orbit.pcoord[1] = pcoord[1];
        last_orbit.pcoord[2] = pcoord[2];
        last_orbit.color = color;
        last_orbit.dashes = dashes * g_VFResults.config_dashes_;
        last_orbit.dir = ((g_VFResults.plweights_ == false) &&
                          (chart == CHART_V1 || chart == CHART_V2))
                             ? g_VFResults.dir_vec_field_ * dir
                             : dir;
        last_orbit.type = type;

        orbit_result.push_back(last_orbit);

        if (last_orbit.dashes)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);

        if (y[0] * y[0] + y[1] * y[1] >= 1.0)
            de_sep.integrating_in_local_chart = false;
        if (!de_sep.integrating_in_local_chart)
            break;

        copy_x_into_y(pcoord, pcoord2);
    }
    de_sep.point[0] = y[0];
    de_sep.point[1] = y[1];
    // FIXME que fer amb el parametre orbit?
    /*if (first_orbit != NULL)
        *orbit = last_orbit;
    return (first_orbit);*/
    return orbit_result;
}

// ---------------------------------------------------------------------------
//                  PLOT_SEP_BLOW_UP
// ---------------------------------------------------------------------------
//
// Starts separatrix integration, and plots the results meanwhile to the screen
//
// The first 100 points are drawn by looking at the formal power series
// development of the separatrix, as calculated by maple.  These points are
// drawn in an epsilon-ball around the singularity, where epsilon can be changed
// by the user.
//
// At the end, a normal integration cycle is added.
static std::vector<p4orbits::orbits_points> plot_sep_blow_up(
    QWinSphere *spherewnd, double x0, double y0, int chart, double epsilon,
    p4blowup::blow_up_points &de_sep, p4orbits::orbits_points &orbit,
    int vfindex)
{
    double h, t{0}, pcoord[3], pcoord2[3], point[2];
    int i, color, dir, dashes, type, ok{true};
    p4orbits::orbits_points last_orbit;
    std::vector<p4orbits::orbits_points> orbit_result;

    switch (chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(x0, y0, pcoord);
        break;
    }
    if (!prepareVfForIntegration(pcoord))
        return std::vector<p4orbits::orbits_points>();

    /* if we have a line of singularities at infinity then we have to change the
     * chart if the chart is V1 or V2 */
    if (g_VFResults.vf_->back().singinf_) {
        if (chart == CHART_V1)
            chart = CHART_U1;
        else if (chart == CHART_V2)
            chart = chart_U2;
    }

    h = epsilon / 100.;

    type = de_sep.type;
    switch (type) {
    case OT_STABLE:
        dir = OT_STABLE;
        break;
    case OT_UNSTABLE:
        dir = OT_UNSTABLE;
        break;
    case OT_CENT_STABLE:
        dir = OT_CENT_STABLE;
        break;
    case OT_CENT_UNSTABLE:
        dir = OT_CENT_UNSTABLE;
        break;
    default:
        dir = 0;
        break;
    }

    point[0] = x0;
    point[1] = y0;
    switch (chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(x0, y0, pcoord);
        color = findSepColor(g_VFResults.vf_->back().gcf_, de_sep.type, point);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(x0, y0, pcoord);
        color =
            findSepColor(g_VFResults.vf_->back().gcf_U1_, de_sep.type, point);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(x0, y0, pcoord);
        color =
            findSepColor(g_VFResults.vf_->back().gcf_V1_, de_sep.type, point);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(x0, y0, pcoord);
        color =
            findSepColor(g_VFResults.vf_->back().gcf_U2_, de_sep.type, point);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(x0, y0, pcoord);
        color =
            findSepColor(g_VFResults.vf_->back().gcf_V2_, de_sep.type, point);
        break;
    }

    // P5 addition: check if one can start with this separatrice
    y = eval_term1(de_sep.sep, h * 100);
    make_transformations(
        de_sep.trans, de_sep.x0 + de_sep.a11 * h * 100 + de_sep.a12 * y,
        de_sep.y0 + de_sep.a21 * h * 100 + de_sep * a22 * y, point);

    switch (chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord2);
        break;
    case CHART_U1:
        if (point[1] >= 0 || !g_VFResults.vf_->back().singinf_)
            MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord2);
        else
            VV1_to_psphere(point[0], point[1], pcoord2);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord2);
        break;
    case CHART_U2:
        if (point[1] >= 0 || !g_VFResults.vf_->back().singinf_)
            MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord2);
        else
            VV2_to_psphere(point[0], point[1], pcoord2);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord2);
        break;
    }

    if (g_ThisVF->getVFIndex_sphere(pcoord2) != vfindex)
        return std::vector<p4orbits::orbits_points>();

    // end of P5 addition

    copy_x_into_y(pcoord, last_orbit.pcoord);
    last_orbit.color = color;
    last_orbit.dashes = 0;
    last_orbit.dir = dir; // TODO: aixo no hi era
    copy_x_into_y(pcoord, pcoord2);
    orbit_result.push_back(last_orbit);
    for (i = 0; i <= 99; i++) {
        dashes = true;
        y = eval_term1(de_sep.sep, t);
        make_transformations(
            de_sep.trans, de_sep.x0 + de_sep.a11 * t + de_sep.a12 * y,
            de_sep.y0 + de_sep.a21 * t + de_sep.a22 * y, point);
        switch (chart) {
        case CHART_R2:
            MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord);
            color =
                findSepColor(g_VFResults.vf_->back().gcf_, de_sep.type, point);
            break;

        case CHART_U1:
            if (point[1] = > 0 || !g_VFResults.vf_->back().singinf_) {
                MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                }
                type = de_sep.type;
                color =
                    findSepColor(g_VFResults.vf_->back().gcf_U1_, type, point);
            } else {
                VV1_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                }
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
                if (g_VFResults.vf_->back().dir_vec_field_ == 1)
                    type = change_type(de_sep.type);
                else
                    type = de_sep.type;
                color =
                    findSepColor(g_VFResults.vf_->back().gcf_V1_, type, point);
            }
            break;

        case CHART_V1:
            MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
            if (!g_VFResults.plweights_)
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor(g_VFResults.vf_->back().gcf_V1_, de_sep->type,
                                 point);
            break;

        case CHART_U2:
            if (point[1] >= 0 || !g_VFResults.vf_->back().singinf_) {
                MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                }
                type = de_sep->type;
                color =
                    findSepColor(g_VFResults.vf_->back().gcf_U2_, type, point);
            } else {
                VV2_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                }
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
                if (g_VFResults.vf_->back().dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;
                color =
                    findSepColor(g_VFResults.vf_->back().gcf_V2_, type, point);
            }
            break;

        case CHART_V2:
            MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
            if (!g_VFResults.plweights_)
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor(g_VFResults.vf_->back().gcf_V2_, de_sep->type,
                                 point);
            break;
        }

        copy_x_into_y(pcoord, last_orbit.pcoord);
        last_orbit.color = color;
        last_orbit.dashes = dashes * g_VFResults.config_dashes_;
        last_orbit.dir = dir;
        last_orbit.type = type;
        if (last_orbit.dashes)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);

        orbit_result.push_back(last_orbit);
    }

    de_sep.point[0] = t;
    de_sep.point[1] = y;
    de_sep.integrating_in_local_chart = true;
    // FIXME tot això
    sep = last_orbit;
    last_orbit.nextpt =
        integrate_blow_up(spherewnd, pcoord2, de_sep, g_VFResults.config_step_,
                          dir, last_orbit.type, &sep, chart);
    if (first_orbit != NULL)
        *orbit = sep;
    return first_orbit;
}
