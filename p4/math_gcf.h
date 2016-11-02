#ifndef MATH_GCF_H
#define MATH_GCF_H


#include "file_tab.h"
#include "win_sphere.h"


#define EVAL_GCF_NONE               0
#define EVAL_GCF_R2                 1
#define EVAL_GCF_U1                 2
#define EVAL_GCF_U2                 3
#define EVAL_GCF_V1                 4
#define EVAL_GCF_V2                 5
#define EVAL_GCF_FINISHPOINCARE     6
#define EVAL_GCF_LYP_R2             7
#define EVAL_GCF_CYL1               8
#define EVAL_GCF_CYL2               9
#define EVAL_GCF_CYL3               10
#define EVAL_GCF_CYL4               11
#define EVAL_GCF_FINISHLYAPUNOV     12


extern orbits_points * last_gcf_point;

bool evalGcfStart( QWinSphere * sp, int dashes, int points, int precis );
bool evalGcfContinue( int points, int prec );
bool evalGcfFinish( void );
bool runTask( int task, int points, int prec );
void rplane_plsphere0( double x, double y, double * pcoord );
void draw_gcf( QWinSphere * spherewnd, orbits_points * sep, int color, int dashes );


#endif // MATH_GCF_H
