#ifndef MATH_ORBITS_H
#define MATH_ORBITS_H

#include "win_sphere.h"
#include "file_tab.h"


void integrate_poincare_orbit( double p0, double p1, double p2, double * pcoord,
                                double * hhi, int * dashes, int * dir,
                                double h_min, double h_max );

void integrate_lyapunov_orbit( double p0, double p1, double p2, double * pcoord,
                                double * hhi, int * dashes, int * dir,
                                double h_min, double h_max );

void IntegrateOrbit( QWinSphere *, int );

orbits_points * integrate_orbit( QWinSphere *, double *, double,int, int, int, orbits_points ** );

void DrawOrbit( QWinSphere * spherewnd, double * pcoord, struct orbits_points * points, int color );


#endif // MATH_ORBITS_H
