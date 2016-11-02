#ifndef MATH_LIMITCYCLES_H
#define MATH_LIMITCYCLES_H

#include "win_sphere.h"


void drawLimitCycle(QWinSphere * spherewnd, double x,double y,double a,double b,double c);
void searchLimitCycle( QWinSphere * spherewnd, double x0, double y0, double x1, double y1, double grid );
void drawLimitCycles( QWinSphere * spherewnd );
void deleteLastLimitCycle( QWinSphere * spherewnd );


#endif // MATH_LIMITCYCLES_H
