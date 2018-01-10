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

#ifndef MATH_P4_H
#define MATH_P4_H

#define MIN_FLOAT -1.0E32
#define MAX_FLOAT 1.0E32

#define p4_finite std::isfinite

#define PI 3.1415926535897932384626433832
#define PI_DIV2 (PI / 2.)
#define PI_DIV4 (PI / 4.)
#define TWOPI (2.0 * PI)

#define __minus_one_to_q pow(-1.0, g_VFResults.double_q_) // (-1)^q
#define __minus_one_to_p pow(-1.0, g_VFResults.double_p_) // (-1)^p
#define __one_over_p (1.0 / g_VFResults.double_p_)        // 1/p
#define __one_over_q (1.0 / g_VFResults.double_q_)        // 1/q

double eval_lc_poincare(double *pp, double, double, double);
double eval_lc_lyapunov(double *pp, double, double, double);
void set_current_step(double);
void rplane_plsphere0(double x, double y, double *pcoord);

bool less_poincare(double *, double *);
bool less_lyapunov(double *, double *);

#endif /* MATH_P4_H */
