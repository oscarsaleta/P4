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

#include "math_curve.h"

#include "custom.h"
#include "file_vf.h"
#include "math_charts.h"
#include "math_p4.h"
#include "plot_tools.h"

#include <cmath>

// static global variables
static int s_CurveTask = EVAL_CURVE_NONE;
static QWinSphere *s_CurveSphere = nullptr;
static int s_CurveDashes = 0;
static bool s_CurveError = false;

// non-static global variables
orbits_points *g_last_curve_point = nullptr;

// static functions
static void insert_curve_point(double x0, double y0, double z0, int dashes);
static bool readTaskResults(int); // , int, int, int );
static bool read_curve(void (*chart)(double, double, double *));

// function definitions
bool evalCurveStart(QWinSphere *sp, int dashes, int precision, int points)
{
    if (g_VFResults.curve_points_ != nullptr) {
        //    sp->prepareDrawing();
        //    draw_curve(sp, g_VFResults.curve_points_, CBACKGROUND,
        //    s_CurveDashes);
        //    sp->finishDrawing();
        g_VFResults.deleteOrbitPoint(g_VFResults.curve_points_);
        g_VFResults.curve_points_ = nullptr;
    }

    if (g_VFResults.plweights_)
        s_CurveTask = EVAL_CURVE_LYP_R2;
    else
        s_CurveTask = EVAL_CURVE_R2;

    s_CurveError = false;
    s_CurveSphere = sp;
    s_CurveDashes = dashes;
    return runTaskCurve(s_CurveTask, precision, points);
}

// returns true when finished. Then
// run EvalGCfFinish to see if error
// occurred or not
bool evalCurveContinue(int precision, int points)
{
    if (s_CurveTask == EVAL_CURVE_NONE)
        return true;

    if (!readTaskResults(s_CurveTask)) {
        s_CurveError = true;
        return true;
    }
    s_CurveTask++;
    if (s_CurveTask == EVAL_CURVE_FINISHPOINCARE ||
        s_CurveTask == EVAL_CURVE_FINISHLYAPUNOV) {
        return true;
    }

    if (!runTaskCurve(s_CurveTask, precision, points)) {
        s_CurveError = true;
        return true;
    }

    return false; // still busy
}

bool evalCurveFinish(void) // return false in case an error occured
{
    if (s_CurveTask != EVAL_CURVE_NONE) {
        s_CurveSphere->prepareDrawing();
        draw_curve(s_CurveSphere, g_VFResults.curve_points_, CCURV, 1);
        s_CurveSphere->finishDrawing();

        s_CurveTask = EVAL_CURVE_NONE;

        if (s_CurveError) {
            s_CurveError = false;
            return false;
        }
    }
    return true;
}

bool runTaskCurve(int task, int precision, int points)
{
    bool value;

    switch (task) {
    case EVAL_CURVE_R2:
        value = g_ThisVF->prepareCurve(g_VFResults.curve_, -1, 1, precision,
                                       points);
        break;
    case EVAL_CURVE_U1:
        value = g_ThisVF->prepareCurve(g_VFResults.curve_U1_, 0, 1, precision,
                                       points);
        break;
    case EVAL_CURVE_V1:
        value = g_ThisVF->prepareCurve(g_VFResults.curve_U1_, -1, 0, precision,
                                       points);
        break;
    case EVAL_CURVE_U2:
        value = g_ThisVF->prepareCurve(g_VFResults.curve_U2_, 0, 1, precision,
                                       points);
        break;
    case EVAL_CURVE_V2:
        value = g_ThisVF->prepareCurve(g_VFResults.curve_U2_, -1, 0, precision,
                                       points);
        break;
    case EVAL_CURVE_LYP_R2:
        value = g_ThisVF->prepareCurve_LyapunovR2(precision, points);
        break;
    case EVAL_CURVE_CYL1:
        value = g_ThisVF->prepareCurve_LyapunovCyl(-PI_DIV4, PI_DIV4, precision,
                                                   points);
        break;
    case EVAL_CURVE_CYL2:
        value = g_ThisVF->prepareCurve_LyapunovCyl(PI_DIV4, PI - PI_DIV4,
                                                   precision, points);
        break;
    case EVAL_CURVE_CYL3:
        value = g_ThisVF->prepareCurve_LyapunovCyl(PI - PI_DIV4, PI + PI_DIV4,
                                                   precision, points);
        break;
    case EVAL_CURVE_CYL4:
        value = g_ThisVF->prepareCurve_LyapunovCyl(-PI + PI_DIV4, -PI_DIV4,
                                                   precision, points);
        break;
    default:
        value = false;
        break;
    }

    if (value)
        return g_ThisVF->evaluateCurve();
    else
        return false;
}

static bool readTaskResults(int task)
{
    bool value;

    switch (task) {
    case EVAL_CURVE_R2:
        value = read_curve(R2_to_psphere);
        break;
    case EVAL_CURVE_U1:
        value = read_curve(U1_to_psphere);
        break;
    case EVAL_CURVE_V1:
        value = read_curve(VV1_to_psphere);
        break;
    case EVAL_CURVE_U2:
        value = read_curve(U2_to_psphere);
        break;
    case EVAL_CURVE_V2:
        value = read_curve(VV2_to_psphere);
        break;
    case EVAL_CURVE_LYP_R2:
        value = read_curve(rplane_plsphere0);
        break;
    // here: the old herc files contained (the equivalent of)
    // value = read_curve( rplane_psphere );
    case EVAL_CURVE_CYL1:
        value = read_curve(cylinder_to_plsphere);
        break;
    case EVAL_CURVE_CYL2:
        value = read_curve(cylinder_to_plsphere);
        break;
    case EVAL_CURVE_CYL3:
        value = read_curve(cylinder_to_plsphere);
        break;
    case EVAL_CURVE_CYL4:
        value = read_curve(cylinder_to_plsphere);
        break;
    default:
        value = false;
    }

    return value;
}

void draw_curve(QWinSphere *spherewnd, struct orbits_points *sep, int color,
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

static void insert_curve_point(double x0, double y0, double z0, int dashes)
{
    if (g_VFResults.curve_points_ != nullptr) {
        g_last_curve_point->next_point = new orbits_points;
        g_last_curve_point = g_last_curve_point->next_point;
    } else {
        g_last_curve_point = new orbits_points;
        g_VFResults.curve_points_ = g_last_curve_point;
    }

    g_last_curve_point->pcoord[0] = x0;
    g_last_curve_point->pcoord[1] = y0;
    g_last_curve_point->pcoord[2] = z0;

    g_last_curve_point->dashes = dashes;
    g_last_curve_point->color = CCURV;
    g_last_curve_point->next_point = nullptr;
}

static bool read_curve(void (*chart)(double, double, double *))
{
    int k;
    FILE *fp;
    double x, y;
    double pcoord[3];
    int d, c;

    fp = fopen(QFile::encodeName(g_ThisVF->getfilename_curve()), "r");
    if (fp == nullptr)
        return false;

    k = 0;
    while (1) {
        d = 0;
        while (fscanf(fp, "%lf %lf", &x, &y) == 2) {
            k++;
            chart(x, y, pcoord);
            insert_curve_point(pcoord[0], pcoord[1], pcoord[2], d);
            // d=1;
            d = s_CurveDashes;
        }
        for (c = getc(fp); isspace(c);)
            c = getc(fp);
        if (c != ',')
            break;
    }

    fclose(fp);
    return true;
}
