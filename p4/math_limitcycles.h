#ifndef MATH_LIMITCYCLES_H
#define MATH_LIMITCYCLES_H

#include "win_sphere.h"


void DrawLimitCycle(QWinSphere * spherewnd, double x,double y,double a,double b,double c);
void SearchLimitCycle( QWinSphere * spherewnd, double x0, double y0, double x1, double y1, double grid );
void DrawLimitCycles( QWinSphere * spherewnd );
void DeleteLastLimitCycle( QWinSphere * spherewnd );


#endif // MATH_LIMITCYCLES_H
