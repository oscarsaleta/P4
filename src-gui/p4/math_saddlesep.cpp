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

#include "math_saddlesep.h"

#include "file_tab.h"

// ---------------------------------------------------------------------------
//      start_plot_saddle_sep
// ---------------------------------------------------------------------------
void start_plot_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex)
{
    double p[3];
    int sepid{g_VFResults.selectedSepIndex_};
    int sadid{g_VFResults.selectedSaddlePointIndex_};
    p4orbits::orbits_points &last_sep_point{
        g_VFResults.seps_[sepid].sep_points.back()};

    draw_sep(spherewnd, g_VFResults.seps_[sepid].sep_points.front());

    if (!last_sep_point.empty()) {
        copy_x_into_y(last_sep_point.pcoord, p);
        auto nextpts = integrate_sep(
            spherewnd, p, g_VFResults.config_currentstep_, last_sep_point.dir,
            last_sep_point.type, g_VFResults.config_intpoints_);

        g_VFResults.seps_[sepid].sep_points.insert(
            g_VFResults.seps_[sepid].sep_points.end(), nextpts.begin(),
            nextpts.end());
    } else {
        g_VFResults.seps_[sepid].sep_points =
            plot_separatrice(spherewnd, g_VFResults.saddlePoints_[sadid].x0,
                             g_VFResults.saddlePoints_[sadid].y0,
                             g_VFResults.saddlePoints_[sadid].a11,
                             g_VFResults.saddlePoints_[sadid].a12,
                             g_VFResults.saddlePoints_[sadid].a21,
                             g_VFResults.saddlePoints_[sadid].a22,
                             g_VFResults.saddlePoints_[sadid].epsilon,
                             g_VFResults.seps_[sepid],
                             g_VFResults.saddlePoints_[sadid].chart, vfindex);
    }
}

void cont_plot_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    double p[3];
    int sepid{g_VFResults.selectedSepIndex_};

    if (!g_VFResults.seps_[sepid].sep_points_.empty())
        return;

    copy_x_into_y(g_VFResults.seps_[sepid].sep_points.back().pcoord, p);

    // compute next vector of points
    auto nextpt =
        integrate_sep(spherewnd, p, g_VFResults.config_currentstep_,
                      g_VFResults.seps_[sepid].sep_points.back().dir,
                      g_VFResults.seps_[sepid].sep_points.back().type,
                      g_VFResults.config_intpoints_);

    // append computed points to existing vector
    auto &seppts{g_VFResults.seps_[sepid].sep_points};
    seppts.insert(seppts.end(), nextpt.begin(), nextpt.end());
}

void plot_next_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex)
{
    int &sepid{g_VFResults.selectedSepIndex_};
    int sadid{g_VFResults.selectedSaddlePointIndex_};
    draw_sep(spherewnd, g_VFResults.seps_[sepid].sep_points.front());

    sepid++;

    if (sepid > g_VFResults.seps_.size()) {
        g_VFResults.seps_ =
            g_VFResults.saddlePoints_[sadid].separatrices;
        sepid = 0;
    }

    start_plot_saddle_sep(spherewnd, vfindex);
}

void select_next_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    int &sepid{g_VFResults.selectedSepIndex_};
    int sadid{g_VFResults.selectedSaddlePointIndex_};
    draw_sep(spherewnd, g_VFResults.seps_[sepid].sep_points.front());

    sepid++;

    if (sepid > g_VFResults.seps_[sepid].size()) {
        g_VFResults.seps_ =
            g_VFResults.saddlePoints_[sadid].separatrices;
        sepid = 0;
    }

    draw_selected_sep(
        spherewnd, g_VFResults.seps_[sepid].sep_points.front(), CW_SEP);
}

void plot_all_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd,
                         std::vector<p4singularity::saddle> point)
{
    std::vector<p4orbits::sep> sep1;
    double p[3];

    for (auto it : point) {
        if (!isARealSingularity(it.x0, it.y0, it.chart, vfindex))
            continue;
        if (point->notadummy) {
            sep1 = it.separatrices;
            for (auto it2 = sep1.begin(); it2 != sep1.end(); ++it2) {
                if (!it2->sep_points.empty()) {
                    copy_x_into_y(it2->sep_points.back().pcoord, p);
                    auto newpts = integrate_sep(
                        spherewnd, p, g_VFResults.config_currentstep_,
                        it2->sep_points.back().dir, it2->sep_points.back().type,
                        g_VFResults.config_intpoints_);
                    it2->sep_points.insert(it2->sep_points.end(),
                                           newpts.begin(), newpts.end());
                } else {
                    it2->sep_points = plot_separatrice(
                        spherewnd, it.x0, it.y0, it.a11, it.a12, it.a21, it.a22,
                        it.epsilon, it, it.chart);
                }
            }
        }
    }
}