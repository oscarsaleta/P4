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

#include "math_findpoint.h"

#include "custom.h"
#include "file_tab.h"
#include "math_p4.h"
#include "math_separatrice.h"
#include "win_separatrice.h"
#include "win_sphere.h"

#include <cmath>

// -----------------------------------------------------------------------
//
// This file implements the selection of a critical point, when the
// user clicks with the mouse inside the plot window.  The nearest singularity
// having separatrices is searched, the corresponding separatrice is selected
// and the separatrice window is set up so that it handes this separatrice.
//
// -----------------------------------------------------------------------

static double find_distance_saddle(std::vector<p4singularities::saddle> points,
                                   double x, double y, double distance,
                                   int &type, double refpos[])
{
    double d, pcoord[3], ucoord[2];

    for (auto it = points.begin(); it != points.end(); ++it) {
        if (it->position == POSITION_VIRTUAL)
            continue;
        switch (it->chart) {
        case CHART_R2:
            MATHFUNC(R2_to_sphere)(it->x0, it->y0, pcoord);
            break;
        case CHART_U1:
            MATHFUNC(U1_to_sphere)(it->x0, it->y0, pcoord);
            break;
        case CHART_V1:
            MATHFUNC(V1_to_sphere)(it->x0, it->y0, pcoord);
            break;
        case CHART_U2:
            MATHFUNC(U2_to_sphere)(it->x0, it->y0, pcoord);
            break;
        case CHART_V2:
            MATHFUNC(V2_to_sphere)(it->x0, it->y0, pcoord);
            break;
        }
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], ucoord);
        if (!isInTheSameRegion(pcoord, refpos))
            continue;

        d = (x - ucoord[0]) * (x - ucoord[0]) +
            (y - ucoord[1]) * (y - ucoord[1]);
        if ((d < distance && !std::isnan(d) && p4_finite(d)) ||
            (distance == -1)) {
            distance = d;
            g_VFResults.saddle_points_ = points;
            g_VFResults.selected_ucoord_[0] = ucoord[0];
            g_VFResults.selected_ucoord_[1] = ucoord[1];
            type = SADDLE;
        }
    }
    return (distance);
}

static double
find_distance_se(std::vector<p4singularities::semi_elementary> points, double x,
                 double y, double distance, int &type, double refpos[])
{
    double d, pcoord[3], ucoord[2];

    for (auto it = points.begin(); it != points.end(); ++it) {
        if (it->position == POSITION_VIRTUAL)
            continue;
        if (!(it->separatrices).empty()) {
            switch (it->chart) {
            case CHART_R2:
                MATHFUNC(R2_to_sphere)(it->x0, it->y0, pcoord);
                break;
            case CHART_U1:
                MATHFUNC(U1_to_sphere)(it->x0, it->y0, pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)(it->x0, it->y0, pcoord);
                break;
            case CHART_U2:
                MATHFUNC(U2_to_sphere)(it->x0, it->y0, pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)(it->x0, it->y0, pcoord);
                break;
            }
            if (!isInTheSameRegion(pcoord, refpos))
                continue;

            MATHFUNC(sphere_to_viewcoord)
            (pcoord[0], pcoord[1], pcoord[2], ucoord);
            d = (x - ucoord[0]) * (x - ucoord[0]) +
                (y - ucoord[1]) * (y - ucoord[1]);
            if ((d < distance && !std::isnan(d) && p4_finite(d)) ||
                (distance == -1)) {
                distance = d;
                g_VFResults.se_points_ = points;
                g_VFResults.selected_ucoord_[0] = ucoord[0];
                g_VFResults.selected_ucoord_[1] = ucoord[1];
                type = SEMI_HYPERBOLIC;
            }
        }
    }
    return (distance);
}

static double find_distance_de(std::vector<p4singularities::degenerate> points,
                               double x, double y, double distance, int &type,
                               double refpos[])
{
    double d, pcoord[3], ucoord[2];

    for (auto it = points.begin(); it != points.end(); ++it) {
        if (it->position == POSITION_VIRTUAL)
            continue;
        if (!(it->blow_up).empty()) {
            switch (it->chart) {
            case CHART_R2:
                MATHFUNC(R2_to_sphere)(it->x0, it->y0, pcoord);
                break;
            case CHART_U1:
                MATHFUNC(U1_to_sphere)(it->x0, it->y0, pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)(it->x0, it->y0, pcoord);
                break;
            case CHART_U2:
                MATHFUNC(U2_to_sphere)(it->x0, it->y0, pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)(it->x0, it->y0, pcoord);
                break;
            }
            if (!isInTheSameRegion(pcoord, refpos))
                continue;

            MATHFUNC(sphere_to_viewcoord)
            (pcoord[0], pcoord[1], pcoord[2], ucoord);
            d = (x - ucoord[0]) * (x - ucoord[0]) +
                (y - ucoord[1]) * (y - ucoord[1]);
            if ((d < distance && !std::isnan(d) && p4_finite(d)) ||
                (distance == -1))
                if ((d < distance) || (distance == -1)) {
                    distance = d;
                    g_VFResults.selected_ucoord_[0] = ucoord[0];
                    g_VFResults.selected_ucoord_[1] = ucoord[1];
                    g_VFResults.de_points_ = points;
                    type = NON_ELEMENTARY;
                }
        }
    }

    return (distance);
}

bool find_critical_point(std::shared_ptr<QWinSphere> spherewnd, double x,
                         double y)
{
    int type;
    double distance, epsilon, pcoord[3];
    QString s, sx, sy, sz;
    std::vector<p4blowup::sep> sepc;
    int vfindex, vfindex0;

    if (g_VFResults.vf_.empty())
        return false;
    MATHFUNC(viewcoord_to_sphere)(x, y, pcoord);

    g_CurrentSingularityInfo[0] = "";
    g_CurrentSingularityInfo[1] = "";
    g_CurrentSingularityInfo[2] = "";
    g_CurrentSingularityInfo[3] = "";

    distance = -1;

    if ((vfindex0 = g_ThisVF->getVFIndex_sphere(pcoord)) == -1)
        vfindex0 = g_ThisVF->numVF_ - 1;

    for (vfindex = vfindex0; vfindex >= 0; vfindex--) {
        distance =
            find_distance_saddle(g_VFResults.vf_[vfindex]->first_saddle_point_,
                                 x, y, distance, type, pcoord);
        distance = find_distance_se(g_VFResults.vf_[vfindex]->first_se_point_,
                                    x, y, distance, type, pcoord);
        distance = find_distance_de(g_VFResults.vf_[vfindex]->first_de_point_,
                                    x, y, distance, type, pcoord);
        if (distance == -1)
            break;
    }
    if (distance == -1 && vfindex0 != g_ThisVF->numVF_ - 1) {
        for (vfindex = g_ThisVF->numVF_ - 1; vfindex > vfindex0; vfindex--) {
            distance = find_distance_saddle(
                g_VFResults.vf_[vfindex]->first_saddle_point_, x, y, distance,
                type, pcoord);
            distance =
                find_distance_se(g_VFResults.vf_[vfindex]->first_se_point_, x,
                                 y, distance, type, pcoord);
            distance =
                find_distance_de(g_VFResults.vf_[vfindex]->first_de_point_, x,
                                 y, distance, type, pcoord);
            if (distance == -1)
                break;
        }
    }
    if (distance == -1)
        return false;

    sx = "";
    sy = "";
    sz = "";
    epsilon = 0;

    g_VFResults.selected_sep_vfindex_ = vfindex;

    switch (type) {
    case SADDLE:
        // FIXME quina es la selected saddle?
        sepc = g_VFResults.saddle_points_.back().separatrices;
        g_VFResults.selected_sep_ = &sepc;
        draw_selected_sep(spherewnd, sepc.back().first_sep_point, CW_SEP);

        start_plot_sep = start_plot_saddle_sep;
        cont_plot_sep = cont_plot_saddle_sep;
        plot_next_sep = plot_next_saddle_sep;
        select_next_sep = select_next_saddle_sep;
        s = "SADDLE";
        epsilon = g_VFResults.saddle_points_.back().epsilon;
        change_epsilon = change_epsilon_saddle;

        if (g_VFResults.saddle_points_.back().chart == CHART_R2) {
            sx.sprintf("x = %f", (float)(g_VFResults.saddle_points_.back().x0));
            sy.sprintf("y = %f", (float)(g_VFResults.saddle_points_.back().y0));
            sz = "";
        } else {
            switch (g_VFResults.saddle_points_.back().chart) {
            case CHART_U1:
                MATHFUNC(U1_to_sphere)
                (g_VFResults.saddle_points_.back().x0, 0.0, pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)
                (g_VFResults.saddle_points_.back().x0, 0.0, pcoord);
                break;
            case CHART_U2:
                MATHFUNC(U2_to_sphere)
                (g_VFResults.saddle_points_.back().x0, 0.0, pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)
                (g_VFResults.saddle_points_.back().x0, 0.0, pcoord);
                break;
            }

            if (g_VFResults.plweights_ == false) {
                sx.sprintf("X= %f", (float)(pcoord[0]));
                sy.sprintf("Y= %f", (float)(pcoord[1]));
                sz.sprintf("Z= 0");
            } else {
                sx.sprintf("r= %f", (float)(pcoord[1]));
                sy.sprintf("theta= %f", (float)(pcoord[2]));
                sz = "";
            }
        }
        break;

    case SEMI_HYPERBOLIC:
        sepc = g_VFResults.se_points_.back().separatrices.front();
        g_VFResults.selected_sep_ = &sepc;
        draw_selected_sep(spherewnd, sepc->first_sep_point, CW_SEP);

        start_plot_sep = start_plot_se_sep;
        cont_plot_sep = cont_plot_se_sep;
        plot_next_sep = plot_next_se_sep;
        select_next_sep = select_next_se_sep;
        s = "SEMI-HYPERBOLIC";
        epsilon = g_VFResults.se_points_.back().epsilon;
        change_epsilon = change_epsilon_se;
        if (g_VFResults.se_points_.back().chart == CHART_R2) {
            sx.sprintf("x=%f", (float)(g_VFResults.se_points_.back().x0));
            sy.sprintf("y=%f", (float)(g_VFResults.se_points_.back().y0));
            sz = "";
        } else {
            switch (g_VFResults.se_points_.back().chart) {
            case CHART_U1:
                MATHFUNC(U1_to_sphere)
                (g_VFResults.se_points_.back().x0, 0.0, pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)
                (g_VFResults.se_points_.back().x0, 0.0, pcoord);
                break;
            case CHART_U2:
                MATHFUNC(U2_to_sphere)
                (g_VFResults.se_points_.back().x0, 0.0, pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)
                (g_VFResults.se_points_.back().x0, 0.0, pcoord);
                break;
            }
            if (g_VFResults.plweights_ == false) {
                sx.sprintf("X= %f", (float)(pcoord[0]));
                sy.sprintf("Y= %f", (float)(pcoord[1]));
                sz.sprintf("Z= 0");
            } else {
                sx.sprintf("r= %f", (float)(pcoord[1]));
                sy.sprintf("theta= %f", (float)(pcoord[2]));
                sz = "";
            }
        }
        break;

    case NON_ELEMENTARY:
        g_VFResults.selected_de_sep_ =
            &(g_VFResults.de_points_.back().blow_up.front());
        draw_selected_sep(spherewnd,
                          g_VFResults.selected_de_sep_->first_sep_point,
                          CW_SEP);

        start_plot_sep = start_plot_de_sep;
        cont_plot_sep = cont_plot_de_sep;
        plot_next_sep = plot_next_de_sep;
        select_next_sep = select_next_de_sep;
        s = "NON-ELEMENTARY";
        epsilon = g_VFResults.de_points_.back().epsilon;
        change_epsilon = change_epsilon_de;
        if (g_VFResults.de_points_.back().chart == CHART_R2) {
            sx.sprintf("x=%f", (float)(g_VFResults.de_points_.back().x0));
            sy.sprintf("y=%f", (float)(g_VFResults.de_points_.back().y0));
            sz = "";
        } else {
            switch (g_VFResults.de_points_.back().chart) {
            case CHART_U1:
                MATHFUNC(U1_to_sphere)
                (g_VFResults.de_points_.back().x0, 0.0, pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)
                (g_VFResults.de_points_.back().x0, 0.0, pcoord);
                break;
            case CHART_U2:
                MATHFUNC(U2_to_sphere)
                (g_VFResults.de_points_.back().x0, 0.0, pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)
                (g_VFResults.de_points_.back().x0, 0.0, pcoord);
                break;
            }
            if (g_VFResults.plweights_ == false) {
                sx.sprintf("X= %f", (float)(pcoord[0]));
                sy.sprintf("Y= %f", (float)(pcoord[1]));
                sz.sprintf("Z= 0");
            } else {
                sx.sprintf("r= %f", (float)(pcoord[1]));
                sy.sprintf("theta= %f", (float)(pcoord[2]));
                sz = "";
            }
        }
        break;
    }

    if (g_ThisVF_->numVF_ > 1) {
        QString msg;
        msg.sprintf(" (VF #%d)", vfindex + 1);
        s += msg;
    }

    g_CurrentSingularityInfo[0] = s;
    g_CurrentSingularityInfo[1] = sx;
    g_CurrentSingularityInfo[2] = sy;
    g_CurrentSingularityInfo[3] = sz;
    g_CurrentSeparatriceEpsilon = epsilon;

    return true;
}
