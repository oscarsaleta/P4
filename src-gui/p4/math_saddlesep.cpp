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

#include "P4ParentStudy.hpp"
#include "math_charts.hpp"
#include "math_orbits.hpp"
#include "math_regions.hpp"
#include "math_separatrice.hpp"
#include "tables.hpp"

// ---------------------------------------------------------------------------
//      start_plot_saddle_sep
// ---------------------------------------------------------------------------
void start_plot_saddle_sep(P4WinSphere *spherewnd, int vfindex)
{
    double p[3];
    const int &sepid{gVFResults.selectedSepIndex_};
    const int &sadid{gVFResults.selectedSaddlePointIndex_};

    draw_sep(spherewnd, gVFResults.seps_[sepid].sep_points);

    if (!gVFResults.seps_[sepid].sep_points.empty()) {
        auto &last_sep_point = gVFResults.seps_[sepid].sep_points.back();

        copy_x_into_y(last_sep_point.pcoord, p);
        auto v = integrate_sep(spherewnd, p, gVFResults.config_currentstep_,
                               last_sep_point.dir, last_sep_point.type,
                               gVFResults.config_intpoints_);
        if (!v.empty())
            gVFResults.seps_[sepid].sep_points.insert(
                std::end(gVFResults.seps_[sepid].sep_points), std::begin(v),
                std::end(v));
    } else {
        gVFResults.seps_[sepid].sep_points = plot_separatrice(
            spherewnd, gVFResults.saddlePoints_[sadid].x0,
            gVFResults.saddlePoints_[sadid].y0,
            gVFResults.saddlePoints_[sadid].a11,
            gVFResults.saddlePoints_[sadid].a12,
            gVFResults.saddlePoints_[sadid].a21,
            gVFResults.saddlePoints_[sadid].a22,
            gVFResults.saddlePoints_[sadid].epsilon, gVFResults.seps_[sepid],
            gVFResults.saddlePoints_[sadid].chart, vfindex);
    }
}

// ---------------------------------------------------------------------------
//          cont_plot_saddle_sep
// ---------------------------------------------------------------------------
// Continuation of the plot is done via the standard integrate_sep method,
// and no longer depends on the type of the singularity.
void cont_plot_saddle_sep(P4WinSphere *spherewnd)
{
    double p[3];
    int sepid{gVFResults.selectedSepIndex_};

    if (!gVFResults.seps_[sepid].sep_points.empty())
        return;

    copy_x_into_y(gVFResults.seps_[sepid].sep_points.back().pcoord, p);

    // compute next vector of points
    auto v = integrate_sep(spherewnd, p, gVFResults.config_currentstep_,
                           gVFResults.seps_[sepid].sep_points.back().dir,
                           gVFResults.seps_[sepid].sep_points.back().type,
                           gVFResults.config_intpoints_);

    // append computed points to existing vector
    if (!v.empty()) {
        auto &seppts = gVFResults.seps_[sepid].sep_points;
        seppts.insert(std::end(seppts), std::begin(v), std::end(v));
    }
}

// ---------------------------------------------------------------------------
//          plot_next_saddle_sep
// ---------------------------------------------------------------------------
void plot_next_saddle_sep(P4WinSphere *spherewnd, int vfindex)
{
    int &sepid{gVFResults.selectedSepIndex_};
    int sadid{gVFResults.selectedSaddlePointIndex_};
    draw_sep(spherewnd, gVFResults.seps_[sepid].sep_points);

    sepid++;

    if (static_cast<std::string::size_type>(sepid) > gVFResults.seps_.size()) {
        gVFResults.seps_ = gVFResults.saddlePoints_[sadid].separatrices;
        sepid = 0;
    }

    start_plot_saddle_sep(spherewnd, vfindex);
}

// ---------------------------------------------------------------------------
//          select_next_saddle_sep
// ---------------------------------------------------------------------------
void select_next_saddle_sep(P4WinSphere *spherewnd)
{
    int &sepid{gVFResults.selectedSepIndex_};
    int sadid{gVFResults.selectedSaddlePointIndex_};
    draw_sep(spherewnd, gVFResults.seps_[sepid].sep_points);

    sepid++;

    if (static_cast<std::string::size_type>(sepid) > gVFResults.seps_.size()) {
        gVFResults.seps_ = gVFResults.saddlePoints_[sadid].separatrices;
        sepid = 0;
    }

    draw_selected_sep(spherewnd, gVFResults.seps_[sepid].sep_points, CW_SEP);
}

// ---------------------------------------------------------------------------
//          plot_all_saddle_sep
// ---------------------------------------------------------------------------
void plot_all_saddle_sep(P4WinSphere *spherewnd, int vfindex,
                         std::vector<p4singularities::saddle> &point)
{
    double p[3];

    for (auto &it1 : point) {
        if (!isARealSingularity(it1.x0, it1.y0, it1.chart, vfindex))
            continue;
        if (it1.notadummy) {
            auto &sep1 = it1.separatrices;
            for (auto &it2 : sep1) {
                if (!it2.sep_points.empty()) {
                    copy_x_into_y(it2.sep_points.back().pcoord, p);
                    auto newpts = integrate_sep(
                        spherewnd, p, gVFResults.config_currentstep_,
                        it2.sep_points.back().dir, it2.sep_points.back().type,
                        gVFResults.config_intpoints_);
                    if (!newpts.empty())
                        it2.sep_points.insert(std::end(it2.sep_points),
                                              std::begin(newpts),
                                              std::end(newpts));
                } else {
                    it2.sep_points = plot_separatrice(
                        spherewnd, it1.x0, it1.y0, it1.a11, it1.a12, it1.a21,
                        it1.a22, it1.epsilon, it2, it1.chart, vfindex);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
//          change_epsilon_saddle
// ---------------------------------------------------------------------------
void change_epsilon_saddle(P4WinSphere *spherewnd, double epsilon)
{
    int sadid{gVFResults.selectedSaddlePointIndex_};
    gVFResults.saddlePoints_[sadid].epsilon = epsilon;

    for (auto &it : gVFResults.saddlePoints_[sadid].separatrices) {
        draw_selected_sep(spherewnd, it.sep_points, bgColours::CBACKGROUND);
        it.sep_points.clear();
    }
}
