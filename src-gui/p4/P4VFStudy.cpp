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

#include "P4VFStudy.h"

// -----------------------------------------------------------------------
//          CONSTRUCTOR
// -----------------------------------------------------------------------
P4VFStudy::P4VFStudy()
{
    parent_ = nullptr;

    // initialize vector field structures:
    f_vec_field_[0] = nullptr;
    f_vec_field_[1] = nullptr;
    vec_field_U1_[0] = nullptr;
    vec_field_U1_[1] = nullptr;
    vec_field_U2_[0] = nullptr;
    vec_field_U2_[1] = nullptr;
    vec_field_V1_[0] = nullptr;
    vec_field_V1_[1] = nullptr;
    vec_field_V2_[0] = nullptr;
    vec_field_V2_[1] = nullptr;
    vec_field_C_[0] = nullptr;
    vec_field_C_[1] = nullptr;

    // initialize others
    singinf_ = false;
    dir_vec_field_ = 1;
}

// -----------------------------------------------------------------------
//          DESTRUCTOR
// -----------------------------------------------------------------------
P4VFStudy::~P4VFStudy() { reset(); }

// -----------------------------------------------------------------------
//          P4VFStudy::reset
// -----------------------------------------------------------------------
void P4VFStudy::reset()
{
    parent_ = nullptr;

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
    first_saddle_point_.clear();
    first_se_point_.clear();
    first_node_point_.clear();
    first_de_point_.clear();
    first_sf_point_.clear();
    first_wf_point_.clear();

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
//          P4VFStudy::readTables FIXME
// -----------------------------------------------------------------------
bool P4VFStudy::readTables(FILE *fpvec, FILE *fpfin, FILE *fpinf)
{
    int j;
    if (!readGCF(fpvec))
        return false;
    if (!readVectorField(fpvec, f_vec_field_0_, f_vec_field_1_))
        return false;
    if (!readVectorField(fpvec, vec_field_U1_0_, vec_field_U1_1_))
        return false;
    if (!readVectorField(fpvec, vec_field_V1_0_, vec_field_V1_1_))
        return false;
    if (!readVectorField(fpvec, vec_field_U2_0_, vec_field_U2_1_))
        return false;
    if (!readVectorField(fpvec, vec_field_V2_0_, vec_field_V2_1_))
        return false;

    if (parent_->plweights_) {
        if (!readVectorFieldCylinder(fpvec, vec_field_C_0_, vec_field_C_1_))
            return false;
        singinf_ = 0;
    } else {
        if (fscanf(fpvec, "%d %d", &singinf, &dir_vec_field_) != 2)
            return false;
    }

    if (fpfin != nullptr) {
        if (!readPoints(fpfin))
            return false;
    }
}

// -----------------------------------------------------------------------
//          P4VFStudy::readVectorField
// -----------------------------------------------------------------------
bool P4VFStudy::readVectorField(FILE *fp, std::vector<p4polynom::term2> &vf0,
                                std::vector<p4polynom::term2> &vf1)
{
    int M, N;

    if (fscanf(fp, "%d", &M) != 1 || M < 0)
        return false;
    if (!readTerm2(fp, vf0, M))
        return false;
    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;
    if (!readTerm2(fp, vf1, N))
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
    int N;

    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;
    if (!readTerm3(fp, vf0, N))
        return false;
    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;
    if (!readTerm3(fp, vf1, N))
        return false;

    return true;
}

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
        }
    }
}

// FIXME no estic segur de que això funcioni bé
bool P4VFStudy::readSaddlePoint(FILE *fp)
{
    int N;
    p4singularities::saddle point;
    p4blowup::sep sep1;
    std::vector<p4blowup::sep> sepvec;

    if (fscanf(fp, "%lf %lf", &(point.x0), &(point.y0)) != 2)
        return false;
    if (fscanf(fp, "%lf %lf %lf %lf", &(point.a11), &(point.a12), &(point.a21),
               &(point.a22)) != 4)
        return false;
    if (!readVectorField(fp, point.vector_field_0, point.vector_field_1))
        return false;
    if (fscanf(fp, "%d ", &(point.chart)))
        return false;

    if (fscanf(fp, "%d ", &sep1.type) != 1)
        return false;
    if (fscanf(fp, "%d ", &N) != 1 || N < 0)
        return false;

    sep1.notadummy = true;
    readTerm1(fp, sep1.separatrice, N);
    sep1.direction = 1;
    sep1.d = 0;
    sepvec.push_back(sep1);

    if (point.chart == CHART_R2 || singinf_) {
        // point is finite hence we have 4 separatrices or we have a line of
        // singularities at infinity and hence we have also 4 separatrices if we
        // delete the line
        // #2
        sep1.direction = -1;
        sep1.d = 0;
        sep1.notadummy = false;
        sepvec.push_back(sep1);
        // #3
        if (fscanf(fp, "%d", &(sep1.type)) != 1)
            return false;
        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        sep1.notadummy = true;
        readTerm1(fp, sep1.separatrice, N);
        sep1.direction = 1;
        sep1.d = 1;
        sepvec.push_back(sep1);
        // #4
        sep1.direction = -1;
        sep1.d = 1;
        sep1.notadummy = false;
        sepvec.push_back(sep1);
    }
    point.separatrices = sepvec;
    fscanf(fp, "%lf ", &(point.epsilon));
    point.notadummy = true;

    first_saddle_point_.push_back(point);

    // line at infinity a line of singularities in poincare disc
    if (singinf_ && point.chart != CHART_R2) {
        point.y0 = 0.0;
        point.chart = ((point.chart == CHART_U1) ? CHART_V1 : CHART_V2);
        point.notadummy = false;
        point.vector_field_0.clear();
        point.vector_field_1.clear();
        first_saddle_point_.push_back(point);
    }
    return true;
}

P4VFStudy::readSemiElementaryPoint(FILE *fp)
{
    int s, N;
    p4singularities::semi_elementary point;
    std::vector<p4blowup::sep> sepvec;

    if (fscanf(fp, "%lf %lf", &(point.x0), &(point.y0)) != 2)
        return false;
    if (fscanf(fp, "%lf %lf %lf %lf", &(point.a11), &(point.a12), &(point.a21),
               &(point.a22)) != 4)
        return false;
    readVectorField(fp, point.vector_field_0, point.vector_field_1);
    if (fscanf(fp, "%d %d %d", &(point.type), &s, &(point.chart)) != 3)
        return false;

    switch (point.type) {
    case 1: // sadle-node
        p4blowup::sep sep1;
        if (!s || (point.chart == CHART_R2)) {
            if (s)
                sep1.type = OT_CENT_STABLE;
            else
                sep1.type = OT_UNSTABLE;
            sep1.d = 0;
            if (((parent_->p_ == 1) && (parent_->q_ == 1) &&
                 ((point.chart == CHART_V1) || (point.chart == CHART_V2))) ||
                (point.chart == CHART_R2))
                sep1.direction = -1;
            else
                sep1.direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1.notadummy = true;
            if (!readTerm1(fp, sep1.separatrice, N))
                return false;

            sepvec.push_back(sep1);

            if (point.chart == CHART_R2) {
                sep1.type = OT_UNSTABLE;
                sep1.d = 1;
                sep1.direction = 1;
                if (fscanf(fd, "%d", &N) != 1)
                    return false;
                sep1.notadummy = true;
                readTerm1(fp, sep1.separatrice, N);
                sep1.first_sep_point.clear();
                sep1.last_sep_point.clear();
                sepvec.push_back(sep1);

                sep1.direction = -1;
                sep1.notadummy = false;
                sepvec.push_back(sep1);
            }
            point.separatrices = sepvec;
            break;
        case 2: // saddle-node
            p4blowup::sep sep1;
            if (s)
                sep1.type = STYPE_CENUNSTABLE;
            else
                sep1.type = STYPE_STABLE;
            sep1.d = 0;
            if ((parent_->p_ == 1) && (parent_->q_ == 1) &&
                ((point.chart == CHART_V1) || (point.chart == CHART_V2)))
                sep1.direction = -1;
            else
                sep1.direction = 1;

            if (fscanf(fp, "%d", &N) != 1)
                return false;
            sep1.notadummy = true;
            readTerm1(fp, sep1.separatrice, N);
            sep1.first_sep_point.clear();
            sep1.last_sep_point.clear();
            sepvec.push_back(sep1);

            if (point.chart == CHART_R2) {
                sep.type = STYPE_STABLE;
                sep1.d = 1;
                sep1.direction = 1;
                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                sep1.notadummy = true;
                readTerm1(fp, sep1.separatrice, N);
                sep1.first_sep_point.clear();
                sep1.last_sep_point.clear();
                sepvec.push_back(sep1);

                sep1.notadummy = false;
                sepvec.push_back(sep1);
            }
            point.separatrices = sepvec;
            break;
        case 3: // saddle-node
            p4blowup::sep sep1;
            if (s)
                sep1.type = STYPE_CENSTABLE;
            else
                sep1.type = STYPE_UNSTABLE;
            sep1.d = 0;
            if ((parent_->p_ == 1) && (parent_->q_ == 1) &&
                ((point.chart == CHART_V1) || (point.chart == CHART_V2)))
                sep1.direction = -1;
            else
                sep1.direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1.notadummy = true;
            readTerm1(fp, sep1.separatrice, N);
            sep1.first_sep_point.clear();
            sep1.last_sep_point.clear();
            sepvec.push_back(sep1);

            if (point.chart == CHART_R2) {
                sep1.type = STYPE_UNSTABLE;
                sep1.d = 1;
                sep1.direction = 1;
                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                sep1.notadummy = true;
                readTerm1(fp, sep1.separatrice, N);
                sep1.first_sep_point.clear();
                sep1.last_sep_point.clear();
                sepvec.push_back(sep1);

                sep1.direction = -1;
                sep1.notadummy = false;
                sepvec.push_back(sep1);
            }
            point.separatrices = sepvec;
            break;
        case 4: // saddle-node
            if (!s || point.chart == CHART_R2) {
                p4blowup::sep sep1;
                if (s)
                    sep1.type = STYPE_CENUNSTABLE;
                else
                    sep1.type = STYPE_STABLE;
                sep1.d = 0;
                if (((parent_->p_ == 1) && (parent_->q_ == 1) &&
                     ((point.chart == CHART_V1) ||
                      (point.chart == CHART_V2))) ||
                    (point.chart == CHART_R2))
                    sep1.direction = -1;
                else
                    sep1.direction = 1;

                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                sep1.notadummy = true;
                readTerm1(fp, sep1.separatrice, N);
                sep1.first_sep_point.clear();
                sep1.last_sep_point.clear();
                sepvec.push_back(sep1);
                if (point.chart == CHART_R2) {
                    sep1.type = STYPE_STABLE;
                    sep1.d = 1;
                    sep1.direction = 1;
                    if (fscanf(fp, "%d ", &N) != 1)
                        return false;
                    sep1.notadummy = true;
                    readTerm1(fp, sep1.separatrice, N);
                    sep1.first_sep_point.clear();
                    sep1.last_sep_point.clear();
                    sepvec.push_back(sep1);

                    sep1.direction = -1;
                    sep1.notadummy = false;
                    sepvec.push_back(sep1);
                }
            }
            point.separatrices = sepvec;
            break;
        case 6: // saddle
            p4blowup::sep sep1;
            if (s)
                sep1.type = STYPE_CENUNSTABLE;
            else
                sep1.type = STYPE_STABLE;
            sep1.d = 0;
            if ((parent_->p_ == 1) && (parent_->q_ == 1) &&
                ((point.chart == CHART_V1) || (point.chart == CHART_V2)))
                sep1.direction = -1;
            else
                sep1.direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1.notadummy = true;
            readTerm1(fp, sep1.separatrice, N);
            sep1.first_sep_point.clear();
            sep1.last_sep_point.clear();
            sepvec.push_back(sep1);

            if (point.chart == CHART_R2) {
                sep1.type = STYPE_CENUNSTABLE;
                sep1.d = 0;
                sep1.direction = -1;
                sep1.notadummy = false;
                sepvec.push_back(sep1);

                sep1.type = STYPE_STABLE;
                sep1.d = 1;
                sep1.direction = 1;
                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                sep1.notadummy = true;
                readTerm1(fp, sep1.separatrice, N);
                sep1.first_sep_point.clear();
                sep1.last_sep_point.clear();
                sepvec.push_back(sep1);

                sep1.d = 1;
                sep1.notadummy = false;
                sepvec.push_back(sep1);
            }
            point.separatrices = sepvec;
            break;
        case 7: // saddle
            p4blowup::sep sep1;
            if (s)
                sep1.type = STYPE_CENSTABLE;
            else
                sep1.type = STYPE_UNSTABLE;
            sep1.d = 0;
            if ((parent_->p_ == 1) && (parent_->q_ == 1) &&
                ((point.chart == CHART_V1) || (point.chart == CHART_V2)))
                sep1.direction = -1;
            else
                sep1.direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1.notadummy = true;
            readTerm1(fp, sep1.separatrice, N);
            sep1.first_sep_point.clear();
            sep1.last_sep_point.clear();
            sepvec.push_back(sep1);

            if (point.chart == CHART_R2) {
                sep1.type = STYPE_CENSTABLE;
                sep1.d = 0;
                sep1.direction = -1;
                sep1.notadummy = false;
                sepvec.push_back(sep1);

                sep1.type = STYPE_UNSTABLE;
                sep1.d = 1;
                sep1.direction = 1;
                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                sep1.notadummy = true;
                readTerm1(fp, sep1.separatrice, N);
                sep1.first_sep_point.clear();
                sep1.last_sep_point.clear();
                sepvec.push_back(sep1);

                sep1.direction = -1;
                sep1.notadummy = false;
                sepvec.push_back(sep1);
            }

            point.separatrices = sepvec;
            break;
        default:
            // change type of node if we have a gcf ?
            double y[2] = {point.x0, point.y0};
            bool ok = false;
            switch (point.chart) {
            case CHART_R2:
                if (eval_term2(gcf_, y) < 0)
                    ok = true;
                break;
            case CHART_U1:
                if (eval_term2(gcf_U1_, y) < 0)
                    ok = true;
                break;
            }
        }
    }
}