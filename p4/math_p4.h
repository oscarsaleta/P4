// unknown:

class QWinSphere;

extern void integrate_poincare_orbit( double p0, double p1, double p2, double * pcoord,
								double * hhi, int * dashes, int * dir,
								double h_min, double h_max );

extern void integrate_lyapunov_orbit( double p0, double p1, double p2, double * pcoord,
								double * hhi, int * dashes, int * dir,
								double h_min, double h_max );

extern void integrate_poincare_sep( double p0, double p1, double p2, double * pcoord,
								double * hhi, int * type, int * color, int * dashes, int * dir,
								double h_min, double h_max);

extern void integrate_lyapunov_sep( double p0, double p1, double p2, double * pcoord,
								double * hhi, int * type, int * color, int * dashes, int * dir,
								double h_min, double h_max);

extern double eval_lc_poincare(double * pp, double, double, double );
extern double eval_lc_lyapunov(double * pp, double, double, double );

extern bool less_poincare( double *, double * );
extern void eval_lc_lyapunov( void );
extern bool less_lyapunov( double *, double * );
extern void finite_annulus( double, double, double * );

// math_changedir.cpp:

extern int change_dir_poincare( double * p );
extern int change_dir_lyapunov( double * p );

// math_charts.cpp:

extern void identitytrf_R2(double x,double y, double * ucoord );
extern void copy_x_into_y( double * x, double * y );
extern void psphere_to_R2( double X, double Y, double Z, double * rcoord );
extern void psphere_ucircle( double X, double Y, double Z, double * ucoord);
extern void ucircle_psphere( double u, double v, double * pcoord );
extern void finite_ucircle( double x, double y, double * ucoord);
extern void psphere_to_U1( double X, double Y, double Z, double * rcoord);
extern void psphere_to_U2( double X, double Y, double Z, double * rcoord);
extern void psphere_to_V1( double X, double Y, double Z, double * rcoord);
extern void psphere_to_V2( double X, double Y, double Z, double * rcoord);
extern void psphere_to_VV1( double X, double Y, double Z, double * rcoord);
extern void psphere_to_VV2( double X, double Y, double Z, double * rcoord);
extern void psphere_to_UU1( double X, double Y, double Z, double * rcoord);
extern void psphere_to_UU2( double X, double Y, double Z, double * rcoord);
extern void R2_to_psphere( double x, double y, double * pcoord );
extern void U1_to_psphere( double z1, double z2, double * pcoord);
extern void V1_to_psphere( double z1, double z2, double * pcoord);
extern void U2_to_psphere( double z1, double z2, double * pcoord);
extern void V2_to_psphere( double z1, double z2, double * pcoord);
extern void VV1_to_psphere( double z1, double z2, double * pcoord);
extern void VV2_to_psphere( double z1, double z2, double * pcoord);
extern void UU1_to_psphere( double z1, double z2, double * pcoord);
extern void UU2_to_psphere( double z1, double z2, double * pcoord);
extern void cylinder_to_plsphere( double r, double theta, double * pcoord);
extern void annulus_plsphere( double u, double v, double * p);
extern void plsphere_annulus( double x, double y, double z, double * u);
extern void plsphere_to_R2( double x, double y, double z, double * c);
extern void R2_to_plsphere( double x, double y, double * pcoord);
extern void U1_to_plsphere( double x0, double y0, double * pcoord);
extern void V1_to_plsphere( double x0, double y0, double * pcoord);
extern void U2_to_plsphere( double x0, double y0, double * pcoord);
extern void V2_to_plsphere( double x0, double y0, double * pcoord);
extern void plsphere_to_U1( double X, double Y, double Z, double * rcoord);
extern void plsphere_to_U2( double X, double Y, double Z, double * rcoord);
extern void plsphere_to_V1( double X, double Y, double Z, double * rcoord);
extern void plsphere_to_V2( double X, double Y, double Z, double * rcoord);

extern double eval_term1( P4POLYNOM1, double );
extern double eval_term2( P4POLYNOM2, double * );
extern double eval_term3( P4POLYNOM3, double * );


extern class QVFStudy VFResults;	// (VFResults.p,VFResults.q) are lyapunov weights

#define PI 3.1415926535897932384626433832
#define PI_DIV2 (PI/2.)
#define PI_DIV4 (PI/4.)
#define TWOPI (2.0*PI)

extern void eval_r_vec_field( double * y, double * f );
extern void eval_U1_vec_field( double * y, double * f );
extern void eval_U2_vec_field( double * y, double * f );
extern void eval_V1_vec_field( double * y, double * f );
extern void eval_V2_vec_field( double * y, double * f );
extern void eval_vec_field_cyl( double * y, double * f );

extern void (*plot_l)( QWinSphere *, double *, double *, int );
extern void (*plot_p)( QWinSphere *, double *, int );

extern void rk78( void (*deriv)( double *, double * ), double y[2], double * hh,
			double hmi, double hma, double e1);
extern double find_root( double (*f)(double), double (*df)(double), double * value );


extern struct orbits_points  *integrate_blow_up(  QWinSphere *, //double, double,
        double *, struct blow_up_points *, double, int, int, struct orbits_points **, int );

extern int change_type( int );
extern int FindSepColor( struct term2 *, int, double * );

extern void draw_sep( QWinSphere *, struct orbits_points * sep );
extern void draw_selected_sep( QWinSphere *, struct orbits_points * sep, int color );
extern void draw_gcf( QWinSphere *, struct orbits_points * sep, int color, int dashes );

extern void IntegrateOrbit( QWinSphere *, int );

#define MATHFUNC(function)		(*(VFResults.function))

extern void set_current_step( double );

extern void plot_all_sep( QWinSphere * );
