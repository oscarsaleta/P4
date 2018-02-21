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
 *  MERCHANTABILITY or FITNESS FOR sA PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "math_charts.h"

#include "custom.h"
#include "file_tab.h"
#include "main.h"
#include "math_numerics.h"
#include "math_p4.h"
#include "math_polynom.h"

#include <cmath>

// -----------------------------------------------------------------------
//                      FUNCTION PROTOTYPES
// -----------------------------------------------------------------------
/*

    There are, logically, 4 categories of FINITE coordinate systems:

    (I)     Real coordinates
    (II)    View coordinates
    (III)   Poincare coordinates
    (IV)    Poincare-Lyapunov coordinates

    Category (I) are coordinates used by Maple/reduce, and also the points
    given in dialog boxes by the user, are always (I)-coordinates.
    Category (II) are coordinates that are obtained by interacting with
    the plot-window or zoom-window.  When the user clicks, the click-coordinates
    are in Category (II). When a local study is wanted, these coordinates are
    exactly the same as (I).  Otherwhise, Category-(II) coordinates are
   coordinates
    inside a rectangular box x=-1.1 .. 1.1, y=-1.1 .. 1.1, wherein the circle
    at infinity is a circle with radius 1.

*/

// -----------------------------------------------------------------------
//                  IMPLEMENTATION OF THE POINCARE CHARTS
// -----------------------------------------------------------------------

void copy_x_into_y(double *x, double *y)
{
    y[0] = x[0];
    y[1] = x[1];
    y[2] = x[2];
}

double floatinfinity(void) { return pow(0, -1); }

void R2_to_psphere(double x, double y, double *pcoord)
{
    double s;

    s = sqrt(1.0 + x * x + y * y);

    pcoord[0] = x / s;
    pcoord[1] = y / s;
    pcoord[2] = 1.0 / s;
}

void psphere_to_R2(double X, double Y, double Z, double *rcoord)
{
    rcoord[0] = X / Z;
    rcoord[1] = Y / Z;
}

void psphere_ucircle(double X, double Y, double Z, double *ucoord)
{
    double k;

    if (gVFResults.config_projection_ == 0) {
        ucoord[0] = X;
        ucoord[1] = Y;
    } else {
        k = -Z / (Z - gVFResults.config_projection_);
        ucoord[0] = (1.0 + k) * X;
        ucoord[1] = (1.0 + k) * Y;
    }
}

void ucircle_psphere(double u, double v, double *pcoord)
{
    double k, projection;

    projection = gVFResults.config_projection_;

    if (projection == 0) {
        pcoord[0] = u;
        pcoord[1] = v;
        pcoord[2] = sqrt(1.0 - u * u - v * v);
    } else {
        k = (sqrt(-projection * projection * (u * u + v * v) +
                  projection * projection + u * u + v * v) -
             u * u - v * v) /
            (projection * projection + u * u + v * v);

        pcoord[0] = (1.0 + k) * u;
        pcoord[1] = (1.0 + k) * v;
        pcoord[2] = -k * projection;
    }
}

void finite_ucircle(double x, double y, double *ucoord)
{
    double pcoord[3];

    R2_to_psphere(x, y, pcoord);
    psphere_ucircle(pcoord[0], pcoord[1], pcoord[2], ucoord);
}

void psphere_to_U1(double X, double Y, double Z, double *rcoord)
{
    rcoord[0] = Y / X;
    rcoord[1] = Z / X;
}

void psphere_to_U2(double X, double Y, double Z, double *rcoord)
{
    rcoord[0] = X / Y;
    rcoord[1] = Z / Y;
}

void psphere_to_V1(double X, double Y, double Z, double *rcoord)
{
    rcoord[0] = -Y / X;
    rcoord[1] = -Z / X;
}

void psphere_to_V2(double X, double Y, double Z, double *rcoord)
{
    rcoord[0] = -X / Y;
    rcoord[1] = -Z / Y;
}

void U1_to_psphere(double z1, double z2, double *pcoord)
{
    pcoord[0] = 1 / (sqrt(1 + z1 * z1 + z2 * z2));
    pcoord[1] = z1 * pcoord[0];
    pcoord[2] = z2 * pcoord[0];
}

void V1_to_psphere(double z1, double z2, double *pcoord)
{
    pcoord[0] = -1 / (sqrt(1 + z1 * z1 + z2 * z2));
    pcoord[1] = -z1 * pcoord[0];
    pcoord[2] = -z2 * pcoord[0];
}

void U2_to_psphere(double z1, double z2, double *pcoord)
{
    pcoord[1] = 1 / (sqrt(1 + z1 * z1 + z2 * z2));
    pcoord[0] = z1 * pcoord[1];
    pcoord[2] = z2 * pcoord[1];
}

void V2_to_psphere(double z1, double z2, double *pcoord)
{
    pcoord[1] = -1 / (sqrt(1 + z1 * z1 + z2 * z2));
    pcoord[0] = -z1 * pcoord[1];
    pcoord[2] = -z2 * pcoord[1];
}

void VV1_to_psphere(double z1, double z2, double *pcoord)
{
    // (z1,z2) is a point on the U1 chart, but on the other side of the line at
    // infinity

    U1_to_psphere(z1, z2, pcoord);
    pcoord[0] = -pcoord[0];
    pcoord[1] = -pcoord[1];
    pcoord[2] = -pcoord[2];
}

void psphere_to_VV1(double X, double Y, double Z, double *rcoord)
{
    psphere_to_U1(-X, -Y, -Z, rcoord);
}

void VV2_to_psphere(double z1, double z2, double *pcoord)
{
    // (z1,z2) is a point on the U2 chart, but on the other side of the line at
    // infinity

    U2_to_psphere(z1, z2, pcoord);
    pcoord[0] = -pcoord[0];
    pcoord[1] = -pcoord[1];
    pcoord[2] = -pcoord[2];
}

void psphere_to_VV2(double X, double Y, double Z, double *rcoord)
{
    psphere_to_U2(-X, -Y, -Z, rcoord);
}

void UU1_to_psphere(double z1, double z2, double *pcoord)
{
    /* (z1,z2) is a point on the V1 chart */
    V1_to_psphere(z1, z2, pcoord);
    pcoord[0] = -pcoord[0];
    pcoord[1] = -pcoord[1];
    pcoord[2] = -pcoord[2];
}

void psphere_to_UU1(double X, double Y, double Z, double *rcoord)
{
    psphere_to_V1(-X, -Y, -Z, rcoord);
}

void UU2_to_psphere(double z1, double z2, double *pcoord)
{
    /* (z1,z2) is a point on the V2 chart */
    V2_to_psphere(z1, z2, pcoord);
    pcoord[0] = -pcoord[0];
    pcoord[1] = -pcoord[1];
    pcoord[2] = -pcoord[2];
}

void psphere_to_UU2(double X, double Y, double Z, double *rcoord)
{
    psphere_to_V2(-X, -Y, -Z, rcoord);
}

bool isvalid_psphereviewcoord(double u, double v, double *pcoord)
{
    if (u * u + v * v > 1.0)
        return false;

    MATHFUNC(viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

bool isvalid_U1viewcoord(double u, double v, double *pcoord)
{
    // the u coordinate must be positive, except when p is odd.

    if (gVFResults.plweights_) {
        if ((gVFResults.p_ % 2) == 0 && u < 0)
            return false;
    }

    MATHFUNC(viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

bool isvalid_U2viewcoord(double u, double v, double *pcoord)
{
    if (gVFResults.plweights_) {
        if ((gVFResults.q_ % 2) == 0 && v < 0)
            return false;
    }

    MATHFUNC(viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

bool isvalid_V1viewcoord(double u, double v, double *pcoord)
{
    if (gVFResults.plweights_) {
        if ((gVFResults.p_ % 2) == 0 && u < 0)
            return false;
    }

    MATHFUNC(viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

bool isvalid_V2viewcoord(double u, double v, double *pcoord)
{
    if (gVFResults.plweights_) {
        if ((gVFResults.q_ % 2) == 0 && v < 0)
            return false;
    }

    MATHFUNC(viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

bool isvalid_R2viewcoord(double u, double v, double *pcoord)
{
    MATHFUNC(viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

void psphere_to_xyrevU1(double X, double Y, double Z, double *rcoord)
{
    double _rcoord[2];
    psphere_to_U1(X, Y, Z, _rcoord);
    rcoord[0] = _rcoord[1];
    rcoord[1] = _rcoord[0];
}

void psphere_to_xyrevV1(double X, double Y, double Z, double *rcoord)
{
    double _rcoord[2];
    psphere_to_V1(X, Y, Z, _rcoord);
    rcoord[0] = _rcoord[1];
    rcoord[1] = _rcoord[0];
}

void xyrevU1_to_psphere(double z1, double z2, double *pcoord)
{
    U1_to_psphere(z2, z1, pcoord);
}

void xyrevV1_to_psphere(double z1, double z2, double *pcoord)
{
    V1_to_psphere(z2, z1, pcoord);
}

// -----------------------------------------------------------------------
//              IMPLEMENTATION OF THE POINCARE-LYPANOV CHARTS
// -----------------------------------------------------------------------

/* function for transformation U1,V1 chart to the cylinder
   on chart x=(+-)1/s^p, y=u/s^q
   on cylinder x=cos(theta)/r^p, y=sin(theta)/r^q
   (u,s) -> (r,theta)
   find positive root of  y^p+u^2*y^q-1
   r=sqrt(y)*s and theta=atan(u*sqrt(y)^(q-p)) if s<>0 !!
   if s=0 and u<>0 then solve (+-1)^q*u^p*cos(theta)^q-sin(theta)^p
                        and r=0
   if u=0 then r=s and theta=0 or Pi
*/

bool isvalid_plsphereviewcoord(double u, double v, double *pcoord)
{
    if (u * u + v * v > 1.0)
        return false;
    MATHFUNC(viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

// static void cylinder_to_U1( double r, double theta, double * c);
// static void cylinder_to_U2( double r, double theta, double * c );
// static void cylinder_to_V1( double r, double theta, double * c);
// static void cylinder_to_V2( double r, double theta, double * c);
static void U1_to_cylinder(double u, double s, double *c);
static void U2_to_cylinder(double u, double s, double *c);
static void V1_to_cylinder(double u, double s, double *c);
static void V2_to_cylinder(double u, double s, double *c);

static double sU{0.0};

static double func_U1(double x)
{
    return pow(x, gVFResults.double_p_) +
           sU * sU * pow(x, gVFResults.double_q_) - 1.0;
}

static double dfunc_U1(double x)
{
    return gVFResults.double_p_ * pow(x, gVFResults.double_p_minus_1_) +
           sU * sU * gVFResults.double_q_ *
               pow(x, gVFResults.double_q_minus_1_);
}

static double func_U1_s0(double theta)
{
    /* find theta if s=0 and u<>0 */
    return sU * pow(cos(theta), gVFResults.double_q_) -
           pow(sin(theta), gVFResults.double_p_);
}

static double dfunc_U1_s0(double theta)
{
    return (-gVFResults.double_q_ * sU *
                pow(cos(theta), gVFResults.double_q_minus_1_) * sin(theta) -
            gVFResults.double_p_ * cos(theta) *
                pow(sin(theta), gVFResults.double_p_minus_1_));
}

static void U1_to_cylinder(double u, double s, double *c)
{
    /* input (u,s) output c=(r,theta)
        x=1/s^p, y=u/s^q
    */
    double x[2], y;
    if (u == 0) {
        c[0] = s;
        c[1] = 0;
    } else if (s == 0) {
        c[0] = 0;
        sU = pow(u, gVFResults.double_p_);
        if (u > 0) {
            x[0] = 0;
            x[1] = PI / 2.0;
        } else {
            x[0] = -PI / 2.0;
            x[1] = 0;
        }
        c[1] = find_root(func_U1_s0, dfunc_U1_s0, x);
    } else {
        x[0] = 0;
        x[1] = 1;
        sU = u;
        y = find_root(func_U1, dfunc_U1, x);
        c[0] = sqrt(y) * s;
        c[1] = atan(u * pow(sqrt(y), gVFResults.double_q_minus_p_));
    }
}

/*
static void cylinder_to_U1( double r, double theta, double * c)
{
    c[1]=r/pow(cos(theta),__one_over_p);
    c[0]=sin(theta)*pow(c[1]/r,gVFResults.double_q_);
}
*/

static void V1_to_cylinder(double u, double s, double *c)
{
    /* input (u,s) output c=(r,theta)
        x=-1/s^p, y=u/s^q
    */
    double x[2], y;
    if (u == 0) {
        c[0] = s;
        c[1] = PI;
    } else if (s == 0) {
        c[0] = 0;
        sU = pow(u, gVFResults.double_p_) * __minus_one_to_q;
        if (u > 0) {
            x[0] = PI / 2;
            x[1] = PI;
        } else {
            x[0] = -PI;
            x[1] = -PI / 2;
        }
        c[1] = find_root(func_U1_s0, dfunc_U1_s0, x);
    } else {
        x[0] = 0;
        x[1] = 1;
        sU = u;
        y = find_root(func_U1, dfunc_U1, x);
        c[0] = sqrt(y) * s;
        c[1] = atan(-u * pow(sqrt(y), gVFResults.double_q_minus_p_));
        if (c[1] > 0)
            c[1] -= PI;
        else
            c[1] += PI;
    }
}

/*
static void cylinder_to_V1( double r, double theta, double * c)
{
    c[1]=r/pow(-cos(theta),__one_over_p);
    c[0]=sin(theta)*pow(c[1]/r,gVFResults.double_q_);
}
*/

/* function for transformation U2,V2 chart to the cylinder
   on chart x=u/s^p, y=(+-)1/s^q
   on cylinder x=cos(theta)/r^p, y=sin(theta)/r^q
   (u,s) -> (r,theta)
   find positive root of  u^2*y^p+y^q-1
   r=sqrt(y)*s and theta=atan(sqrt(y)^(q-p)/u) if s<>0
   if s=0 then solve u^q*sin(theta)^p-cos(theta)^q
*/

static double func_U2(double x)
{
    return (sU * sU * pow(x, gVFResults.double_p_) +
            pow(x, gVFResults.double_q_) - 1.0);
}

static double dfunc_U2(double x)
{
    return (gVFResults.double_p_ * sU * sU *
                pow(x, gVFResults.double_p_minus_1_) +
            gVFResults.double_q_ * pow(x, gVFResults.double_q_minus_1_));
}

static double func_U2_s0(double theta)
{
    return (sU * pow(sin(theta), gVFResults.double_p_) -
            pow(cos(theta), gVFResults.double_q_));
}

static double dfunc_U2_s0(double theta)
{
    return (gVFResults.double_p_ * sU * cos(theta) *
                pow(sin(theta), gVFResults.double_p_minus_1_) +
            gVFResults.double_q_ * sin(theta) *
                pow(cos(theta), gVFResults.double_q_minus_1_));
}

static void U2_to_cylinder(double u, double s, double *c)
/* input (u,s) output c=(r,theta)
    x=u/s^p, y=1/s^q
*/
{
    double x[2], y;
    if (u == 0) {
        c[0] = s;
        c[1] = PI / 2;
    } else if (s == 0) {
        c[0] = 0;
        sU = pow(u, gVFResults.double_q_);
        if (u > 0) {
            x[0] = 0;
            x[1] = PI / 2.0;
        } else {
            x[0] = PI / 2.0;
            x[1] = PI;
        }
        c[1] = find_root(func_U2_s0, dfunc_U2_s0, x);
    } else {
        x[0] = 0;
        x[1] = 1;
        sU = u;
        y = find_root(func_U2, dfunc_U2, x);
        c[0] = sqrt(y) * s;
        c[1] = atan(pow(sqrt(y), gVFResults.double_q_minus_p_) / u);
        if (c[1] < 0)
            c[1] += PI;
    }
}

/*
static void cylinder_to_U2( double r, double theta, double * c )
{
    c[1] = r/pow(sin(theta),__one_over_q);
    c[0] = cos(theta)*pow(c[1]/r,gVFResults.double_p_);
}
*/

static void V2_to_cylinder(double u, double s, double *c)
/* input (u,s) output c=(r,theta)
    x=u/s^p, y=-1/s^q
*/
{
    double x[2], y;

    if (u == 0) {
        c[0] = s;
        c[1] = -PI / 2;
    } else {
        if (s == 0) {
            c[0] = 0;
            sU = pow(u, gVFResults.double_q_) * __minus_one_to_p;
            if (u > 0) {
                x[0] = -PI / 2;
                x[1] = 0;
            } else {
                x[0] = -PI;
                x[1] = -PI / 2;
            }
            c[1] = find_root(func_U2_s0, dfunc_U2_s0, x);
        } else {
            x[0] = 0;
            x[1] = 1;
            sU = u;
            y = find_root(func_U2, dfunc_U2, x);
            c[0] = sqrt(y) * s;
            c[1] = atan(-pow(sqrt(y), gVFResults.double_q_minus_p_) / u);
            if (c[1] > 0)
                c[1] -= PI;
        }
    }
}

/*
static void cylinder_to_V2( double r, double theta, double * c)
{
    c[1] = r / pow( -sin(theta), __one_over_q );
    c[0] = cos(theta) * pow( c[1]/r, gVFResults.double_p_ );
}
*/

// -----------------------------------------------------------------------
//                          ANNULUS_PLSPHERE
// -----------------------------------------------------------------------
//
// Change of coordinates from plot-coordinates to Poincare-Lyapunov coordinates
//
// In the plot window, coordinates are mapped into a rectangular box x: -1..1,
// y: -1..1.
// Inside this box, the circle at infinity is a circle with radius 1.0.
//
// The change of coordinates to PL-coordinates is given by this routine:

void annulus_plsphere(double x, double y, double *p)
{
    if (x * x + y * y < RADIUS2) {
        p[0] = 0;  // mark it as a finite point
        p[1] = x / RADIUS;
        p[2] = y / RADIUS;
    } else {
        p[0] = 1;  // mark it as a point near infinity
        p[1] = (1.0 - x * x - y * y) / (1.0 - RADIUS2);
        p[2] = atan2(y, x);
    }
}

// -----------------------------------------------------------------------
//                          PLSPHERE_ANNULUS
// -----------------------------------------------------------------------
//
// Inverse of annulus_plsphere.
//
// projection cylinder on annulus
//  first we project the cylinder the cone x^2+y^2=1-(1-RADIUS2)*r

void plsphere_annulus(double x, double y, double z, double *u)
{
    double zz;

    if (x == 0) {
        u[0] = y * RADIUS;
        u[1] = z * RADIUS;
    } else {
        zz = sqrt(1.0 - (1.0 - RADIUS2) * y);
        u[0] = zz * cos(z);
        u[1] = zz * sin(z);
    }
}

// -----------------------------------------------------------------------
//                          R2_TO_PLSPHERE
// -----------------------------------------------------------------------
//
// Change of real coordinates to Poincare-Lyapunov sphere.
//
// PL-sphere coordinates correspond to R^2-coordinates inside a ball
// with radius 1 around the origin.
//
// Therefore, the first plsphere coordinate is either 0.0 or 1.0, indicating
// which chart the point belongs to:
//
//
//      (x,y)  --> (0,x,y)              if (x,y) in ball with radius 1
//      (x,y)  --> (1,u,v)              outside ball
//
//
//  We calculate (u,v) through the inverse of the prescription (x,y) =
//  (cos(v)/u^p, sin(v)/u^q):
//
//      (cos(v),sin(v)) = ( x*u^p, y*u^q )
//
//  Hence, u is a root of "x^2 u^(2p) + y^2 u^(2q) - 1 = 0".
//  We calculate a root using numeric analysis.
//
//  Once we have calculated u, we determine v using atan2.

static double sA{0.0};
static double sB{0.0};

static double func(double z)
{
    return pow(z, gVFResults.double_p_) * sA +
           pow(z, gVFResults.double_q_) * sB - 1.0;
}

static double dfunc(double z)
{
    return gVFResults.double_p_ * pow(z, gVFResults.double_p_minus_1_) * sA +
           gVFResults.double_q_ * pow(z, gVFResults.double_q_minus_1_) * sB;
}

void R2_to_plsphere(double x, double y, double *pcoord)
{
    double z[2];

    if ((x * x + y * y) <= 1.0) {
        pcoord[0] = 0.0;
        pcoord[1] = x;
        pcoord[2] = y;
    } else {
        pcoord[0] = 1.0;
        sA = x * x;
        sB = y * y;
        z[0] = 0.0;
        z[1] = 1.0;

        pcoord[1] = find_root(func, dfunc, z);
        pcoord[1] = sqrt(pcoord[1]);
        pcoord[2] = atan2(pow(pcoord[1], gVFResults.double_q_) * y,
                          pow(pcoord[1], gVFResults.double_p_) * x);
    }
}

// -----------------------------------------------------------------------
//                          plsphere_to_R2
// -----------------------------------------------------------------------
//
// Change of coordinates from Poincare-Lyapunov sphere to real coordinates
//
// PL-sphere coordinates correspond to R^2-coordinates inside a ball
// with radius 1 around the origin.
//
// Therefore, the first plsphere coordinate is either 0.0 or 1.0, indicating
// which chart the point belongs to:
//
//
//      (0,u,v)  --> (x,y) = (u,v)
//      (1,u,v)  --> (x,y) = (cos(v)/u^p, sin(v)/u^q)
//

void plsphere_to_R2(double ch, double u, double v, double *c)
{
    if (ch) {
        c[0] = cos(v) / pow(u, gVFResults.double_p_);
        c[1] = sin(v) / pow(u, gVFResults.double_q_);
    } else {
        c[0] = u;
        c[1] = v;
    }
}

// -----------------------------------------------------------------------
//                          FINITE_ANNULUS
// -----------------------------------------------------------------------
//
// Change "Real" coordinates to "Plot" coordinates.
//
// It is simply by combining Symbolic-->PL, PL-->Plot

void finite_annulus(double x, double y, double *u)
{
    double pcoord[3];

    R2_to_plsphere(x, y, pcoord);
    plsphere_annulus(pcoord[0], pcoord[1], pcoord[2], u);
}

void cylinder_to_plsphere(double r, double theta, double *pcoord)
{
    if (r < 1.0) {
        pcoord[0] = 1;
        pcoord[1] = r;
        pcoord[2] = theta;
    } else {
        pcoord[0] = 0;
        pcoord[1] = cos(theta) / pow(r, gVFResults.double_p_);
        pcoord[2] = sin(theta) / pow(r, gVFResults.double_q_);
    }
}

void U1_to_plsphere(double x0, double y0, double *pcoord)
{
    double c[2];
    U1_to_cylinder(x0, y0, c);
    cylinder_to_plsphere(c[0], c[1], pcoord);
}

void xyrevU1_to_plsphere(double z1, double z2, double *pcoord)
{
    U1_to_plsphere(z2, z1, pcoord);
}

void V1_to_plsphere(double x0, double y0, double *pcoord)
{
    double c[2];
    V1_to_cylinder(x0, y0, c);
    cylinder_to_plsphere(c[0], c[1], pcoord);
}

void xyrevV1_to_plsphere(double z1, double z2, double *pcoord)
{
    V1_to_plsphere(z2, z1, pcoord);
}

void U2_to_plsphere(double x0, double y0, double *pcoord)
{
    double c[2];
    U2_to_cylinder(x0, y0, c);
    cylinder_to_plsphere(c[0], c[1], pcoord);
}

void V2_to_plsphere(double x0, double y0, double *pcoord)
{
    double c[2];
    V2_to_cylinder(x0, y0, c);
    cylinder_to_plsphere(c[0], c[1], pcoord);
}

void identitytrf_R2(double x, double y, double *ucoord)
{
    ucoord[0] = x;
    ucoord[1] = y;
}

void plsphere_to_U1(double ch, double x, double y, double *rcoord)
{
    double a;

    if (ch == 1) {
        a = cos(y);

        if (a < 0) {
            if ((gVFResults.p_ % 2) == 0) {
                // p is even: so we have a problem
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-a, -__one_over_p);  // - (-cos(y))^(-1/p)
        } else
            a = pow(a, -__one_over_p);  // cos(y)^(-1/p)

        rcoord[0] =
            sin(y) * pow(a, gVFResults.double_q_);  // sin(y) * cos(y)^(-q/p)
        rcoord[1] = x * a;                          // x * cos(y)^(-1/p)
    } else {
        if (x < 0) {
            if ((gVFResults.p_ % 2) == 0) {
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-x, -__one_over_p);
        } else
            a = pow(x, -__one_over_p);  // x^(-1/p)

        rcoord[0] = y * pow(a, gVFResults.double_q_);
        rcoord[1] = a;
    }
}

void plsphere_to_xyrevU1(double ch, double x, double y, double *rcoord)
{
    double _rcoord[2];
    plsphere_to_U1(ch, x, y, _rcoord);
    rcoord[0] = _rcoord[1];
    rcoord[1] = _rcoord[0];
}

void plsphere_to_U2(double ch, double x, double y, double *rcoord)
{
    double a;

    if (ch == 1) {
        a = sin(y);

        if (a < 0) {
            if ((gVFResults.q_ % 2) == 0) {
                // p is even: so we have a problem
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-a, -__one_over_q);  // - (-sin(y))^(-1/q)
        } else
            a = pow(a, -__one_over_q);  // sin(y)^(-1/q)

        rcoord[0] =
            cos(y) * pow(a, gVFResults.double_p_);  // cos(y) * sin(y)^(-p/q)
        rcoord[1] = x * a;                          // x * sin(y)^(-1/q)
    } else {
        if (y < 0) {
            if ((gVFResults.q_ % 2) == 0) {
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-y, -__one_over_q);
        } else
            a = pow(y, -__one_over_q);  // x^(-1/p)

        rcoord[0] = x * pow(a, gVFResults.double_p_);
        rcoord[1] = a;
    }
}

void plsphere_to_V1(double ch, double x, double y, double *rcoord)
{
    double a;

    if (ch == 1) {
        a = -cos(y);

        if (a < 0) {
            if ((gVFResults.p_ % 2) == 0) {
                // p is even: so we have a problem
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-a, -__one_over_p);  // - (-cos(y))^(-1/p)
        } else
            a = pow(a, -__one_over_p);  // cos(y)^(-1/p)

        rcoord[0] =
            sin(y) * pow(a, gVFResults.double_q_);  // sin(y) * cos(y)^(-q/p)
        rcoord[1] = x * a;                          // x * cos(y)^(-1/p)
    } else {
        if (x > 0) {
            if ((gVFResults.p_ % 2) == 0) {
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(x, -__one_over_p);
        } else
            a = pow(-x, -__one_over_p);  // x^(-1/p)

        rcoord[0] = y * pow(a, gVFResults.double_q_);
        rcoord[1] = a;
    }
}

void plsphere_to_xyrevV1(double ch, double x, double y, double *rcoord)
{
    double _rcoord[2];
    plsphere_to_V1(ch, x, y, _rcoord);
    rcoord[0] = _rcoord[1];
    rcoord[1] = _rcoord[0];
}

void plsphere_to_V2(double ch, double x, double y, double *rcoord)
{
    double a;

    if (ch == 1) {
        a = -sin(y);

        if (a < 0) {
            if ((gVFResults.q_ % 2) == 0) {
                // p is even: so we have a problem
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-a, -__one_over_q);  // - (-sin(y))^(-1/q)
        } else
            a = pow(a, -__one_over_q);  // sin(y)^(-1/q)

        rcoord[0] =
            cos(y) * pow(a, gVFResults.double_p_);  // cos(y) * sin(y)^(-p/q)
        rcoord[1] = x * a;                          // x * sin(y)^(-1/q)
    } else {
        if (y > 0) {
            if ((gVFResults.q_ % 2) == 0) {
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(y, -__one_over_q);
        } else
            a = pow(-y, -__one_over_q);  // x^(-1/p)

        rcoord[0] = x * pow(a, gVFResults.double_p_);
        rcoord[1] = a;
    }
}

// -----------------------------------------------------------------------
//                      EVALUATION OF VECTOR FIELDS
// -----------------------------------------------------------------------

void eval_r_vec_field(double *y, double *f)
{
    double s = 1.0;

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL &&
        gVFResults.vf_->back().gcf_ != nullptr)
        s = eval_term2(gVFResults.vf_->back().gcf_, y);

    f[0] = s * eval_term2(gVFResults.vf_->back().f_vec_field_[0], y);
    f[1] = s * eval_term2(gVFResults.vf_->back().f_vec_field_[1], y);
}

void eval_U1_vec_field(double *y, double *f)
{
    double s = 1.0;

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL &&
        gVFResults.vf_->back().gcf_U1_ != nullptr)
        s = eval_term2(gVFResults.vf_->back().gcf_U1_, y);

    if (gVFResults.vf_->back().config_kindvf_ == INTCONFIG_ORIGINAL &&
        gVFResults.vf_->back().singinf_)
        s *= y[1];

    f[0] = s * eval_term2(gVFResults.vf_->back().vec_field_U1_[0], y);
    f[1] = s * eval_term2(gVFResults.vf_->back().vec_field_U1_[1], y);
}

void eval_U2_vec_field(double *y, double *f)
{
    double s = 1.0;

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL &&
        gVFResults.vf_->back().gcf_U2_ != nullptr)
        s = eval_term2(gVFResults.vf_->back().gcf_U2_, y);

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL &&
        gVFResults.vf_->back().singinf_)
        s *= y[1];

    f[0] = s * eval_term2(gVFResults.vf_->back().vec_field_U2_[0], y);
    f[1] = s * eval_term2(gVFResults.vf_->back().vec_field_U2_[1], y);
}

void eval_V1_vec_field(double *y, double *f)
{
    double s = 1.0;

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL &&
        gVFResults.vf_->back().gcf_V1_ != nullptr)
        s = eval_term2(gVFResults.vf_->back().gcf_V1_, y);

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL &&
        gVFResults.vf_->back().singinf_)
        s *= y[1];

    f[0] = s * eval_term2(gVFResults.vf_->back().vec_field_V1_[0], y);
    f[1] = s * eval_term2(gVFResults.vf_->back().vec_field_V1_[1], y);
}

void eval_V2_vec_field(double *y, double *f)
{
    double s = 1.0;

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL &&
        gVFResults.vf_->back().gcf_V2_ != nullptr)
        s = eval_term2(gVFResults.vf_->back().gcf_V2_, y);

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL &&
        gVFResults.vf_->back().singinf_)
        s *= y[1];

    f[0] = s * eval_term2(gVFResults.vf_->back().vec_field_V2_[0], y);
    f[1] = s * eval_term2(gVFResults.vf_->back().vec_field_V2_[1], y);
}

void eval_vec_field_cyl(double *y, double *f)
{
    double s = 1.0;

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL &&
        gVFResults.vf_->back().gcf_C_ != nullptr)
        s = eval_term3(gVFResults.vf_->back().gcf_C_, y);

    f[0] = s * eval_term3(gVFResults.vf_->back().vec_field_C_[0], y);
    f[1] = s * eval_term3(gVFResults.vf_->back().vec_field_C_[1], y);
}

void default_finite_to_viewcoord(double x, double y, double *ucoord)
{
    double pcoord[3];

    MATHFUNC(R2_to_sphere)(x, y, pcoord);
    MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], ucoord);
}

bool default_sphere_to_viewcoordpair(double *p, double *q, double *u1,
                                     double *u2, double *u3, double *u4)
{
    /*
        when plotting a line, one checks plot coordinates for both endpoints.
        Normally, this is just it.

        However, when the plot canvas has a discontinuity (for example when
       plotting
        two charts at infinity at the same time), this routine should be
       bypassed,
        so that intermediate points ensure that the line plotting is done
       correctly.
    */
    MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
    MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u2);
    return true;  // returns true: line piece does not undergo a discontinuity
                  // in coordinates
}

bool psphere_to_viewcoordpair_discontinuousx(double *p, double *q, double *u1,
                                             double *u2, double *u3, double *u4)
{
    /*
        when plotting a line, one checks plot coordinates for both endpoints.
        Normally, this is just it.

        However, when the plot canvas has a discontinuity (for example when
       plotting
        two charts at infinity at the same time), this routine should be
       bypassed,
        so that intermediate points ensure that the line plotting is done
       correctly.
    */

    if (p[0] * q[0] < 0) {
        double t, y, z;
        t = q[0] / (q[0] - p[0]);
        y = t * p[1] + (1 - t) * q[1];
        z = t * p[2] + (1 - t) * q[2];

        MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        if (p[0] < 0) {
            MATHFUNC(sphere_to_viewcoord)(-1e-8, y, z, u2);
            MATHFUNC(sphere_to_viewcoord)(1e-8, y, z, u3);
        } else {
            MATHFUNC(sphere_to_viewcoord)(1e-8, y, z, u2);
            MATHFUNC(sphere_to_viewcoord)(-1e-8, y, z, u3);
        }
        MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u4);
        return false;
    }

    MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
    MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u2);

    return true;  // returns true: line piece does not undergo a discontinuity
                  // in coordinates
}

bool psphere_to_viewcoordpair_discontinuousy(double *p, double *q, double *u1,
                                             double *u2, double *u3, double *u4)
{
    /*
        when plotting a line, one checks plot coordinates for both endpoints.
        Normally, this is just it.

        However, when the plot canvas has a discontinuity (for example when
       plotting
        two charts at infinity at the same time), this routine should be
       bypassed,
        so that intermediate points ensure that the line plotting is done
       correctly.
    */

    if (p[1] * q[1] < 0) {
        double t, x, z;
        t = q[1] / (q[1] - p[1]);
        x = t * p[0] + (1 - t) * q[0];
        z = t * p[2] + (1 - t) * q[2];

        MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        if (p[1] < 0) {
            MATHFUNC(sphere_to_viewcoord)(x, -1e-8, z, u2);
            MATHFUNC(sphere_to_viewcoord)(x, 1e-8, z, u3);
        } else {
            MATHFUNC(sphere_to_viewcoord)(x, 1e-8, z, u2);
            MATHFUNC(sphere_to_viewcoord)(x, -1e-8, z, u3);
        }
        MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u4);
        return false;
    }

    MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
    MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u2);

    return true;  // returns true: line piece does not undergo a discontinuity
                  // in coordinates
}

bool plsphere_to_viewcoordpair_discontinuousx(double *p, double *q, double *u1,
                                              double *u2, double *u3,
                                              double *u4)
{
    double c[2], qx, qy, px, py;
    double pp[3], qq[3];
    double ap[2], aq[2];
    double t, y;

    if (p[0] == 0 || q[0] == 0) {
        plsphere_to_R2(p[0], p[1], p[2], c);
        px = c[0];
        py = c[1];
        plsphere_to_R2(q[0], q[1], q[2], c);
        qx = c[0];
        qy = c[1];

        if (qx * px < 0) {
            // the line y=0 has been crossed
            t = px / (px - qx);
            y = t * qy + (1 - t) * py;  // (x,0) is an intermediate point.

            MATHFUNC(sphere_to_viewcoord)(0, px, py, u1);
            if (px < 0) {
                MATHFUNC(sphere_to_viewcoord)(0, -1e-8, y, u2);
                MATHFUNC(sphere_to_viewcoord)(0, 1e-8, y, u3);
            } else {
                MATHFUNC(sphere_to_viewcoord)(0, 1e-8, y, u2);
                MATHFUNC(sphere_to_viewcoord)(0, -1e-8, y, u3);
            }
            MATHFUNC(sphere_to_viewcoord)(0, qx, qy, u4);
            return false;
        }
        MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u2);
        return true;
    }

    // both p and q lie in the annulus.

    if (cos(p[2]) * cos(q[2]) < 0) {
        plsphere_annulus(p[0], p[1], p[2], ap);
        plsphere_annulus(p[0], p[1], p[2], aq);

        t = ap[0] / (ap[0] - aq[0]);
        y = t * aq[1] + (1 - t) * ap[1];

        if (cos(p[2]) < 0) {
            annulus_plsphere(-1e-8, y, pp);
            annulus_plsphere(1e-8, y, qq);
        } else {
            annulus_plsphere(1e-8, y, pp);
            annulus_plsphere(-1e-8, y, qq);
        }
        MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        MATHFUNC(sphere_to_viewcoord)(pp[0], pp[1], pp[2], u2);
        MATHFUNC(sphere_to_viewcoord)(qq[0], qq[1], qq[2], u3);
        MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u4);

        return false;
    }
    MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
    MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u2);
    return true;
}

bool plsphere_to_viewcoordpair_discontinuousy(double *p, double *q, double *u1,
                                              double *u2, double *u3,
                                              double *u4)
{
    double c[2], qx, qy, px, py;
    double pp[3], qq[3];
    double ap[2], aq[2];
    double t, x;

    if (p[0] == 0 || q[0] == 0) {
        plsphere_to_R2(p[0], p[1], p[2], c);
        px = c[0];
        py = c[1];
        plsphere_to_R2(q[0], q[1], q[2], c);
        qx = c[0];
        qy = c[1];

        if (qy * py < 0) {
            // the line y=0 has been crossed
            t = py / (py - qy);
            x = t * qx + (1 - t) * px;  // (x,0) is an intermediate point.

            MATHFUNC(sphere_to_viewcoord)(0, px, py, u1);
            if (py < 0) {
                MATHFUNC(sphere_to_viewcoord)(0, x, -1e-8, u2);
                MATHFUNC(sphere_to_viewcoord)(0, x, 1e-8, u3);
            } else {
                MATHFUNC(sphere_to_viewcoord)(0, x, 1e-8, u2);
                MATHFUNC(sphere_to_viewcoord)(0, x, -1e-8, u3);
            }
            MATHFUNC(sphere_to_viewcoord)(0, qx, qy, u4);
            return false;
        }
        MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u2);
        return true;
    }

    // both p and q lie in the annulus.

    if (sin(p[2]) * sin(q[2]) < 0) {
        plsphere_annulus(p[0], p[1], p[2], ap);
        plsphere_annulus(p[0], p[1], p[2], aq);

        t = ap[1] / (ap[1] - aq[1]);
        x = t * aq[0] + (1 - t) * ap[0];

        if (sin(p[2]) < 0) {
            annulus_plsphere(x, -1e-8, pp);
            annulus_plsphere(x, 1e-8, qq);
        } else {
            annulus_plsphere(x, 1e-8, pp);
            annulus_plsphere(x, -1e-8, qq);
        }
        MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        MATHFUNC(sphere_to_viewcoord)(pp[0], pp[1], pp[2], u2);
        MATHFUNC(sphere_to_viewcoord)(qq[0], qq[1], qq[2], u3);
        MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u4);

        return false;
    }
    MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], u1);
    MATHFUNC(sphere_to_viewcoord)(q[0], q[1], q[2], u2);
    return true;
}
