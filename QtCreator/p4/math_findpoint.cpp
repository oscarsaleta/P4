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

static double find_distance_saddle(P4Singularities::saddle *point, double x,
                                   double y, double distance, int &type,
                                   double *refpos)
{
    double d, pcoord[3], ucoord[2];

    if (point != nullptr) {

        do {
            if (point->position == P4Singularities::position_virtual)
                continue;
            switch (point->chart) {
            case P4Charts::chart_R2:
                MATHFUNC(R2_to_sphere)(point->x0, point->y0, pcoord);
                break;
            case P4Charts::chart_U1:
                MATHFUNC(U1_to_sphere)(point->x0, point->y0, pcoord);
                break;
            case P4Charts::chart_V1:
                MATHFUNC(V1_to_sphere)(point->x0, point->y0, pcoord);
                break;
            case P4Charts::chart_U2:
                MATHFUNC(U2_to_sphere)(point->x0, point->y0, pcoord);
                break;
            case P4Charts::chart_V2:
                MATHFUNC(V2_to_sphere)(point->x0, point->y0, pcoord);
                break;
            }
            MATHFUNC(sphere_to_viewcoord)
            (pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!isInTheSameRegion(pcoord, refpos))
                continue;

            d = (x - ucoord[0]) * (x - ucoord[0]) +
                (y - ucoord[1]) * (y - ucoord[1]);
            if ((d < distance && !std::isnan(d) && std::isfinite(d)) ||
                (distance == -1)) {
                distance = d;
                gVFResults.selectedSaddlePoint_ = point;
                gVFResults.selected_ucoord_[0] = ucoord[0];
                gVFResults.selected_ucoord_[1] = ucoord[1];
                type = P4SingularityType::saddle;
            }
        } while ((point = point->next_saddle) != nullptr);
    }
    return (distance);
}

static double find_distance_se(P4Singularities::semi_elementary *point,
                               double x, double y, double distance, int &type,
                               double *refpos)
{
    double d, pcoord[3], ucoord[2];

    if (point != nullptr) {
        do {
            if (point->position == P4Singularities::position_virtual)
                continue;
            if (point->separatrices != nullptr) {
                switch (point->chart) {
                case P4Charts::chart_R2:
                    MATHFUNC(R2_to_sphere)(point->x0, point->y0, pcoord);
                    break;
                case P4Charts::chart_U1:
                    MATHFUNC(U1_to_sphere)(point->x0, point->y0, pcoord);
                    break;
                case P4Charts::chart_V1:
                    MATHFUNC(V1_to_sphere)(point->x0, point->y0, pcoord);
                    break;
                case P4Charts::chart_U2:
                    MATHFUNC(U2_to_sphere)(point->x0, point->y0, pcoord);
                    break;
                case P4Charts::chart_V2:
                    MATHFUNC(V2_to_sphere)(point->x0, point->y0, pcoord);
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
                    gVFResults.selectedSePoint_ = point;
                    gVFResults.selected_ucoord_[0] = ucoord[0];
                    gVFResults.selected_ucoord_[1] = ucoord[1];
                    type = P4SingularityType::semi_hyperbolic;
                }
            }
        } while ((point = point->next_se) != nullptr);
    }
    return (distance);
}

static double find_distance_de(P4Singularities::degenerate *point, double x,
                               double y, double distance, int &type,
                               double *refpos)
{
    double d, pcoord[3], ucoord[2];

    if (point != nullptr) {
        do {
            if (point->position == P4Singularities::position_virtual)
                continue;
            if (point->blow_up != nullptr) {
                switch (point->chart) {
                case P4Charts::chart_R2:
                    MATHFUNC(R2_to_sphere)(point->x0, point->y0, pcoord);
                    break;
                case P4Charts::chart_U1:
                    MATHFUNC(U1_to_sphere)(point->x0, point->y0, pcoord);
                    break;
                case P4Charts::chart_V1:
                    MATHFUNC(V1_to_sphere)(point->x0, point->y0, pcoord);
                    break;
                case P4Charts::chart_U2:
                    MATHFUNC(U2_to_sphere)(point->x0, point->y0, pcoord);
                    break;
                case P4Charts::chart_V2:
                    MATHFUNC(V2_to_sphere)(point->x0, point->y0, pcoord);
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
                        gVFResults.selectedDePoint_ = point;
                        gVFResults.selected_ucoord_[0] = ucoord[0];
                        gVFResults.selected_ucoord_[1] = ucoord[1];
                        type = P4SingularityType::non_elementary;
                    }
                }
            }
        } while ((point = point->next_de) != nullptr);
    }

    return (distance);
}

bool find_critical_point(P4Sphere *spherewnd, double x, double y)
{
    int type;
    double distance, epsilon, pcoord[3];
    QString s, sx, sy, sz;
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
        distance =
            find_distance_saddle(gVFResults.vf_[vfindex]->firstSaddlePoint_, x,
                                 y, distance, type, pcoord);
        distance = find_distance_se(gVFResults.vf_[vfindex]->firstSePoint_, x,
                                    y, distance, type, pcoord);
        distance = find_distance_de(gVFResults.vf_[vfindex]->firstDePoint_, x,
                                    y, distance, type, pcoord);
        if (distance != -1)
            break;
    }
    if (distance == -1 && vfindex0 != static_cast<int>(gThisVF->numVF_) - 1) {
        for (vfindex = gThisVF->numVF_ - 1; vfindex > vfindex0; vfindex--) {
            distance =
                find_distance_saddle(gVFResults.vf_[vfindex]->firstSaddlePoint_,
                                     x, y, distance, type, pcoord);
            distance = find_distance_se(gVFResults.vf_[vfindex]->firstSePoint_,
                                        x, y, distance, type, pcoord);
            distance = find_distance_de(gVFResults.vf_[vfindex]->firstDePoint_,
                                        x, y, distance, type, pcoord);
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
    case P4SingularityType::saddle: {
        gVFResults.selectedSep_ = gVFResults.selectedSaddlePoint_->separatrices;
        auto sepc = gVFResults.selectedSep_;
        draw_selected_sep(spherewnd, sepc->first_sep_point,
                          P4ColourSettings::colour_selected_separatrice);

        start_plot_sep = start_plot_saddle_sep;
        cont_plot_sep = cont_plot_saddle_sep;
        plot_next_sep = plot_next_saddle_sep;
        select_next_sep = select_next_saddle_sep;
        s = "SADDLE";
        epsilon = gVFResults.selectedSaddlePoint_->epsilon;
        change_epsilon = change_epsilon_saddle;

        if (gVFResults.selectedSaddlePoint_->chart == P4Charts::chart_R2) {
            sx.sprintf("x = %8.5g", gVFResults.selectedSaddlePoint_->x0);
            sy.sprintf("y = %8.5g", gVFResults.selectedSaddlePoint_->y0);
            sz = "";
        } else {
            switch (gVFResults.selectedSaddlePoint_->chart) {
            case P4Charts::chart_U1:
                MATHFUNC(U1_to_sphere)
                (gVFResults.selectedSaddlePoint_->x0, 0.0, pcoord);
                break;
            case P4Charts::chart_V1:
                MATHFUNC(V1_to_sphere)
                (gVFResults.selectedSaddlePoint_->x0, 0.0, pcoord);
                break;
            case P4Charts::chart_U2:
                MATHFUNC(U2_to_sphere)
                (gVFResults.selectedSaddlePoint_->x0, 0.0, pcoord);
                break;
            case P4Charts::chart_V2:
                MATHFUNC(V2_to_sphere)
                (gVFResults.selectedSaddlePoint_->x0, 0.0, pcoord);
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

    case P4SingularityType::semi_hyperbolic: {
        gVFResults.selectedSep_ = gVFResults.selectedSePoint_->separatrices;
        auto sepc = gVFResults.selectedSep_;
        draw_selected_sep(spherewnd, sepc->first_sep_point,
                          P4ColourSettings::colour_selected_separatrice);

        start_plot_sep = start_plot_se_sep;
        cont_plot_sep = cont_plot_se_sep;
        plot_next_sep = plot_next_se_sep;
        select_next_sep = select_next_se_sep;
        s = "SEMI-HYPERBOLIC";
        epsilon = gVFResults.selectedSePoint_->epsilon;
        change_epsilon = change_epsilon_se;
        if (gVFResults.selectedSePoint_->chart == P4Charts::chart_R2) {
            sx.sprintf("x=%8.5g", gVFResults.selectedSePoint_->x0);
            sy.sprintf("y=%8.5g", gVFResults.selectedSePoint_->y0);
            sz = "";
        } else {
            switch (gVFResults.selectedSePoint_->chart) {
            case P4Charts::chart_U1:
                MATHFUNC(U1_to_sphere)
                (gVFResults.selectedSePoint_->x0, 0.0, pcoord);
                break;
            case P4Charts::chart_V1:
                MATHFUNC(V1_to_sphere)
                (gVFResults.selectedSePoint_->x0, 0.0, pcoord);
                break;
            case P4Charts::chart_U2:
                MATHFUNC(U2_to_sphere)
                (gVFResults.selectedSePoint_->x0, 0.0, pcoord);
                break;
            case P4Charts::chart_V2:
                MATHFUNC(V2_to_sphere)
                (gVFResults.selectedSePoint_->x0, 0.0, pcoord);
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

    case P4SingularityType::non_elementary:
        gVFResults.selectedDeSep_ = gVFResults.selectedDePoint_->blow_up;

        draw_selected_sep(spherewnd, gVFResults.selectedDeSep_->first_sep_point,
                          P4ColourSettings::colour_selected_separatrice);

        start_plot_sep = start_plot_de_sep;
        cont_plot_sep = cont_plot_de_sep;
        plot_next_sep = plot_next_de_sep;
        select_next_sep = select_next_de_sep;
        s = "NON-ELEMENTARY";
        epsilon = gVFResults.selectedDePoint_->epsilon;
        change_epsilon = change_epsilon_de;
        if (gVFResults.selectedDePoint_->chart == P4Charts::chart_R2) {
            sx.sprintf("x=%8.5g", gVFResults.selectedDePoint_->x0);
            sy.sprintf("y=%8.5g", gVFResults.selectedDePoint_->y0);
            sz = "";
        } else {
            switch (gVFResults.selectedDePoint_->chart) {
            case P4Charts::chart_U1:
                MATHFUNC(U1_to_sphere)
                (gVFResults.selectedDePoint_->x0, 0.0, pcoord);
                break;
            case P4Charts::chart_V1:
                MATHFUNC(V1_to_sphere)
                (gVFResults.selectedDePoint_->x0, 0.0, pcoord);
                break;
            case P4Charts::chart_U2:
                MATHFUNC(U2_to_sphere)
                (gVFResults.selectedDePoint_->x0, 0.0, pcoord);
                break;
            case P4Charts::chart_V2:
                MATHFUNC(V2_to_sphere)
                (gVFResults.selectedDePoint_->x0, 0.0, pcoord);
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
