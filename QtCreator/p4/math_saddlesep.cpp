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

#include "math_saddlesep.hpp"

#include <QDebug>

#include "P4ParentStudy.hpp"
#include "math_charts.hpp"
#include "math_orbits.hpp"
#include "math_regions.hpp"
#include "math_separatrice.hpp"
#include "structures.hpp"

// ---------------------------------------------------------------------------
//      start_plot_saddle_sep
// ---------------------------------------------------------------------------
// Start plotting a separatrice for a saddle singular point
void start_plot_saddle_sep(P4Sphere *spherewnd, int vfindex)
{
    double p[3];

    draw_sep(spherewnd, gVFResults.selectedSep_->first_sep_point);

    auto points = gVFResults.selectedSep_->last_sep_point;

    // If there are already computed points for this separatrice, use the last
    // point's direction and type to integrate the next points.
    if (gVFResults.selectedSep_->last_sep_point != nullptr) {
        copy_x_into_y(gVFResults.selectedSep_->last_sep_point->pcoord, p);
        gVFResults.selectedSep_->last_sep_point->nextpt =
            integrate_sep(spherewnd, p, gVFResults.config_currentstep_,
                          gVFResults.selectedSep_->last_sep_point->dir,
                          gVFResults.selectedSep_->last_sep_point->type,
                          gVFResults.config_intpoints_, &points);
    } else {
        // if there are no computed points, start the integration from the
        // selected saddle singularity
        gVFResults.selectedSep_->first_sep_point = plot_separatrice(
            spherewnd, gVFResults.selectedSaddlePoint_->x0,
            gVFResults.selectedSaddlePoint_->y0,
            gVFResults.selectedSaddlePoint_->a11,
            gVFResults.selectedSaddlePoint_->a12,
            gVFResults.selectedSaddlePoint_->a21,
            gVFResults.selectedSaddlePoint_->a22,
            gVFResults.selectedSaddlePoint_->epsilon, gVFResults.selectedSep_,
            &points, gVFResults.selectedSaddlePoint_->chart, vfindex);
    }
    gVFResults.selectedSep_->last_sep_point = points;
}

// ---------------------------------------------------------------------------
//          cont_plot_saddle_sep
// ---------------------------------------------------------------------------
// Continuation of the plot is done via the standard integrate_sep method,
// and no longer depends on the type of the singularity.
void cont_plot_saddle_sep(P4Sphere *spherewnd)
{
    double p[3];

    if (gVFResults.selectedSep_->first_sep_point == nullptr)
        return;

    copy_x_into_y(gVFResults.selectedSep_->last_sep_point->pcoord, p);
    auto points = gVFResults.selectedSep_->last_sep_point;

    // compute next list of points
    gVFResults.selectedSep_->last_sep_point->nextpt =
        integrate_sep(spherewnd, p, gVFResults.config_currentstep_,
                      gVFResults.selectedSep_->last_sep_point->dir,
                      gVFResults.selectedSep_->last_sep_point->type,
                      gVFResults.config_intpoints_, &points);

    gVFResults.selectedSep_->last_sep_point = points;
}

// ---------------------------------------------------------------------------
//          plot_next_saddle_sep
// ---------------------------------------------------------------------------
void plot_next_saddle_sep(P4Sphere *spherewnd, int vfindex)
{
    draw_sep(spherewnd, gVFResults.selectedSep_->first_sep_point);

    gVFResults.selectedSep_ = gVFResults.selectedSep_->next_sep;
    if (gVFResults.selectedSep_ == nullptr)
        gVFResults.selectedSep_ = gVFResults.selectedSaddlePoint_->separatrices;

    start_plot_saddle_sep(spherewnd, vfindex);
}

// ---------------------------------------------------------------------------
//          select_next_saddle_sep
// ---------------------------------------------------------------------------
void select_next_saddle_sep(P4Sphere *spherewnd)
{
    draw_sep(spherewnd, gVFResults.selectedSep_->first_sep_point);

    gVFResults.selectedSep_ = gVFResults.selectedSep_->next_sep;
    if (gVFResults.selectedSep_ == nullptr)
        gVFResults.selectedSep_ = gVFResults.selectedSaddlePoint_->separatrices;

    draw_selected_sep(spherewnd, gVFResults.selectedSep_->first_sep_point,
                      P4ColourSettings::colour_selected_separatrice);
}

// ---------------------------------------------------------------------------
//          plot_all_saddle_sep
// ---------------------------------------------------------------------------
void plot_all_saddle_sep(P4Sphere *spherewnd, int vfindex,
                         P4Singularities::saddle *point)
{
    double p[3];

    while (point != nullptr) {
        if (!isARealSingularity(point->x0, point->y0, point->chart, vfindex)) {
            point = point->next_saddle;
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
        point = point->next_saddle;
    }
}

// ---------------------------------------------------------------------------
//          change_epsilon_saddle
// ---------------------------------------------------------------------------
void change_epsilon_saddle(P4Sphere *spherewnd, double epsilon)
{
    gVFResults.selectedSaddlePoint_->epsilon = epsilon;
    auto separatrice = gVFResults.selectedSaddlePoint_->separatrices;

    while (separatrice != nullptr) {
        draw_selected_sep(spherewnd, separatrice->first_sep_point,
                          P4ColourSettings::colour_background);
        delete separatrice->first_sep_point;
        separatrice->first_sep_point = nullptr;
        separatrice->last_sep_point = nullptr;
        separatrice = separatrice->next_sep;
    }
}
