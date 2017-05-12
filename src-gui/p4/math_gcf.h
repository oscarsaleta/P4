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

#ifndef MATH_GCF_H
#define MATH_GCF_H

#include "file_tab.h"
#include "win_sphere.h"

#define EVAL_GCF_NONE 0
#define EVAL_GCF_R2 1
#define EVAL_GCF_U1 2
#define EVAL_GCF_U2 3
#define EVAL_GCF_V1 4
#define EVAL_GCF_V2 5
#define EVAL_GCF_FINISHPOINCARE 6
#define EVAL_GCF_LYP_R2 7
#define EVAL_GCF_CYL1 8
#define EVAL_GCF_CYL2 9
#define EVAL_GCF_CYL3 10
#define EVAL_GCF_CYL4 11
#define EVAL_GCF_FINISHLYAPUNOV 12

extern orbits_points *g_last_gcf_point;

bool evalGcfStart(QWinSphere *sp, int dashes, int points, int precis);
bool evalGcfContinue(int points, int prec);
bool evalGcfFinish(void);
bool runTask(int task, int points, int prec);
void rplane_plsphere0(double x, double y, double *pcoord);
void draw_gcf(QWinSphere *spherewnd, orbits_points *sep, int color, int dashes);

#endif // MATH_GCF_H
