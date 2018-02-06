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

#include "math_p4.h"

#include "file_tab.h"
#include "math_charts.h"
#include "win_intparams.h"
#include "win_main.h"
#include "win_plot.h"
#include "win_sphere.h"

#include <cmath>

bool less_poincare(double *p1, double *p2)
{
    if ((p1[0] * p2[2]) < (p2[0] * p1[2]))
        return (1);
    if (((p1[0] * p2[2]) == (p2[0] * p1[2])) &&
        ((p1[1] * p2[2]) < (p2[1] * p1[2])))
        return (true);
    return (false);
}

// eval_lc_poincare: given a transverse section ax+by+cz=0, this function
// calculates the value of the level function ax+by+cz.  By looking at the sign,
// it is seen when the section is crossed.
double eval_lc_poincare(double *pp, double a, double b, double c)
{
    return (a * pp[0] + b * pp[1] + c * pp[2]);
}

double eval_lc_lyapunov(double *pp, double a, double b, double c)
{
    if (pp[0])
        return a * pow(pp[1], g_VFResults.double_q_) * cos(pp[2]) +
               b * pow(pp[1], g_VFResults.double_p_) * sin(pp[2]) +
               c * pow(pp[1], g_VFResults.double_p_plus_q_);
    else
        return (a * pp[1] + b * pp[2] + c);
}

bool less_lyapunov(double *p1, double *p2)
{
    double u[2], v[2];

    plsphere_annulus(p1[0], p1[1], p1[2], u);
    plsphere_annulus(p2[0], p2[1], p2[2], v);
    if (u[0] < v[0])
        return (1);
    if ((u[0] == v[0]) && (u[1] < v[1]))
        return (1);
    return (0);
}

void set_current_step(double curstep)
{
    g_VFResults.config_currentstep_ = curstep;

    if (g_p4stardlg && g_p4stardlg->plotWindow_ &&
        g_p4stardlg->plotWindow_->intParamsWindow_)
        g_p4stardlg->plotWindow_->intParamsWindow_->setCurrentStep(curstep);
}

void rplane_plsphere0(double x, double y, double *pcoord)
{
    R2_to_plsphere(x * cos(y), x * sin(y), pcoord);
}