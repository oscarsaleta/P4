/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  P4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

class P4WinSphere;

extern void (*plot_l)(P4WinSphere *, const double *, const double *, int);
extern void (*plot_p)(P4WinSphere *, const double *, int);

void spherePlotLine(P4WinSphere *sp, const double *p1, const double *p2,
                    int color);
void spherePlotPoint(P4WinSphere *sp, const double *p, int color);
void spherePrintLine(P4WinSphere *sp, const double *p1, const double *p2,
                     int color);
void spherePrintPoint(P4WinSphere *sp, const double *p, int color);
bool lineRectangleIntersect(double &x1, double &y1, double &x2, double &y2,
                            double xmin, double xmax, double ymin, double ymax);
