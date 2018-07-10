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

#include "math_desep.hpp"

#include <cmath>

#include <QDebug>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4Sphere.hpp"
#include "P4VFStudy.hpp"
#include "custom.hpp"
#include "math_charts.hpp"
#include "math_numerics.hpp"
#include "math_orbits.hpp"
#include "math_p4.hpp"
#include "math_polynom.hpp"
#include "math_regions.hpp"
#include "math_separatrice.hpp"
#include "plot_tools.hpp"
#include "structures.hpp"

static P4Polynom::term2 *sVecField[2]{nullptr, nullptr};

// ---------------------------------------------------------------------------
//          eval_blow_vec_field
// ---------------------------------------------------------------------------
void eval_blow_vec_field(const double *y, double *f)
{
    f[0] = eval_term2(sVecField[0], y);
    f[1] = eval_term2(sVecField[1], y);
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
        return std::pow(a, static_cast<double>(b));
}

// ---------------------------------------------------------------------------
//          make_transformations
// ---------------------------------------------------------------------------
void make_transformations(P4Blowup::transformations *trans, double x0,
                          double y0, double *point)
{
    double x{x0}, y{y0};
    while (trans != nullptr) {
        point[0] = trans->x0 + static_cast<double>(trans->c1) *
                                   power(x, trans->d1) * power(y, trans->d2);
        point[1] = trans->y0 + static_cast<double>(trans->c2) *
                                   power(x, trans->d3) * power(y, trans->d4);
        x = point[0];
        y = point[1];
        trans = trans->next_trans;
    }
}

// ---------------------------------------------------------------------------
//          integrate_blow_up
// ---------------------------------------------------------------------------
P4Orbits::orbits_points *integrate_blow_up(P4Sphere *spherewnd, double *pcoord2,
                                           P4Blowup::blow_up_points *de_sep,
                                           double step, int dir, int type,
                                           P4Orbits::orbits_points **orbit,
                                           int chart)
{
    int i;
    double hhi, hhi0;
    double y0[2], y[2];
    double h_min{gVFResults.config_hmi_}, h_max{gVFResults.config_hma_};
    double point[2], pcoord[3];
    int color;
    bool dashes, ok{true};
    P4Orbits::orbits_points *first_orbit{nullptr};
    P4Orbits::orbits_points *last_orbit{nullptr};

    auto &vfResultsK = gVFResults.vf_[gVFResults.K_];

    sVecField[0] = de_sep->vector_field[0];
    sVecField[1] = de_sep->vector_field[1];

    y[0] = de_sep->point[0];
    y[1] = de_sep->point[1];

    make_transformations(
        de_sep->trans, de_sep->x0 + de_sep->a11 * y[0] + de_sep->a12 * y[1],
        de_sep->y0 + de_sep->a21 * y[0] + de_sep->a22 * y[1], point);

    switch (chart) {
    case P4Charts::chart_R2:
        MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord);
        break;
    case P4Charts::chart_U1:
        MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
        break;
    case P4Charts::chart_V1:
        MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
        break;
    case P4Charts::chart_U2:
        MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
        break;
    case P4Charts::chart_V2:
        MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
        break;
    }

    if (!prepareVfForIntegration(pcoord))
        return nullptr;

    if (gVFResults.plweights_ == false &&
        (chart == P4Charts::chart_V1 || chart == P4Charts::chart_V2))
        dir *= vfResultsK->dir_vec_field_;

    hhi = static_cast<double>(dir) * step;
    y[0] = de_sep->point[0];
    y[1] = de_sep->point[1];

    for (i = 1; i <= gVFResults.config_intpoints_; ++i) {
        hhi0 = hhi;
        y0[0] = y[0];
        y0[1] = y[1];
        while (1) {
            y[0] = y0[0];
            y[1] = y0[1];
            rk78(eval_blow_vec_field, y, &hhi0, gVFResults.config_hmi_,
                 gVFResults.config_hma_, gVFResults.config_tolerance_);
            make_transformations(
                de_sep->trans,
                de_sep->x0 + de_sep->a11 * y[0] + de_sep->a12 * y[1],
                de_sep->y0 + de_sep->a21 * y[0] + de_sep->a22 * y[1], point);
            switch (chart) {
            case P4Charts::chart_R2:
                MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord);
                break;
            case P4Charts::chart_U1:
                if (point[1] >= 0 || !vfResultsK->singinf_)
                    MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                else
                    VV1_to_psphere(point[0], point[1], pcoord);
                break;
            case P4Charts::chart_V1:
                MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
                break;
            case P4Charts::chart_U2:
                if (point[1] >= 0 || !vfResultsK->singinf_)
                    MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
                else
                    VV2_to_psphere(point[0], point[1], pcoord);
                break;
            case P4Charts::chart_V2:
                MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
                break;
            }

            if (gThisVF->getVFIndex_sphere(pcoord) == gVFResults.K_)
                break;
            h_min = gVFResults.config_branchhmi_;
            h_max /= 2;
            hhi0 = std::abs(hhi0 * hhi) / 2 / hhi;
            if (std::abs(hhi0) < h_min ||
                h_max < gVFResults.config_branchhmi_) {
                hhi0 = h_min;
                if (hhi < 0)
                    hhi0 = -hhi0;
                de_sep->integrating_in_local_chart = false;
                break;
            }
        }
        hhi = hhi0;

        dashes = true;
        switch (chart) {
        case P4Charts::chart_R2:
            MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord);
            color = findSepColor2(vfResultsK->gcf_, type, point);
            break;

        case P4Charts::chart_U1:
            if (point[1] >= 0 || !vfResultsK->singinf_) {
                MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (vfResultsK->dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = de_sep->type;
                color = findSepColor2(vfResultsK->gcf_U1_, type, point);
            } else {
                VV1_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (vfResultsK->dir_vec_field_ == 1)
                        dir *= -1;
                }
                if (vfResultsK->dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
                color = findSepColor2(vfResultsK->gcf_V1_, type, point);
            }
            break;

        case P4Charts::chart_V1:
            MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
            if (!gVFResults.plweights_)
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(vfResultsK->gcf_V1_, type, point);
            break;

        case P4Charts::chart_U2:
            if (point[1] >= 0 || !vfResultsK->singinf_) {
                MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (vfResultsK->dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = de_sep->type;
                color = findSepColor2(vfResultsK->gcf_U2_, type, point);
            } else {
                VV2_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (vfResultsK->dir_vec_field_ == 1)
                        dir *= -1;
                }
                if (vfResultsK->dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
                color = findSepColor2(vfResultsK->gcf_V2_, type, point);
            }
            break;

        case P4Charts::chart_V2:
            MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
            if (!gVFResults.plweights_)
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(vfResultsK->gcf_V2_, type, point);
            break;

        default:
            color = 0;
            break;
        }

        int newdir =
            ((gVFResults.plweights_ == false) &&
             (chart == P4Charts::chart_V1 || chart == P4Charts::chart_V2))
                ? vfResultsK->dir_vec_field_ * dir
                : dir;

        // create or append the new orbit to the linked list
        if (last_orbit == nullptr) {
            first_orbit = new P4Orbits::orbits_points{
                pcoord, color, dashes * gVFResults.config_dashes_, newdir,
                type};
            last_orbit = first_orbit;
        } else {
            last_orbit->nextpt = new P4Orbits::orbits_points{
                pcoord, color, dashes * gVFResults.config_dashes_, newdir,
                type};
            last_orbit = last_orbit->nextpt;
        }

        if (last_orbit->dashes)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);

        if (y[0] * y[0] + y[1] * y[1] >= 1.0)
            de_sep->integrating_in_local_chart = false;
        if (!de_sep->integrating_in_local_chart) {
            // either outside ball, or a different vf region
            break;
        }

        copy_x_into_y(pcoord, pcoord2);
    }
    de_sep->point[0] = y[0];
    de_sep->point[1] = y[1];

    set_current_step(std::abs(hhi));

    // set pointer to last orbit of the linked list
    if (first_orbit != nullptr)
        *orbit = last_orbit;
    return first_orbit;
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
static P4Orbits::orbits_points *
plot_sep_blow_up(P4Sphere *spherewnd, double x0, double y0, int chart,
                 double epsilon, P4Blowup::blow_up_points *de_sep,
                 P4Orbits::orbits_points **orbit, int vfindex)
{
    double h, t{0}, y, pcoord[3], pcoord2[3], point[2];
    int i, color, dir, dashes, type, ok{true};
    P4Orbits::orbits_points *first_orbit{nullptr}, *last_orbit{nullptr};

    auto &vfResultsK = gVFResults.vf_[gVFResults.K_];

    switch (chart) {
    case P4Charts::chart_R2:
        MATHFUNC(R2_to_sphere)(x0, y0, pcoord);
        break;
    case P4Charts::chart_U1:
        MATHFUNC(U1_to_sphere)(x0, y0, pcoord);
        break;
    case P4Charts::chart_V1:
        MATHFUNC(V1_to_sphere)(x0, y0, pcoord);
        break;
    case P4Charts::chart_V2:
        MATHFUNC(V2_to_sphere)(x0, y0, pcoord);
        break;
    }
    if (!prepareVfForIntegration(pcoord))
        return nullptr;

    /* if we have a line of singularities at infinity then we have to
     * change the chart if the chart is V1 or V2 */
    if (vfResultsK->singinf_) {
        if (chart == P4Charts::chart_V1)
            chart = P4Charts::chart_U1;
        else if (chart == P4Charts::chart_V2)
            chart = P4Charts::chart_U2;
    }

    h = epsilon / 100.;

    type = de_sep->type;
    switch (type) {
    case P4OrbitType::stable:
        dir = P4OrbitType::stable;
        break;
    case P4OrbitType::unstable:
        dir = P4OrbitType::unstable;
        break;
    case P4OrbitType::cent_stable:
        dir = P4OrbitType::cent_stable;
        break;
    case P4OrbitType::cent_unstable:
        dir = P4OrbitType::cent_unstable;
        break;
    default:
        dir = 0;
        break;
    }

    point[0] = x0;
    point[1] = y0;

    first_orbit = new P4Orbits::orbits_points;
    last_orbit = first_orbit;

    switch (chart) {
    case P4Charts::chart_R2:
        MATHFUNC(R2_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(vfResultsK->gcf_, de_sep->type, point);
        break;
    case P4Charts::chart_U1:
        MATHFUNC(U1_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(vfResultsK->gcf_U1_, de_sep->type, point);
        break;
    case P4Charts::chart_V1:
        MATHFUNC(V1_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(vfResultsK->gcf_V1_, de_sep->type, point);
        break;
    case P4Charts::chart_U2:
        MATHFUNC(U2_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(vfResultsK->gcf_U2_, de_sep->type, point);
        break;
    case P4Charts::chart_V2:
        MATHFUNC(V2_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(vfResultsK->gcf_V2_, de_sep->type, point);
        break;
    default:
        color = P4ColourSettings::colour_background;
    }

    // P5 addition: check if one can start with this separatrice
    y = eval_term1(de_sep->sep, h * 100);
    make_transformations(
        de_sep->trans, de_sep->x0 + de_sep->a11 * h * 100 + de_sep->a12 * y,
        de_sep->y0 + de_sep->a21 * h * 100 + de_sep->a22 * y, point);

    switch (chart) {
    case P4Charts::chart_R2:
        MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord2);
        break;
    case P4Charts::chart_U1:
        if (point[1] >= 0 || !vfResultsK->singinf_)
            MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord2);
        else
            VV1_to_psphere(point[0], point[1], pcoord2);
        break;
    case P4Charts::chart_V1:
        MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord2);
        break;
    case P4Charts::chart_U2:
        if (point[1] >= 0 || !vfResultsK->singinf_)
            MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord2);
        else
            VV2_to_psphere(point[0], point[1], pcoord2);
        break;
    case P4Charts::chart_V2:
        MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord2);
        break;
    }

    if (gThisVF->getVFIndex_sphere(pcoord2) != vfindex) {
        delete first_orbit;
        return nullptr;
    }

    // end of P5 addition

    copy_x_into_y(pcoord, last_orbit->pcoord);
    last_orbit->color = color;
    last_orbit->dashes = 0;
    last_orbit->dir = dir; // NOTE: aixo no hi era
    copy_x_into_y(pcoord, pcoord2);

    for (i = 0; i <= 99; i++) {
        last_orbit->nextpt = new P4Orbits::orbits_points;
        last_orbit = last_orbit->nextpt;

        dashes = true;
        y = eval_term1(de_sep->sep, t);
        make_transformations(
            de_sep->trans, de_sep->x0 + de_sep->a11 * t + de_sep->a12 * y,
            de_sep->y0 + de_sep->a21 * t + de_sep->a22 * y, point);
        switch (chart) {
        case P4Charts::chart_R2:
            MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord);
            color = findSepColor2(vfResultsK->gcf_, de_sep->type, point);
            break;

        case P4Charts::chart_U1:
            if (point[1] >= 0 || !vfResultsK->singinf_) {
                MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                }
                type = de_sep->type;
                color = findSepColor2(vfResultsK->gcf_U1_, type, point);
            } else {
                VV1_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                }
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
                if (vfResultsK->dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;
                color = findSepColor2(vfResultsK->gcf_V1_, type, point);
            }
            break;

        case P4Charts::chart_V1:
            MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
            if (!gVFResults.plweights_)
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(vfResultsK->gcf_V1_, de_sep->type, point);
            break;

        case P4Charts::chart_U2:
            if (point[1] >= 0 || !vfResultsK->singinf_) {
                MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                }
                type = de_sep->type;
                color = findSepColor2(vfResultsK->gcf_U2_, type, point);
            } else {
                VV2_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                }
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
                if (vfResultsK->dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;
                color = findSepColor2(vfResultsK->gcf_V2_, type, point);
            }
            break;

        case P4Charts::chart_V2:
            MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
            if (!gVFResults.plweights_)
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(vfResultsK->gcf_V2_, de_sep->type, point);
            break;
        }

        copy_x_into_y(pcoord, last_orbit->pcoord);
        last_orbit->color = color;
        last_orbit->dashes = dashes * gVFResults.config_dashes_;
        last_orbit->dir = dir;
        last_orbit->type = type;
        if (last_orbit->dashes)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);
    }

    // normal integration cycle
    de_sep->point[0] = t;
    de_sep->point[1] = y;
    de_sep->integrating_in_local_chart = true;

    auto sep = last_orbit;
    last_orbit->nextpt =
        integrate_blow_up(spherewnd, pcoord2, de_sep, gVFResults.config_step_,
                          dir, last_orbit->type, &sep, chart);

    if (first_orbit != nullptr)
        *orbit = sep;
    return first_orbit;
}

// ---------------------------------------------------------------------------
//          start_plot_de_sep
// ---------------------------------------------------------------------------
void start_plot_de_sep(P4Sphere *spherewnd, int vfindex)
{
    P4Orbits::orbits_points *points;

    auto &de_sep = gVFResults.selectedDeSep_;
    auto &de_poi = gVFResults.selectedDePoint_;
    double p[3];

    draw_sep(spherewnd, de_sep->first_sep_point);

    if (de_sep->last_sep_point != nullptr) {
        if (de_sep->integrating_in_local_chart) {
            copy_x_into_y(de_sep->last_sep_point->pcoord, p);
            points = de_sep->last_sep_point;
            de_sep->last_sep_point->nextpt = integrate_blow_up(
                spherewnd, p, de_sep, gVFResults.config_currentstep_,
                de_sep->last_sep_point->dir, de_sep->last_sep_point->type,
                &points, de_poi->chart);
            de_sep->last_sep_point = points;
        } else {
            copy_x_into_y(de_sep->last_sep_point->pcoord, p);
            points = de_sep->last_sep_point;
            de_sep->last_sep_point->nextpt = integrate_sep(
                spherewnd, p, gVFResults.config_currentstep_,
                de_sep->last_sep_point->dir, de_sep->last_sep_point->type,
                gVFResults.config_intpoints_, &points);
            de_sep->last_sep_point = points;
        }
    } else {
        de_sep->first_sep_point =
            plot_sep_blow_up(spherewnd, de_poi->x0, de_poi->y0, de_poi->chart,
                             de_poi->epsilon, de_sep, &points, vfindex);
    }
}

// ---------------------------------------------------------------------------
//          cont_plot_de_sep
// ---------------------------------------------------------------------------
void cont_plot_de_sep(P4Sphere *spherewnd)
{
    double p[3];
    auto &de_sep = gVFResults.selectedDeSep_;
    auto points = de_sep->last_sep_point;

    copy_x_into_y(points->pcoord, p);
    if (de_sep->integrating_in_local_chart) {
        de_sep->last_sep_point->nextpt = integrate_blow_up(
            spherewnd, p, de_sep, gVFResults.config_currentstep_,
            de_sep->last_sep_point->dir, de_sep->last_sep_point->type, &points,
            gVFResults.selectedDePoint_->chart);
    } else {
        de_sep->last_sep_point->nextpt = integrate_sep(
            spherewnd, p, gVFResults.config_currentstep_,
            de_sep->last_sep_point->dir, de_sep->last_sep_point->type,
            gVFResults.config_intpoints_, &points);
    }
}

// ---------------------------------------------------------------------------
//          plot_next_de_sep
// ---------------------------------------------------------------------------
void plot_next_de_sep(P4Sphere *spherewnd, int vfindex)
{
    draw_sep(spherewnd, gVFResults.selectedDeSep_->first_sep_point);

    gVFResults.selectedDeSep_ = gVFResults.selectedDeSep_->next_blow_up_point;
    if (gVFResults.selectedDeSep_ == nullptr)
        gVFResults.selectedDeSep_ = gVFResults.selectedDePoint_->blow_up;

    start_plot_de_sep(spherewnd, vfindex);
}

// ---------------------------------------------------------------------------
//          select_next_de_sep
// ---------------------------------------------------------------------------
void select_next_de_sep(P4Sphere *spherewnd)
{
    draw_sep(spherewnd, gVFResults.selectedDeSep_->first_sep_point);

    gVFResults.selectedDeSep_ = gVFResults.selectedDeSep_->next_blow_up_point;
    if (gVFResults.selectedDeSep_ == nullptr)
        gVFResults.selectedDeSep_ = gVFResults.selectedDePoint_->blow_up;

    draw_selected_sep(spherewnd, gVFResults.selectedDeSep_->first_sep_point,
                      P4ColourSettings::colour_selected_separatrice);
}

// ---------------------------------------------------------------------------
//          plot_all_de_sep
// ---------------------------------------------------------------------------
void plot_all_de_sep(P4Sphere *spherewnd, int vfindex,
                     P4Singularities::degenerate *point)
{
    double p[3];

    while (point != nullptr) {
        if (!isARealSingularity(point->x0, point->y0, point->chart, vfindex) ||
            !point->notadummy) {
            point = point->next_de;
            continue;
        }
        auto de_sep = point->blow_up;
        while (de_sep != nullptr) {
            auto sep = de_sep->last_sep_point;
            if (de_sep->last_sep_point != nullptr) {
                copy_x_into_y(de_sep->last_sep_point->pcoord, p);
                if (de_sep->integrating_in_local_chart)
                    de_sep->last_sep_point->nextpt = integrate_blow_up(
                        spherewnd, p, de_sep, gVFResults.config_currentstep_,
                        de_sep->last_sep_point->dir,
                        de_sep->last_sep_point->type, &sep, point->chart);
                else
                    de_sep->last_sep_point->nextpt = integrate_sep(
                        spherewnd, p, gVFResults.config_currentstep_,
                        de_sep->last_sep_point->dir,
                        de_sep->last_sep_point->type,
                        gVFResults.config_intpoints_, &sep);
                de_sep->last_sep_point = sep;
            } else {
                de_sep->first_sep_point = plot_sep_blow_up(
                    spherewnd, point->x0, point->y0, point->chart,
                    point->epsilon, de_sep, &sep, vfindex);
                de_sep->last_sep_point = sep;
            }
            de_sep = de_sep->next_blow_up_point;
        }
        point = point->next_de;
    }
}

// ---------------------------------------------------------------------------
//          change_epsilon_de
// ---------------------------------------------------------------------------
void change_epsilon_de(P4Sphere *spherewnd, double epsilon)
{
    gVFResults.selectedDePoint_->epsilon = epsilon;
    auto separatrice = gVFResults.selectedDePoint_->blow_up;

    while (separatrice != nullptr) {
        draw_selected_sep(spherewnd, separatrice->first_sep_point,
                          P4ColourSettings::colour_background);
        delete separatrice->first_sep_point;
        separatrice->first_sep_point = nullptr;
        separatrice->last_sep_point = nullptr;
        separatrice = separatrice->next_blow_up_point;
    }
}
