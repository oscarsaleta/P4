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

#include "math_isoclines.hpp"

#include "P4InputVF.hpp"
#include "P4WinSphere.hpp"
#include "custom.hpp"
#include "file_tab.hpp"
#include "math_charts.hpp"
#include "math_p4.hpp"
#include "plot_tools.hpp"

#include <QFile>

#include <cmath>

// static global variables
static int sIsoclinesTask{EVAL_ISOCLINES_NONE};
static P4WinSphere *sIsoclinesSphere{nullptr};
static int sIsoclinesDashes{0};
static bool sIsoclinesError{false};
static int sIsoclinesVfIndex{0};

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
    return runTaskIsoclines(sIsoclinesTask, precision, points, 0);
}

// returns true when finished. Then run EvalIsoclinesFinish to see if error
// occurred or not
bool evalIsoclinesContinue(int precision, int points)
{
    if (sIsoclinesTask == EVAL_ISOCLINES_NONE)
        return true;

    if (!readTaskResults(sIsoclinesTask, sIsoclinesVfIndex)) {
        sIsoclinesError = true;
        return true;
    }
    sIsoclinesTask++;
    if (sIsoclinesTask == EVAL_ISOCLINES_FINISHPOINCARE ||
        sIsoclinesTask == EVAL_ISOCLINES_FINISHLYAPUNOV) {
        while (1) {
            if (++sIsoclinesVfIndex >= gThisVF.numVF_)
                break;
            if (!gVFResults.vf_[sIsoclinesVfIndex]->isoclines_.empty())
                break;
        }
        if (sIsoclinesVfIndex >= gThisVF.numVF_)
            // all isoclines are evaluated
            return true;
        // retrigger new set of tasks
        if (gVFResults.plweights_)
            sIsoclinesTask = EVAL_ISOCLINES_LYP_R2;
        else
            sIsoclinesTask = EVAL_ISOCLINES_R2;
    }

    if (!runTaskIsoclines(sIsoclinesTask, precision, points,
                          sIsoclinesVfIndex)) {
        sIsoclinesError = true;
        return true;
    }
    return false;  // still busy
}

bool evalIsoclinesFinish()  // return false in case an error occured
{
    // set color for the last isocline of each VF
    for (auto &vf : gVFResults.vf_) {
        int nisocs{(vf->isoclines_.size() - 1) % 4};
        vf->isoclines_.back().color = CISOC + nisocs;
    }

    if (sIsoclinesTask != EVAL_ISOCLINES_NONE) {
        // resample isoclines to avoid drawing out of region
        for (int index = 0; index < gThisVF.numVF_; index++) {
            gThisVF.resampleIsoclines(index);
        }
        // start drawing the last isocline for every VF
        sIsoclinesSphere->prepareDrawing();
        for (auto const &vf : gVFResults.vf_) {
            draw_isoclines(sIsoclinesSphere, vf->isoclines_.back().points,
                vf->isoclines_.back().color, 1);
        }
        sIsoclinesSphere->finishDrawing();

        // finish
        sIsoclinesTask = EVAL_ISOCLINES_NONE;

        if (sIsoclinesError) {
            sIsoclinesError = false;
            return false;
        }
    }
    return true;
}

bool runTaskIsoclines(int task, int precision, int points, int index)
{
    bool value;

    switch (task) {
    case EVAL_ISOCLINES_R2:
        value = gThisVF.prepareIsoclines(gVFResults.isoclines_.back().r2, -1, 1,
                                         precision, points);
        break;
    case EVAL_ISOCLINES_U1:
        value = gThisVF.prepareIsoclines(gVFResults.isoclines_.back().u1, 0, 1,
                                         precision, points);
        break;
    case EVAL_ISOCLINES_V1:
        value = gThisVF.prepareIsoclines(gVFResults.isoclines_.back().u1, -1, 0,
                                         precision, points);
        break;
    case EVAL_ISOCLINES_U2:
        value = gThisVF.prepareIsoclines(gVFResults.isoclines_.back().u2, 0, 1,
                                         precision, points);
        break;
    case EVAL_ISOCLINES_V2:
        value = gThisVF.prepareIsoclines(gVFResults.isoclines_.back().u2, -1, 0,
                                         precision, points);
        break;
    case EVAL_ISOCLINES_LYP_R2:
        value = gThisVF.prepareIsoclines_LyapunovR2(precision, points);
        break;
    case EVAL_ISOCLINES_CYL1:
        value = gThisVF.prepareIsoclines_LyapunovCyl(-PI_DIV4, PI_DIV4,
                                                     precision, points);
        break;
    case EVAL_ISOCLINES_CYL2:
        value = gThisVF.prepareIsoclines_LyapunovCyl(PI_DIV4, PI - PI_DIV4,
                                                     precision, points);
        break;
    case EVAL_ISOCLINES_CYL3:
        value = gThisVF.prepareIsoclines_LyapunovCyl(PI - PI_DIV4, PI + PI_DIV4,
                                                     precision, points);
        break;
    case EVAL_ISOCLINES_CYL4:
        value = gThisVF.prepareIsoclines_LyapunovCyl(-PI + PI_DIV4, -PI_DIV4,
                                                     precision, points);
        break;
    default:
        value = false;
        break;
    }

    if (value)
        return gThisVF.evaluateIsoclines();
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
    if (gVFResults.isoclines_.back().points != nullptr) {
        gLastIsoclinesPoint->next_point = new orbits_points;
        gLastIsoclinesPoint = gLastIsoclinesPoint->next_point;
    } else {
        gLastIsoclinesPoint = new orbits_points;
        gVFResults.isoclines_.back().points = gLastIsoclinesPoint;
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

    fp = fopen(QFile::encodeName(gThisVF.getfilename_isoclines()), "r");
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
    if (gVFResults.isoclines_.empty())
        return;

    gVFResults.isoclines_.pop_back();

    sp->refresh();
}
