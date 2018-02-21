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

#include "math_arbitrarycurve.h"

#include "P4InputVF.h"
#include "P4WinSphere.h"
#include "custom.h"
#include "math_charts.h"
#include "math_p4.h"
#include "plot_tools.h"

#include <QFile>

#include <cmath>

// static global variables
static int sCurveTask = EVAL_CURVE_NONE;
static P4WinSphere *sCurveSphere = nullptr;
static int sCurveDashes = 0;
static bool sCurveError = false;

// non-static global variables
P4ORBIT gLastArbitraryCurvePoint = nullptr;

// static functions
static void insert_curve_point(double x0, double y0, double z0, int dashes);
static bool readTaskResults(int);
static bool read_curve(void (*chart)(double, double, double *));

// function definitions
bool evalCurveStart(P4WinSphere *sp, int dashes, int precision, int points)
{
    if (gVFResults.plweights_)
        sCurveTask = EVAL_CURVE_LYP_R2;
    else
        sCurveTask = EVAL_CURVE_R2;

    sCurveError = false;
    sCurveSphere = sp;
    sCurveDashes = dashes;
    return runTaskCurve(sCurveTask, precision, points);
}

// returns true when finished. Then
// run EvalGCfFinish to see if error
// occurred or not
bool evalCurveContinue(int precision, int points)
{
    if (sCurveTask == EVAL_CURVE_NONE)
        return true;

    if (!readTaskResults(sCurveTask)) {
        sCurveError = true;
        return true;
    }
    sCurveTask++;
    if (sCurveTask == EVAL_CURVE_FINISHPOINCARE ||
        sCurveTask == EVAL_CURVE_FINISHLYAPUNOV) {
        return true;
    }

    if (!runTaskCurve(sCurveTask, precision, points)) {
        sCurveError = true;
        return true;
    }

    return false;  // still busy
}

bool evalCurveFinish(void)  // return false in case an error occured
{
    if (sCurveTask != EVAL_CURVE_NONE) {
        sCurveSphere->prepareDrawing();
        draw_curve(sCurveSphere, gVFResults.arbitraryCurveVector_.back().points,
                   CCURV, 1);
        sCurveSphere->finishDrawing();

        sCurveTask = EVAL_CURVE_NONE;

        if (sCurveError) {
            sCurveError = false;
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
        value =
            gThisVF->prepareCurve(gVFResults.arbitraryCurveVector_.back().r2,
                                  -1, 1, precision, points);
        break;
    case EVAL_CURVE_U1:
        value =
            gThisVF->prepareCurve(gVFResults.arbitraryCurveVector_.back().u1, 0,
                                  1, precision, points);
        break;
    case EVAL_CURVE_V1:
        value =
            gThisVF->prepareCurve(gVFResults.arbitraryCurveVector_.back().u1,
                                  -1, 0, precision, points);
        break;
    case EVAL_CURVE_U2:
        value =
            gThisVF->prepareCurve(gVFResults.arbitraryCurveVector_.back().u2, 0,
                                  1, precision, points);
        break;
    case EVAL_CURVE_V2:
        value =
            gThisVF->prepareCurve(gVFResults.arbitraryCurveVector_.back().u2,
                                  -1, 0, precision, points);
        break;
    case EVAL_CURVE_LYP_R2:
        value = gThisVF->prepareCurve_LyapunovR2(precision, points);
        break;
    case EVAL_CURVE_CYL1:
        value = gThisVF->prepareCurve_LyapunovCyl(-PI_DIV4, PI_DIV4, precision,
                                                  points);
        break;
    case EVAL_CURVE_CYL2:
        value = gThisVF->prepareCurve_LyapunovCyl(PI_DIV4, PI - PI_DIV4,
                                                  precision, points);
        break;
    case EVAL_CURVE_CYL3:
        value = gThisVF->prepareCurve_LyapunovCyl(PI - PI_DIV4, PI + PI_DIV4,
                                                  precision, points);
        break;
    case EVAL_CURVE_CYL4:
        value = gThisVF->prepareCurve_LyapunovCyl(-PI + PI_DIV4, -PI_DIV4,
                                                  precision, points);
        break;
    default:
        value = false;
        break;
    }

    if (value)
        return gThisVF->evaluateCurve();
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

void draw_curve(P4WinSphere *spherewnd, P4ORBIT sep, int color, int dashes)
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
    if (gVFResults.arbitraryCurveVector_.back().points != nullptr) {
        gLastArbitraryCurvePoint->next_point = new orbits_points;
        gLastArbitraryCurvePoint = gLastArbitraryCurvePoint->next_point;
    } else {
        gLastArbitraryCurvePoint = new orbits_points;
        gVFResults.arbitraryCurveVector_.back().points =
            gLastArbitraryCurvePoint;
    }

    gLastArbitraryCurvePoint->pcoord[0] = x0;
    gLastArbitraryCurvePoint->pcoord[1] = y0;
    gLastArbitraryCurvePoint->pcoord[2] = z0;

    gLastArbitraryCurvePoint->dashes = dashes;
    gLastArbitraryCurvePoint->color = CCURV;
    gLastArbitraryCurvePoint->next_point = nullptr;
}

static bool read_curve(void (*chart)(double, double, double *))
{
    int k;
    FILE *fp;
    double x, y;
    double pcoord[3];
    int d, c;

    fp = fopen(QFile::encodeName(gThisVF->getfilename_curve()), "r");
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
            d = sCurveDashes;
        }
        for (c = getc(fp); isspace(c);)
            c = getc(fp);
        if (c != ',')
            break;
    }

    fclose(fp);
    return true;
}

void deleteLastCurve(P4WinSphere *sp)
{
    if (gVFResults.arbitraryCurveVector_.empty())
        return;

    gVFResults.arbitraryCurveVector_.pop_back();

    sp->refresh();
}