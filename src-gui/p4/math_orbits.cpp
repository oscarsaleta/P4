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

#include <cmath>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4VFStudy.hpp"
#include "P4WinSphere.hpp"
#include "custom.hpp"
#include "math_charts.hpp"
#include "math_numerics.hpp"
#include "math_p4.hpp"
#include "math_polynom.hpp"
#include "plot_tools.hpp"
#include "tables.hpp"

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
void integrateOrbit(P4WinSphere *sphere, int dir)
{
    // std::vector<p4orbits::orbits_points> pts;
    double pcoord[3], ucoord[2];
    auto &currentpt = gVFResults.orbits_.back().points.back();

    if (dir == 0) {
        // continue orbit button has been pressed
        dir = currentpt.dir;

        copy_x_into_y(currentpt.pcoord, pcoord);
        if (!prepareVfForIntegration(pcoord))
            return;

        auto pts =
            integrate_orbit(sphere, pcoord, gVFResults.config_currentstep_, dir,
                            bgColours::CORBIT, gVFResults.config_intpoints_);
        auto &gpoints = gVFResults.orbits_.back().points;
        gpoints.insert(std::end(gpoints), std::begin(pts), std::end(pts));

        return;
    }

    copy_x_into_y(gVFResults.orbits_.back().pcoord, pcoord);
    MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);

    if (!prepareVfForIntegration(pcoord))
        return;

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL)
        if (eval_term2(gVFResults.vf_[gVFResults.K_]->gcf_, ucoord) < 0)
            dir = -dir;

    if (gVFResults.orbits_.back().points.empty()) {
        auto pts =
            integrate_orbit(sphere, pcoord, gVFResults.config_step_, dir,
                            bgColours::CORBIT, gVFResults.config_intpoints_);
        gVFResults.orbits_.back().points = std::move(pts);
    } else {
        // create an orbit point
        auto pts =
            p4orbits::orbits_points{bgColours::CORBIT, pcoord, 0, dir, 0};
        // integrate more points
        auto int_pts =
            integrate_orbit(sphere, pcoord, gVFResults.config_step_, dir,
                            bgColours::CORBIT, gVFResults.config_intpoints_);
        // create a vector starting by the first point and appending the
        // integrated ones to the end
        std::vector<p4orbits::orbits_points> orbit_result{pts};
        orbit_result.insert(std::end(orbit_result), std::begin(int_pts),
                            std::end(int_pts));
        // append this vector to gVFResults.orbits_.back().points
        gVFResults.orbits_.back().points.insert(
            std::end(gVFResults.orbits_.back().points),
            std::begin(orbit_result), std::end(orbit_result));
    }
}

// -----------------------------------------------------------------------
//          startOrbit
// -----------------------------------------------------------------------
/* R=0 then point selected in the drawing canvas else in the orbit window */
bool startOrbit(P4WinSphere *sphere, double x, double y, bool R)
{
    double pcoord[3];
    double ucoord[2];

    if (R)
        MATHFUNC(R2_to_sphere)(x, y, pcoord);
    else if ((MATHFUNC(is_valid_viewcoord)(x, y, pcoord)) == false)
        return false;

    if (!prepareVfForIntegration(pcoord))
        return false;

    gVFResults.orbits_.emplace_back(pcoord, bgColours::CORBIT,
                                    std::vector<p4orbits::orbits_points>{}, 0);

    MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], ucoord);
    sphere->drawPoint(ucoord[0], ucoord[1], bgColours::CORBIT);

    return true;
}

// -----------------------------------------------------------------------
//          drawOrbit
// -----------------------------------------------------------------------
void drawOrbit(P4WinSphere *spherewnd, const double *pcoord,
               const std::vector<p4orbits::orbits_points> &points, int color)
{
    double pcoord1[3];

    copy_x_into_y(pcoord, pcoord1);
    (*plot_p)(spherewnd, pcoord, color);

    for (auto const &it : points) {
        if (it.dashes) {
            (*plot_l)(spherewnd, pcoord1, it.pcoord, color);
        } else {
            (*plot_p)(spherewnd, it.pcoord, color);
        }
        copy_x_into_y(it.pcoord, pcoord1);
    }
}

// -----------------------------------------------------------------------
//                      DRAWORBITS
// -----------------------------------------------------------------------
void drawOrbits(P4WinSphere *spherewnd)
{
    for (auto const &it : gVFResults.orbits_)
        drawOrbit(spherewnd, it.pcoord, it.points, it.color);
}

// -----------------------------------------------------------------------
//          deleteLastOrbit
// -----------------------------------------------------------------------
void deleteLastOrbit(P4WinSphere *spherewnd)
{
    if (gVFResults.orbits_.empty())
        return;

    auto &orbit1 = gVFResults.orbits_.back();
    drawOrbit(spherewnd, orbit1.pcoord, orbit1.points,
              spherewnd->spherebgcolor_);

    gVFResults.orbits_.pop_back();
}

// ---------------------------------------------------------------------------
//          integrate_poincare_orbit
// ---------------------------------------------------------------------------
// integrate poincare sphere case p=q=1
//
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
            // NOTE: changed hhi for hhi0, works?
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
        R2_to_plsphere(y[0], y[1], pcoord);
    } else {
        while (1) {
            dashes = true;
            dir = 1;
            y[0] = p1;
            y[1] = p2;
            // NOTE: changed hhi for hhi0, works?
            rk78(eval_vec_field_cyl, y, &hhi0, h_min, h_max,
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
std::vector<p4orbits::orbits_points>
integrate_orbit(P4WinSphere *spherewnd, double pcoord[3], double step, int dir,
                int color, int points_to_int)
{
    int d, h;
    int dashes;
    double pcoord2[3];
    p4orbits::orbits_points new_orbit;
    std::vector<p4orbits::orbits_points> orbit_result;

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

        if (orbit_result.empty())
            h = dir;
        else
            h = orbit_result.back().dir;

        orbit_result.emplace_back(
            color, pcoord, dashes && gVFResults.config_dashes_, d * h, 0);

        if (dashes && gVFResults.config_dashes_)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);
    }
    set_current_step(fabs(hhi));
    return orbit_result;
}
