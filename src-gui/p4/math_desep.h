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

struct p4blowup::transformations;
struct p4singularities::degenerate;
class QWinSphere;

// math_intblowup.cpp
void eval_blow_vec_field(const double *y, double *f);
// math_separatrice.cpp
void make_transformations(std::vector<p4blowup::transformations> &trans,
                          double x0, double y0, double *point);
// math_intblowup.cpp
std::vector<p4orbits::orbits_points>
integrate_blow_up(QWinSphere *spherewnd, double *pcoord2,
                  std::vector<p4blowup::blow_up_points> de_sep, double step,
                  int dir, int type,
                  std::vector<p4orbits::orbits_points> orbit /*vector?*/,
                  int chart);
// math_separatrice.cpp
void change_epsilon_de(QWinSphere *spherewnd, double epsilon);
// math_separatrice.cpp
void start_plot_de_sep(QWinSphere *spherewnd, int vfindex);
// math_separatrice.cpp
void cont_plot_de_sep(QWinSphere *spherewnd);
// math_separatrice.cpp
void plot_next_de_sep(QWinSphere *spherewnd, int vfindex);
// math_separatrice.cpp
void select_next_de_sep(QWinSphere *spherewnd);
// math_separatrice.cpp
void plot_all_de_sep(QWinSphere *spherewnd, int vfindex,
                     p4singularities::degenerate point /*maybe vector*/);