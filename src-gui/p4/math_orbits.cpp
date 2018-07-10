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

#include "math_orbits.hpp"

#include <QDebug>

#include <cmath>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4Sphere.hpp"
#include "P4VFStudy.hpp"
#include "custom.hpp"
#include "math_charts.hpp"
#include "math_numerics.hpp"
#include "math_p4.hpp"
#include "math_polynom.hpp"
#include "plot_tools.hpp"
#include "structures.hpp"

// -----------------------------------------------------------------------
//          prepareVfForIntegration
// -----------------------------------------------------------------------
bool prepareVfForIntegration(double *pcoord)
{
    int K{gThisVF->getVFIndex_sphere(pcoord)};
    if (K >= 0) {
        gVFResults.K_ = K;
        return true;
    } else {
        return false;
    }
}

// -----------------------------------------------------------------------
//          integrateOrbit
// -----------------------------------------------------------------------
// dir = -1: backwards, dir=0: continue, dir=+1: forwards
// Continues orbit integration
void integrateOrbit(P4Sphere *sphere, int dir)
{
    P4Orbits::orbits_points *sep;
    double pcoord[3], ucoord[2];

    if (dir == 0) {
        // continue orbit button has been pressed
        dir = gVFResults.currentOrbit_->currentpt->dir;

        copy_x_into_y(gVFResults.currentOrbit_->currentpt->pcoord, pcoord);
        if (!prepareVfForIntegration(pcoord))
            return;

        sep = gVFResults.currentOrbit_->currentpt;
        gVFResults.currentOrbit_->currentpt->nextpt = integrate_orbit(
            sphere, pcoord, gVFResults.config_currentstep_, dir,
            P4ColourSettings::colour_orbit, gVFResults.config_intpoints_, &sep);
        gVFResults.currentOrbit_->currentpt = sep;

        return;
    }

    copy_x_into_y(gVFResults.currentOrbit_->pcoord, pcoord);
    MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);

    if (!prepareVfForIntegration(pcoord))
        return;

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL)
        if (eval_term2(gVFResults.vf_[gVFResults.K_]->gcf_, ucoord) < 0)
            dir = -dir;

    if (gVFResults.currentOrbit_->firstpt == nullptr) {
        sep = nullptr;
        gVFResults.currentOrbit_->firstpt = integrate_orbit(
            sphere, pcoord, gVFResults.config_step_, dir,
            P4ColourSettings::colour_orbit, gVFResults.config_intpoints_, &sep);
    } else {
        gVFResults.currentOrbit_->currentpt->nextpt =
            new P4Orbits::orbits_points{pcoord, P4ColourSettings::colour_orbit,
                                        0, dir};
        gVFResults.currentOrbit_->currentpt =
            gVFResults.currentOrbit_->currentpt->nextpt;
        sep = gVFResults.currentOrbit_->currentpt;
        gVFResults.currentOrbit_->currentpt->nextpt = integrate_orbit(
            sphere, pcoord, gVFResults.config_step_, dir,
            P4ColourSettings::colour_orbit, gVFResults.config_intpoints_, &sep);
    }
    gVFResults.currentOrbit_->currentpt = sep;
}

// -----------------------------------------------------------------------
//          startOrbit
// -----------------------------------------------------------------------
/* R=0 then point selected in the drawing canvas else in the orbit window */
bool startOrbit(P4Sphere *sphere, double x, double y, bool R)
{
    double pcoord[3], ucoord[2];

    if (R)
        MATHFUNC(R2_to_sphere)(x, y, pcoord);
    else if ((MATHFUNC(is_valid_viewcoord)(x, y, pcoord)) == false)
        return false;

    if (!prepareVfForIntegration(pcoord))
        return false;

    if (gVFResults.firstOrbit_ == nullptr) {
        gVFResults.firstOrbit_ =
            new P4Orbits::orbits{pcoord, P4ColourSettings::colour_orbit};
        gVFResults.currentOrbit_ = gVFResults.firstOrbit_;
    } else {
        gVFResults.currentOrbit_->next =
            new P4Orbits::orbits{pcoord, P4ColourSettings::colour_orbit};
        gVFResults.currentOrbit_ = gVFResults.currentOrbit_->next;
    }

    MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], ucoord);
    sphere->drawPoint(ucoord[0], ucoord[1], P4ColourSettings::colour_orbit);

    return true;
}

// -----------------------------------------------------------------------
//          drawOrbit
// -----------------------------------------------------------------------
void drawOrbit(P4Sphere *spherewnd, const double *pcoord,
               P4Orbits::orbits_points *points, int color)
{
    double pcoord1[3];

    copy_x_into_y(pcoord, pcoord1);
    (*plot_p)(spherewnd, pcoord, color);

    while (points != nullptr) {
        if (points->dashes) {
            (*plot_l)(spherewnd, pcoord1, points->pcoord, color);
        } else {
            (*plot_p)(spherewnd, points->pcoord, color);
        }
        copy_x_into_y(points->pcoord, pcoord1);
        points = points->nextpt;
    }
}

// -----------------------------------------------------------------------
//                      DRAWORBITS
// -----------------------------------------------------------------------
void drawOrbits(P4Sphere *spherewnd)
{
    for (auto orbit = gVFResults.firstOrbit_; orbit != nullptr;
         orbit = orbit->next)
        drawOrbit(spherewnd, orbit->pcoord, orbit->firstpt, orbit->color);
}

// -----------------------------------------------------------------------
//          deleteLastOrbit
// -----------------------------------------------------------------------
void deleteLastOrbit(P4Sphere *spherewnd)
{
    if (gVFResults.currentOrbit_ == nullptr)
        return;

    auto orbit1 = gVFResults.currentOrbit_;
    drawOrbit(spherewnd, orbit1->pcoord, orbit1->firstpt,
              spherewnd->spherebgcolor_);

    if (gVFResults.firstOrbit_ == gVFResults.currentOrbit_) {
        gVFResults.firstOrbit_ = nullptr;
        gVFResults.currentOrbit_ = nullptr;
    } else {
        // move gVFResults.currentOrbit_ to the one before it, we search it
        // beginning from gVFResults.firstOrbit_
        auto orbit2 = gVFResults.firstOrbit_;
        do {
            gVFResults.currentOrbit_ = orbit2;
            orbit2 = orbit2->next;
        } while (orbit2 != orbit1);
        gVFResults.currentOrbit_->next = nullptr;
    }
    delete orbit1->firstpt;
}

// ---------------------------------------------------------------------------
//          integrate_poincare_orbit
// ---------------------------------------------------------------------------
// integrate poincare sphere case p=q=1
// This calculates 1 step.
void integrate_poincare_orbit(double p0, double p1, double p2, double *pcoord,
                              double &hhi, int &dashes, int &dir, double h_min,
                              double h_max)
{
    double y[2], theta, hhi0;

    if (pcoord[2] > ZCOORD) {
        hhi0 = hhi;
        while (1) {
            dashes = true;
            dir = 1;
            psphere_to_R2(p0, p1, p2, y);
            rk78(eval_r_vec_field, y, &hhi0, h_min, h_max,
                 gVFResults.config_tolerance_);
            if (gThisVF->getVFIndex_R2(y) == gVFResults.K_)
                break;
            h_min = gVFResults.config_branchhmi_;
            h_max /= 2;
            hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
            if (fabs(hhi0) < h_min || h_max < gVFResults.config_branchhmi_) {
                hhi0 = h_min;
                if (hhi < 0)
                    hhi0 = -hhi0;
                break;
            }
        }
        hhi = hhi0;
        R2_to_psphere(y[0], y[1], pcoord);
    } else {
        theta = atan2(fabs(p1), fabs(p0));
        if ((theta < PI_DIV4) && (theta > -PI_DIV4)) {
            if (p0 > 0) {
                hhi0 = hhi;
                while (1) {
                    dashes = true;
                    dir = 1;
                    psphere_to_U1(p0, p1, p2, y);
                    rk78(eval_U1_vec_field, y, &hhi0, h_min, h_max,
                         gVFResults.config_tolerance_);
                    if (y[1] >= 0 || !gVFResults.vf_[gVFResults.K_]->singinf_) {
                        if (gThisVF->getVFIndex_U1(y) == gVFResults.K_)
                            break;
                    } else {
                        if (gThisVF->getVFIndex_VV1(y) == gVFResults.K_)
                            break;
                    }
                    h_min = gVFResults.config_branchhmi_;
                    h_max /= 2;
                    hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                    if (fabs(hhi0) < h_min ||
                        h_max < gVFResults.config_branchhmi_) {
                        hhi0 = h_min;
                        if (hhi < 0)
                            hhi0 = -hhi0;
                        break;
                    }
                }
                hhi = hhi0;
                if (y[1] >= 0 || !gVFResults.vf_[gVFResults.K_]->singinf_)
                    U1_to_psphere(y[0], y[1], pcoord);
                else {
                    VV1_to_psphere(y[0], y[1], pcoord);
                    if (gVFResults.vf_[gVFResults.K_]->dir_vec_field_ == 1) {
                        dir = -1;
                        hhi = -hhi;
                    }
                    psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], y);
                    dashes = false;
                }
            } else {
                hhi0 = hhi;
                while (1) {
                    dashes = true;
                    dir = 1;
                    psphere_to_V1(p0, p1, p2, y);
                    rk78(eval_V1_vec_field, y, &hhi0, h_min, h_max,
                         gVFResults.config_tolerance_);
                    if (y[1] >= 0 || !gVFResults.vf_[gVFResults.K_]->singinf_) {
                        if (gThisVF->getVFIndex_V1(y) == gVFResults.K_)
                            break;
                    } else {
                        if (gThisVF->getVFIndex_UU1(y) == gVFResults.K_)
                            break;
                    }
                    h_min = gVFResults.config_branchhmi_;
                    h_max /= 2;
                    hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                    if (fabs(hhi0) < h_min ||
                        h_max < gVFResults.config_branchhmi_) {
                        hhi0 = h_min;
                        if (hhi < 0)
                            hhi0 = -hhi0;
                        break;
                    }
                }
                hhi = hhi0;
                if (y[1] >= 0 || !gVFResults.vf_[gVFResults.K_]->singinf_)
                    V1_to_psphere(y[0], y[1], pcoord);
                else {
                    UU1_to_psphere(y[0], y[1], pcoord);
                    if (gVFResults.vf_[gVFResults.K_]->dir_vec_field_ == 1) {
                        dir = -1;
                        hhi = -hhi;
                    }
                    psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], y);
                    dashes = false;
                }
            }
        } else {
            if (p1 > 0) {
                hhi0 = hhi;
                while (1) {
                    psphere_to_U2(p0, p1, p2, y);
                    rk78(eval_U2_vec_field, y, &hhi0, h_min, h_max,
                         gVFResults.config_tolerance_);
                    if (y[1] >= 0 || !gVFResults.vf_[gVFResults.K_]->singinf_) {
                        if (gThisVF->getVFIndex_U2(y) == gVFResults.K_)
                            break;
                    } else {
                        if (gThisVF->getVFIndex_VV2(y) == gVFResults.K_)
                            break;
                    }
                    h_min = gVFResults.config_branchhmi_;
                    h_max /= 2;
                    hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                    if (fabs(hhi0) < h_min ||
                        h_max < gVFResults.config_branchhmi_) {
                        hhi0 = h_min;
                        if (hhi < 0)
                            hhi0 = -hhi0;
                        break;
                    }
                }
                hhi = hhi0;
                if (y[1] >= 0 || !gVFResults.vf_[gVFResults.K_]->singinf_)
                    U2_to_psphere(y[0], y[1], pcoord);
                else {
                    VV2_to_psphere(y[0], y[1], pcoord);
                    if (gVFResults.vf_[gVFResults.K_]->dir_vec_field_ == 1) {
                        dir = -1;
                        hhi = -hhi;
                    }
                    psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], y);
                    dashes = false;
                }
            } else {
                hhi0 = hhi;
                while (1) {
                    psphere_to_V2(p0, p1, p2, y);
                    rk78(eval_V2_vec_field, y, &hhi, h_min, h_max,
                         gVFResults.config_tolerance_);
                    if (y[1] >= 0 || !gVFResults.vf_[gVFResults.K_]->singinf_) {
                        if (gThisVF->getVFIndex_V2(y) == gVFResults.K_)
                            break;
                    } else {
                        if (gThisVF->getVFIndex_UU2(y) == gVFResults.K_)
                            break;
                    }
                    h_min = gVFResults.config_branchhmi_;
                    h_max /= 2;
                    hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                    if (fabs(hhi0) < h_min ||
                        h_max < gVFResults.config_branchhmi_) {
                        hhi0 = h_min;
                        if (hhi < 0)
                            hhi0 = -hhi0;
                        break;
                    }
                }
                if (y[1] >= 0 || !gVFResults.vf_[gVFResults.K_]->singinf_)
                    V2_to_psphere(y[0], y[1], pcoord);
                else {
                    UU2_to_psphere(y[0], y[1], pcoord);
                    if (gVFResults.vf_[gVFResults.K_]->dir_vec_field_ == 1) {
                        dir = -1;
                        hhi = -hhi;
                    }
                    psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], y);
                    dashes = false;
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
//          integrate_lyapunov_orbit
// ---------------------------------------------------------------------------
// integrate on the Poincare-Lyapunov sphere
// This calculates 1 step.
void integrate_lyapunov_orbit(double p0, double p1, double p2, double *pcoord,
                              double &hhi, int &dashes, int &dir, double h_min,
                              double h_max)
{
    double y[2];
    double hhi0{hhi};

    if (p0 == 0) {
        while (1) {
            dir = 1;
            dashes = true;
            y[0] = p1;
            y[1] = p2;
            rk78(eval_r_vec_field, y, &hhi, h_min, h_max,
                 gVFResults.config_tolerance_);
            if (gThisVF->getVFIndex_R2(y) == gVFResults.K_)
                break;
            h_min = gVFResults.config_branchhmi_;
            h_max /= 2;
            hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
            if (fabs(hhi0) < h_min || h_max < gVFResults.config_branchhmi_) {
                hhi0 = h_min;
                if (hhi < 0)
                    hhi0 = -hhi0;
                break;
            }
        }
        hhi = hhi0;
        R2_to_plsphere(y[0], y[1], pcoord);
    } else {
        while (1) {
            dashes = true;
            dir = 1;
            y[0] = p1;
            y[1] = p2;
            rk78(eval_vec_field_cyl, y, &hhi, h_min, h_max,
                 gVFResults.config_tolerance_);
            if (gThisVF->getVFIndex_cyl(y) == gVFResults.K_)
                break;
            h_min = gVFResults.config_branchhmi_;
            h_max /= 2;
            hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
            if (fabs(hhi0) < h_min || h_max < gVFResults.config_branchhmi_) {
                hhi0 = h_min;
                if (hhi < 0)
                    hhi0 = -hhi0;
                break;
            }
        }
        hhi = hhi0;
        cylinder_to_plsphere(y[0], y[1], pcoord);
    }
}

// ---------------------------------------------------------------------------
//          integrate_orbit
// ---------------------------------------------------------------------------
// Integrate a number of points (user-dependent)
P4Orbits::orbits_points *integrate_orbit(P4Sphere *spherewnd, double pcoord[3],
                                         double step, int dir, int color,
                                         int points_to_int,
                                         P4Orbits::orbits_points **orbit)
{
    int d, h;
    int dashes;
    double pcoord2[3];
    P4Orbits::orbits_points *first_orbit{nullptr}, *last_orbit{nullptr};

    double hhi{dir * step};
    double h_min{gVFResults.config_hmi_};
    double h_max{gVFResults.config_hma_};

    copy_x_into_y(pcoord, pcoord2);

    for (int i = 1; i <= points_to_int; ++i) {
        if (!prepareVfForIntegration(pcoord))
            break;

        MATHFUNC(integrate_sphere_orbit)
        (pcoord[0], pcoord[1], pcoord[2], pcoord, hhi, dashes, d, h_min, h_max);

        if ((i % UPDATEFREQ_STEPSIZE) == 0)
            set_current_step(fabs(hhi));

        if (last_orbit == nullptr) {
            h = dir;
            first_orbit = new P4Orbits::orbits_points{
                pcoord, color, dashes && gVFResults.config_dashes_, d * h};
            last_orbit = first_orbit;
        } else {
            h = last_orbit->dir;
            last_orbit->nextpt = new P4Orbits::orbits_points{
                pcoord, color, dashes && gVFResults.config_dashes_, d * h};
            last_orbit = last_orbit->nextpt;
        }

        if (dashes && gVFResults.config_dashes_)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);
    }
    set_current_step(fabs(hhi));
    if (first_orbit != nullptr)
        *orbit = last_orbit;
    return first_orbit;
}
