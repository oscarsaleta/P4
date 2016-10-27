#ifndef MATH_P4_H
#define MATH_P4_H


#include "win_sphere.h"
#include "table.h"


// math_orbits.cpp:

void integrate_poincare_orbit( double p0, double p1, double p2, double * pcoord,
								double * hhi, int * dashes, int * dir,
								double h_min, double h_max );

void integrate_lyapunov_orbit( double p0, double p1, double p2, double * pcoord,
								double * hhi, int * dashes, int * dir,
								double h_min, double h_max );

// math_separatrice.cpp:

void integrate_poincare_sep( double p0, double p1, double p2, double * pcoord,
								double * hhi, int * type, int * color, int * dashes, int * dir,
								double h_min, double h_max);

void integrate_lyapunov_sep( double p0, double p1, double p2, double * pcoord,
								double * hhi, int * type, int * color, int * dashes, int * dir,
								double h_min, double h_max);

// math_p4.cpp:

double eval_lc_poincare(double * pp, double, double, double );
double eval_lc_lyapunov(double * pp, double, double, double );

bool less_poincare( double *, double * );
void eval_lc_lyapunov( void );
bool less_lyapunov( double *, double * );

// math_changedir.cpp:

int change_dir_poincare( double * p );
int change_dir_lyapunov( double * p );

// math_charts.cpp:

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

double eval_term1( P4POLYNOM1, double );
double eval_term2( P4POLYNOM2, double * );
double eval_term3( P4POLYNOM3, double * );


extern QVFStudy VFResults;	// (VFResults.p,VFResults.q) are lyapunov weights

#define PI 3.1415926535897932384626433832
#define PI_DIV2 (PI/2.)
#define PI_DIV4 (PI/4.)
#define TWOPI (2.0*PI)

void eval_r_vec_field( double * y, double * f );
void eval_U1_vec_field( double * y, double * f );
void eval_U2_vec_field( double * y, double * f );
void eval_V1_vec_field( double * y, double * f );
void eval_V2_vec_field( double * y, double * f );
void eval_vec_field_cyl( double * y, double * f );

extern void (*plot_l)( QWinSphere *, double *, double *, int );
extern void (*plot_p)( QWinSphere *, double *, int );

void rk78( void (*deriv)( double *, double * ), double y[2], double * hh,
			double hmi, double hma, double e1);
double find_root( double (*f)(double), double (*df)(double), double * value );


orbits_points  *integrate_blow_up(  QWinSphere *,
        double *, blow_up_points *, double, int, int, orbits_points **, int );

int change_type( int );
int FindSepColor( struct term2 *, int, double * );

void draw_sep( QWinSphere *, orbits_points * sep );
void draw_selected_sep( QWinSphere *, orbits_points * sep, int color );
void draw_gcf( QWinSphere *, orbits_points * sep, int color, int dashes );

void IntegrateOrbit( QWinSphere *, int );

#define MATHFUNC(function) (*(VFResults.function))

void set_current_step( double );

void plot_all_sep( QWinSphere * );


#endif /* MATH_P4_H */
