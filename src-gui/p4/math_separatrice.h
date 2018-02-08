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

#include <memory>
#include <vector>

class P4WinSphere;
struct p4singularity::orbits_points;
struct p4blowup::sep;
struct p4polynom::term2;
struct p4polynom::term3;

extern void (*change_epsilon)(std::shared_ptr<P4WinSphere>, double);
extern void (*start_plot_sep)(std::shared_ptr<P4WinSphere>);
extern void (*cont_plot_sep)(std::shared_ptr<P4WinSphere>);
extern void (*plot_next_sep)(std::shared_ptr<P4WinSphere>);
extern void (*select_next_sep)(std::shared_ptr<P4WinSphere>);

void plot_all_sep(std::shared_ptr<P4WinSphere> spherewnd);
void draw_sep(std::shared_ptr<P4WinSphere> spherewnd,
              std::vector<p4orbits::orbits_points> sep);
void draw_selected_sep(std::shared_ptr<P4WinSphere> spherewnd,
                       std::vector<p4orbits::orbits_points> sep, int color);

int findSepColor2(std::vector<p4polynom::term2> f, int type, double y[2]);
int findSepColor3(std::vector<p4polynom::term3> f, int type, double y[2]);

void integrate_poincare_sep(double p0, double p1, double p2, double *pcoord,
                            double &hhi, int &type, int &color, int &dashes,
                            int &dir, double h_min, double h_max);
void integrate_lyapunov_sep(double p0, double p1, double p2, double *pcoord,
                            double &hhi, int &type, int &color, int &dashes,
                            int &dir, double h_min, double h_max);
std::vector<p4orbits::orbits_points> integrate_sep(
    std::shared_ptr<P4WinSphere> spherewnd, double pcoord[3], double step,
    int dir, int type, int points_to_int);

int change_type(int type);

std::vector<p4orbits::orbits_points> plot_separatrice(
    std::shared_ptr<P4WinSphere> spherewnd, double x0, double y0, double a11,
    double a12, double a21, double a22, double epsilon, p4blowup::sep sep1,
    short int chart);
