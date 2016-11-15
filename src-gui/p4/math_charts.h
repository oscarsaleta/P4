/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
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

#ifndef MATH_CHARTS_H
#define MATH_CHARTS_H


// -----------------------------------------------------------------------
//                  IMPLEMENTATION OF THE POINCARE CHARTS
// -----------------------------------------------------------------------
void copy_x_into_y( double * x, double * y );
void R2_to_psphere( double x, double y, double * pcoord );
void psphere_to_R2( double X, double Y, double Z, double * rcoord );
void psphere_ucircle( double X, double Y, double Z, double * ucoord);
void ucircle_psphere( double u, double v, double * pcoord );
void finite_ucircle( double x, double y, double * ucoord);
void psphere_to_U1( double X, double Y, double Z, double * rcoord);
void psphere_to_U2( double X, double Y, double Z, double * rcoord);
void psphere_to_V1( double X, double Y, double Z, double * rcoord);
void psphere_to_V2( double X, double Y, double Z, double * rcoord);
void U1_to_psphere( double z1, double z2, double * pcoord);
void V1_to_psphere( double z1, double z2, double * pcoord);
void U2_to_psphere( double z1, double z2, double * pcoord);
void V2_to_psphere( double z1, double z2, double * pcoord);
void VV1_to_psphere( double z1, double z2, double * pcoord);
void psphere_to_VV1( double X, double Y, double Z, double * rcoord );
void VV2_to_psphere( double z1, double z2, double * pcoord);
void psphere_to_VV2( double X, double Y, double Z, double * rcoord );
void UU1_to_psphere( double z1, double z2, double * pcoord);
void psphere_to_UU1( double X, double Y, double Z, double * rcoord );
void UU2_to_psphere( double z1, double z2, double * pcoord);
void psphere_to_UU2( double X, double Y, double Z, double * rcoord );
bool isvalid_psphereviewcoord( double u, double v, double * pcoord );
bool isvalid_U1viewcoord( double u, double v, double * pcoord );
bool isvalid_U2viewcoord( double u, double v, double * pcoord );
bool isvalid_V1viewcoord( double u, double v, double * pcoord );
bool isvalid_V2viewcoord( double u, double v, double * pcoord );
bool isvalid_R2viewcoord( double u, double v, double * pcoord );
void psphere_to_xyrevU1( double X, double Y, double Z, double * rcoord);
void psphere_to_xyrevV1( double X, double Y, double Z, double * rcoord);
void xyrevU1_to_psphere( double z1, double z2, double * pcoord);
void xyrevV1_to_psphere( double z1, double z2, double * pcoord);
// -----------------------------------------------------------------------
//              IMPLEMENTATION OF THE POINCARE-LYPANOV CHARTS
// -----------------------------------------------------------------------
bool isvalid_plsphereviewcoord( double u, double v, double * pcoord );
// -----------------------------------------------------------------------
//                          ANNULUS_PLSPHERE
// -----------------------------------------------------------------------
void annulus_plsphere( double x, double y, double * p);
// -----------------------------------------------------------------------
//                          PLSPHERE_ANNULUS
// -----------------------------------------------------------------------
void plsphere_annulus( double x, double y, double z, double * u );
// -----------------------------------------------------------------------
//                          R2_TO_PLSPHERE
// -----------------------------------------------------------------------
void R2_to_plsphere( double x, double y, double * pcoord);
// -----------------------------------------------------------------------
//                          plsphere_to_R2
// -----------------------------------------------------------------------
void plsphere_to_R2( double ch, double u, double v, double * c);
// -----------------------------------------------------------------------
//                          FINITE_ANNULUS
// -----------------------------------------------------------------------
void finite_annulus( double x, double y, double * u );
void cylinder_to_plsphere( double r, double theta, double * pcoord);
void U1_to_plsphere( double x0, double y0, double * pcoord);
void xyrevU1_to_plsphere( double z1, double z2, double * pcoord);
void V1_to_plsphere( double x0, double y0, double * pcoord);
void xyrevV1_to_plsphere( double z1, double z2, double * pcoord);
void U2_to_plsphere( double x0, double y0, double * pcoord);
void V2_to_plsphere( double x0, double y0, double * pcoord);
void identitytrf_R2(double x,double y, double * ucoord );
double floatinfinity( void );
void plsphere_to_U1( double ch, double x, double y, double * rcoord );
void plsphere_to_xyrevU1( double ch, double x, double y, double * rcoord);
void plsphere_to_U2( double ch, double x, double y, double * rcoord );
void plsphere_to_V1( double ch, double x, double y, double * rcoord );
void plsphere_to_xyrevV1( double ch, double x, double y, double * rcoord);
void plsphere_to_V2( double ch, double x, double y, double * rcoord );
// -----------------------------------------------------------------------
//                      EVALUATION OF VECTOR FIELDS
// -----------------------------------------------------------------------
void eval_r_vec_field( double * y, double * f );
void eval_U1_vec_field( double * y, double * f );
void eval_U2_vec_field( double * y, double * f );
void eval_V1_vec_field( double * y, double * f );
void eval_V2_vec_field( double * y, double * f );
void eval_vec_field_cyl( double * y, double * f );
void default_finite_to_viewcoord( double x, double y, double * ucoord);
bool default_sphere_to_viewcoordpair( double * p, double * q,
                                      double * u1, double * u2, double * u3, double * u4 );
bool psphere_to_viewcoordpair_discontinuousx( double * p, double * q,
                                              double * u1, double * u2, double * u3, double * u4 );
bool psphere_to_viewcoordpair_discontinuousy( double * p, double * q,
                                              double * u1, double * u2, double * u3, double * u4 );
bool plsphere_to_viewcoordpair_discontinuousx( double * p, double * q,
                                               double * u1, double * u2, double * u3, double * u4 );
bool plsphere_to_viewcoordpair_discontinuousy( double * p, double * q,
                                               double * u1, double * u2, double * u3, double * u4 );


#endif // MATH_CHARTS_H
