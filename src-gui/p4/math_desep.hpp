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

struct p4blowup::blow_up_points;
struct p4blowup::transformations;
struct p4singularities::degenerate;
struct p4orbits::orbits_points;
class P4WinSphere;

void eval_blow_vec_field(const double *y, double *f);
void make_transformations(const std::vector<p4blowup::transformations> &trans,
                          double x0, double y0, double *point);

std::vector<p4orbits::orbits_points>
integrate_blow_up(P4WinSphere *spherewnd, double *pcoord2,
                  std::vector<p4blowup::blow_up_points> &de_sep, double step,
                  int dir, int type, int chart);

void change_epsilon_de(P4WinSphere *spherewnd, double epsilon);
void start_plot_de_sep(P4WinSphere *spherewnd, int vfindex);
void cont_plot_de_sep(P4WinSphere *spherewnd);
void plot_next_de_sep(P4WinSphere *spherewnd, int vfindex);
void select_next_de_sep(P4WinSphere *spherewnd);
void plot_all_de_sep(P4WinSphere *spherewnd, int vfindex,
                     const std::vector<p4singularities::degenerate> &point);