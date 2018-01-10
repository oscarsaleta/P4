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

#ifndef MATH_CURVE_H
#define MATH_CURVE_H

#include "file_tab.h"

#define EVAL_CURVE_NONE 0
#define EVAL_CURVE_R2 1
#define EVAL_CURVE_U1 2
#define EVAL_CURVE_U2 3
#define EVAL_CURVE_V1 4
#define EVAL_CURVE_V2 5
#define EVAL_CURVE_FINISHPOINCARE 6
#define EVAL_CURVE_LYP_R2 7
#define EVAL_CURVE_CYL1 8
#define EVAL_CURVE_CYL2 9
#define EVAL_CURVE_CYL3 10
#define EVAL_CURVE_CYL4 11
#define EVAL_CURVE_FINISHLYAPUNOV 12

class QWinSphere;
struct orbits_points;

extern orbits_points *g_last_curve_point;

bool evalCurveStart(QWinSphere *sp, int dashes, int precision, int points);
bool evalCurveContinue(int precision, int points);
bool evalCurveFinish(void);
bool runTaskCurve(int task, int precision, int points);
void draw_curve(QWinSphere *spherewnd, orbits_points *sep, int color, int dashes);
void deleteLastCurve(QWinSphere *);

#endif // MATH_CURVE_H
