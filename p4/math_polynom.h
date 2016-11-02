#ifndef MATH_POLYNOM_H
#define MATH_POLYNOM_H

#include "file_tab.h"


double eval_term1( P4POLYNOM1, double );
double eval_term2( P4POLYNOM2, double * );
double eval_term3( P4POLYNOM3, double * );

void delete_term1( P4POLYNOM1 p );
void delete_term2( P4POLYNOM2 p );
void delete_term3( P4POLYNOM3 p );

const char * dumpPoly1( P4POLYNOM1 f, const char * x );
const char * dumpPoly2( P4POLYNOM2 f, const char * x, const char * y );
const char * dumpPoly3( P4POLYNOM3 f, const char * x, const char * y, const char * z );

char * printterm2( char * buf, term2 * f, bool isfirst, const char * x, const char * y );
char * printterm3( char * buf, term3 * f, bool isfirst, const char * r, const char * Co, const char * Si );


#endif // MATH_POLYNOM_H
