#ifndef MATH_P4_H
#define MATH_P4_H


#include "file_tab.h"
#include "win_sphere.h"


#define PI 3.1415926535897932384626433832
#define PI_DIV2 (PI/2.)
#define PI_DIV4 (PI/4.)
#define TWOPI (2.0*PI)

#define __p                 VFResults.double_p              // p
#define __q                 VFResults.double_q              // q
#define __q_minus_p         VFResults.double_q_minus_p      // q-p
#define __p_minus_1         VFResults.double_p_minus_1      // p-1
#define __q_minus_1         VFResults.double_q_minus_1      // q-1
#define __minus_one_to_q    pow(-1.0,__q)                   // (-1)^q
#define __minus_one_to_p    pow(-1.0,__p)                   // (-1)^p
#define __one_over_p        (1.0/__p)                       // 1/p
#define __one_over_q        (1.0/__q)                       // 1/q

#define MATHFUNC(function) (*(VFResults.function))


extern QVFStudy VFResults;	// (VFResults.p,VFResults.q) are lyapunov weights

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
void plot_all_sep( QWinSphere * );

// math_p4.cpp:

double eval_lc_poincare(double * pp, double, double, double );
double eval_lc_lyapunov(double * pp, double, double, double );
void set_current_step( double );

bool less_poincare( double *, double * );
void eval_lc_lyapunov( void );
bool less_lyapunov( double *, double * );

// math_changedir.cpp:



// math_polynom.cpp

double eval_term1( P4POLYNOM1, double );
double eval_term2( P4POLYNOM2, double * );
double eval_term3( P4POLYNOM3, double * );
void delete_term1( P4POLYNOM1 p );
void delete_term2( P4POLYNOM2 p );
void delete_term3( P4POLYNOM3 p );
const char * DumpPoly1( P4POLYNOM1 f, const char * x );
const char * DumpPoly2( P4POLYNOM2 f, const char * x, const char * y );
const char * DumpPoly3( P4POLYNOM3 f, const char * x, const char * y, const char * z );
char * printterm2( char * buf, term2 * f, bool isfirst, const char * x, const char * y );
char * printterm3( char * buf, term3 * f, bool isfirst, const char * r, const char * Co, const char * Si );

// math_charts.cpp:

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
//void draw_gcf( QWinSphere *, orbits_points * sep, int color, int dashes );

void IntegrateOrbit( QWinSphere *, int );

#endif /* MATH_P4_H */
