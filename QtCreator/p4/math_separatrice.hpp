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

#include <vector>

class P4Sphere;

namespace P4Polynom
{
struct term2;
struct term3;
}

namespace P4Orbits
{
struct orbits_points;
}

namespace P4Blowup
{
struct sep;
}

extern void (*change_epsilon)(P4Sphere *, double);
extern void (*start_plot_sep)(P4Sphere *, int);
extern void (*cont_plot_sep)(P4Sphere *);
extern void (*plot_next_sep)(P4Sphere *, int);
extern void (*select_next_sep)(P4Sphere *);

void plot_all_sep(P4Sphere *spherewnd);
void draw_sep(P4Sphere *spherewnd,
              P4Orbits::orbits_points *sep);
void draw_selected_sep(P4Sphere *spherewnd,
                       P4Orbits::orbits_points *sep,
                       int color);

int findSepColor2(P4Polynom::term2 *f, int type,
                  double y[2]);
int findSepColor3(P4Polynom::term3 *f, int type,
                  double y[2]);

void integrate_poincare_sep(double p0, double p1, double p2, double *pcoord,
                            double &hhi, int &type, int &color, int &dashes,
                            int &dir, double h_min, double h_max);
void integrate_lyapunov_sep(double p0, double p1, double p2, double *pcoord,
                            double &hhi, int &type, int &color, int &dashes,
                            int &dir, double h_min, double h_max);

P4Orbits::orbits_points *integrate_sep(P4Sphere *spherewnd,
                                                   double pcoord[3],
                                                   double step, int dir,
                                                   int type, int points_to_int, P4Orbits::orbits_points **orbit);

int change_type(int type);

std::vector<P4Orbits::orbits_points>
plot_separatrice(P4Sphere *spherewnd, double x0, double y0, double a11,
                 double a12, double a21, double a22, double epsilon,
                 const P4Blowup::sep &sep1, short int chart, int index);
