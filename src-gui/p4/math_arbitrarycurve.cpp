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

#include "math_arbitrarycurve.hpp"

#include <QFile>

#include <cmath>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4Sphere.hpp"
#include "custom.hpp"
#include "math_charts.hpp"
#include "math_p4.hpp"
#include "plot_tools.hpp"

// global variables
P4Orbits::orbits_points *gLastArbitraryCurvePoint{nullptr};

// static global variables
static int sCurveTask{EVAL_CURVE_NONE};
static P4Sphere *sCurveSphere{nullptr};
static int sCurveDashes{0};
static bool sCurveError{false};

// static functions
static void insert_curve_point(double x0, double y0, double z0, int dashes);
static bool readTaskResults(int);
static bool read_curve(void (*chart)(double, double, double *));

// function definitions
bool evalArbitraryCurveStart(P4Sphere *sp, int dashes, int precision,
                             int points)
{
    if (gVFResults.plweights_)
        sCurveTask = EVAL_CURVE_LYP_R2;
    else
        sCurveTask = EVAL_CURVE_R2;

    sCurveError = false;
    sCurveSphere = sp;
    sCurveDashes = dashes;
    return runTaskArbitraryCurve(sCurveTask, precision, points);
}

// returns true when finished. Then run evalArbitraryCurveFinish to see if error
// occurred or not
bool evalArbitraryCurveContinue(int precision, int points)
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

    if (!runTaskArbitraryCurve(sCurveTask, precision, points)) {
        sCurveError = true;
        return true;
    }

    return false; // still busy
}

bool evalArbitraryCurveFinish() // return false in case an error occured
{
    if (sCurveTask != EVAL_CURVE_NONE) {
        sCurveSphere->prepareDrawing();
        drawArbitraryCurve(sCurveSphere,
                           gVFResults.arbitraryCurves_.back().points,
                           P4ColourSettings::colour_arbitrary_curve, 1);
        sCurveSphere->finishDrawing();

        sCurveTask = EVAL_CURVE_NONE;

        if (sCurveError) {
            sCurveError = false;
            return false;
        }
    }
    return true;
}

bool runTaskArbitraryCurve(int task, int precision, int points)
{
    bool value;

    switch (task) {
    case EVAL_CURVE_R2:
        value = gThisVF->prepareArbitraryCurve(
            gVFResults.arbitraryCurves_.back().r2, -1, 1, precision, points);
        break;
    case EVAL_CURVE_U1:
        value = gThisVF->prepareArbitraryCurve(
            gVFResults.arbitraryCurves_.back().u1, 0, 1, precision, points);
        break;
    case EVAL_CURVE_V1:
        value = gThisVF->prepareArbitraryCurve(
            gVFResults.arbitraryCurves_.back().u1, -1, 0, precision, points);
        break;
    case EVAL_CURVE_U2:
        value = gThisVF->prepareArbitraryCurve(
            gVFResults.arbitraryCurves_.back().u2, 0, 1, precision, points);
        break;
    case EVAL_CURVE_V2:
        value = gThisVF->prepareArbitraryCurve(
            gVFResults.arbitraryCurves_.back().u2, -1, 0, precision, points);
        break;
    case EVAL_CURVE_LYP_R2:
        value = gThisVF->prepareArbitraryCurve_LyapunovR2(precision, points);
        break;
    case EVAL_CURVE_CYL1:
        value = gThisVF->prepareArbitraryCurve_LyapunovCyl(-PI_DIV4, PI_DIV4,
                                                           precision, points);
        break;
    case EVAL_CURVE_CYL2:
        value = gThisVF->prepareArbitraryCurve_LyapunovCyl(
            PI_DIV4, PI - PI_DIV4, precision, points);
        break;
    case EVAL_CURVE_CYL3:
        value = gThisVF->prepareArbitraryCurve_LyapunovCyl(
            PI - PI_DIV4, PI + PI_DIV4, precision, points);
        break;
    case EVAL_CURVE_CYL4:
        value = gThisVF->prepareArbitraryCurve_LyapunovCyl(
            -PI + PI_DIV4, -PI_DIV4, precision, points);
        break;
    default:
        value = false;
        break;
    }

    if (value)
        return gThisVF->evaluateArbitraryCurve();
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

void drawArbitraryCurve(P4Sphere *spherewnd, P4Orbits::orbits_points *sep,
                        int color, int dashes)
{
    double pcoord[3];

    while (sep != nullptr) {
        if (sep->dashes && dashes)
            (*plot_l)(spherewnd, pcoord, sep->pcoord, color);
        else
            (*plot_p)(spherewnd, sep->pcoord, color);
        copy_x_into_y(sep->pcoord, pcoord);
        sep = sep->nextpt;
    }
}

static void insert_curve_point(double x0, double y0, double z0, int dashes)
{
    double pcoord[3]{x0, y0, z0};

    if (gVFResults.arbitraryCurves_.back().points != nullptr) {
        gLastArbitraryCurvePoint->nextpt = new P4Orbits::orbits_points{
            pcoord, P4ColourSettings::colour_arbitrary_curve, dashes, 0, 0};
        gLastArbitraryCurvePoint = gLastArbitraryCurvePoint->nextpt;
    } else {
        gLastArbitraryCurvePoint = new P4Orbits::orbits_points{
            pcoord, P4ColourSettings::colour_arbitrary_curve, dashes, 0, 0};
        gVFResults.arbitraryCurves_.back().points = gLastArbitraryCurvePoint;
    }
}

static bool read_curve(void (*chart)(double, double, double *))
{
    int k{0};
    FILE *fp;
    double x, y;
    double pcoord[3];
    int d, c;

    fp = fopen(QFile::encodeName(gThisVF->getfilename_arbitrarycurve()), "r");
    if (fp == nullptr)
        return false;

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

void deleteLastArbitraryCurve(P4Sphere *sp)
{
    if (gVFResults.arbitraryCurves_.empty())
        return;

    gVFResults.arbitraryCurves_.pop_back();

    sp->refresh();
}
