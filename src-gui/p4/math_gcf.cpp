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

#include "math_gcf.hpp"

#include <QFile>

#include <cmath>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4Sphere.hpp"
#include "P4VFStudy.hpp"
#include "custom.hpp"
#include "math_charts.hpp"
#include "math_p4.hpp"
#include "plot_tools.hpp"
#include "structures.hpp"

// global variables
P4Orbits::orbits_points *gLastGcfPoint{nullptr};

// static global variables
static int sGcfTask{EVAL_GCF_NONE};
static P4Sphere *sGcfSphere{nullptr};
static int sGcfDashes{0};
static bool sGcfError{false};
static unsigned int sGcfVfIndex{0};

// static functions
static void insert_gcf_point(double x0, double y0, double z0, int dashes,
                             int index);
static bool readTaskResults(int task, int index);
static bool read_gcf(void (*chart)(double, double, double *), int index);

// function definitions
bool evalGcfStart(P4Sphere *sp, int dashes, int precision, int points)
{
    sp->prepareDrawing();
    for (unsigned int r = 0; r < gThisVF->numVF_; r++) {
        if (gVFResults.vf_[r]->gcf_points_ != nullptr) {
            draw_gcf(sp, gVFResults.vf_[r]->gcf_points_,
                     P4ColourSettings::colour_background, sGcfDashes);
            delete gVFResults.vf_[r]->gcf_points_;
            gVFResults.vf_[r]->gcf_points_ = nullptr;
        }
    }
    sp->finishDrawing();

    if (gVFResults.plweights_)
        sGcfTask = EVAL_GCF_LYP_R2;
    else
        sGcfTask = EVAL_GCF_R2;

    sGcfError = false;
    sGcfSphere = sp;
    sGcfDashes = dashes;
    return runTask(sGcfTask, precision, points, 0);
}

// returns true when finished.  Then run EvalGCfFinish to see if error occurred
// or not
bool evalGcfContinue(int precision, int points)
{
    if (sGcfTask == EVAL_GCF_NONE)
        return true;

    if (!readTaskResults(sGcfTask, sGcfVfIndex)) {
        sGcfError = true;
        return true;
    }
    sGcfTask++;
    if (sGcfTask == EVAL_GCF_FINISHPOINCARE ||
        sGcfTask == EVAL_GCF_FINISHLYAPUNOV) {
        while (1) {
            if (++sGcfVfIndex >= gThisVF->numVF_)
                break;
            if (gVFResults.vf_[sGcfVfIndex]->gcf_ != nullptr)
                break;
        }
        if (sGcfVfIndex >= gThisVF->numVF_)
            // all gcfs are evaluated
            return true;
        // retrigger new set of tasks
        if (gVFResults.plweights_)
            sGcfTask = EVAL_GCF_LYP_R2;
        else
            sGcfTask = EVAL_GCF_R2;
    }

    if (!runTask(sGcfTask, precision, points, sGcfVfIndex)) {
        sGcfError = true;
        return true;
    }

    return false; // still busy
}

bool evalGcfFinish() // return false in case an error occured
{
    if (sGcfTask != EVAL_GCF_NONE) {
        for (unsigned int index = 0; index < gThisVF->numVF_; index++)
            gThisVF->resampleGcf(index);
        sGcfSphere->prepareDrawing();
        for (unsigned int index = 0; index < gThisVF->numVF_; index++) {
            if (gVFResults.vf_[index]->gcf_points_ != nullptr)
                draw_gcf(sGcfSphere, gVFResults.vf_[index]->gcf_points_,
                         P4ColourSettings::colour_curve_singularities, 1);
        }
        sGcfSphere->finishDrawing();

        sGcfTask = EVAL_GCF_NONE;

        if (sGcfError) {
            sGcfError = false;
            return false;
        }
    }
    return true;
}

bool runTask(int task, int precision, int points, unsigned int index)
{
    bool value;

    while (gVFResults.vf_[index]->gcf_ == nullptr) {
        if (++index == gThisVF->numVF_)
            return false;
    }
    sGcfVfIndex = index;

    switch (task) {
    case EVAL_GCF_R2:
        value = gThisVF->prepareGcf(gVFResults.vf_[index]->gcf_, -1, 1,
                                    precision, points);
        break;
    case EVAL_GCF_U1:
        value = gThisVF->prepareGcf(gVFResults.vf_[index]->gcf_U1_, 0, 1,
                                    precision, points);
        break;
    case EVAL_GCF_V1:
        value = gThisVF->prepareGcf(gVFResults.vf_[index]->gcf_U1_, -1, 0,
                                    precision, points);
        break;
    case EVAL_GCF_U2:
        value = gThisVF->prepareGcf(gVFResults.vf_[index]->gcf_U2_, 0, 1,
                                    precision, points);
        break;
    case EVAL_GCF_V2:
        value = gThisVF->prepareGcf(gVFResults.vf_[index]->gcf_U2_, -1, 0,
                                    precision, points);
        break;
    case EVAL_GCF_LYP_R2:
        value = gThisVF->prepareGcf_LyapunovR2(precision, points, index);
        break;
    case EVAL_GCF_CYL1:
        value = gThisVF->prepareGcf_LyapunovCyl(-PI_DIV4, PI_DIV4, precision,
                                                points, index);
        break;
    case EVAL_GCF_CYL2:
        value = gThisVF->prepareGcf_LyapunovCyl(PI_DIV4, PI - PI_DIV4,
                                                precision, points, index);
        break;
    case EVAL_GCF_CYL3:
        value = gThisVF->prepareGcf_LyapunovCyl(PI - PI_DIV4, PI + PI_DIV4,
                                                precision, points, index);
        break;
    case EVAL_GCF_CYL4:
        value = gThisVF->prepareGcf_LyapunovCyl(-PI + PI_DIV4, -PI_DIV4,
                                                precision, points, index);
        break;
    default:
        value = false;
        break;
    }

    if (value)
        return gThisVF->evaluateGcf();
    else
        return false;
}

static bool readTaskResults(int task, int index)
{
    bool value;

    switch (task) {
    case EVAL_GCF_R2:
        value = read_gcf(R2_to_psphere, index);
        break;
    case EVAL_GCF_U1:
        value = read_gcf(U1_to_psphere, index);
        break;
    case EVAL_GCF_V1:
        value = read_gcf(VV1_to_psphere, index);
        break;
    case EVAL_GCF_U2:
        value = read_gcf(U2_to_psphere, index);
        break;
    case EVAL_GCF_V2:
        value = read_gcf(VV2_to_psphere, index);
        break;
    case EVAL_GCF_LYP_R2:
        value = read_gcf(rplane_plsphere0, index);
        break;
    // here: the old herc files contained (the equivalent of)
    // value = read_gcf( rplane_psphere ,index);
    case EVAL_GCF_CYL1:
        value = read_gcf(cylinder_to_plsphere, index);
        break;
    case EVAL_GCF_CYL2:
        value = read_gcf(cylinder_to_plsphere, index);
        break;
    case EVAL_GCF_CYL3:
        value = read_gcf(cylinder_to_plsphere, index);
        break;
    case EVAL_GCF_CYL4:
        value = read_gcf(cylinder_to_plsphere, index);
        break;
    default:
        value = false;
    }

    return value;
}

void draw_gcf(P4Sphere *spherewnd, P4Orbits::orbits_points *sep, int color,
              int dashes)
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

static void insert_gcf_point(double x0, double y0, double z0, int dashes,
                             int index)
{
    double pcoord[3]{x0, y0, z0};

    if (gVFResults.vf_[index]->gcf_points_ != nullptr) {
        gLastGcfPoint->nextpt = new P4Orbits::orbits_points{
            pcoord, P4ColourSettings::colour_curve_singularities, dashes, 0, 0};
        gLastGcfPoint = gLastGcfPoint->nextpt;
    } else {
        gLastGcfPoint = new P4Orbits::orbits_points{
            pcoord, P4ColourSettings::colour_curve_singularities, dashes, 0, 0};
        gVFResults.vf_[index]->gcf_points_ = gLastGcfPoint;
    }
}

static bool read_gcf(void (*chart)(double, double, double *), int index)
{
    int k;
    FILE *fp;
    double x, y;
    double pcoord[3];
    int d, c;

    fp = fopen(QFile::encodeName(gThisVF->getfilename_gcf()), "r");
    if (fp == nullptr)
        return false;

    k = 0;
    while (1) {
        d = 0;
        while (fscanf(fp, "%lf %lf", &x, &y) == 2) {
            k++;
            chart(x, y, pcoord);
            insert_gcf_point(pcoord[0], pcoord[1], pcoord[2], d, index);
            // d=1;
            d = sGcfDashes;
        }
        for (c = getc(fp); isspace(c);)
            c = getc(fp);
        if (c != ',')
            break;
    }

    fclose(fp);
    return true;
}
