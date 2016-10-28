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



// math_p4.cpp:

double eval_lc_poincare(double * pp, double, double, double );
double eval_lc_lyapunov(double * pp, double, double, double );
void set_current_step( double );

bool less_poincare( double *, double * );
void eval_lc_lyapunov( void );
bool less_lyapunov( double *, double * );

// math_numeric

void rk78( void (*deriv)( double *, double * ), double y[2], double * hh,
            double hmi, double hma, double e1);
double find_root( double (*f)(double), double (*df)(double), double * value );








#endif /* MATH_P4_H */
