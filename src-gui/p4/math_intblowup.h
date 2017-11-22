/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
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

#ifndef MATH_INTBLOWUP_H
#define MATH_INTBLOWUP_H

class QWinSphere;
struct orbits_points;
struct blow_up_points;

void eval_blow_vec_field(double *y, double *f);
orbits_points *integrate_blow_up(QWinSphere *spherewnd, double *pcoord2,
                                 blow_up_points *de_sep, double step, int dir,
                                 int type, orbits_points **orbit, int chart);

#endif // MATH_INTBLOWUP_H
