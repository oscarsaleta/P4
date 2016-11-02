#ifndef MATH_NUMERIC_H
#define MATH_NUMERIC_H


void rk78( void (*deriv)( double *, double * ), double y[2], double * hh,
            double hmi, double hma, double e1);
double find_root( double (*f)(double), double (*df)(double), double * value );



#endif // MATH_NUMERIC_H
