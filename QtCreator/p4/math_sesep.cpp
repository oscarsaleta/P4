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

#include "math_sesep.hpp"

#include "P4ParentStudy.hpp"
#include "math_charts.hpp"
#include "math_regions.hpp"
#include "math_separatrice.hpp"

// ---------------------------------------------------------------------------
//          start_plot_se_sep
// ---------------------------------------------------------------------------
void start_plot_se_sep(P4Sphere *spherewnd, int vfindex)
{
    double p[3];
    auto points = gVFResults.selectedSep_->last_sep_point;

    draw_sep(spherewnd, gVFResults.selectedSep_->first_sep_point);

    if (points != nullptr) {
        copy_x_into_y(points->pcoord, p);
        gVFResults.selectedSep_->last_sep_point->nextpt =
            integrate_sep(spherewnd, p, gVFResults.config_currentstep_,
                          gVFResults.selectedSep_->last_sep_point->dir,
                          gVFResults.selectedSep_->last_sep_point->type,
                          gVFResults.config_intpoints_, &points);
    } else {
        gVFResults.selectedSep_->first_sep_point = plot_separatrice(
            spherewnd, gVFResults.selectedSePoint_->x0,
            gVFResults.selectedSePoint_->y0, gVFResults.selectedSePoint_->a11,
            gVFResults.selectedSePoint_->a12, gVFResults.selectedSePoint_->a21,
            gVFResults.selectedSePoint_->a22,
            gVFResults.selectedSePoint_->epsilon, gVFResults.selectedSep_,
            &points, gVFResults.selectedSePoint_->chart, vfindex);
    }
    gVFResults.selectedSep_->last_sep_point = points;
}

// ---------------------------------------------------------------------------
//          cont_plot_se_sep
// ---------------------------------------------------------------------------
// Continuation of the plot is done via the standard integrate_sep method,
// and no longer depends on the type of the singularity.
void cont_plot_se_sep(P4Sphere *spherewnd)
{
    double p[3];
    auto points = gVFResults.selectedSep_->last_sep_point;

    copy_x_into_y(points->pcoord, p);

    // compute next list of points
    gVFResults.selectedSep_->last_sep_point->nextpt =
        integrate_sep(spherewnd, p, gVFResults.config_currentstep_,
                      gVFResults.selectedSep_->last_sep_point->dir,
                      gVFResults.selectedSep_->last_sep_point->type,
                      gVFResults.config_intpoints_, &points);

    gVFResults.selectedSep_->last_sep_point = points;
}

// ---------------------------------------------------------------------------
//          plot_next_se_sep
// ---------------------------------------------------------------------------
void plot_next_se_sep(P4Sphere *spherewnd, int vfindex)
{
    draw_sep(spherewnd, gVFResults.selectedSep_->first_sep_point);

    gVFResults.selectedSep_ = gVFResults.selectedSep_->next_sep;
    if (gVFResults.selectedSep_ == nullptr)
        gVFResults.selectedSep_ = gVFResults.selectedSePoint_->separatrices;

    start_plot_se_sep(spherewnd, vfindex);
}

// ---------------------------------------------------------------------------
//          select_next_se_sep
// ---------------------------------------------------------------------------
void select_next_se_sep(P4Sphere *spherewnd)
{
    draw_sep(spherewnd, gVFResults.selectedSep_->first_sep_point);

    gVFResults.selectedSep_ = gVFResults.selectedSep_->next_sep;
    if (gVFResults.selectedSep_ == nullptr)
        gVFResults.selectedSep_ = gVFResults.selectedSePoint_->separatrices;

    draw_selected_sep(spherewnd, gVFResults.selectedSep_->first_sep_point,
                      P4ColourSettings::colour_selected_separatrice);
}

// ---------------------------------------------------------------------------
//          plot_all_se_sep
// ---------------------------------------------------------------------------
void plot_all_se_sep(P4Sphere *spherewnd, int vfindex,
                     P4Singularities::semi_elementary *point)
{
    double p[3];

    while (point != nullptr) {
        if (!isARealSingularity(point->x0, point->y0, point->chart, vfindex)) {
            point = point->next_se;
            continue;
        }

        if (point->notadummy) {
            auto sep1 = point->separatrices;
            while (sep1 != nullptr) {
                auto points = sep1->last_sep_point;
                if (points != nullptr) {
                    copy_x_into_y(points->pcoord, p);
                    sep1->last_sep_point->nextpt = integrate_sep(
                        spherewnd, p, gVFResults.config_currentstep_,
                        sep1->last_sep_point->dir, sep1->last_sep_point->type,
                        gVFResults.config_intpoints_, &points);
                } else {
                    sep1->first_sep_point = plot_separatrice(
                        spherewnd, point->x0, point->y0, point->a11, point->a12,
                        point->a21, point->a22, point->epsilon, sep1, &points,
                        point->chart, vfindex);
                }
                sep1->last_sep_point = points;
                sep1 = sep1->next_sep;
            }
        }
        point = point->next_se;
    }
}

// ---------------------------------------------------------------------------
//          change_epsilon_se
// ---------------------------------------------------------------------------
void change_epsilon_se(P4Sphere *spherewnd, double epsilon)
{
    auto separatrice = gVFResults.selectedSePoint_->separatrices;

    gVFResults.selectedSePoint_->epsilon = epsilon;

    while (separatrice != nullptr) {
        draw_selected_sep(spherewnd, separatrice->first_sep_point,
                          P4ColourSettings::colour_background);
        delete separatrice->first_sep_point;
        separatrice->first_sep_point = nullptr;
        separatrice->last_sep_point = nullptr;
        separatrice = separatrice->next_sep;
    }
}
