#ifndef MATH_CHARTS_H
#define MATH_CHARTS_H


void finite_annulus( double, double, double * );
void identitytrf_R2(double x,double y, double * ucoord );
void copy_x_into_y( double * x, double * y );
void psphere_to_R2( double X, double Y, double Z, double * rcoord );
void psphere_ucircle( double X, double Y, double Z, double * ucoord);
void ucircle_psphere( double u, double v, double * pcoord );
void finite_ucircle( double x, double y, double * ucoord);
void psphere_to_U1( double X, double Y, double Z, double * rcoord);
void psphere_to_U2( double X, double Y, double Z, double * rcoord);
void psphere_to_V1( double X, double Y, double Z, double * rcoord);
void psphere_to_V2( double X, double Y, double Z, double * rcoord);
void psphere_to_VV1( double X, double Y, double Z, double * rcoord);
void psphere_to_VV2( double X, double Y, double Z, double * rcoord);
void psphere_to_UU1( double X, double Y, double Z, double * rcoord);
void psphere_to_UU2( double X, double Y, double Z, double * rcoord);
void R2_to_psphere( double x, double y, double * pcoord );
void U1_to_psphere( double z1, double z2, double * pcoord);
void V1_to_psphere( double z1, double z2, double * pcoord);
void U2_to_psphere( double z1, double z2, double * pcoord);
void V2_to_psphere( double z1, double z2, double * pcoord);
void VV1_to_psphere( double z1, double z2, double * pcoord);
void VV2_to_psphere( double z1, double z2, double * pcoord);
void UU1_to_psphere( double z1, double z2, double * pcoord);
void UU2_to_psphere( double z1, double z2, double * pcoord);
void cylinder_to_plsphere( double r, double theta, double * pcoord);
void annulus_plsphere( double u, double v, double * p);
void plsphere_annulus( double x, double y, double z, double * u);
void plsphere_to_R2( double x, double y, double z, double * c);
void R2_to_plsphere( double x, double y, double * pcoord);
void U1_to_plsphere( double x0, double y0, double * pcoord);
void V1_to_plsphere( double x0, double y0, double * pcoord);
void U2_to_plsphere( double x0, double y0, double * pcoord);
void V2_to_plsphere( double x0, double y0, double * pcoord);
void plsphere_to_U1( double X, double Y, double Z, double * rcoord);
void plsphere_to_U2( double X, double Y, double Z, double * rcoord);
void plsphere_to_V1( double X, double Y, double Z, double * rcoord);
void plsphere_to_V2( double X, double Y, double Z, double * rcoord);

void eval_r_vec_field( double * y, double * f );
void eval_U1_vec_field( double * y, double * f );
void eval_U2_vec_field( double * y, double * f );
void eval_V1_vec_field( double * y, double * f );
void eval_V2_vec_field( double * y, double * f );
void eval_vec_field_cyl( double * y, double * f );


#endif // MATH_CHARTS_H
