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

#include "math_separatrice.hpp"

#include "custom.hpp"
#include "tables.hpp"
#include "math_charts.hpp"
#include "math_numerics.hpp"
#include "math_p4.hpp"
#include "math_polynom.hpp"
#include "plot_tools.hpp"
#include "P4ParentStudy.hpp"

#include <cmath>

void (*change_epsilon)(P4WinSphere *, double) = nullptr;
void (*start_plot_sep)(P4WinSphere *, int) = nullptr;
void (*cont_plot_sep)(P4WinSphere *) = nullptr;
void (*plot_next_sep)(P4WinSphere *, int) = nullptr;
void (*select_next_sep)(P4WinSphere *) = nullptr;

// ---------------------------------------------------------------------------
//          findSepColor2
//          findSepColor3
// ---------------------------------------------------------------------------
//
// The stability of separatrices is found for the reduced vector field.  In case
// a greatest common factor is present, we have to take into account the sign
// of this common factor to determine the plot color of the separatrice.
//
// The point y is a point in one of the charts.
int findSepColor2(const std::vector<p4polynom::term2> &f, int type, double y[2])
{
    int color;

    if (eval_term2(f, y) >= 0) {
        switch (type) {
        case OT_STABLE:
            color = CSTABLE;
            break;
        case OT_UNSTABLE:
            color = CUNSTABLE;
            break;
        case OT_CENT_STABLE:
            color = CCENT_STABLE;
            break;
        case OT_CENT_UNSTABLE:
            color = CCENT_UNSTABLE;
            break;
        default:
            color = 0;
            break;  // to avoid compiler warnings
        }
    } else {
        switch (type) {
        case OT_STABLE:
            color = CUNSTABLE;
            break;
        case OT_UNSTABLE:
            color = CSTABLE;
            break;
        case OT_CENT_STABLE:
            color = CCENT_UNSTABLE;
            break;
        case OT_CENT_UNSTABLE:
            color = CCENT_STABLE;
            break;
        default:
            color = 0;
            break;  // to avoid compiler warnings
        }
    }
    return (color);
}

int findSepColor3(const std::vector<p4polynom::term3> &f, int type, double y[2])
{
    int color;

    if (eval_term3(f, y) >= 0) {
        switch (type) {
        case OT_STABLE:
            color = CSTABLE;
            break;
        case OT_UNSTABLE:
            color = CUNSTABLE;
            break;
        case OT_CENT_STABLE:
            color = CCENT_STABLE;
            break;
        case OT_CENT_UNSTABLE:
            color = CCENT_UNSTABLE;
            break;
        default:
            color = 0;
            break;
        }
    } else {
        switch (type) {
        case OT_STABLE:
            color = CUNSTABLE;
            break;
        case OT_UNSTABLE:
            color = CSTABLE;
            break;
        case OT_CENT_STABLE:
            color = CCENT_UNSTABLE;
            break;
        case OT_CENT_UNSTABLE:
            color = CCENT_STABLE;
            break;
        default:
            color = 0;
            break;
        }
    }
    return (color);
}

// ---------------------------------------------------------------------------
//          change_type
// ---------------------------------------------------------------------------
//
// Sometimes, when a separatrix is integrated close to the line of infinity,
// it is possible that the line of infinity is crossed (only when p=q=1).
// By crossing the line of singularity, the separatrix may change its stability
// (depending on the degree of the vector field).
int change_type(int type)
{
    int t;

    switch (type) {
    case OT_STABLE:
        t = OT_UNSTABLE;
        break;
    case OT_UNSTABLE:
        t = OT_STABLE;
        break;
    case OT_CENT_STABLE:
        t = OT_CENT_UNSTABLE;
        break;
    case OT_CENT_UNSTABLE:
        t = OT_CENT_STABLE;
        break;
    case OT_ORBIT:
        t = OT_ORBIT;
        break;
    default:
        t = type;
        break;
    }
    return t;
}

// ---------------------------------------------------------------------------
//          integrate_poincare_sep
// ---------------------------------------------------------------------------
//
// Integrate Separatrix on the Poincare Sphere, i.e. in case p=q=1
// This routine calcuates 1 integration step.
void integrate_poincare_sep(double p0, double p1, double p2, double *pcoord,
                            double &hhi, int &type, int &color, int &dashes,
                            int &dir, double h_min, double h_max)
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
            if (gThisVF.getVFIndex_R2(y) == gVFResults.K_)
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
        color = findSepColor2(gVFResults.gcf_, type, y);
    } else {  // infinite region (annulus)
        theta = atan2(fabs(p1), fabs(p0));
        if ((theta < PI_DIV4) && (theta > -PI_DIV4)) {
            if (p0 > 0) {
                hhi0 = hhi;
                while (1) {
                    dashes = true;
                    dir = 1;
                    psphere_to_U1(p0, p1, p2, y);
                    rk78(eval_U1_vec_field, y, hhi0, h_min, h_max,
                         gVFResults.config_tolerance_);
                    if (y[1] >= 0 || !gVFResults.singinf_) {
                        if (gThisVF.getVFIndex_U1(y) == gVFResults.K_)
                            break;
                    } else {
                        if (gThisVF.getVFIndex_VV1(y) == gVFResults.K_)
                            break;
                    }
                    h_min = gVFResults.config_branchhmi;
                    h_max /= 2;
                    hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                    if (fabs(hhi0) < h_min ||
                        h_max < gVFResults.config_branchhmi) {
                        hhi0 = h_min;
                        if (hhi < 0)
                            hhi0 = -hhi0;
                        break;
                    }
                }
                hhi = hhi0;
                if (y[1] >= 0 || !gVFResults.singinf_) {
                    U1_to_psphere(y[0], y[1], pcoord);
                    color = findSepColor2(
                        gVFResults.vf_[gVFResults.K_].gcf_U1_, type, y);
                } else {
                    VV1_to_psphere(y[0], y[1], pcoord);
                    if (gVFResults.vf_[gVFResults.K_].dir_vec_field_ == 1) {
                        dir = -1;
                        hhi = -hhi;
                        type = change_type(type);
                    }
                    psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], y);
                    color = findSepColor2(
                        gVFResults.vf_[gVFResults.K_].gcf_V1_, type, y);
                    dashes = false;
                }
            } else {
                hhi0 = hhi;
                while (1) {
                    dashes = true;
                    dir = 1;
                    psphere_to_V1(p0, p1, p2, y);
                    rk78(eval_V1_vec_field, y, hhi0, h_min, h_max,
                         gVFResults.config_tolerance_);
                    if (y[1] >= 0 || !gVFResults.singinf_) {
                        if (gThisVF.getVFIndex_V1(y) == gVFResults.K_)
                            break;
                    } else {
                        if (gThisVF.getVFIndex_UU1(y) == gVFResults.K_)
                            break;
                    }
                    h_min = gVFResults.config_branchhmi;
                    h_max /= 2;
                    hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                    if (fabs(hhi0) < h_min ||
                        h_max < gVFResults.config_branchhmi) {
                        hhi0 = h_min;
                        if (hhi < 0)
                            hhi0 = -hhi0;
                        break;
                    }
                }
                hhi = hhi0;
                if (y[1] >= 0 || !gVFResults.singinf_) {
                    V1_to_psphere(y[0], y[1], pcoord);
                    color = findSepColor2(
                        gVFResults.vf_[gVFResults.K_].gcf_V1_, type, y);
                } else {
                    UU1_to_psphere(y[0], y[1], pcoord);
                    if (gVFResults.vf_[gVFResults.K_].dir_vec_field_ == 1) {
                        dir = -1;
                        hhi = -hhi;
                        type = change_type(type);
                    }
                    psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], y);
                    color = findSepColor2(
                        gVFResults.vf_[gVFResults.K_].gcf_U1_, type, y);
                    dashes = false;
                }
            }
        } else if (p1 > 0) {
            hhi0 = hhi;
            while (1) {
                psphere_to_U2(p0, p1, p2, y);
                rk78(eval_U2_vec_field, y, hhi0, h_min, h_max,
                     gVFResults.config_tolerance_);
                if (y[1] >= 0 || !gVFResults.singinf_) {
                    if (gThisVF.getVFIndex_U2(y) == gVFResults.K_)
                        break;
                } else {
                    if (gThisVF.getVFIndex_VV2(y) == gVFResults.K_)
                        break;
                }
                h_min = gVFResults.config_branchhmi;
                h_max /= 2;
                hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                if (fabs(hhi0) < h_min ||
                    h_max < gVFResults.config_branchhmi) {
                    hhi0 = h_min;
                    if (hhi < 0)
                        hhi0 = -hhi0;
                    break;
                }
            }
            hhi = hhi0;
            if (y[1] >= 0 || !gVFResults.singinf_) {
                U2_to_psphere(y[0], y[1], pcoord);
                color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_U2_,
                                      type, y);
            } else {
                VV2_to_psphere(y[0], y[1], pcoord);
                if (gVFResults.vf_[gVFResults.K_].dir_vec_field_ == 1) {
                    dir = -1;
                    hhi = -(hhi);
                    type = change_type(type);
                }
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], y);
                color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_V2_,
                                      type, y);
                dashes = false;
            }
        } else {
            hhi0 = hhi;
            while (1) {
                dashes = true;
                dir = 1;
                psphere_to_V2(p0, p1, p2, y);
                // TODO: comprovar tots els rk78 pq hhi0 era hhi al codi
                // original pero sembla que hauria de ferse servir hhi0
                rk78(eval_V2_vec_field, y, hhi0, h_min, h_max,
                     gVFResults.config_tolerance_);
                if (y[1] >= 0 || !gVFResults.singinf_) {
                    if (gThisVF.getVFIndex_V2(y) == gVFResults.K_)
                        break;
                } else {
                    if (gThisVF.getVFIndex_UU2(y) == gVFResults.K_)
                        break;
                }
                h_min = gVFResults.config_branchhmi;
                h_max /= 2;
                hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
                if (fabs(hhi0) < h_min ||
                    h_max < gVFResults.config_branchhmi) {
                    hhi0 = h_min;
                    if (hhi < 0)
                        hhi0 = -hhi0;
                    break;
                }
            }
            hhi = hhi0;
            if (y[1] >= 0 || !gVFResults.singinf_) {
                V2_to_psphere(y[0], y[1], pcoord);
                color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_V2_,
                                      type, y);
            } else {
                UU2_to_psphere(y[0], y[1], pcoord);
                if (gVFResults.vf_[gVFResults.K_].dir_vec_field_ == 1) {
                    dir = -1;
                    hhi = -(hhi);
                    type = change_type(type);
                }
                psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], y);
                color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_U2_,
                                      type, y);
                dashes = false;
            }
        }
    }
}

// ---------------------------------------------------------------------------
//                  INTEGRATE_LYAPUNOV_SEP
// ---------------------------------------------------------------------------
//
// Integrate Separatrix on the Poincare-Lyapunov sphere
// This routine calcuates 1 integration step.
void integrate_lyapunov_sep(double p0, double p1, double p2, double *pcoord,
                            double &hhi, int &type, int &color, int &dashes,
                            int &dir, double h_min, double h_max)
{
    double y[2], hhi0;

    if (p0 == 0) {
        hhi0 = hhi;
        while (1) {
            dashes = true;
            dir = 1;
            y[0] = p1;
            y[1] = p2;
            rk78(eval_r_vec_field, y, hhi0, h_min, h_max,
                 gVFResults.config_tolerance_);
            if (gThisVF.getVFIndex_R2(y) == gVFResults.K_)
                break;
            h_min = gVFResults.config_branchhmi;
            h_max /= 2;
            hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
            if (fabs(hhi0) < h_min || h_max < gVFResults.config_branchhmi) {
                hhi0 = h_min;
                if (hhi < 0)
                    hhi0 = -hhi0;
                break;
            }
        }
        hhi = hhi0;
        R2_to_plsphere(y[0], y[1], pcoord);
        color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_, type, y);
    } else {
        hhi0 = hhi;
        while (1) {
            dashes = true;
            dir = 1;
            y[0] = p1;
            y[1] = p2;
            rk78(eval_vec_field_cyl, y, hhi0, h_min, h_max,
                 gVFResults.config_tolerance_);
            if (y[1] >= TWOPI)
                y[1] -= TWOPI;
            if (gThisVF.getVFIndex_cyl(y) == gVFResults.K_)
                break;
            h_min = gVFResults.config_branchhmi;
            h_max /= 2;
            hhi0 = fabs(hhi0 * hhi) / 2 / hhi;
            if (fabs(hhi0) < h_min || h_max < gVFResults.config_branchhmi) {
                hhi0 = h_min;
                if (hhi < 0)
                    hhi0 = -hhi0;
                break;
            }
        }
        hhi = hhi0;
        cylinder_to_plsphere(y[0], y[1], pcoord);
        color = findSepColor3(gVFResults.vf_[gVFResults.K_].gcf_C_, type, y);
    }
}

// ---------------------------------------------------------------------------
//                  INTEGRATE_SEP
// ---------------------------------------------------------------------------
// All the points are stored in a vector, which is returned by the function. In
// case of error an emtpy vector is returned, so the user should test if the
// result is valid before operating on it.
//
// The vector field vfK need not be prepared
std::optional<std::vector<p4orbits::orbits_points>> integrate_sep(
    std::shared_ptr<P4WinSphere> spherewnd, double pcoord[3], double step,
    int dir, int type, int points_to_int)
{
    int i, d, h;
    int color, dashes;
    double hhi;
    double pcoord2[3];
    double h_min{gVFResults.config_hmi_}, h_max{gVFResults.config_hma_};
    p4orbits::orbits_points last_orbit;
    std::vector<p4orbits::orbits_points> orbit_result;

    /* if we intergrate a separatrice and use the original vector field
    then it is possible that we have to change the direction of the
    separatrice, because the separatrice is evaluate for the reduced
    vector field
    */
    if (!prepareVfForIntegration(pcoord))
        return {};

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL &&
        MATHFUNC(change_dir)(pcoord))
        hhi = -gVFResults.config_step_ * dir;
    else
        hhi = static_cast<double>(dir) * step;

    copy_x_into_y(pcoord, pcoord2);
    for (i = 1; i <= points_to_int; ++i) {
        MATHFUNC(integrate_sphere_sep)
        (pcoord[0], pcoord[1], pcoord[2], pcoord, hhi, type, color, dashes, d,
         h_min, h_max);

        if ((i % UPDATEFREQ_STEPSIZE) == 0)
            set_current_step(fabs(hhi));

        copy_x_into_y(pcoord, last_orbit.pcoord);
        last_orbit.color = color;
        last_orbit.dashes = dashes && gVFResults.config_dashes_;
        last_orbit.dir = d * h;
        last_orbit.type = type;

        if (dashes && gVFResults.config_dashes_)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);

        orbit_result.push_back(std::move(last_orbit));

        if (!prepareVfForIntegration(pcoord))
            break;
    }
    set_current_step(fabs(hhi));
    return orbit_result;
}

// ---------------------------------------------------------------------------
//                  FINDINITIALSEPPOINT
// ---------------------------------------------------------------------------
//
// Given a parametrized curve (t,sep(t)) with sep(0)=0.  The parametrized curve
// is in fact a Taylor polynomial of a separatrix.
//
// Given a value of epsilon.
//
// We find a value of t such that Norm2( t,sep(t) ) is approximately given by
// epsilon. (Norm2 is the standard Euclidean norm)
//
// More precisely we find a t such that the Norm2^2 lies in a 1% - error
// interval from epsilon^2
static double findInitialSepPoint(const std::vector<p4polynom::term1> &sep,
                                  double epsilon, int dir)
{
    double t, t1, t2, r0, a, b;

    t = epsilon * dir;
    a = pow(epsilon - epsilon / 100, 2.0);
    b = pow(epsilon + epsilon / 100, 2.0);
    r0 = pow(t, 2.0) + pow(eval_term1(sep, t), 2.0);

    if (r0 > a && r0 < b)
        return t;

    if (r0 <= a) {
        while (r0 <= a) {
            t *= 2.;
            r0 = pow(t, 2.0) + pow(eval_term1(sep, t), 2.0);
        }
        if (r0 < b)
            t2 = t;
        else {
            t1 = t / 2;
            t2 = (t + t1) / 2;
            while (1) {
                r0 = pow(t2, 2.0) + pow(eval_term1(sep, t2), 2.0);
                if ((r0 > a) && (r0 < b))
                    break;
                if (r0 <= a)
                    t1 = t2;
                else
                    t = t2;
                t2 = (t + t1) / 2;
            }
        }
    } else {
        while (r0 >= b) {
            t /= 2.;
            r0 = pow(t, 2.0) + pow(eval_term1(sep, t), 2.0);
        }
        if (r0 > a)
            t2 = t;
        else {
            t1 = 2. * t;
            t2 = (t + t1) / 2;
            for (;;) {
                r0 = pow(t2, 2.0) + pow(eval_term1(sep, t2), 2.0);
                if (r0 > a && r0 < b)
                    break;
                if (r0 <= a)
                    t = t2;
                else
                    t1 = t2;
                t2 = (t + t1) / 2;
            }
        }
    }
    return t2;
}

// ---------------------------------------------------------------------------
//                  PLOT_SEPARATRICE
// ---------------------------------------------------------------------------
//
// Starts separatrix integration, and plots the results meanwhile to the screen
//
// The first 100 points are drawn by looking at the formal power series
// development of the separatrix, as calculated by maple.  These points are
// drawn in an epsilon-ball around the singularity, where epsilon can be changed
// by the user.
//
// At the end, a normal integration cycle is added.
//
// The vector field vfK needs not be prepared.
std::optional<std::vector<p4orbits::orbits_points>> plot_separatrice(
    std::shared_ptr<P4WinSphere> spherewnd, double x0, double y0, double a11,
    double a12, double a21, double a22, double epsilon,
    const p4blowup::sep &sep1, short int chart, int vfindex)
{
    double t{0.0}, h, y;
    double pcoord[3], pcoord2[3], point[2];
    int i, color, type, dir;
    bool dashes, ok{true}, prepok;
    std::vector<p4orbits::orbits_points> orbit_result;
    p4obrbits::orbits_points new_orbit;

    switch (chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(x0, y0, pcoord);
        break;
    }
    if (!prepareVfForIntegration(pcoord))
        return {};

    /* if we have a line of singularities at infinity then we have to change
    the chart if the chart is V1 or V2 */
    if (gVFResults.vf_[gVFResults.K_].singinf_) {
        if (chart == CHART_V1)
            chart = CHART_U1;
        else if (chart == CHART_V2)
            chart = CHART_U2;
    }

    /* h=(epsilon/100)*sep1->direction; */
    h = findInitialSepPoint(sep1.separatrice, epsilon, sep1.direction) / 100;

    point[0] = x0;
    point[1] = y0;

    switch (chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_, sep1.type,
                              point);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_U1_,
                              sep1.type, point);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(x0, y0, pcoord);
        if ((gVFResults.p_ == 1) && (gVFResults.q_ == 1))
            psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
        color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_V1_,
                              sep1.type, point);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(x0, y0, pcoord);
        color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_U2_,
                              sep1.type, point);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(x0, y0, pcoord);
        if ((gVFResults.p_ == 1) && (gVFResults.q_ == 1))
            psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
        color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_U2_,
                              sep1.type, point);
        break;
    default:
        color = 0;
        break;
    }

    copy_x_into_y(pcoord, new_orbit.pcoord);
    type = sep1.type;
    switch (sep1.type) {
    case OT_STABLE:
        dir = OT_STABLE;
        break;
    case OT_UNSTABLE:
        dir = OT_UNSTABLE;
        break;
    case OT_CENT_STABLE:
        dir = OT_STABLE;
        break;
    case OT_CENT_UNSTABLE:
        dir = OT_UNSTABLE;
        break;
    default:
        dir = 0;
        break;
    }
    new_orbit.color = color;
    new_orbit.dashes = 0;
    new_orbit.dir = dir;
    new_orbit.type = type;
    orbit_result.push_back(std::move(new_orbit));

    // P5 addition: check if we can start with this separatrix

    y = eval_term1(sep1.separatrice, 100 * h);
    if (sep1.d) {
        point[0] = x0 + a11 * y + a12 * 100 * h;
        point[1] = y0 + a21 * y + a22 * 100 * h;
    } else {
        point[0] = x0 + a11 * 100 * h + a12 * y;
        point[1] = y0 + a21 * 100 * h + a22 * y;
    }

    switch (chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord2);
        break;
    case CHART_U1:
        if (point[1] >= 0 || !gVFResults.vf_[gVFResults.K_].singinf_)
            MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord2);
        else
            VV1_to_psphere(point[0], point[1], pcoord2);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord2);
        break;
    case CHART_U2:
        if (point[1] >= 0 || !gVFResults.vf_[gVFResults.K_].singinf_)
            MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord2);
        else
            VV2_to_psphere(point[0], point[1], pcoord2);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord2);
        break;
    }
    if (gThisVF.getVFIndex_sphere(pcoord2) != vfindex)
        return {};

    // end of P5 addition

    copy_x_into_y(pcoord, pcoord2);
    for (i = 0; i <= 99; i++) {
        t = t + h;
        y = eval_term1(sep1.separatrice, t);
        if ((t * t + y * y) > epsilon * epsilon)
            break;
        if (sep1.d) {
            point[0] = x0 + a11 * y + a12 * t;
            point[1] = y0 + a21 * y + a22 * t;
        } else {
            point[0] = x0 + a11 * t + a12 * y;
            point[1] = y0 + a21 * t + a22 * y;
        }
        dashes = true;

        switch (chart) {
        case CHART_R2:
            MATHFUNC(R2_to_sphere)(point[0], point[1], pcoord);
            prepok = prepareVfForIntegration(pcoord);
            if (!prepok)
                break;
            color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_,
                                  sep1.type, point);
            break;
        case CHART_U1:
            if (point[1] >= 0 || !gVFResults.vf_[gVFResults.K_].singinf_) {
                MATHFUNC(U1_to_sphere)(point[0], point[1], pcoord);
                prepok = prepareVfForIntegration(pcoord);
                if (!prepok)
                    break;
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (gVFResults.vf_[gVFResults.K_].dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = sep1.type;
                color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_U1_,
                                      type, point);
            } else {
                VV1_to_psphere(point[0], point[1], pcoord);
                prepok = prepareVfForIntegration(pcoord);
                if (!prepok)
                    break;
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (gVFResults.vf_[gVFResults.K_].dir_vec_field_ == 1)
                        dir *= -1;
                }
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
                prepok = prepareVfForIntegration(pcoord);
                if (!prepok)
                    break;
                if (gVFResults.vf_[gVFResults.K_].dir_vec_field_ == 1)
                    type = change_type(sep1.type);
                else
                    type = sep1.type;
                color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_V1_,
                                      type, point);
            }
            break;
        case CHART_V1:
            MATHFUNC(V1_to_sphere)(point[0], point[1], pcoord);
            prepok = prepareVfForIntegration(pcoord);
            if (!prepok)
                break;
            if (!gVFResults.plweights_)
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_V1_,
                                  sep1.type, point);
            break;
        case CHART_U2:
            if (point[1] >= 0 || !gVFResults.vf_[gVFResults.K_].singinf_) {
                MATHFUNC(U2_to_sphere)(point[0], point[1], pcoord);
                prepok = prepareVfForIntegration(pcoord);
                if (!prepok)
                    break;
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (gVFResults.vf_[gVFResults.K_].dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = sep1.type;
                color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_U2_,
                                      type, point);
            } else {
                VV2_to_psphere(point[0], point[1], pcoord);
                prepok = prepareVfForIntegration(pcoord);
                if (!prepok)
                    break;
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (gVFResults.vf_[gVFResults.K_].dir_vec_field_ == 1)
                        dir *= -1;
                }
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
                if (gVFResults.vf_[gVFResults.K_].dir_vec_field_ == 1)
                    type = change_type(sep1.type);
                else
                    type = sep1.type;
                color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_V2_,
                                      type, point);
            }
            break;
        case CHART_V2:
            MATHFUNC(V2_to_sphere)(point[0], point[1], pcoord);
            prepok = prepareVfForIntegration(pcoord);
            if (!prepok)
                break;
            if (!gVFResults.plweights_)
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], point);
            color = findSepColor2(gVFResults.vf_[gVFResults.K_].gcf_V2_,
                                  sep1.type, point);
            break;
        default:
            prepok = false;
            break;
        }

        copy_x_into_y(pcoord, new_orbit.pcoord);
        new_orbit.color = color;
        new_orbit.dashes = dashes && gVFResults.config_dashes_;
        new_orbit.dir = dir;
        new_orbit.type = type;
        if (new_orbit.dashes)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);

        orbit_result.push_back(std::move(new_orbit));

        if (!prepok)
            break;
    }

    auto lastpoints = integrate_sep(spherewnd, pcoord, gVFResults.config_step_,
                                    orbit_result.back().dir, type,
                                    gVFResults.config_intpoints_);
    if (!lastpoints.empty())
        orbit_result.insert(std::end(orbit_result), std::begin(lastpoints),
                            std::end(lastpoints));

    return (orbit_result);
}

// ---------------------------------------------------------------------------
//          plot_all_sep
// ---------------------------------------------------------------------------
// Plots all separatrices.  If the separatrix plotting has not started yet,
// it will be started; otherwhise it will be continued.
void plot_all_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    if (!gVFResults.vf_.empty()) {
        for (int i = 0; i < gThisVF.numVF_; i++) {
            plot_all_saddle_sep(spherewnd, i, gVFResults.vf_[i]->saddlePoints_);
            plot_all_se_sep(spherewnd, i, gVFResults.vf_[i]->sePoints_);
            plot_all_de_sep(spherewnd, i, gVFResults.vf_[i]->dePoints_);
        }
    }
}

// ---------------------------------------------------------------------------
//          draw_sep
// ---------------------------------------------------------------------------
// Does the plotting of a separatrix that was previously calculated.
// The separatrix is plotted in the color according to the type and stability.
void draw_sep(std::shared_ptr<P4WinSphere> spherewnd,
              const std::vector<p4orbits::orbits_points> &sep)
{
    double pcoord[3];

    if (!sep.empty()) {
        for (auto const &it : sep) {
            if (it.dashes)
                (*plot_l)(spherewnd, it.pcoord, pcoord, it.color);
            else
                (*plot_p)(spherewnd, it.pcoord, it.color);
            copy_x_into_y(it.pcoord, pcoord);
        }
    }
}

// ---------------------------------------------------------------------------
//          draw_selected_sep
// ---------------------------------------------------------------------------
// Does the plotting of a separatrix that was previously calculated.
// The separatrix is plotted in a specified color.
void draw_selected_sep(std::shared_ptr<P4WinSphere> spherewnd,
                       const std::vector<p4orbits::orbits_points> &sep,
                       int color)
{
    double pcoord[3];

    if (!sep.empty()) {
        for (auto const &it : sep) {
            if (it.dashes)
                (*plot_l)(spherewnd, it.pcoord, pcoord, color);
            else
                (*plot_p)(spherewnd, it.pcoord, color);
            copy_x_into_y(it.pcoord, pcoord);
        }
    }
}
