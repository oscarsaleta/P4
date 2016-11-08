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

void integrateOrbit( QWinSphere *, int );

orbits_points * integrate_orbit( QWinSphere *, double *, double,int, int, int, orbits_points ** );

void drawOrbit( QWinSphere * spherewnd, double * pcoord, struct orbits_points * points, int color );

bool startOrbit( QWinSphere * sphere, double x, double y, bool R );

void drawOrbits( QWinSphere * spherewnd );

void deleteLastOrbit( QWinSphere * spherewnd );

#endif // MATH_ORBITS_H
