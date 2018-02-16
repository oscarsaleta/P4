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

#include "math_orbits.h"

#include "custom.h"
#include "file_tab.h"
#include "math_charts.h"
#include "math_numerics.h"
#include "math_p4.h"
#include "math_polynom.h"
#include "plot_tools.h"
#include "win_sphere.h"

#include <cmath>

// -----------------------------------------------------------------------
//          prepareVfForIntegration
// -----------------------------------------------------------------------
bool prepareVfForIntegration(double *pcoord)
{
    int K = g_ThisVF->getVFIndex_sphere(pcoord);
    if (K >= 0) {
        // TODO: does this work?
        // g_VFResults.vfK_ = g_VFResults.vf_[K];
        g_VFResults.K_ = K;
        return true;
    } else {
        // g_VFResults.vfK_ = nullptr;
        return false;
    }
}

// -----------------------------------------------------------------------
//          integrateOrbit
// -----------------------------------------------------------------------
// dir = -1: backwards, dir=0: continue, dir=+1: forwards
// Continues orbit integration
// TODO check
void integrateOrbit(std::shared_ptr<P4WinSphere> sphere, int dir)
{
    std::vector<p4orbits::orbits_points> pts double pcoord[3], ucoord[2];

    if (dir == 0) {
        // continue orbit button has been pressed
        dir = g_VFResults.orbits.back().points.back().dir;

        copy_x_into_y(g_VFResults.orbits_.back().points.back().pcoord, pcoord);
        if (!prepareVfForIntegration(pcoord))
            return;

        pts = integrate_orbit(sphere, pcoord, g_VFResults.config_currentstep_,
                              dir, bgColours::CORBIT,
                              g_VFResults.config_intpoints_);
        g_VFResults.orbits_.back().points.push_back(pts);

        return;
    }

    copy_x_into_y(g_VFResults.orbits_.back().pcoord, pcoord);
    MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);

    if (!prepareVfForIntegration(pcoord))
        return;

    if (g_VFResults.config_kindvf_ == INTCONFIG_ORIGINAL)
        if (eval_term2(g_VFResults.gcf_, ucoord) < 0)
            dir = -dir;

    if (g_VFResults.orbits_.back().points.empty()) {
        pts = integrate_orbit(sphere, pcoord, g_VFResults.config_step_, dir,
                              bgColours::CORBIT, g_VFResults.config_intpoints_);
        g_VFResults.orbits_.back().points = pts;
    } else {
        // create an orbit point
        pts = p4orbits::orbits_points{bgColours::CORBIT, pcoord, 0, dir, 0};
        // integrate more points
        std::vector<p4orbits::orbits_points> int_pts =
            integrate_orbit(sphere, pcoord, g_VFResults.config_step_, dir,
                            bgColours::CORBIT, g_VFResults.config_intpoints_);
        // create a vector starting by the first point and appending the
        // integrated ones to the end
        std::vector<p4orbits::orbits_points> orbit_result{pts};
        orbit_result.insert(orbit_result.end(), int_pts.begin(), int_pts.end());
        // append this vector to g_VFResults.orbits_.back().points
        g_VFResults.orbits_.back().points.insert(
            g_VFResults.orbits_.back().points.end(), orbit_result.begin(),
            orbit_result.end());
    }
}

// -----------------------------------------------------------------------
//          startOrbit
// -----------------------------------------------------------------------
/* R=0 then point selected in the drawing canvas else in the orbit window */
bool startOrbit(std::shared_ptr<P4WinSphere> sphere, double x, double y, bool R)
{
    double pcoord[3];
    double ucoord[2];

    if (R)
        MATHFUNC(R2_to_sphere)(x, y, pcoord);
    else if ((MATHFUNC(is_valid_viewcoord)(x, y, pcoord)) == false)
        return false;

    if (!prepareVfForIntegration(pcoord))
        return false;

    p4orbits::orbits newOrbit{pcoord, bgColours::CORBIT,
                              std::vector<p4orbits::orbits_points>(), 0};
    g_VFResults.orbits_.push_back(newOrbit);

    MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], ucoord);
    sphere->drawPoint(ucoord[0], ucoord[1], bgColours::CORBIT);

    return true;
}

// -----------------------------------------------------------------------
//          drawOrbit
// -----------------------------------------------------------------------
void drawOrbit(std::shared_ptr<P4WinSphere> spherewnd, double *pcoord,
               std::vector<p4orbits::orbits_points> points, int color)
{
    double pcoord1[3];

    copy_x_into_y(pcoord, pcoord1);
    (*plot_p)(spherewnd, pcoord, color);

    for (auto it : points) {
        if (points->dashes) {
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
void drawOrbits(std::shared_ptr<P4WinSphere> spherewnd)
{
    orbits *orbit;

    for (auto it : g_VFResults.orbits_)
        drawOrbit(spherewnd, it.pcoord, it.points, it.color);
}

// -----------------------------------------------------------------------
//          deleteLastOrbit
// -----------------------------------------------------------------------
void deleteLastOrbit(std::shared_ptr<P4WinSphere> spherewnd)
{
    if (g_VFResults.orbits_.empty())
        return;

    p4orbits::orbits &orbit1 = g_VFResults.orbits_.back();
    drawOrbit(spherewnd, orbit2.pcoord, orbit2.points,
              spherewnd->spherebgcolor_);

    g_VFResults.orbits_.pop_back();
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
            rk78(eval_r_vec_field, y, hhi0, h_min, h_max,
                 g_VFResults.config_tolerance_);
            if (g_ThisVF->getVFIndex_R2(y) == g_VFResults.K_)
                break;
            h_min = g_VFResults.config_branchhmi_;
            h_max /= 2;
            hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
            if (fabs(hhi0) < h_min || h_max < g_VFResults.config_branchhmi_) {
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
                    rk78(eval_U1_vec_field, y, hhi0, h_min, h_max,
                         g_VFResults.config_tolerance_);
                    if (y[1] >= 0 ||
                        !g_VFResults.vf_[g_VFResults.K_].singinf_) {
                        if (g_ThisVF->getVFIndex_U1(y) == g_VFResults.K_)
                            break;
                    } else {
                        if (g_ThisVF->getVFIndex_VV1(y) == g_VFResults.K_)
                            break;
                    }
                    h_min = g_VFResults.config_branchhmi_;
                    h_max /= 2;
                    hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                    if (fabs(hhi0) < h_min ||
                        h_max < g_VFResults.config_branchhmi_) {
                        hhi0 = h_min;
                        if (hhi < 0)
                            hhi0 = -hhi0;
                        break;
                    }
                }
                hhi = hhi0;
                if (y[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_)
                    U1_to_psphere(y[0], y[1], pcoord);
                else {
                    VV1_to_psphere(y[0], y[1], pcoord);
                    if (g_VFResults.vf_[g_VFResults.K_].dir_vec_field_ == 1) {
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
                    rk78(eval_V1_vec_field, y, hhi0, h_min, h_max,
                         g_VFResults.vf_[g_VFResults.K_].config_tolerance_);
                    if (y[1] >= 0 ||
                        !g_VFResults.vf_[g_VFResults.K_].singinf_) {
                        if (g_ThisVF->getVFIndex_V1(y) == g_VFResults.K_)
                            break;
                    } else {
                        if (g_ThisVF->getVFIndex_UU1(y) == g_VFResults.K_)
                            break;
                    }
                    h_min = g_VFResults.config_branchhmi_;
                    h_max /= 2;
                    hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                    if (fabs(hhi0) < h_min ||
                        h_max < g_VFResults.config_branchhmi_) {
                        hhi0 = h_min;
                        if (hhi < 0)
                            hhi0 = -hhi0;
                        break;
                    }
                }
                hhi = hhi0;
                if (y[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_)
                    V1_to_psphere(y[0], y[1], pcoord);
                else {
                    UU1_to_psphere(y[0], y[1], pcoord);
                    if (g_VFResults.vf_[g_VFResults.K_].dir_vec_field_ == 1) {
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
                    rk78(eval_U2_vec_field, y, hhi0, h_min, h_max,
                         g_VFResults.vf_[g_VFResults.K_].config_tolerance_);
                    if (y[1] >= 0 ||
                        !g_VFResults.vf_[g_VFResults.K_].singinf_) {
                        if (g_ThisVF->getVFIndex_U2(y) == g_VFResults.K_)
                            break;
                    } else {
                        if (g_ThisVF->getVFIndex_VV2(y) == g_VFResults.K_)
                            break;
                    }
                    h_min = g_VFResults.config_branchhmi_;
                    h_max /= 2;
                    hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                    if (fabs(hhi0) < h_min ||
                        h_max < g_VFResults.config_branchhmi_) {
                        hhi0 = h_min;
                        if (hhi < 0)
                            hhi0 = -hhi0;
                        break;
                    }
                }
                hhi = hhi0;
                if (y[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_)
                    U2_to_psphere(y[0], y[1], pcoord);
                else {
                    VV2_to_psphere(y[0], y[1], pcoord);
                    if (g_VFResults.dir_vec_field_ == 1) {
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
                    rk78(eval_V2_vec_field, y, hhi, h_min, h_max,
                         g_VFResults.config_tolerance_);
                    if (y[1] >= 0 ||
                        !g_VFResults.vf_[g_VFResults.K_].singinf_) {
                        if (g_ThisVF->getVFIndex_V2(y) == g_VFResults.K_)
                            break;
                    } else {
                        if (g_ThisVF->getVFIndex_UU2(y) == g_VFResults.K_)
                            break;
                    }
                    h_min = g_VFResults.config_branchhmi_;
                    h_max /= 2;
                    hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                    if (fabs(hhi0) < h_min ||
                        h_max < g_VFResults.config_branchhmi_) {
                        hhi0 = h_min;
                        if (hhi < 0)
                            hhi0 = -hhi0;
                        break;
                    }
                }
                if (y[1] >= 0 || !g_VFResults.vf_[g_VFResults.K_].singinf_)
                    V2_to_psphere(y[0], y[1], pcoord);
                else {
                    UU2_to_psphere(y[0], y[1], pcoord);
                    if (g_VFResults.dir_vec_field_ == 1) {
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
            // TODO: changed hhi for hhi0, works?
            rk78(eval_r_vec_field, y, hhi0, h_min, h_max,
                 g_VFResults.config_tolerance_);
            if (g_ThisVF->getVFIndex_R2(y) == g_VFResults.K_)
                break;
            h_min = g_VFResults.config_branchhmi_;
            h_max /= 2;
            hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
            if (fabs(hhi0) < h_min || h_max < g_VFResults.config_branchhmi_) {
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
            // TODO: changed hhi for hhi0, works?
            rk78(eval_vec_field_cyl, y, hhi0, h_min, h_max,
                 g_VFResults.config_tolerance_);
            if (g_ThisVF->getVFIndex_cyl(y) == g_VFResults.K_)
                break;
            h_min = g_VFResults.config_branchhmi_;
            h_max /= 2;
            hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
            if (fabs(hhi0) < h_min || h_max < g_VFResults.config_branchhmi_) {
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
std::vector<p4orbits::orbits_points> integrate_orbit(
    std::shared_ptr<P4WinSphere> spherewnd, double pcoord[3], double step,
    int dir, int color, int points_to_int)
{
    int i, d, h;
    int dashes;
    double hhi;
    double pcoord2[3], h_min, h_max;
    p4orbits::orbits_points new_orbit;
    std::vector<p4orbits::orbits_points> orbit_result;

    hhi = dir * step;
    h_min = g_VFResults.config_hmi_;
    h_max = g_VFResults.config_hma_;
    copy_x_into_y(pcoord, pcoord2);
    for (i = 1; i <= points_to_int; ++i) {
        if (!prepareVfForIntegration(pcoord))
            break;
        MATHFUNC(integrate_sphere_orbit)
        (pcoord[0], pcoord[1], pcoord[2], pcoord, hhi, dashes, d, h_min, h_max);

        if ((i % UPDATEFREQ_STEPSIZE) == 0)
            set_current_step(fabs(hhi));

        /*if (last_orbit == nullptr) {
            first_orbit = new orbits_points;
            last_orbit = first_orbit;
            h = dir;
        } else {
            last_orbit->next_point = new orbits_points;
            h = last_orbit->dir;
            last_orbit = last_orbit->next_point;
        }*/

        copy_x_into_y(pcoord, last_orbit.pcoord);
        last_orbit.color = color;
        last_orbit.dashes = dashes * g_VFResults.config_dashes_;
        last_orbit.dir = d * h;

        orbit_result.push_back(last_orbit);

        if (dashes && g_VFResults.config_dashes_)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);
    }
    set_current_step(fabs(hhi));
    return orbit_result;
}
