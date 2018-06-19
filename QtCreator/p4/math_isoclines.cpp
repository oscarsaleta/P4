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

#include <QFile>

#include <cmath>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4VFStudy.hpp"
#include "P4WinSphere.hpp"
#include "custom.hpp"
#include "math_charts.hpp"
#include "math_p4.hpp"
#include "plot_tools.hpp"
#include "tables.hpp"

// static global variables
static int sIsoclinesTask{EVAL_ISOCLINES_NONE};
static P4WinSphere *sIsoclinesSphere{nullptr};
static int sIsoclinesDashes{0};
static bool sIsoclinesError{false};
static unsigned int sIsoclinesVfIndex{0};

// static functions
static void insert_isocline_vector_point(double x0, double y0, double z0,
                                         int dashes, int index);
static bool readTaskResults(int, int);
static bool read_isoclines(void (*chart)(double, double, double *), int index);

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
            if (++sIsoclinesVfIndex >= gThisVF->numVF_)
                break;
            if (!gVFResults.vf_[sIsoclinesVfIndex]->isocline_vector_.empty())
                break;
        }
        if (sIsoclinesVfIndex >= gThisVF->numVF_)
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
    return false; // still busy
}

bool evalIsoclinesFinish() // return false in case an error occured
{
    // set color for the last isocline of each VF
    for (auto &vf : gVFResults.vf_) {
        int nisocs{static_cast<int>((vf->isocline_vector_.size() - 1) % 4)};
        vf->isocline_vector_.back().color = CISOC + nisocs;
    }

    if (sIsoclinesTask != EVAL_ISOCLINES_NONE) {
        // resample isoclines to avoid drawing out of region
        for (unsigned int index = 0; index < gThisVF->numVF_; index++) {
            gThisVF->resampleIsoclines(index);
        }
        // start drawing the last isocline for every VF
        sIsoclinesSphere->prepareDrawing();
        for (auto const &vf : gVFResults.vf_) {
            draw_isoclines(sIsoclinesSphere, vf->isocline_vector_.back().points,
                           vf->isocline_vector_.back().color, 1);
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

    while (!gVFResults.vf_[index]->isocline_vector_.empty()) {
        if (++index == gThisVF->numVF_)
            return false;
    }

    auto &vf = gVFResults.vf_[index];

    switch (task) {
    case EVAL_ISOCLINES_R2:
        value = gThisVF->prepareIsoclines(vf->isocline_vector_.back().r2, -1, 1,
                                          precision, points);
        break;
    case EVAL_ISOCLINES_U1:
        value = gThisVF->prepareIsoclines(vf->isocline_vector_.back().u1, 0, 1,
                                          precision, points);
        break;
    case EVAL_ISOCLINES_V1:
        value = gThisVF->prepareIsoclines(vf->isocline_vector_.back().u1, -1, 0,
                                          precision, points);
        break;
    case EVAL_ISOCLINES_U2:
        value = gThisVF->prepareIsoclines(vf->isocline_vector_.back().u2, 0, 1,
                                          precision, points);
        break;
    case EVAL_ISOCLINES_V2:
        value = gThisVF->prepareIsoclines(vf->isocline_vector_.back().u2, -1, 0,
                                          precision, points);
        break;
    case EVAL_ISOCLINES_LYP_R2:
        value = gThisVF->prepareIsoclines_LyapunovR2(precision, points, index);
        break;
    case EVAL_ISOCLINES_CYL1:
        value = gThisVF->prepareIsoclines_LyapunovCyl(-PI_DIV4, PI_DIV4,
                                                      precision, points, index);
        break;
    case EVAL_ISOCLINES_CYL2:
        value = gThisVF->prepareIsoclines_LyapunovCyl(PI_DIV4, PI - PI_DIV4,
                                                      precision, points, index);
        break;
    case EVAL_ISOCLINES_CYL3:
        value = gThisVF->prepareIsoclines_LyapunovCyl(
            PI - PI_DIV4, PI + PI_DIV4, precision, points, index);
        break;
    case EVAL_ISOCLINES_CYL4:
        value = gThisVF->prepareIsoclines_LyapunovCyl(-PI + PI_DIV4, -PI_DIV4,
                                                      precision, points, index);
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

static bool readTaskResults(int task, int index)
{
    bool value;

    switch (task) {
    case EVAL_ISOCLINES_R2:
        value = read_isoclines(R2_to_psphere, index);
        break;
    case EVAL_ISOCLINES_U1:
        value = read_isoclines(U1_to_psphere, index);
        break;
    case EVAL_ISOCLINES_V1:
        value = read_isoclines(VV1_to_psphere, index);
        break;
    case EVAL_ISOCLINES_U2:
        value = read_isoclines(U2_to_psphere, index);
        break;
    case EVAL_ISOCLINES_V2:
        value = read_isoclines(VV2_to_psphere, index);
        break;
    case EVAL_ISOCLINES_LYP_R2:
        value = read_isoclines(rplane_plsphere0, index);
        break;
    case EVAL_ISOCLINES_CYL1:
        value = read_isoclines(cylinder_to_plsphere, index);
        break;
    case EVAL_ISOCLINES_CYL2:
        value = read_isoclines(cylinder_to_plsphere, index);
        break;
    case EVAL_ISOCLINES_CYL3:
        value = read_isoclines(cylinder_to_plsphere, index);
        break;
    case EVAL_ISOCLINES_CYL4:
        value = read_isoclines(cylinder_to_plsphere, index);
        break;
    default:
        value = false;
    }

    return value;
}

void draw_isoclines(P4WinSphere *spherewnd,
                    const std::vector<p4orbits::orbits_points> &isoc, int color,
                    int dashes)
{
    double pcoord[3];

    for (auto const &it : isoc) {
        if (it.dashes && dashes)
            (*plot_l)(spherewnd, pcoord, it.pcoord, color);
        else
            (*plot_p)(spherewnd, it.pcoord, color);
        copy_x_into_y(it.pcoord, pcoord);
    }
}

static void insert_isocline_vector_point(double x0, double y0, double z0,
                                         int dashes, int index)
{
    double pcoord[3]{x0, y0, z0};

    gVFResults.vf_[index]->isocline_vector_.back().points.emplace_back(
        CISOC, pcoord, dashes, 0, 0);
}

static bool read_isoclines(void (*chart)(double, double, double *), int index)
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
            insert_isocline_vector_point(pcoord[0], pcoord[1], pcoord[2], d,
                                         index);
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
    for (auto &vf : gVFResults.vf_) {
        if (vf->isocline_vector_.empty())
            continue;
        vf->isocline_vector_.pop_back();
    }

    sp->refresh();
}
