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

#include "custom.h"
#include "file_tab.h"
#include "math_separatrice.h"

static std::vector<p4polynom::term2> s_vec_field_0;
static std::vector<p4polynom::term2> s_vec_field_1;

// ---------------------------------------------------------------------------
//          eval_blow_vec_field
// ---------------------------------------------------------------------------
void eval_blow_vec_field(const double *y, double *f)
{
    f[0] = eval_term2(s_vec_field_0, y);
    f[1] = eval_term2(s_vec_field_1, y);
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
void make_transformations(const std::vector<p4blowup::transformations> &trans,
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
    std::shared_ptr<P4WinSphere> spherewnd, double *pcoord2,
    p4blowup::blow_up_points &de_sep, double step, int dir, int type, int chart)
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

    s_vec_field_0 = de_sep.vector_field_0;
    s_vec_field_1 = de_sep.vector_field_1;

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
        return std::vector<p4orbits::orbits_points>{};

    if (g_VFResults.plweights_ == false &&
        (chart == CHART_V1 || chart == CHART_V2))
        dir *= g_VFResults.vf_[g_VFResults.K_].dir_vec_field_;

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
                MATHFUNC(R2_to_sphere)(point[0], point[1], point[2], pcoord);
                break;
            case CHART_U1:
                if (point[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_)
                    MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                else
                    VV1_to_psphere(point[0], point[1], pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
                break;
            case CHART_U2:
                if (point[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_)
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
            color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_, type,
                                  point);
            break;

        case CHART_U1:
            if (point[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_) {
                MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (g_VFResults.vf_[g_VFResults.K_].dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = de_sep.type;
                color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_U1_,
                                      type, point);
            } else {
                VV1_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (g_VFResults.vf_[g_VFResults.K_].dir_vec_field_ == 1)
                        dir *= -1;
                }
                if (g_VFResults.vf_[g_VFResults.K_].dir_vec_field_ == 1)
                    type = change_type(de_sep.type);
                else
                    type = de_sep.type;
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
                color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_V1_,
                                      type, point);
            }
            break;

        case CHART_V1:
            MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
            if (!g_VFResults.plweights_)
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_V1_, type,
                                  point);
            break;

        case CHART_U2:
            if (point[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_) {
                MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (g_VFResults.vf_[g_VFResults.K_].dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = de_sep.type;
                color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_U2_,
                                      type, point);
            } else {
                VV2_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (g_VFResults.vf_[g_VFResults.K_].dir_vec_field_ == 1)
                        dir *= -1;
                }
                if (g_VFResults.vf_[g_VFResults.K_].dir_vec_field_ == 1)
                    type = change_type(de_sep.type);
                else
                    type = de_sep.type;
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
                color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_V2_,
                                      type, point);
            }
            break;

        case CHART_V2:
            MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
            if (!g_VFResults.plweights_)
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_V2_, type,
                                  point);
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
    return orbit_result;
}

// ---------------------------------------------------------------------------
//          plot_sep_blow_up
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
static std::optional<std::vector<p4orbits::orbits_points>> plot_sep_blow_up(
    std::shared_ptr<P4WinSphere> spherewnd, double x0, double y0, int chart,
    double epsilon, p4blowup::blow_up_points &de_sep, int vfindex)
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
        return {};

    /* if we have a line of singularities at infinity then we have to change the
     * chart if the chart is V1 or V2 */
    if (g_VFResults.vf_[g_VFResults.K_].singinf_) {
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
        color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_, de_sep.type,
                              point);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_U1_,
                              de_sep.type, point);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_V1_,
                              de_sep.type, point);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_U2_,
                              de_sep.type, point);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_V2_,
                              de_sep.type, point);
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
        if (point[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_)
            MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord2);
        else
            VV1_to_psphere(point[0], point[1], pcoord2);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord2);
        break;
    case CHART_U2:
        if (point[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_)
            MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord2);
        else
            VV2_to_psphere(point[0], point[1], pcoord2);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord2);
        break;
    }

    if (g_ThisVF->getVFIndex_sphere(pcoord2) != vfindex)
        return {};

    // end of P5 addition

    copy_x_into_y(pcoord, last_orbit.pcoord);
    last_orbit.color = color;
    last_orbit.dashes = 0;
    last_orbit.dir = dir;  // TODO: aixo no hi era
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
            color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_,
                                  de_sep.type, point);
            break;

        case CHART_U1:
            if (point[1] = > 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_) {
                MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                }
                type = de_sep.type;
                color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_U1_,
                                      type, point);
            } else {
                VV1_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                }
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
                if (g_VFResults.vf_[g_VFResults.K_].dir_vec_field_ == 1)
                    type = change_type(de_sep.type);
                else
                    type = de_sep.type;
                color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_V1_,
                                      type, point);
            }
            break;

        case CHART_V1:
            MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
            if (!g_VFResults.plweights_)
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_V1_,
                                  de_sep->type, point);
            break;

        case CHART_U2:
            if (point[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_) {
                MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                }
                type = de_sep->type;
                color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_U2_,
                                      type, point);
            } else {
                VV2_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                }
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
                if (g_VFResults.vf_[g_VFResults.K_].dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;
                color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_V2_,
                                      type, point);
            }
            break;

        case CHART_V2:
            MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
            if (!g_VFResults.plweights_)
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(g_VFResults.vf_[g_VFResults.K_].gcf_V2_,
                                  de_sep->type, point);
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

    // normal integration cycle
    de_sep.point[0] = t;
    de_sep.point[1] = y;
    de_sep.integrating_in_local_chart = true;

    auto last_int_cycle =
        integrate_blow_up(spherewnd, pcoord2, de_sep, g_VFResults.config_step_,
                          dir, last_orbit.type, chart);
    orbit_result.insert(std::end(orbit_result), std::begin(last_int_cycle),
                        std::end(last_int_cycle));

    return std::move(orbit_result);
}

// ---------------------------------------------------------------------------
//          change_epsilon_de
// ---------------------------------------------------------------------------
void change_epsilon_de(std::shared_ptr<P4WinSphere> spherewnd, double epsilon)
{
    int deid{g_VFResults.selectedDePointIndex_};
    g_VFResults.dePoints_[deid].epsilon = epsilon;

    for (auto it = std::begin(g_VFResults.dePoints_[deid].blow_up);
         it != std::end(g_VFResults.dePoints_[deid].blow_up); ++it) {
        draw_selected_sep(spherewnd, it->sep_points, bgColours::CBACKGROUND);
        it->sep_points.clear();
    }
}

// ---------------------------------------------------------------------------
//          start_plot_de_sep
// ---------------------------------------------------------------------------
void start_plot_de_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex)
{
    int deid{g_VFResults.selectedDeSepIndex_};
    auto &de_sep{g_VFResults.deSeps_[selectedDeSepIndex_]};
    std::vector<p4orbits::orbits_points> points;
    double p[3];

    draw_sep(spherewnd, de_sep.sep_points);

    if (!de_sep.sep_points.empty()) {
        if (de_sep.integrating_in_local_chart) {
            copy_x_into_y(de_sep.sep_points.back().pcoord, p);
            // generate a vector of points by integrating the blowup
            points = integrate_blow_up(
                spherewnd, p, de_sep, g_VFResults.config_currentstep_,
                de_sep.sep_points.back().dir, de_sep.sep_points.back().type,
                g_VFResults.selected_de_point_.back().chart);
            // append this vector to the previous one in the structure
            if (!points.empty())
                de_sep.sep_points.insert(std::end(de_sep.sep_points),
                                         std::begin(points), std::end(points));
        } else {
            copy_x_into_y(de_sep.sep_points.back().pcoord, p);
            // generate a vector of points by integrating the separatrice
            points = integrate_sep(
                spherewnd, p, g_VFResults.config_currentstep_,
                de_sep.sep_points.back().dir, de_sep.sep_points.back().type,
                g_VFResults.config_intpoints_);
            // append this vector to the previous one in the structure
            if (!points.empty())
                de_sep.sep_points.insert(std::end(de_sep), std::begin(points),
                                         std::end(points));
        }
    } else {
        auto v = plot_sep_blow_up(
            spherewnd, g_VFResults.selected_de_point.back().x0,
            g_VFResults.selected_de_point.back().y0,
            g_VFResults.selected_de_point.back().chart,
            g_VFResults.selected_de_point.back().epsilon, de_sep, vfindex);
        if (v)
            de_sep.sep_points = std::move(v);
    }
}

// ---------------------------------------------------------------------------
//          cont_plot_de_sep
// ---------------------------------------------------------------------------
void cont_plot_de_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    double p[3];
    std::vector<p4orbits::orbits_points> points;
    auto &de_sep = g_VFResults.deSeps_[g_VFResults.selectedDeSepIndex_];

    copy_x_into_y(de_sep.sep_points.back().pcoord, p);
    if (de_sep.integrating_in_local_chart) {
        points = integrate_blow_up(
            spherewnd, p, de_sep, g_VFResults.config_currentstep_,
            de_sep.sep_points.back().dir, de_sep.sep_points.back().type,
            g_VFResults.dePoints_[g_VFResults.selectedDePointIndex_].chart);
    } else {
        points = integrate_sep(spherewnd, p, g_VFResults.config_currentstep_,
                               de_sep.sep_points.back().dir,
                               de_sep.sep_points.back().type,
                               g_VFResults.config_intpoints_);
    }

    de_sep.sep_points.insert(std::end(de_sep.sep_points), std::begin(points),
                             std::end(points));
}

// ---------------------------------------------------------------------------
//          plot_next_de_sep
// ---------------------------------------------------------------------------
void plot_next_de_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex)
{
    int &desepid{g_VFResults.selectedDeSepIndex_};
    int depointid{g_VFResults.selectedDePointIndex_};

    draw_sep(spherewnd, g_VFResults.deSeps_[desepid].sep_points);

    desepid++;

    if (desepid > g_VFResults.deSeps_.size()) {
        g_VFResults.deSeps_ = g_VFResults.dePoints_[depointid].blow_up;
        desepid = 0;
    }

    start_plot_de_sep(spherewnd, vfindex);
}

// ---------------------------------------------------------------------------
//          select_next_de_sep
// ---------------------------------------------------------------------------
void select_next_de_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    int &desepid{g_VFResults.selectedDeSepIndex_};
    int depointid{g_VFResults.selectedDePointIndex_};

    draw_sep(spherewnd, g_VFResults.deSeps_[desepid].sep_points);

    desepid++;

    if (desepid > g_VFResults.deSeps_.size()) {
        g_VFResults.deSeps_ = g_VFResults.dePoints_[depointid].blow_up;
        desepid = 0;
    }

    draw_selected_sep(spherewnd,
                      g_VFResults.deSeps_[desepid].sep_points.front(), CW_SEP);
}

// ---------------------------------------------------------------------------
//          plot_all_de_sep
// ---------------------------------------------------------------------------
void plot_all_de_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex,
                     const std::vector<p4singularity::degenerate> &point)
{
    std::vector<p4blowup::blow_up_points> de_sep;
    std::vector<p4orbits::orbits_points> sep;
    double p[3];

    for (auto it1 : point) {
        if (!isARealSingularity(it.x0, it.y0, it.chart, vfindex))
            continue;

        if (it.notadummy) {
            de_sep = it.blow_up;
            for (auto it2 = std::begin(de_sep); it2 != std::end(de_sep);
                 ++it2) {
                if (!it2->sep_points.empty()) {
                    sep = it2->sep_points.back();
                    copy_x_into_y(sep.pcoord, p);
                    if (it2->integrating_in_local_chart) {
                        auto nextpt =
                            integrate_blow_up(spherewnd, p, de_sep,
                                              g_VFResults.config_currentstep,
                                              sep.dir, sep.type, it1.chart);
                        it2->sep_points.insert(std::end(it2->sep_points),
                                               std::begin(nextpt),
                                               std::end(nextpt));
                    } else {
                        auto nextpt = integrate_sep(
                            spherewnd, p, g_VFResults.config_currentstep,
                            sep.dir, sep.type, g_VFResults.config_intpoints);
                        if (!nextpt.empty())
                            it2->sep_points.insert(std::end(it2->sep_points),
                                                   std::begin(nextpt),
                                                   std::end(nextpt));
                    }
                } else {
                    auto v =
                        plot_sep_blow_up(spherewnd, it1.x0, it1.y0, it1.chart,
                                         it1.epsilon, de_sep, vfindex);
                    if (v)
                        it2->sep_points = std::move(v);
                }
            }
        }
    }
}
