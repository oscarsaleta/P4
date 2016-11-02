#ifndef PLOT_TOOLS_H
#define PLOT_TOOLS_H

#include "win_sphere.h"

extern void (*plot_l)( QWinSphere *, double *, double *, int );
extern void (*plot_p)( QWinSphere *, double *, int );

void spherePlotLine( QWinSphere * sp, double * p1, double * p2, int color );
void spherePlotPoint( QWinSphere * sp, double * p, int color );
void spherePrintLine( QWinSphere * sp, double * p1, double * p2, int color );
void spherePrintPoint( QWinSphere * sp, double * p, int color );
bool lineRectangleIntersect( double & x1, double & y1, double & x2, double & y2,
                                        double xmin, double xmax, double ymin, double ymax );

#endif // PLOT_TOOLS_H
