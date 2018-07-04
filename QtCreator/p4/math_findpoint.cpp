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

#include "math_findpoint.hpp"

#include <cmath>

#include <QDebug>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4SepDlg.hpp"
#include "P4Sphere.hpp"
#include "P4VFStudy.hpp"
#include "custom.hpp"
#include "math_desep.hpp"
#include "math_p4.hpp"
#include "math_regions.hpp"
#include "math_saddlesep.hpp"
#include "math_separatrice.hpp"
#include "math_sesep.hpp"
#include "structures.hpp"

// -----------------------------------------------------------------------
//
// This file implements the selection of a critical point, when the
// user clicks with the mouse inside the plot window.  The nearest singularity
// having separatrices is searched, the corresponding separatrice is selected
// and the separatrice window is set up so that it handes this separatrice.
//
// -----------------------------------------------------------------------

static double
find_distance_saddle(const std::vector<p4singularities::saddle> &points,
                     double x, double y, double distance, int &type,
                     double *refpos)
{
    double d, pcoord[3], ucoord[2];
    int i{0};

    for (auto const &it : points) {
        if (it.position == POSITION_VIRTUAL)
            continue;
        switch (it.chart) {
        case CHART_R2:
            MATHFUNC(R2_to_sphere)(it.x0, it.y0, pcoord);
            break;
        case CHART_U1:
            MATHFUNC(U1_to_sphere)(it.x0, it.y0, pcoord);
            break;
        case CHART_V1:
            MATHFUNC(V1_to_sphere)(it.x0, it.y0, pcoord);
            break;
        case CHART_U2:
            MATHFUNC(U2_to_sphere)(it.x0, it.y0, pcoord);
            break;
        case CHART_V2:
            MATHFUNC(V2_to_sphere)(it.x0, it.y0, pcoord);
            break;
        }
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], ucoord);
        if (!isInTheSameRegion(pcoord, refpos))
            continue;

        d = (x - ucoord[0]) * (x - ucoord[0]) +
            (y - ucoord[1]) * (y - ucoord[1]);
        if ((d < distance && !std::isnan(d) && std::isfinite(d)) ||
            (distance == -1)) {
            distance = d;
            gVFResults.selectedSaddlePointIndex_ = i;
            gVFResults.saddlePoints_ = points;
            gVFResults.selected_ucoord_[0] = ucoord[0];
            gVFResults.selected_ucoord_[1] = ucoord[1];
            type = SADDLE;
        }
        i++;
    }
    return (distance);
}

static double
find_distance_se(const std::vector<p4singularities::semi_elementary> &points,
                 double x, double y, double distance, int &type,
                 double refpos[])
{
    double d, pcoord[3], ucoord[2];

    int i{0};
    for (auto const &it : points) {
        if (it.position == POSITION_VIRTUAL)
            continue;
        if (!(it.separatrices).empty()) {
            switch (it.chart) {
            case CHART_R2:
                MATHFUNC(R2_to_sphere)(it.x0, it.y0, pcoord);
                break;
            case CHART_U1:
                MATHFUNC(U1_to_sphere)(it.x0, it.y0, pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)(it.x0, it.y0, pcoord);
                break;
            case CHART_U2:
                MATHFUNC(U2_to_sphere)(it.x0, it.y0, pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)(it.x0, it.y0, pcoord);
                break;
            }
            if (!isInTheSameRegion(pcoord, refpos))
                continue;

            MATHFUNC(sphere_to_viewcoord)
            (pcoord[0], pcoord[1], pcoord[2], ucoord);
            d = (x - ucoord[0]) * (x - ucoord[0]) +
                (y - ucoord[1]) * (y - ucoord[1]);
            if ((d < distance && !std::isnan(d) && std::isfinite(d)) ||
                (distance == -1)) {
                distance = d;
                gVFResults.selectedSePointIndex_ = i;
                gVFResults.sePoints_ = points;
                gVFResults.selected_ucoord_[0] = ucoord[0];
                gVFResults.selected_ucoord_[1] = ucoord[1];
                type = SEMI_HYPERBOLIC;
            }
        }
        i++;
    }
    return (distance);
}

static double
find_distance_de(const std::vector<p4singularities::degenerate> &points,
                 double x, double y, double distance, int &type,
                 double refpos[])
{
    double d, pcoord[3], ucoord[2];

    int i{0};
    for (auto const &it : points) {
        if (it.position == POSITION_VIRTUAL)
            continue;
        if (!(it.blow_up).empty()) {
            switch (it.chart) {
            case CHART_R2:
                MATHFUNC(R2_to_sphere)(it.x0, it.y0, pcoord);
                break;
            case CHART_U1:
                MATHFUNC(U1_to_sphere)(it.x0, it.y0, pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)(it.x0, it.y0, pcoord);
                break;
            case CHART_U2:
                MATHFUNC(U2_to_sphere)(it.x0, it.y0, pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)(it.x0, it.y0, pcoord);
                break;
            }
            if (!isInTheSameRegion(pcoord, refpos))
                continue;

            MATHFUNC(sphere_to_viewcoord)
            (pcoord[0], pcoord[1], pcoord[2], ucoord);
            d = (x - ucoord[0]) * (x - ucoord[0]) +
                (y - ucoord[1]) * (y - ucoord[1]);
            if ((d < distance && !std::isnan(d) && std::isfinite(d)) ||
                (distance == -1)) {
                if ((d < distance) || (distance == -1)) {
                    distance = d;
                    gVFResults.selected_ucoord_[0] = ucoord[0];
                    gVFResults.selected_ucoord_[1] = ucoord[1];
                    gVFResults.dePoints_ = points;
                    gVFResults.selectedDePointIndex_ = i;
                    type = NON_ELEMENTARY;
                }
            }
        }
        i++;
    }

    return (distance);
}

bool find_critical_point(P4Sphere *spherewnd, double x, double y)
{
    int type;
    double distance, epsilon, pcoord[3];
    QString s, sx, sy, sz;
    std::vector<p4blowup::sep> sepc;
    int vfindex, vfindex0;

    if (gVFResults.vf_.empty())
        return false;

    MATHFUNC(viewcoord_to_sphere)(x, y, pcoord);

    gCurrentSingularityInfo[0] = "";
    gCurrentSingularityInfo[1] = "";
    gCurrentSingularityInfo[2] = "";
    gCurrentSingularityInfo[3] = "";

    distance = -1;

    if ((vfindex0 = gThisVF->getVFIndex_sphere(pcoord)) == -1)
        vfindex0 = gThisVF->numVF_ - 1;

    for (vfindex = vfindex0; vfindex >= 0; vfindex--) {
        distance = find_distance_saddle(gVFResults.vf_[vfindex]->saddlePoints_,
                                        x, y, distance, type, pcoord);
        distance = find_distance_se(gVFResults.vf_[vfindex]->sePoints_, x, y,
                                    distance, type, pcoord);
        distance = find_distance_de(gVFResults.vf_[vfindex]->dePoints_, x, y,
                                    distance, type, pcoord);
        if (distance != -1)
            break;
    }
    if (distance == -1 && vfindex0 != static_cast<int>(gThisVF->numVF_) - 1) {
        for (vfindex = gThisVF->numVF_ - 1; vfindex > vfindex0; vfindex--) {
            distance =
                find_distance_saddle(gVFResults.vf_[vfindex]->saddlePoints_, x,
                                     y, distance, type, pcoord);
            distance = find_distance_se(gVFResults.vf_[vfindex]->sePoints_, x,
                                        y, distance, type, pcoord);
            distance = find_distance_de(gVFResults.vf_[vfindex]->dePoints_, x,
                                        y, distance, type, pcoord);
            if (distance != -1)
                break;
        }
    }
    if (distance == -1)
        return false;

    sx = "";
    sy = "";
    sz = "";
    epsilon = 0;

    gVFResults.selected_sep_vfindex_ = vfindex;

    switch (type) {
    case SADDLE: {
        auto &sepc =
            gVFResults.saddlePoints_[gVFResults.selectedSaddlePointIndex_]
                .separatrices;
        gVFResults.seps_ = sepc;
        gVFResults.selectedSepIndex_ = 0;
        draw_selected_sep(spherewnd, sepc.front().sep_points, CW_SEP);

        start_plot_sep = start_plot_saddle_sep;
        cont_plot_sep = cont_plot_saddle_sep;
        plot_next_sep = plot_next_saddle_sep;
        select_next_sep = select_next_saddle_sep;
        s = "SADDLE";
        epsilon = gVFResults.saddlePoints_[gVFResults.selectedSaddlePointIndex_]
                      .epsilon;
        change_epsilon = change_epsilon_saddle;

        if (gVFResults.saddlePoints_[gVFResults.selectedSaddlePointIndex_]
                .chart == CHART_R2) {
            sx.sprintf(
                "x = %8.5g",
                gVFResults.saddlePoints_[gVFResults.selectedSaddlePointIndex_]
                    .x0);
            sy.sprintf(
                "y = %8.5g",
                gVFResults.saddlePoints_[gVFResults.selectedSaddlePointIndex_]
                    .y0);
            sz = "";
        } else {
            switch (
                gVFResults.saddlePoints_[gVFResults.selectedSaddlePointIndex_]
                    .chart) {
            case CHART_U1:
                MATHFUNC(U1_to_sphere)
                (gVFResults.saddlePoints_[gVFResults.selectedSaddlePointIndex_]
                     .x0,
                 0.0, pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)
                (gVFResults.saddlePoints_[gVFResults.selectedSaddlePointIndex_]
                     .x0,
                 0.0, pcoord);
                break;
            case CHART_U2:
                MATHFUNC(U2_to_sphere)
                (gVFResults.saddlePoints_[gVFResults.selectedSaddlePointIndex_]
                     .x0,
                 0.0, pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)
                (gVFResults.saddlePoints_[gVFResults.selectedSaddlePointIndex_]
                     .x0,
                 0.0, pcoord);
                break;
            }

            if (gVFResults.plweights_ == false) {
                sx.sprintf("X= %8.5g", pcoord[0]);
                sy.sprintf("Y= %8.5g", pcoord[1]);
                sz.sprintf("Z= 0");
            } else {
                sx.sprintf("r= %8.5g", pcoord[1]);
                sy.sprintf("theta= %8.5g", pcoord[2]);
                sz = "";
            }
        }
    } break;

    case SEMI_HYPERBOLIC: {
        auto &sepc =
            gVFResults.sePoints_[gVFResults.selectedSePointIndex_].separatrices;
        gVFResults.seps_ = sepc;
        gVFResults.selectedSepIndex_ = 0;
        draw_selected_sep(spherewnd, sepc.front().sep_points, CW_SEP);

        start_plot_sep = start_plot_se_sep;
        cont_plot_sep = cont_plot_se_sep;
        plot_next_sep = plot_next_se_sep;
        select_next_sep = select_next_se_sep;
        s = "SEMI-HYPERBOLIC";
        epsilon =
            gVFResults.sePoints_[gVFResults.selectedSePointIndex_].epsilon;
        change_epsilon = change_epsilon_se;
        if (gVFResults.sePoints_[gVFResults.selectedSePointIndex_].chart ==
            CHART_R2) {
            sx.sprintf(
                "x=%8.5g",
                gVFResults.sePoints_[gVFResults.selectedSePointIndex_].x0);
            sy.sprintf(
                "y=%8.5g",
                gVFResults.sePoints_[gVFResults.selectedSePointIndex_].y0);
            sz = "";
        } else {
            switch (
                gVFResults.sePoints_[gVFResults.selectedSePointIndex_].chart) {
            case CHART_U1:
                MATHFUNC(U1_to_sphere)
                (gVFResults.sePoints_[gVFResults.selectedSePointIndex_].x0, 0.0,
                 pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)
                (gVFResults.sePoints_[gVFResults.selectedSePointIndex_].x0, 0.0,
                 pcoord);
                break;
            case CHART_U2:
                MATHFUNC(U2_to_sphere)
                (gVFResults.sePoints_[gVFResults.selectedSePointIndex_].x0, 0.0,
                 pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)
                (gVFResults.sePoints_[gVFResults.selectedSePointIndex_].x0, 0.0,
                 pcoord);
                break;
            }
            if (gVFResults.plweights_ == false) {
                sx.sprintf("X= %8.5g", pcoord[0]);
                sy.sprintf("Y= %8.5g", pcoord[1]);
                sz.sprintf("Z= 0");
            } else {
                sx.sprintf("r= %8.5g", pcoord[1]);
                sy.sprintf("theta= %8.5g", pcoord[2]);
                sz = "";
            }
        }
    } break;

    case NON_ELEMENTARY:
        gVFResults.deSeps_ =
            gVFResults.dePoints_[gVFResults.selectedDePointIndex_].blow_up;
        gVFResults.selectedDeSepIndex_ = 0;

        qDebug() << "deSeps_.size()=" << gVFResults.deSeps_.size();
        //        qDebug() <<
        //        "deSeps_[0].sep_points.size()="<<gVFResults.deSeps_[0].sep_points;

        if (!gVFResults.deSeps_.empty()) {
            draw_selected_sep(
                spherewnd,
                gVFResults.deSeps_[gVFResults.selectedDeSepIndex_].sep_points,
                CW_SEP);
        }

        start_plot_sep = start_plot_de_sep;
        cont_plot_sep = cont_plot_de_sep;
        plot_next_sep = plot_next_de_sep;
        select_next_sep = select_next_de_sep;
        s = "NON-ELEMENTARY";
        epsilon =
            gVFResults.dePoints_[gVFResults.selectedDePointIndex_].epsilon;
        change_epsilon = change_epsilon_de;
        if (gVFResults.dePoints_[gVFResults.selectedDePointIndex_].chart ==
            CHART_R2) {
            sx.sprintf(
                "x=%8.5g",
                gVFResults.dePoints_[gVFResults.selectedDePointIndex_].x0);
            sy.sprintf(
                "y=%8.5g",
                gVFResults.dePoints_[gVFResults.selectedDePointIndex_].y0);
            sz = "";
        } else {
            switch (
                gVFResults.dePoints_[gVFResults.selectedDePointIndex_].chart) {
            case CHART_U1:
                MATHFUNC(U1_to_sphere)
                (gVFResults.dePoints_[gVFResults.selectedDePointIndex_].x0, 0.0,
                 pcoord);
                break;
            case CHART_V1:
                MATHFUNC(V1_to_sphere)
                (gVFResults.dePoints_[gVFResults.selectedDePointIndex_].x0, 0.0,
                 pcoord);
                break;
            case CHART_U2:
                MATHFUNC(U2_to_sphere)
                (gVFResults.dePoints_[gVFResults.selectedDePointIndex_].x0, 0.0,
                 pcoord);
                break;
            case CHART_V2:
                MATHFUNC(V2_to_sphere)
                (gVFResults.dePoints_[gVFResults.selectedDePointIndex_].x0, 0.0,
                 pcoord);
                break;
            }
            if (gVFResults.plweights_ == false) {
                sx.sprintf("X= %8.5g", pcoord[0]);
                sy.sprintf("Y= %8.5g", pcoord[1]);
                sz.sprintf("Z= 0");
            } else {
                sx.sprintf("r= %8.5g", pcoord[1]);
                sy.sprintf("theta= %8.5g", pcoord[2]);
                sz = "";
            }
        }
        break;
    }

    if (gThisVF->numVF_ > 1) {
        QString msg;
        msg.sprintf(" (VF #%d)", vfindex + 1);
        s += msg;
    }

    gCurrentSingularityInfo[0] = s;
    gCurrentSingularityInfo[1] = sx;
    gCurrentSingularityInfo[2] = sy;
    gCurrentSingularityInfo[3] = sz;
    gCurrentSeparatriceEpsilon = epsilon;

    return true;
}
