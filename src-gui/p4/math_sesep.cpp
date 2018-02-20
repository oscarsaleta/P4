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

#include "math_sesep.h"

#include "file_tab.h"
#include "math_orbits.h"
#include "math_regions.h"
#include "math_separatrice.h"

// ---------------------------------------------------------------------------
//          start_plot_se_sep
// ---------------------------------------------------------------------------
void start_plot_se_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex)
{
    double p[3];
    int sepid{g_VFResults.selectedSepIndex_};
    int seid{g_VFResults.selectedSePointIndex_};
    p4orbits::orbits_points &points{g_VFResults.seps_[sepid].sep_points.back()};

    draw_sep(spherewnd, g_VFResults.seps_[sepid].sep_points);

    if (!points.empty()) {
        copy_x_into_y(points.pcoord, p);
        auto v = integrate_sep(spherewnd, p, g_VFResults.config_currentstep_,
                               g_VFResults.selected_sep_->last_sep_point->dir,
                               g_VFResults.selected_sep_->last_sep_point->type,
                               g_VFResults.config_intpoints_);
        if (v && !v.empty())
            g_VFResults.seps_[sepid].sep_points.insert(
                std::end(g_VFResults.seps_[sepid].sep_points), std::begin(v),
                std::end(v));
    } else {
        auto v = plot_separatrice(
            spherewnd, g_VFResults.sePoints_[seid].x0,
            g_VFResults.sePoints_[seid].y0, g_VFResults.sePoints_[seid].a11,
            g_VFResults.sePoints_[seid].a12, g_VFResults.sePoints_[seid].a21,
            g_VFResults.sePoints_[seid].a22,
            g_VFResults.sePoints_[seid].epsilon, g_VFResults.seps_[sepid],
            g_VFResults.sePoints_[seid].chart, vfindex);
        if (v && !v.empty())
            g_VFResults.seps_[sepid].sep_points = std::move(v);
    }
}

// ---------------------------------------------------------------------------
//          cont_plot_se_sep
// ---------------------------------------------------------------------------
// Continuation of the plot is done via the standard integrate_sep method,
// and no longer depends on the type of the singularity.
void cont_plot_se_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    double p[3];
    int sepid{g_VFResults.selectedSepIndex_};

    if (!g_VFResults.seps_[sepid].sep_points.empty())
        return;

    copy_x_into_y(g_VFResults.seps_[sepid].sep_points.back().pcoord, p);

    // compute next vector of points
    auto v = integrate_sep(spherewnd, p, g_VFResults.config_currentstep_,
                           g_VFResults.seps_[sepid].sep_points.back().dir,
                           g_VFResults.seps_[sepid].sep_points.back().type,
                           g_VFResults.config_intpoints_);

    // append computed points to existing vector
    if (v && !v.empty()) {
        auto &seppts = g_VFResults.seps_[sepid].sep_points;
        seppts.insert(std::end(seppts), std::begin(v), std::end(v));
    }
}

// ---------------------------------------------------------------------------
//          plot_next_se_sep
// ---------------------------------------------------------------------------
void plot_next_se_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex)
{
    int &sepid{g_VFResults.selectedSepIndex_};
    int seid{g_VFResults.selectedSePointIndex_};
    draw_sep(spherewnd, g_VFResults.seps_[sepid].sep_points);
    sepid++;
    if (sepid > g_VFResults.seps_.size()) {
        g_VFResults.seps_ = g_VFResults.sePoints_[seid].separatrices;
        sepid = 0;
    }
    start_plot_se_sep(spherewnd, vfindex);
}

// ---------------------------------------------------------------------------
//          select_next_se_sep
// ---------------------------------------------------------------------------
void select_next_se_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    int &sepid{g_VFResults.selectedSepIndex_};
    int seid{g_VFResults.selectedSePointIndex_};
    draw_sep(spherewnd, g_VFResults.seps_[sepid].sep_points);
    sepid++;
    if (sepid > g_VFResults.seps_[sepid].size()) {
        g_VFResults.seps_ = g_VFResults.sePoints_[seid].separatrices;
        sepid = 0;
    }
    draw_selected_sep(spherewnd, g_VFResults.seps_[sepid].sep_points.front(),
                      CW_SEP);
}

// ---------------------------------------------------------------------------
//          plot_all_se_sep
// ---------------------------------------------------------------------------
void plot_all_se_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex,
                     std::vector<p4sigularity::semi_elementary> &point)
{
    double p[3];

    for (auto &it1 : point) {
        if (!isARealSingularity(it1.x0, it1.y0, it1.chart, vfindex))
            continue;
        if (it1.notadummy) {
            auto &sep1 = it1.separatrices;
            for (auto &it2 : sep1)
                if (!it2.sep_points.empty()) {
                    copy_x_into_y(it2.sep_points.back().pcoord, p);
                    auto v = integrate_sep(
                        spherewnd, p, g_VFResults.config_currentstep_,
                        it2.sep_points.back().dir, it2.sep_points.back().type,
                        g_VFResults.config_intpoints_);
                    if (v && !v.empty())
                        it2.sep_points.insert(std::end(it2.sep_points),
                                              std::begin(v), std::end(v));
                } else {
                    auto v = plot_separatrice(
                        spherewnd, it1.x0, it1.y0, it1.a11, it1.a12, it1.a21,
                        it1.a22, it1.epsilon, it1, it1.chart);
                    if (v && !v.empty())
                        it2.sep_points = std::move(v);
                }
        }
    }
}
}

// ---------------------------------------------------------------------------
//          change_epsilon_se
// ---------------------------------------------------------------------------
void change_epsilon_se(std::shared_ptr<P4WinSphere> spherewnd, double epsilon)
{
    int seid{g_VFResults.selectedSePointIndex_};
    p4blowup::sep &separatrice{g_VFResults.sePoints_[sadid].separatrices};
    g_VFResults.sePoints_[seid].epsilon = epsilon;
    for (auto it : separatrice) {
        draw_selected_sep(spherewnd, it.sep_points.back(),
                          bgColours::CBACKGROUND);
        it.sep_points.clear();
    }
}
