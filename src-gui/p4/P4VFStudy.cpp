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

#include "P4VFStudy.hpp"

#include <QByteArray>
#include <QFile>
#include <QString>
#include <QTextEdit>

#include "P4ParentStudy.hpp"
#include "math_polynom.hpp"

// -----------------------------------------------------------------------
//          CONSTRUCTOR
// -----------------------------------------------------------------------
P4VFStudy::P4VFStudy(P4ParentStudy *parent) : parent_{parent} {}

// -----------------------------------------------------------------------
//          P4VFStudy::reset
// -----------------------------------------------------------------------
void P4VFStudy::reset()
{
    // Delete Vector Field
    f_vec_field_0_.clear();
    f_vec_field_1_.clear();
    vec_field_U1_0_.clear();
    vec_field_U1_1_.clear();
    vec_field_U2_0_.clear();
    vec_field_U2_1_.clear();
    vec_field_V1_0_.clear();
    vec_field_V1_1_.clear();
    vec_field_V2_0_.clear();
    vec_field_V2_1_.clear();
    vec_field_C_0_.clear();
    vec_field_C_1_.clear();

    // Delete singular points
    saddlePoints_.clear();
    sePoints_.clear();
    nodePoints_.clear();
    dePoints_.clear();
    sfPoints_.clear();
    wfPoints_.clear();

    // Delete GCF:
    gcf_.clear();
    gcf_U1_.clear();
    gcf_U2_.clear();
    gcf_V1_.clear();
    gcf_V2_.clear();
    gcf_C_.clear();
    gcf_points_.clear();

    // reset others
    singinf_ = false;
    dir_vec_field_ = 1;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readTables
// -----------------------------------------------------------------------
bool P4VFStudy::readTables(FILE *fpvec, FILE *fpfin, FILE *fpinf)
{
    int j;
    if (!readGCF(fpvec) ||
        !readVectorField(fpvec, f_vec_field_0_, f_vec_field_1_) ||
        !readVectorField(fpvec, vec_field_U1_0_, vec_field_U1_1_) ||
        !readVectorField(fpvec, vec_field_V1_0_, vec_field_V1_1_) ||
        !readVectorField(fpvec, vec_field_U2_0_, vec_field_U2_1_) ||
        !readVectorField(fpvec, vec_field_V2_0_, vec_field_V2_1_))
        return false;

    if (parent_->plweights_) {
        if (!readVectorFieldCylinder(fpvec, vec_field_C_0_, vec_field_C_1_))
            return false;
        singinf_ = 0;
    } else {
        int aux;
        if (fscanf(fpvec, "%d %d", &aux, &dir_vec_field_) != 2)
            return false;
        singinf_ = (aux == 1 ? true : false);
    }

    if (fpfin != nullptr) {
        if (!readPoints(fpfin))
            return false;
    }
    if (fpinf != nullptr) {
        if (parent_->p_ == 1 && parent_->q_ == 1) {
            for (j = 0; j < 2; j++)
                if (!readPoints(fpinf))
                    return false;
        } else {
            for (j = 0; j < 4; j++)
                if (!readPoints(fpinf))
                    return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readGCF
// -----------------------------------------------------------------------
bool P4VFStudy::readGCF(FILE *fp)
{
    int N, degree_gcf;

    if (fscanf(fp, "%d", &degree_gcf) != 1 || degree_gcf < 0)
        return false;

    if (degree_gcf) {
        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm2(fp, gcf_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm2(fp, gcf_U1_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm2(fp, gcf_U2_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm2(fp, gcf_V1_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm2(fp, gcf_V2_, N))
            return false;

        if (parent_->p_ != 1 || parent_->q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1 || N < 0)
                return false;
            if (!readTerm3(fp, gcf_C_, N))
                return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------
//                      P4VFStudy::readIsoclines
// -----------------------------------------------------------------------
// FIXME: how to have an isoclines file for every different VF?
bool P4VFStudy::readIsoclines(QString basename)
{
    int N, degree_curve;
    FILE *fp = nullptr;
    setlocale(LC_ALL, "C");

    fp = fopen(QFile::encodeName(basename + "_vecisoclines.tab"), "rt");
    if (fp == nullptr) {
        // dump(basename, "Cannot open file " + basename + "_vecisoclines.tab");
        return false;
    }

    p4curves::isoclines new_isocline;
    if (fscanf(fp, "%d", &degree_curve) != 1)
        return false;

    if (degree_curve > 0) {
        if (fscanf(fp, "%d", &N) != 1)
            return false;

        // prepare a new isocline and link it to the list

        if (!readTerm2(fp, new_isocline.r2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        if (!readTerm2(fp, new_isocline.u1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        if (!readTerm2(fp, new_isocline.u2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        if (!readTerm2(fp, new_isocline.v1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        if (!readTerm2(fp, new_isocline.v2, N))
            return false;

        if (parent_->p_ != 1 || parent_->q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1)
                return false;

            if (!readTerm3(fp, new_isocline.c, N))
                return false;
        }
    } else {
        return false;
    }

    isocline_vector_.push_back(std::move(new_isocline));
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readVectorField
// -----------------------------------------------------------------------
bool P4VFStudy::readVectorField(FILE *fp, std::vector<p4polynom::term2> &vf0,
                                std::vector<p4polynom::term2> &vf1)
{
    int M, N;

    if (fscanf(fp, "%d", &M) != 1 || M < 0 || !readTerm2(fp, vf0, M) ||
        fscanf(fp, "%d", &N) != 1 || N < 0 || !readTerm2(fp, vf1, N))
        return false;

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readVectorFieldCylinder
// -----------------------------------------------------------------------
bool P4VFStudy::readVectorFieldCylinder(FILE *fp,
                                        std::vector<p4polynom::term3> &vf0,
                                        std::vector<p4polynom::term3> &vf1)
{
    int M, N;

    if (fscanf(fp, "%d", &M) != 1 || M < 0 || !readTerm3(fp, vf0, M) ||
        fscanf(fp, "%d", &N) != 1 || N < 0 || !readTerm3(fp, vf1, N))
        return false;

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readPoints
// -----------------------------------------------------------------------
bool P4VFStudy::readPoints(FILE *fp)
{
    int N, i, typ;

    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;

    for (i = 0; i < N; i++) {
        if (fscanf(fp, "%d ", &typ) != 1)
            return false;

        switch (typ) {
        case SADDLE:
            if (!readSaddlePoint(fp))
                return false;
            break;
        case SEMI_HYPERBOLIC:
            if (!readSemiElementaryPoint(fp))
                return false;
            break;
        case NODE:
            if (!readNodePoint(fp))
                return false;
            break;
        case STRONG_FOCUS:
            if (!readStrongFocusPoint(fp))
                return false;
            break;
        case WEAK_FOCUS:
            if (!readWeakFocusPoint(fp))
                return false;
            break;
        default:
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readSaddlePoint
// -----------------------------------------------------------------------
bool P4VFStudy::readSaddlePoint(FILE *fp)
{
    int N;

    // variables for constructing a p4singularities::saddle
    double x0, y0;
    int chart;
    double a11, a12, a21, a22;
    double epsilon;
    std::vector<p4blowup::sep> psep;
    std::vector<p4polynom::term2> vf0, vf1;

    // variables for constructing a p4blowup::sep
    int stype;
    std::vector<p4polynom::term1> ssep;

    if (fscanf(fp, "%lf %lf", &x0, &y0) != 2 ||
        fscanf(fp, "%lf %lf %lf %lf", &a11, &a12, &a21, &a22) != 4 ||
        !readVectorField(fp, vf0, vf1) || fscanf(fp, "%d ", &chart) ||
        fscanf(fp, "%d ", &stype) != 1 || fscanf(fp, "%d ", &N) != 1 || N < 0)
        return false;

    if (!readTerm1(fp, ssep, N))
        return false;

    psep.emplace_back(std::vector<p4orbits::orbits_points>{}, stype, 1, 0, true,
                      ssep);

    if (chart == CHART_R2 || singinf_) {
        // point is finite hence we have 4 separatrices or we have a line of
        // singularities at infinity and hence we have also 4 separatrices if we
        // delete the line
        // #2
        psep.emplace_back(std::vector<p4orbits::orbits_points>{}, stype, -1, 0,
                          false);
        // #3
        if (fscanf(fp, "%d", &stype) != 1 || fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm1(fp, ssep, N))
            return false;
        psep.emplace_back(std::vector<p4orbits::orbits_points>{}, stype, 1, 1,
                          true);
        // #4
        psep.emplace_back(std::vector<p4orbits::orbits_points>{}, stype, -1, 1,
                          false);
    }
    fscanf(fp, "%lf ", &epsilon);
    saddlePoints_.emplace_back(x0, y0, chart, 0, epsilon, true, psep, vf0, vf1,
                               a11, a12, a21, a22);

    // line at infinity a line of singularities in poincare disc
    if (singinf_ && chart != CHART_R2) {
        y0 = 0.0;
        chart = ((chart == CHART_U1) ? CHART_V1 : CHART_V2);
        vf0.clear();
        vf1.clear();
        saddlePoints_.emplace_back(x0, y0, chart, 0, epsilon, false, psep, vf0,
                                   vf1, a11, a12, a21, a22);
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readSemiElementaryPoint
// -----------------------------------------------------------------------
bool P4VFStudy::readSemiElementaryPoint(FILE *fp)
{
    int s, N;

    // variables for constructing a p4singularities::semi_elementary
    double x0, y0;
    int chart;
    double epsilon;
    double a11, a12, a21, a22;
    int ptype;
    std::vector<p4polynom::term2> vf0, vf1;
    std::vector<p4blowup::sep> pseps;

    // variables for constructing a p4blowup::sep
    int stype;
    int sdirection;
    std::vector<p4polynom::term1> ssep;

    // p4singularities::semi_elementary point;
    // std::vector<p4blowup::sep> sepvec;

    if (fscanf(fp, "%lf %lf", &x0, &y0) != 2 ||
        fscanf(fp, "%lf %lf %lf %lf", &a11, &a12, &a21, &a22) != 4 ||
        !readVectorField(fp, vf0, vf1) ||
        fscanf(fp, "%d %d %d", &ptype, &s, &chart) != 3)
        return false;

    switch (ptype) {
    case 1: { // sadle-node
        if (!s || (chart == CHART_R2)) {
            if (s)
                stype = OT_CENT_STABLE;
            else
                stype = OT_UNSTABLE;
            if (((parent_->p_ == 1) && (parent_->q_ == 1) &&
                 ((chart == CHART_V1) || (chart == CHART_V2))) ||
                (chart == CHART_R2))
                sdirection = -1;
            else
                sdirection = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            if (!readTerm1(fp, ssep, N))
                return false;

            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 0, true, ssep);

            if (chart == CHART_R2) {
                stype = OT_UNSTABLE;
                sdirection = 1;
                if (fscanf(fp, "%d", &N) != 1)
                    return false;
                if (!readTerm1(fp, ssep, N))
                    return false;
                pseps.emplace_back(std::vector<p4orbits::orbits_points>{},
                                   stype, sdirection, 1, true, ssep);

                sdirection = -1;
                pseps.emplace_back(std::vector<p4orbits::orbits_points>{},
                                   stype, sdirection, 1, false, ssep);
            }
        }
    } break;
    case 2: { // saddle-node
        if (s)
            stype = STYPE_CENUNSTABLE;
        else
            stype = STYPE_STABLE;
        if ((parent_->p_ == 1) && (parent_->q_ == 1) &&
            ((chart == CHART_V1) || (chart == CHART_V2)))
            sdirection = -1;
        else
            sdirection = 1;

        if (fscanf(fp, "%d", &N) != 1)
            return false;
        if (!readTerm1(fp, ssep, N))
            return false;
        pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                           sdirection, 0, true, ssep);

        if (chart == CHART_R2) {
            stype = STYPE_STABLE;
            sdirection = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            if (!readTerm1(fp, ssep, N))
                return false;
            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 1, true, ssep);

            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 1, false, ssep);
        }
    } break;
    case 3: { // saddle-node
        if (s)
            stype = STYPE_CENSTABLE;
        else
            stype = STYPE_UNSTABLE;
        if ((parent_->p_ == 1) && (parent_->q_ == 1) &&
            ((chart == CHART_V1) || (chart == CHART_V2)))
            sdirection = -1;
        else
            sdirection = 1;
        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        if (!readTerm1(fp, ssep, N))
            return false;
        pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                           sdirection, 0, true, ssep);

        if (chart == CHART_R2) {
            stype = STYPE_UNSTABLE;
            sdirection = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            if (!readTerm1(fp, ssep, N))
                return false;
            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 1, true, ssep);

            sdirection = -1;
            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 1, false, ssep);
        }
    } break;
    case 4: { // saddle-node
        if (!s || chart == CHART_R2) {
            if (s)
                stype = STYPE_CENUNSTABLE;
            else
                stype = STYPE_STABLE;
            if (((parent_->p_ == 1) && (parent_->q_ == 1) &&
                 ((chart == CHART_V1) || (chart == CHART_V2))) ||
                (chart == CHART_R2))
                sdirection = -1;
            else
                sdirection = 1;

            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            if (!readTerm1(fp, ssep, N))
                return false;
            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 0, true, ssep);

            if (chart == CHART_R2) {
                stype = STYPE_STABLE;
                sdirection = 1;
                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                if (!readTerm1(fp, ssep, N))
                    return false;
                pseps.emplace_back(std::vector<p4orbits::orbits_points>{},
                                   stype, sdirection, 1, true, ssep);

                sdirection = -1;
                pseps.emplace_back(std::vector<p4orbits::orbits_points>{},
                                   stype, sdirection, 1, false, ssep);
            }
        }
    } break;
    case 6: { // saddle
        if (s)
            stype = STYPE_CENUNSTABLE;
        else
            stype = STYPE_STABLE;
        if ((parent_->p_ == 1) && (parent_->q_ == 1) &&
            ((chart == CHART_V1) || (chart == CHART_V2)))
            sdirection = -1;
        else
            sdirection = 1;
        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        if (!readTerm1(fp, ssep, N))
            return false;
        pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                           sdirection, 0, true, ssep);

        if (chart == CHART_R2) {
            stype = STYPE_CENUNSTABLE;
            sdirection = -1;
            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 0, false, ssep);

            stype = STYPE_STABLE;
            sdirection = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            if (!readTerm1(fp, ssep, N))
                return false;
            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 1, true, ssep);

            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 1, false, ssep);
        }
    } break;
    case 7: { // saddle
        if (s)
            stype = STYPE_CENSTABLE;
        else
            stype = STYPE_UNSTABLE;
        if ((parent_->p_ == 1) && (parent_->q_ == 1) &&
            ((chart == CHART_V1) || (chart == CHART_V2)))
            sdirection = -1;
        else
            sdirection = 1;
        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        if (!readTerm1(fp, ssep, N))
            return false;
        pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                           sdirection, 0, true, ssep);

        if (chart == CHART_R2) {
            stype = STYPE_CENSTABLE;
            sdirection = -1;
            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 0, false, ssep);

            stype = STYPE_UNSTABLE;
            sdirection = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            if (!readTerm1(fp, ssep, N))
                return false;
            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 1, true, ssep);

            sdirection = -1;
            pseps.emplace_back(std::vector<p4orbits::orbits_points>{}, stype,
                               sdirection, 1, false, ssep);
        }
    } break;
    default: { // change type of node if we have a gcf ?
        double y[2]{x0, y0};
        bool ok{false};
        switch (chart) {
        case CHART_R2:
            if (eval_term2(gcf_, y) < 0)
                ok = true;
            break;
        case CHART_U1:
            if (eval_term2(gcf_U1_, y) < 0)
                ok = true;
            break;
        case CHART_V1:
            if (parent_->p_ == 1 && parent_->q_ == 1)
                y[0] = -y[0];
            if (eval_term2(gcf_V1_, y) < 0)
                ok = true;
            break;
        case CHART_U2:
            if (eval_term2(gcf_U2_, y) < 0)
                ok = true;
            break;
        case CHART_V2:
            if (parent_->p_ == 1 && parent_->q_ == 1)
                y[0] = -y[0];
            if (eval_term2(gcf_V2_, y) < 0)
                ok = true;
            break;
        }
        if (ok) {
            if (ptype == 5)
                ptype = 8;
            else
                ptype = 5;
        }
    }
    }

    if (fscanf(fp, "%lf", &epsilon) != 1)
        return false;

    // point.notadummy = true;
    sePoints_.emplace_back(x0, y0, chart, 0, epsilon, true, pseps, vf0, vf1,
                           a11, a12, a21, a22, ptype);

    // line at infinity a line of singularities in poincare disc
    if (singinf_ && chart != CHART_R2) {
        y0 = 0.0;
        if (dir_vec_field_ != 1) {
            int tp;
            switch (ptype) {
            case 1:
                tp = 3;
                break;
            case 2:
                tp = 4;
                break;
            case 3:
                tp = 1;
                break;
            case 4:
                tp = 2;
                break;
            case 6:
                tp = 7;
                break;
            case 7:
                tp = 6;
                break;
            default:
                tp = ptype;
            }
            ptype = tp;
        }
        chart = (chart == CHART_U1) ? CHART_V1 : CHART_V2;
        // point.notadummy = false;
        vf0.clear();
        vf1.clear();
        sePoints_.emplace_back(x0, y0, chart, 0, epsilon, false, pseps, vf0,
                               vf1, a11, a12, a21, a22, ptype);
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readNodePoint
// -----------------------------------------------------------------------
bool P4VFStudy::readNodePoint(FILE *fp)
{
    double y[2];

    // variables for constructing a p4singularities::node
    double x0, y0;
    int chart;
    int stable;

    if (fscanf(fp, "%lf %lf %d ", &x0, &y0, &stable) != 3 ||
        fscanf(fp, "%d ", &chart) != 1)
        return false;

    // change type of node if we have a gcf?
    y[0] = x0;
    y[1] = y0;

    switch (chart) {
    case CHART_R2:
        if (eval_term2(gcf_, y) < 0)
            stable *= -1;
        break;
    case CHART_U1:
        if (eval_term2(gcf_U1_, y) < 0)
            stable *= -1;
        break;
    case CHART_V1:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V1_, y) < 0)
            stable *= -1;
        break;
    case CHART_U2:
        if (eval_term2(gcf_U2_, y) < 0)
            stable *= -1;
        break;
    case CHART_V2:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V2_, y) < 0)
            stable *= -1;
        break;
    }

    nodePoints_.emplace_back(x0, y0, chart, 0, stable);

    // if line at infinity is a line of singularities in Poincare disk:
    // duplicate singularity using symmetry
    if (singinf_ != 0 && chart != CHART_R2) {
        y0 = 0.0;
        chart = ((chart == CHART_U1) ? CHART_V1 : CHART_V2);
        stable = stable * ((dir_vec_field_ == -1) ? -1 : 1);
        nodePoints_.emplace_back(x0, y0, chart, 0, stable);
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readStrongFocusPoint
// -----------------------------------------------------------------------
bool P4VFStudy::readStrongFocusPoint(FILE *fp)
{
    double y[2];

    // variables for constructing a p4singularities::strong_focus
    double x0, y0;
    int chart;
    int stable;

    if (fscanf(fp, "%d %lf %lf ", &stable, &x0, &y0) != 3 ||
        fscanf(fp, "%d ", &chart) != 1)
        return false;

    // change type of node if we have a gcf?
    y[0] = x0;
    y[1] = y0;

    switch (chart) {
    case CHART_R2:
        if (eval_term2(gcf_, y) < 0)
            stable *= -1;
        break;
    case CHART_U1:
        if (eval_term2(gcf_U1_, y) < 0)
            stable *= -1;
        break;
    case CHART_V1:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V1_, y) < 0)
            stable *= -1;
        break;
    case CHART_U2:
        if (eval_term2(gcf_U2_, y) < 0)
            stable *= -1;
        break;
    case CHART_V2:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V2_, y) < 0)
            stable *= -1;
        break;
    }

    sfPoints_.emplace_back(x0, y0, chart, 0, stable);

    // if line at infinity is a line of singularities in Poincare disk:
    // duplicate singularity using symmetry
    if (singinf_ && chart != CHART_R2) {
        y0 = 0.0;
        chart = ((chart == CHART_U1) ? CHART_V1 : CHART_V2);
        stable = stable * ((dir_vec_field_ == -1) ? -1 : 1);
        sfPoints_.emplace_back(x0, y0, chart, 0, stable);
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readWeakFocusPoint
// -----------------------------------------------------------------------
bool P4VFStudy::readWeakFocusPoint(FILE *fp)
{
    double y[2];

    // variables for constructing a p4singularities::weak_focus
    double x0, y0;
    int chart;
    int type;

    if (fscanf(fp, "%lf %lf ", &x0, &y0) != 2 ||
        fscanf(fp, "%d %d ", &type, &chart) != 2)
        return false;

    // change node type if we have a gcf?

    if ((type == STYPE_STABLE) || (type == STYPE_UNSTABLE)) {
        y[0] = x0;
        y[1] = y0;
        switch (chart) {
        case CHART_R2:
            if (eval_term2(gcf_, y) < 0)
                type *= -1;
            break;
        case CHART_U1:
            if (eval_term2(gcf_U1_, y) < 0)
                type *= -1;
            break;
        case CHART_V1:
            if ((parent_->p_ == 1) && (parent_->q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V1_, y) < 0)
                type *= -1;
            break;
        case CHART_U2:
            if (eval_term2(gcf_U2_, y) < 0)
                type *= -1;
            break;
        case CHART_V2:
            if ((parent_->p_ == 1) && (parent_->q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V2_, y) < 0)
                type *= -1;
            break;
        }
    }
    wfPoints_.emplace_back(x0, y0, chart, 0, type);

    // if line at infinity is a line of singularities in Poincare disk:
    // duplicate singularity using symmetry
    if (singinf_ && (chart != CHART_R2)) {
        if (dir_vec_field_ != 1) {
            switch (type) {
            case STYPE_STABLE:
                type = STYPE_UNSTABLE;
                break;
            case STYPE_UNSTABLE:
                type = STYPE_STABLE;
                break;
            }
        }
        chart = ((chart == CHART_U1) ? CHART_V1 : CHART_V2);
        wfPoints_.emplace_back(x0, y0, chart, 0, type);
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readDegeneratePoint
// -----------------------------------------------------------------------
bool P4VFStudy::readDegeneratePoint(FILE *fp)
{
    int n;

    // variables for constructing a p4singularities::degenerate
    double x0, y0;
    int chart;
    double epsilon;
    bool notadummy;
    std::vector<p4blowup::blow_up_points> pblw;

    if (fscanf(fp, "%lf %lf %lf %d ", &x0, &y0, &epsilon, &n) != 4)
        return false;
    if (n) {
        if (!readBlowupPoints(fp, pblw, n))
            return false;
        pblw.back().integrating_in_local_chart = true;
    }
    if (fscanf(fp, "%d ", &chart) != 1)
        return false;
    notadummy = true;

    dePoints_.emplace_back(x0, y0, chart, 0, epsilon, notadummy, pblw);

    // if line at infinity is a line of singularities in Poincare disk:
    // duplicate singularity using symmetry
    if (singinf_ && chart != CHART_R2) {
        y0 = 0.0;
        chart = ((chart == CHART_U1) ? CHART_V1 : CHART_V2);
        notadummy = false;
        dePoints_.emplace_back(x0, y0, chart, 0, epsilon, notadummy, pblw);
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readTransformations
// -----------------------------------------------------------------------
bool P4VFStudy::readTransformations(
    FILE *fp, std::vector<p4blowup::transformations> &trans, int n)
{
    double x0, y0;
    int c1, c2, d1, d2, d3, d4;
    int d;
    for (int i = 0; i < n; i++) {
        if (fscanf(fp, "%lf %lf %d %d %d %d %d %d %d", &x0, &y0, &c1, &c2, &d1,
                   &d2, &d3, &d4, &d) != 9)
            return false;
        trans.emplace_back(x0, y0, c1, c2, d1, d2, d3, d4, d);
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readBlowupPoints
// -----------------------------------------------------------------------
bool P4VFStudy::readBlowupPoints(FILE *fp,
                                 std::vector<p4blowup::blow_up_points> &blowup,
                                 int n)
{
    int N, typ;

    // variables per construir p4blowup::blow_up_points
    int pn;
    std::vector<p4blowup::transformations> ptrans;
    double x0, y0;
    double a11, a12, a21, a22;
    std::vector<p4polynom::term2> vf0, vf1;
    std::vector<p4polynom::term1> psep;
    int ptype;

    for (int i = 0; i < n; i++) {
        if (fscanf(fp, "%d ", &pn) != 1 ||
            !readTransformations(fp, ptrans, pn) ||
            fscanf(fp, "%lf %lf ", &x0, &y0) != 2 ||
            fscanf(fp, "%lf %lf %lf %lf ", &a11, &a12, &a21, &a22) != 4 ||
            !readVectorField(fp, vf0, vf1) || fscanf(fp, "%d ", &N) != 1 ||
            !readTerm1(fp, psep, N) || fscanf(fp, "%d ", &typ) != 1)
            return false;

        switch (typ) {
        case 1:
        case 3:
        case 9:
        case 10:
            ptype = STYPE_UNSTABLE;
            break;
        case 2:
        case 4:
        case 11:
        case 12:
            ptype = STYPE_STABLE;
            break;
        case 5:
        case 7:
            ptype = STYPE_CENUNSTABLE;
            break;
        case 6:
        case 8:
            ptype = STYPE_CENSTABLE;
            break;
        default:
            return false;
        }
        blowup.emplace_back(pn, ptrans, x0, y0, a11, a12, a21, a22, vf0, vf1,
                            psep, ptype, true,
                            std::vector<p4orbits::orbits_points>{});
    }
    return true;
}

// -----------------------------------------------------------------------
//          DUMP FUNCTIONS
// -----------------------------------------------------------------------

void dumpSeparatrices(QTextEdit &m, const std::vector<p4blowup::sep> &separ,
                      int margin)
{
    QString s;
    char smargin[80];
    std::string stype;
    strcpy(smargin, "                              ");
    smargin[margin] = 0;

    if (separ.empty()) {
        m.append(s.sprintf("%s/", smargin));
        return;
    }

    for (auto const &it : separ) {
        switch (it.type) {
        case STYPE_STABLE:
            stype = "stable         ";
            break;
        case STYPE_UNSTABLE:
            stype = "unstable       ";
            break;
        case STYPE_CENSTABLE:
            stype = "center-stable  ";
            break;
        case STYPE_CENUNSTABLE:
            stype = "center-unstable";
            break;
        default:
            stype = "???????????????";
            break;
        }
        m.append(s.sprintf("%sType=%s  Dir=%-2d  original=%d", smargin,
                           stype.data(), it.direction, it.notadummy));
        if (it.d == 0)
            m.append(s.sprintf("%sTaylor: (x,y)=(t,%s)", smargin,
                               dumpPoly1(it.separatrice, "t")));
        else
            m.append(s.sprintf("%sTaylor: (x,y)=(%s,t)", smargin,
                               dumpPoly1(it.separatrice, "t")));
    }
}

// dump saddle singularities
void dumpSingularities(QTextEdit &m,
                       const std::vector<p4singularities::saddle> &p,
                       bool longversion)
{
    const char *chart;
    QString s;

    for (auto const &it : p) {
        switch (it.chart) {
        case CHART_R2:
            chart = "R2";
            break;
        case CHART_U1:
            chart = "U1";
            break;
        case CHART_U2:
            chart = "U2";
            break;
        case CHART_V1:
            chart = "V1";
            break;
        case CHART_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(s.sprintf("SADDLE:\t(x0,y0)=(%g,%g)  Chart %s", it.x0, it.y0,
                           chart));
        if (longversion) {
            m.append(s.sprintf("   Epsilon = %g  original=%d", it.epsilon,
                               it.notadummy));
            m.append(
                s.sprintf("   Transformation Matrix = [ [%g,%g], [%g,%g] ]",
                          it.a11, it.a12, it.a21, it.a22));
            m.append(s.sprintf("   Vector Field:"));
            m.append(s.sprintf("      x' = %s",
                               dumpPoly2(it.vector_field_0, "x", "y")));
            m.append(s.sprintf("      y' = %s",
                               dumpPoly2(it.vector_field_1, "x", "y")));
            m.append(s.sprintf("   Separatrices:"));
            dumpSeparatrices(m, it.separatrices, 6);
            m.append(s.sprintf(" "));
        }
    }
}

// dump degenerate singularities
void dumpSingularities(QTextEdit &m,
                       const std::vector<p4singularities::degenerate> &p,
                       bool longversion)
{
    const char *chart;
    QString s;

    for (auto const &it : p) {
        switch (it.chart) {
        case CHART_R2:
            chart = "R2";
            break;
        case CHART_U1:
            chart = "U1";
            break;
        case CHART_U2:
            chart = "U2";
            break;
        case CHART_V1:
            chart = "V1";
            break;
        case CHART_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(s.sprintf("DEGENERATE:\t(x0,y0)=(%g,%g)  Chart %s", it.x0,
                           it.y0, chart));
        if (longversion) {
            m.append(s.sprintf("   Epsilon = %g  original=%d", it.epsilon,
                               it.notadummy));
            m.append(s.sprintf(" "));
        }
    }
}

// dump strong focus singularities
void dumpSingularities(QTextEdit &m,
                       const std::vector<p4singularities::strong_focus> &p,
                       bool longversion)
{
    const char *chart;
    QString s;
    QByteArray ss;

    for (auto const &it : p) {
        switch (it.chart) {
        case CHART_R2:
            chart = "R2";
            break;
        case CHART_U1:
            chart = "U1";
            break;
        case CHART_U2:
            chart = "U2";
            break;
        case CHART_V1:
            chart = "V1";
            break;
        case CHART_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(s.sprintf("STRONG FOCUS:\t(x0,y0)=(%g,%g)  Chart %s", it.x0,
                           it.y0, chart));
        if (longversion) {
            if (it.stable == -1)
                s = "(stable)";
            else if (it.stable == +1)
                s = "(unstable)";
            else
                s = "( ??? )";
            ss = s.toLatin1();
            m.append(
                s.sprintf("    Stability = %d %s", it.stable, ss.constData()));
            m.append(s.sprintf(" "));
        }
    }
}

// dump weak focus singularities
void dumpSingularities(QTextEdit &m,
                       const std::vector<p4singularities::weak_focus> &p,
                       bool longversion)
{
    const char *chart;
    QString s;

    for (auto const &it : p) {
        switch (it.chart) {
        case CHART_R2:
            chart = "R2";
            break;
        case CHART_U1:
            chart = "U1";
            break;
        case CHART_U2:
            chart = "U2";
            break;
        case CHART_V1:
            chart = "V1";
            break;
        case CHART_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(s.sprintf("WEAK FOCUS:\t(x0,y0)=(%g,%g)  Chart %s", it.x0,
                           it.y0, chart));
        if (longversion) {
            m.append(s.sprintf("    Type = %d", it.type));
            m.append(s.sprintf(" "));
        }
    }
}

// dump node singularities
void dumpSingularities(QTextEdit &m,
                       const std::vector<p4singularities::node> &p,
                       bool longversion)
{
    const char *chart;
    QString s;
    QByteArray ss;

    for (auto const &it : p) {
        switch (it.chart) {
        case CHART_R2:
            chart = "R2";
            break;
        case CHART_U1:
            chart = "U1";
            break;
        case CHART_U2:
            chart = "U2";
            break;
        case CHART_V1:
            chart = "V1";
            break;
        case CHART_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(
            s.sprintf("NODE:\t(x0,y0)=(%g,%g)  Chart %s", it.x0, it.y0, chart));
        if (longversion) {
            if (it.stable == -1)
                s = "(stable)";
            else if (it.stable == +1)
                s = "(unstable)";
            else
                s = "( ??? )";
            ss = s.toLatin1();
            m.append(
                s.sprintf("    Stability = %d %s", it.stable, ss.constData()));
            m.append(s.sprintf(" "));
        }
    }
}

// dump semi elementary singularities
void dumpSingularities(QTextEdit &m,
                       const std::vector<p4singularities::semi_elementary> &p,
                       bool longversion)
{
    const char *chart;
    QString s;
    QByteArray ss;

    for (auto const &it : p) {
        switch (it.chart) {
        case CHART_R2:
            chart = "R2";
            break;
        case CHART_U1:
            chart = "U1";
            break;
        case CHART_U2:
            chart = "U2";
            break;
        case CHART_V1:
            chart = "V1";
            break;
        case CHART_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(s.sprintf("SEMI ELEMENTARY:\t(x0,y0)=(%g,%g)  Chart %s",
                           it.x0, it.y0, chart));
        if (longversion) {
            m.append(s.sprintf("   Type    = %d", it.type));
            m.append(s.sprintf("   Epsilon = %g  original=%d", it.epsilon,
                               it.notadummy));
            m.append(
                s.sprintf("   Transformation Matrix = [ [%g,%g], [%g,%g] ]",
                          it.a11, it.a12, it.a21, it.a22));
            m.append(s.sprintf("   Vector Field = "));
            m.append(s.sprintf("      x' = %s",
                               dumpPoly2(it.vector_field_0, "x", "y")));
            m.append(s.sprintf("      y' = %s",
                               dumpPoly2(it.vector_field_1, "x", "y")));
            m.append(s.sprintf("   Separatrices:"));
            dumpSeparatrices(m, it.separatrices, 6);
            m.append(s.sprintf(" "));
        }
    }
}

void P4VFStudy::dump(QTextEdit &m)
{
    QString s;
    QByteArray ss;

    m.append(s.sprintf("General Info"));
    m.append(s.sprintf("------------"));
    m.append(s.sprintf("  Line at infinity singular? %d", singinf_));
    m.append(s.sprintf("  Direction of vector field: %d", dir_vec_field_));
    m.append(s.sprintf(" "));
    m.append(s.sprintf("Vector Fields"));
    m.append(s.sprintf("-------------"));
    m.append(s.sprintf("  Finite chart:"));
    m.append(s.sprintf("     x' = %s", dumpPoly2(f_vec_field_0_, "x", "y")));
    m.append(s.sprintf("     y' = %s", dumpPoly2(f_vec_field_1_, "x", "y")));
    m.append(s.sprintf("  U1 chart:"));
    m.append(s.sprintf("     x' = %s", dumpPoly2(vec_field_U1_0_, "x", "y")));
    m.append(s.sprintf("     y' = %s", dumpPoly2(vec_field_U1_1_, "x", "y")));
    m.append(s.sprintf("  U2 chart:"));
    m.append(s.sprintf("     x' = %s", dumpPoly2(vec_field_U2_0_, "x", "y")));
    m.append(s.sprintf("     y' = %s", dumpPoly2(vec_field_U2_1_, "x", "y")));
    m.append(s.sprintf("  V1 chart:"));
    m.append(s.sprintf("     x' = %s", dumpPoly2(vec_field_V1_0_, "x", "y")));
    m.append(s.sprintf("     y' = %s", dumpPoly2(vec_field_V1_1_, "x", "y")));
    m.append(s.sprintf("  V2 chart:"));
    m.append(s.sprintf("     x' = %s", dumpPoly2(vec_field_V2_0_, "x", "y")));
    m.append(s.sprintf("     y' = %s", dumpPoly2(vec_field_V2_1_, "x", "y")));
    m.append(s.sprintf("  Cylinder chart: (Co=cos(theta),Si=sin(theta))"));
    m.append(s.sprintf("     r'     = %s",
                       dumpPoly3(vec_field_C_0_, "r", "Co", "Si")));
    m.append(s.sprintf("     theta' = %s",
                       dumpPoly3(vec_field_C_1_, "r", "Co", "Si")));
    m.append(s.sprintf(" "));
    m.append(s.sprintf("Greatest Common Factor"));
    m.append(s.sprintf("----------------------"));
    m.append(s.sprintf("  Finite chart:   %s", dumpPoly2(gcf_, "x", "y")));
    m.append(s.sprintf("  U1 chart:       %s", dumpPoly2(gcf_U1_, "x", "y")));
    m.append(s.sprintf("  U2 chart:       %s", dumpPoly2(gcf_U2_, "x", "y")));
    m.append(s.sprintf("  V1 chart:       %s", dumpPoly2(gcf_V1_, "x", "y")));
    m.append(s.sprintf("  V2 chart:       %s", dumpPoly2(gcf_V2_, "x", "y")));
    m.append(
        s.sprintf("  Cylinder chart: %s", dumpPoly3(gcf_C_, "r", "Co", "Si")));
    m.append(s.sprintf(" "));
    m.append(s.sprintf("Singular points - summary"));
    m.append(s.sprintf("-------------------------"));
    m.append(s.sprintf(" "));
    dumpSingularities(m, saddlePoints_, false);
    dumpSingularities(m, sePoints_, false);
    dumpSingularities(m, nodePoints_, false);
    dumpSingularities(m, wfPoints_, false);
    dumpSingularities(m, sfPoints_, false);
    dumpSingularities(m, dePoints_, false);
    m.append(s.sprintf(" "));
    m.append(s.sprintf("Singular points - full description"));
    m.append(s.sprintf("----------------------------------"));
    m.append(s.sprintf(" "));
    dumpSingularities(m, saddlePoints_, true);
    dumpSingularities(m, sePoints_, true);
    dumpSingularities(m, nodePoints_, true);
    dumpSingularities(m, wfPoints_, true);
    dumpSingularities(m, sfPoints_, true);
    dumpSingularities(m, dePoints_, true);
}