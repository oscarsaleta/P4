#ifndef MATH_P4_H
#define MATH_P4_H


#define MIN_FLOAT   -1.0E32
#define MAX_FLOAT   1.0E32

#define PI 3.1415926535897932384626433832
#define PI_DIV2 (PI/2.)
#define PI_DIV4 (PI/4.)
#define TWOPI (2.0*PI)

#define __minus_one_to_q    pow(-1.0,__q)                   // (-1)^q
#define __minus_one_to_p    pow(-1.0,__p)                   // (-1)^p
#define __one_over_p        (1.0/__p)                       // 1/p
#define __one_over_q        (1.0/__q)                       // 1/q


double eval_lc_poincare(double * pp, double, double, double );
double eval_lc_lyapunov(double * pp, double, double, double );
void set_current_step( double );

bool less_poincare( double *, double * );
bool less_lyapunov( double *, double * );


#endif /* MATH_P4_H */
