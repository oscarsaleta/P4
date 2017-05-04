/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
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
static int CurveTask = EVAL_CURVE_NONE;
static QWinSphere *CurveSphere = nullptr;
static int CurveDashes = 0;
static bool CurveError = false;

// non-static global variables
orbits_points *last_curve_point = nullptr;

// static functions
static void insert_curve_point(double x0, double y0, double z0, int dashes);
static bool ReadTaskResults(int); // , int, int, int );
static bool read_curve(void (*chart)(double, double, double *));

// function definitions
bool evalCurveStart(QWinSphere *sp, int dashes, int points, int precis)
{
    if (VFResults.curve_points != nullptr) {
        sp->prepareDrawing();
        draw_curve(sp, VFResults.curve_points, CBACKGROUND, CurveDashes);
        sp->finishDrawing();
        VFResults.deleteOrbitPoint(VFResults.curve_points);
        VFResults.curve_points = nullptr;
    }

    if (VFResults.plweights)
        CurveTask = EVAL_CURVE_LYP_R2;
    else
        CurveTask = EVAL_CURVE_R2;

    CurveError = false;
    CurveSphere = sp;
    CurveDashes = dashes;
    return runTaskCurve(CurveTask, points, precis);
}

bool evalCurveContinue(int points, int prec) // returns true when finished.  Then
                                           // run EvalGCfFinish to see if error
                                           // occurred or not
{
    if (CurveTask == EVAL_CURVE_NONE)
        return true;

    if (!ReadTaskResults(CurveTask)) // , points, prec, memory ) )
    {
        CurveError = true;
        return true;
    }
    CurveTask++;
    if (CurveTask == EVAL_CURVE_FINISHPOINCARE ||
        CurveTask == EVAL_CURVE_FINISHLYAPUNOV) {
        return true;
    }

    if (!runTaskCurve(CurveTask, points, prec)) {
        CurveError = true;
        return true;
    }

    return false; // still busy
}

bool evalCurveFinish(void) // return false in case an error occured
{
    if (CurveTask != EVAL_CURVE_NONE) {
        CurveSphere->prepareDrawing();
        draw_curve(CurveSphere, VFResults.curve_points, CSING, 1);
        CurveSphere->finishDrawing();

        CurveTask = EVAL_CURVE_NONE;

        if (CurveError) {
            CurveError = false;
            return false;
        }
    }
    return true;
}

bool runTaskCurve(int task, int points, int prec)
{
    bool value;

    switch (task) {
    case EVAL_CURVE_R2:
        value = ThisVF->prepareCurve(VFResults.curve, -1, 1, prec, points);
        break;
    case EVAL_CURVE_U1:
        value = ThisVF->prepareCurve(VFResults.curve_U1, 0, 1, prec, points);
        break;
    case EVAL_CURVE_V1:
        value = ThisVF->prepareCurve(VFResults.curve_U1, -1, 0, prec, points);
        break;
    case EVAL_CURVE_U2:
        value = ThisVF->prepareCurve(VFResults.curve_U2, 0, 1, prec, points);
        break;
    case EVAL_CURVE_V2:
        value = ThisVF->prepareCurve(VFResults.curve_U2, -1, 0, prec, points);
        break;
    case EVAL_CURVE_LYP_R2:
        value = ThisVF->prepareCurve_LyapunovR2(prec, points);
        break;
    case EVAL_CURVE_CYL1:
        value = ThisVF->prepareCurve_LyapunovCyl(-PI_DIV4, PI_DIV4, prec, points);
        break;
    case EVAL_CURVE_CYL2:
        value =
            ThisVF->prepareCurve_LyapunovCyl(PI_DIV4, PI - PI_DIV4, prec, points);
        break;
    case EVAL_CURVE_CYL3:
        value = ThisVF->prepareCurve_LyapunovCyl(PI - PI_DIV4, PI + PI_DIV4, prec,
                                               points);
        break;
    case EVAL_CURVE_CYL4:
        value = ThisVF->prepareCurve_LyapunovCyl(-PI + PI_DIV4, -PI_DIV4, prec,
                                               points);
        break;
    default:
        value = false;
        break;
    }

    if (value)
        return ThisVF->evaluateCurve();
    else
        return false;
}

static bool ReadTaskResults(int task) // , int points, int prec, int memory )
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
    if (VFResults.curve_points != nullptr) {
        last_curve_point->next_point = new orbits_points;
        last_curve_point = last_curve_point->next_point;
    } else {
        last_curve_point = new orbits_points;
        VFResults.curve_points = last_curve_point;
    }

    last_curve_point->pcoord[0] = x0;
    last_curve_point->pcoord[1] = y0;
    last_curve_point->pcoord[2] = z0;

    last_curve_point->dashes = dashes;
    last_curve_point->color = CSING;
    last_curve_point->next_point = nullptr;
}

static bool read_curve(void (*chart)(double, double, double *))
{
    int k;
    FILE *fp;
    double x, y;
    double pcoord[3];
    int d, c;

    fp = fopen(QFile::encodeName(ThisVF->getfilename_curve()), "r");
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
            d = CurveDashes;
        }
        for (c = getc(fp); isspace(c);)
            c = getc(fp);
        if (c != ',')
            break;
    }

    fclose(fp);
    return true;
}
