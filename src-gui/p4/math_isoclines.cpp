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

#include "math_isoclines.h"

#include "custom.h"
#include "file_tab.h"
#include "P4InputVF.h"
#include "math_charts.h"
#include "math_p4.h"
#include "plot_tools.h"
#include "P4WinSphere.h"

#include <QFile>

#include <cmath>

// static global variables
static int sIsoclinesTask = EVAL_ISOCLINES_NONE;
static P4WinSphere *sIsoclinesSphere = nullptr;
static int sIsoclinesDashes = 0;
static bool sIsoclinesError = false;

// non-static global variables
P4ORBIT gLastIsoclinesPoint = nullptr;

// static functions
static void insert_isoclines_point(double x0, double y0, double z0, int dashes);
static bool readTaskResults(int);
static bool read_isoclines(void (*chart)(double, double, double *));

// function definitions
bool evalIsoclinesStart(P4WinSphere *sp, int dashes, int precision, int points)
{
    if (gVFResults.plweights_)
        sIsoclinesTask = EVAL_ISOCLINES_LYP_R2;
    else
        sIsoclinesTask = EVAL_ISOCLINES_R2;

    sIsoclinesError = false;
    sIsoclinesSphere = sp;
    sIsoclinesDashes = dashes;
    return runTaskIsoclines(sIsoclinesTask, precision, points);
}

// returns true when finished. Then
// run EvalIsoclinesFinish to see if error
// occurred or not
bool evalIsoclinesContinue(int precision, int points)
{
    if (sIsoclinesTask == EVAL_ISOCLINES_NONE)
        return true;

    if (!readTaskResults(sIsoclinesTask)) {
        sIsoclinesError = true;
        return true;
    }
    sIsoclinesTask++;
    if (sIsoclinesTask == EVAL_ISOCLINES_FINISHPOINCARE ||
        sIsoclinesTask == EVAL_ISOCLINES_FINISHLYAPUNOV) {
        return true;
    }
    if (!runTaskIsoclines(sIsoclinesTask, precision, points)) {
        sIsoclinesError = true;
        return true;
    }
    return false; // still busy
}

bool evalIsoclinesFinish() // return false in case an error occured
{
    int nisocs = (gVFResults.isocline_vector_.size() - 1) % 4;
    gVFResults.isocline_vector_.back().color = CISOC + nisocs;

    if (sIsoclinesTask != EVAL_ISOCLINES_NONE) {
        sIsoclinesSphere->prepareDrawing();
        draw_isoclines(sIsoclinesSphere,
                       gVFResults.isocline_vector_.back().points,
                       gVFResults.isocline_vector_.back().color, 1);
        sIsoclinesSphere->finishDrawing();

        sIsoclinesTask = EVAL_ISOCLINES_NONE;

        if (sIsoclinesError) {
            sIsoclinesError = false;
            return false;
        }
    }
    return true;
}

bool runTaskIsoclines(int task, int precision, int points)
{
    bool value;

    switch (task) {
    case EVAL_ISOCLINES_R2:
        value = gThisVF->prepareIsoclines(
            gVFResults.isocline_vector_.back().r2, -1, 1, precision, points);
        break;
    case EVAL_ISOCLINES_U1:
        value = gThisVF->prepareIsoclines(
            gVFResults.isocline_vector_.back().u1, 0, 1, precision, points);
        break;
    case EVAL_ISOCLINES_V1:
        value = gThisVF->prepareIsoclines(
            gVFResults.isocline_vector_.back().u1, -1, 0, precision, points);
        break;
    case EVAL_ISOCLINES_U2:
        value = gThisVF->prepareIsoclines(
            gVFResults.isocline_vector_.back().u2, 0, 1, precision, points);
        break;
    case EVAL_ISOCLINES_V2:
        value = gThisVF->prepareIsoclines(
            gVFResults.isocline_vector_.back().u2, -1, 0, precision, points);
        break;
    case EVAL_ISOCLINES_LYP_R2:
        value = gThisVF->prepareIsoclines_LyapunovR2(precision, points);
        break;
    case EVAL_ISOCLINES_CYL1:
        value = gThisVF->prepareIsoclines_LyapunovCyl(-PI_DIV4, PI_DIV4,
                                                       precision, points);
        break;
    case EVAL_ISOCLINES_CYL2:
        value = gThisVF->prepareIsoclines_LyapunovCyl(PI_DIV4, PI - PI_DIV4,
                                                       precision, points);
        break;
    case EVAL_ISOCLINES_CYL3:
        value = gThisVF->prepareIsoclines_LyapunovCyl(
            PI - PI_DIV4, PI + PI_DIV4, precision, points);
        break;
    case EVAL_ISOCLINES_CYL4:
        value = gThisVF->prepareIsoclines_LyapunovCyl(-PI + PI_DIV4, -PI_DIV4,
                                                       precision, points);
        break;
    default:
        value = false;
        break;
    }

    if (value)
        return gThisVF->evaluateIsoclines();
    else
        return false;
}

static bool readTaskResults(int task)
{
    bool value;

    switch (task) {
    case EVAL_ISOCLINES_R2:
        value = read_isoclines(R2_to_psphere);
        break;
    case EVAL_ISOCLINES_U1:
        value = read_isoclines(U1_to_psphere);
        break;
    case EVAL_ISOCLINES_V1:
        value = read_isoclines(VV1_to_psphere);
        break;
    case EVAL_ISOCLINES_U2:
        value = read_isoclines(U2_to_psphere);
        break;
    case EVAL_ISOCLINES_V2:
        value = read_isoclines(VV2_to_psphere);
        break;
    case EVAL_ISOCLINES_LYP_R2:
        value = read_isoclines(rplane_plsphere0);
        break;
    // here: the old herc files contained (the equivalent of)
    // value = read_isoclines(rplane_psphere );
    case EVAL_ISOCLINES_CYL1:
        value = read_isoclines(cylinder_to_plsphere);
        break;
    case EVAL_ISOCLINES_CYL2:
        value = read_isoclines(cylinder_to_plsphere);
        break;
    case EVAL_ISOCLINES_CYL3:
        value = read_isoclines(cylinder_to_plsphere);
        break;
    case EVAL_ISOCLINES_CYL4:
        value = read_isoclines(cylinder_to_plsphere);
        break;
    default:
        value = false;
    }

    return value;
}

void draw_isoclines(P4WinSphere *spherewnd, P4ORBIT isoc, int color, int dashes)
{
    double pcoord[3];

    while (isoc != nullptr) {
        if (isoc->dashes && dashes)
            (*plot_l)(spherewnd, pcoord, isoc->pcoord, color);
        else
            (*plot_p)(spherewnd, isoc->pcoord, color);
        copy_x_into_y(isoc->pcoord, pcoord);

        isoc = isoc->next_point;
    }
}

static void insert_isoclines_point(double x0, double y0, double z0, int dashes)
{
    if (gVFResults.isocline_vector_.back().points != nullptr) {
        gLastIsoclinesPoint->next_point = new orbits_points;
        gLastIsoclinesPoint = gLastIsoclinesPoint->next_point;
    } else {
        gLastIsoclinesPoint = new orbits_points;
        gVFResults.isocline_vector_.back().points = gLastIsoclinesPoint;
    }

    gLastIsoclinesPoint->pcoord[0] = x0;
    gLastIsoclinesPoint->pcoord[1] = y0;
    gLastIsoclinesPoint->pcoord[2] = z0;

    gLastIsoclinesPoint->dashes = dashes;
    gLastIsoclinesPoint->color = CISOC;
    gLastIsoclinesPoint->next_point = nullptr;
}

static bool read_isoclines(void (*chart)(double, double, double *))
{
    int k;
    FILE *fp;
    double x, y;
    double pcoord[3];
    int d, c;

    fp = fopen(QFile::encodeName(gThisVF->getfilename_isoclines()), "r");
    if (fp == nullptr)
        return false;

    k = 0;
    while (1) {
        d = 0;
        while (fscanf(fp, "%lf %lf", &x, &y) == 2) {
            k++;
            chart(x, y, pcoord);
            insert_isoclines_point(pcoord[0], pcoord[1], pcoord[2], d);
            d = sIsoclinesDashes;
        }
        for (c = getc(fp); isspace(c);)
            c = getc(fp);
        if (c != ',')
            break;
    }

    fclose(fp);
    return true;
}

void deleteLastIsocline(P4WinSphere *sp)
{
    if (gVFResults.isocline_vector_.empty())
        return;

    gVFResults.isocline_vector_.pop_back();

    sp->refresh();
}
