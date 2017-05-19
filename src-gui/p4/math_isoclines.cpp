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

#include "math_isoclines.h"

#include "custom.h"
#include "file_vf.h"
#include "math_charts.h"
#include "math_p4.h"
#include "plot_tools.h"

#include <cmath>

// static global variables
static int s_IsoclinesTask = EVAL_ISOCLINES_NONE;
static QWinSphere *s_IsoclinesSphere = nullptr;
static int s_IsoclinesDashes = 0;
static bool s_IsoclinesError = false;

// non-static global variables
orbits_points *g_last_isoclines_point = nullptr;

// static functions
static void insert_isoclines_point(int i, double x0, double y0, double z0,
                                   int dashes);
static bool readTaskResults(int, int);
static bool read_isoclines(int, void (*chart)(double, double, double *));

// function definitions FIXME: process already running
bool evalIsoclinesStart(int i, QWinSphere *sp, int dashes, int precision,
                        int points)
{
    /*if (g_VFResults.current_isoclines_->points[i] != nullptr) {
        sp->prepareDrawing();
        draw_isoclines(sp, g_VFResults.current_isoclines_->points[i],
                       CBACKGROUND, s_IsoclinesDashes);
        sp->finishDrawing();
        g_VFResults.deleteOrbitPoint(
            g_VFResults.current_isoclines_->points[i]);
        g_VFResults.current_isoclines_->points[i] = nullptr;
    }*/

    if (g_VFResults.plweights_)
        s_IsoclinesTask = EVAL_ISOCLINES_LYP_R2;
    else
        s_IsoclinesTask = EVAL_ISOCLINES_R2;

    s_IsoclinesError = false;
    s_IsoclinesSphere = sp;
    s_IsoclinesDashes = dashes;
    if (!runTaskIsoclines(i, s_IsoclinesTask, precision, points)) {
        s_IsoclinesError = true;
        return false;
    }
    return true;
}

// returns true when finished. Then
// run EvalIsoclinesFinish to see if error
// occurred or not FIXME:
bool evalIsoclinesContinue(int i, int precision, int points)
{
    if (s_IsoclinesTask == EVAL_ISOCLINES_NONE)
        return true;

    if (!readTaskResults(i, s_IsoclinesTask)) {
        s_IsoclinesError = true;
        return true;
    }
    s_IsoclinesTask++;
    if (s_IsoclinesTask == EVAL_ISOCLINES_FINISHPOINCARE ||
        s_IsoclinesTask == EVAL_ISOCLINES_FINISHLYAPUNOV) {
        return true;
    }
    if (!runTaskIsoclines(i, s_IsoclinesTask, precision, points)) {
        s_IsoclinesError = true;
        return true;
    }
    return false; // still busy
}

bool evalIsoclinesFinish(int i) // return false in case an error occured
{
    if (s_IsoclinesTask != EVAL_ISOCLINES_NONE) {
        s_IsoclinesSphere->prepareDrawing();
        draw_isoclines(s_IsoclinesSphere,
                       g_VFResults.current_isoclines_->curves[i].points, CCURV,
                       1);
        s_IsoclinesSphere->finishDrawing();

        s_IsoclinesTask = EVAL_ISOCLINES_NONE;

        if (s_IsoclinesError) {
            s_IsoclinesError = false;
            return false;
        }
    }
    return true;
}

bool runTaskIsoclines(int i, int task, int precision, int points)
{
    bool value;

    switch (task) {
    case EVAL_ISOCLINES_R2:
        value = g_ThisVF->prepareIsoclines(
            g_VFResults.current_isoclines_->curves[i].r2, -1, 1, precision,
            points);
        break;
    case EVAL_ISOCLINES_U1:
        value = g_ThisVF->prepareIsoclines(
            g_VFResults.current_isoclines_->curves[i].u1, 0, 1, precision,
            points);
        break;
    case EVAL_ISOCLINES_V1:
        value = g_ThisVF->prepareIsoclines(
            g_VFResults.current_isoclines_->curves[i].u1, -1, 0, precision,
            points);
        break;
    case EVAL_ISOCLINES_U2:
        value = g_ThisVF->prepareIsoclines(
            g_VFResults.current_isoclines_->curves[i].u2, 0, 1, precision,
            points);
        break;
    case EVAL_ISOCLINES_V2:
        value = g_ThisVF->prepareIsoclines(
            g_VFResults.current_isoclines_->curves[i].u2, -1, 0, precision,
            points);
        break;
    case EVAL_ISOCLINES_LYP_R2:
        value = g_ThisVF->prepareIsoclines_LyapunovR2(precision, points);
        break;
    case EVAL_ISOCLINES_CYL1:
        value = g_ThisVF->prepareIsoclines_LyapunovCyl(-PI_DIV4, PI_DIV4,
                                                       precision, points);
        break;
    case EVAL_ISOCLINES_CYL2:
        value = g_ThisVF->prepareIsoclines_LyapunovCyl(PI_DIV4, PI - PI_DIV4,
                                                       precision, points);
        break;
    case EVAL_ISOCLINES_CYL3:
        value = g_ThisVF->prepareIsoclines_LyapunovCyl(
            PI - PI_DIV4, PI + PI_DIV4, precision, points);
        break;
    case EVAL_ISOCLINES_CYL4:
        value = g_ThisVF->prepareIsoclines_LyapunovCyl(-PI + PI_DIV4, -PI_DIV4,
                                                       precision, points);
        break;
    default:
        value = false;
        break;
    }

    if (value)
        return g_ThisVF->evaluateIsoclines(i);
    else
        return false;
}

static bool readTaskResults(int i, int task)
{
    bool value;

    switch (task) {
    case EVAL_ISOCLINES_R2:
        value = read_isoclines(i, R2_to_psphere);
        break;
    case EVAL_ISOCLINES_U1:
        value = read_isoclines(i, U1_to_psphere);
        break;
    case EVAL_ISOCLINES_V1:
        value = read_isoclines(i, VV1_to_psphere);
        break;
    case EVAL_ISOCLINES_U2:
        value = read_isoclines(i, U2_to_psphere);
        break;
    case EVAL_ISOCLINES_V2:
        value = read_isoclines(i, VV2_to_psphere);
        break;
    case EVAL_ISOCLINES_LYP_R2:
        value = read_isoclines(i, rplane_plsphere0);
        break;
    // here: the old herc files contained (the equivalent of)
    // value = read_isoclines(i, rplane_psphere );
    case EVAL_ISOCLINES_CYL1:
        value = read_isoclines(i, cylinder_to_plsphere);
        break;
    case EVAL_ISOCLINES_CYL2:
        value = read_isoclines(i, cylinder_to_plsphere);
        break;
    case EVAL_ISOCLINES_CYL3:
        value = read_isoclines(i, cylinder_to_plsphere);
        break;
    case EVAL_ISOCLINES_CYL4:
        value = read_isoclines(i, cylinder_to_plsphere);
        break;
    default:
        value = false;
    }

    return value;
}

void draw_isoclines(QWinSphere *spherewnd, struct orbits_points *sep, int color,
                    int dashes)
{
    double pcoord[3];

    while (sep != nullptr) {
        if (sep->dashes && dashes)
            (*plot_l)(spherewnd, pcoord, sep->pcoord, color);
        else
            (*plot_p)(spherewnd, sep->pcoord, color);
        copy_x_into_y(sep->pcoord, pcoord);

        sep = sep->next_point;
    }
}

static void insert_isoclines_point(int i, double x0, double y0, double z0,
                                   int dashes)
{
    if (/*g_VFResults.current_isoclines_->points[i]*/
        g_last_isoclines_point != nullptr) {
        g_last_isoclines_point->next_point = new orbits_points;
        g_last_isoclines_point = g_last_isoclines_point->next_point;
    } else {
        g_last_isoclines_point = new orbits_points;
        g_VFResults.current_isoclines_->curves[i].points =
            g_last_isoclines_point;
    }

    g_last_isoclines_point->pcoord[0] = x0;
    g_last_isoclines_point->pcoord[1] = y0;
    g_last_isoclines_point->pcoord[2] = z0;

    g_last_isoclines_point->dashes = dashes;
    g_last_isoclines_point->color = CCURV;
    g_last_isoclines_point->next_point = nullptr;
}

static bool read_isoclines(int i, void (*chart)(double, double, double *))
{
    int k;
    FILE *fp;
    double x, y;
    double pcoord[3];
    int d, c;

    fp = fopen(QFile::encodeName(g_ThisVF->getfilename_isoclines()), "r");
    if (fp == nullptr)
        return false;

    k = 0;
    while (1) {
        d = 0;
        while (fscanf(fp, "%lf %lf", &x, &y) == 2) {
            k++;
            chart(x, y, pcoord);
            insert_isoclines_point(i, pcoord[0], pcoord[1], pcoord[2], d);
            d = s_IsoclinesDashes;
        }
        for (c = getc(fp); isspace(c);)
            c = getc(fp);
        if (c != ',')
            break;
    }

    fclose(fp);
    return true;
}
