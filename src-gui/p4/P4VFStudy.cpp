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

#include "P4VFStudy.h"
#include <QByteArray>
#include <QString>

// -----------------------------------------------------------------------
//          CONSTRUCTOR
// -----------------------------------------------------------------------
P4VFStudy::P4VFStudy(std::shared_ptr<P4ParentStudy> parent)
    : parent_{parent}, singinf_{false}, dir_vec_field_{1}
{
}

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
        if (fscanf(fpvec, "%d %d", &singinf_, &dir_vec_field_) != 2)
            return false;
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
                if (!readPoints(fpintf))
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
//                      QVFStudy::ReadIsoclines
// -----------------------------------------------------------------------
// FIXME posar a readTables (P4VFStudy)
bool QVFStudy::readIsoclines(QString basename)
{
    int N, degree_curve;
    FILE *fp = nullptr;
    setlocale(LC_ALL, "C");

    fp = fopen(QFile::encodeName(basename + "_vecisoclines.tab"), "rt");
    if (fp == nullptr) {
        dump(basename, "Cannot open file " + basename + "_vecisoclines.tab");
        return false;
    }

    isoclines new_isocline;
    if (fscanf(fp, "%d", &degree_curve) != 1)
        return false;

    if (degree_curve > 0) {
        if (fscanf(fp, "%d", &N) != 1)
            return false;

        // prepare a new isocline and link it to the list

        new_isocline.r2 = new term2;
        new_isocline.r2->next_term2 = nullptr;

        if (!readTerm2(fp, new_isocline.r2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_isocline.u1 = new term2;
        new_isocline.u1->next_term2 = nullptr;

        if (!readTerm2(fp, new_isocline.u1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_isocline.u2 = new term2;
        new_isocline.u2->next_term2 = nullptr;

        if (!readTerm2(fp, new_isocline.u2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_isocline.v1 = new term2;
        new_isocline.v1->next_term2 = nullptr;
        if (!readTerm2(fp, new_isocline.v1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;
        new_isocline.v2 = new term2;
        new_isocline.v2->next_term2 = nullptr;
        if (!readTerm2(fp, new_isocline.v2, N))
            return false;

        if (p_ != 1 || q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1)
                return false;

            new_isocline.c = new term3;
            new_isocline.c->next_term3 = nullptr;
            if (!readTerm3(fp, new_isocline.c, N))
                return false;
        } else {
            new_isocline.c = nullptr;
        }
    } else {
        return false;
    }

    isocline_vector_.push_back(new_isocline);
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
    p4singularities::saddle point;
    p4blowup::sep sep1;
    std::vector<p4blowup::sep> sepvec;

    if (fscanf(fp, "%lf %lf", &(point.x0), &(point.y0)) != 2 ||
        fscanf(fp, "%lf %lf %lf %lf", &(point.a11), &(point.a12), &(point.a21),
               &(point.a22)) != 4 ||
        !readVectorField(fp, point.vector_field_0, point.vector_field_1) ||
        fscanf(fp, "%d ", &(point.chart)) ||
        fscanf(fp, "%d ", &sep1.type) != 1 || fscanf(fp, "%d ", &N) != 1 ||
        N < 0)
        return false;

    sep1.notadummy = true;
    if (!readTerm1(fp, sep1.separatrice, N))
        return false;
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
        if (fscanf(fp, "%d", &(sep1.type)) != 1 || fscanf(fp, "%d", &N) != 1 ||
            N < 0)
            return false;
        sep1.notadummy = true;
        if (!readTerm1(fp, sep1.separatrice, N))
            return false;
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

// -----------------------------------------------------------------------
//          P4VFStudy::readSemiElementaryPoint
// -----------------------------------------------------------------------
P4VFStudy::readSemiElementaryPoint(FILE *fp)
{
    int s, N;
    p4singularities::semi_elementary point;
    std::vector<p4blowup::sep> sepvec;

    if (fscanf(fp, "%lf %lf", &(point.x0), &(point.y0)) != 2 ||
        fscanf(fp, "%lf %lf %lf %lf", &(point.a11), &(point.a12), &(point.a21),
               &(point.a22)) != 4 ||
        !readVectorField(fp, point.vector_field_0, point.vector_field_1) ||
        fscanf(fp, "%d %d %d", &(point.type), &s, &(point.chart)) != 3)
        return false;

    switch (point.type) {
    case 1:  // sadle-node
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
                if (!readTerm1(fp, sep1.separatrice, N))
                    return false;
                sep1.sep_points.clear();
                sepvec.push_back(sep1);

                sep1.direction = -1;
                sep1.notadummy = false;
                sepvec.push_back(sep1);
            }
            point.separatrices = sepvec;
        }
        break;
    case 2:  // saddle-node
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
        if (!readTerm1(fp, sep1.separatrice, N))
            return false;
        sep1.sep_points.clear();
        sepvec.push_back(sep1);

        if (point.chart == CHART_R2) {
            sep.type = STYPE_STABLE;
            sep1.d = 1;
            sep1.direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1.notadummy = true;
            if (!readTerm1(fp, sep1.separatrice, N))
                return false;
            sep1.sep_points.clear();
            sepvec.push_back(sep1);

            sep1.notadummy = false;
            sepvec.push_back(sep1);
        }
        point.separatrices = sepvec;
        break;
    case 3:  // saddle-node
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
        if (!readTerm1(fp, sep1.separatrice, N))
            return false;
        sep1.sep_points.clear();
        sepvec.push_back(sep1);

        if (point.chart == CHART_R2) {
            sep1.type = STYPE_UNSTABLE;
            sep1.d = 1;
            sep1.direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1.notadummy = true;
            if (!readTerm1(fp, sep1.separatrice, N))
                return false;
            sep1.sep_points.clear();
            sepvec.push_back(sep1);

            sep1.direction = -1;
            sep1.notadummy = false;
            sepvec.push_back(sep1);
        }
        point.separatrices = sepvec;
        break;
    case 4:  // saddle-node
        if (!s || point.chart == CHART_R2) {
            p4blowup::sep sep1;
            if (s)
                sep1.type = STYPE_CENUNSTABLE;
            else
                sep1.type = STYPE_STABLE;
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
            sep1.sep_points.clear();
            sepvec.push_back(sep1);
            if (point.chart == CHART_R2) {
                sep1.type = STYPE_STABLE;
                sep1.d = 1;
                sep1.direction = 1;
                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                sep1.notadummy = true;
                if (!readTerm1(fp, sep1.separatrice, N))
                    return false;
                sep1.sep_points.clear();
                sepvec.push_back(sep1);

                sep1.direction = -1;
                sep1.notadummy = false;
                sepvec.push_back(sep1);
            }
        }
        point.separatrices = sepvec;
        break;
    case 6:  // saddle
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
        if (!readTerm1(fp, sep1.separatrice, N))
            return false;
        sep1.sep_points.clear();
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
            if (!readTerm1(fp, sep1.separatrice, N))
                return false;
            sep1.sep_points.clear();
            sepvec.push_back(sep1);

            sep1.d = 1;
            sep1.notadummy = false;
            sepvec.push_back(sep1);
        }
        point.separatrices = sepvec;
        break;
    case 7:  // saddle
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
        if (!readTerm1(fp, sep1.separatrice, N))
            return false;
        sep1.sep_points.clear();
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
            if (!readTerm1(fp, sep1.separatrice, N))
                return false;
            sep1.sep_points.clear();
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
            if (point.type == 5)
                point.type = 8;
            else
                point.type = 5;
        }
    }
    if (fscanf(fp, "%lf", &(point.epsilon)) != 1)
        return false;
    point.notadummy = true;
    first_se_point_.push_back(point);
    // line at infinity a line of singularities in poincare disc
    if (singinf_ && point.chart != CHART_R2) {
        point.y0 = 0.0;
        if (dir_vec_field_ != 1) {
            int tp;
            switch (point.type) {
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
                tp = point.type;
            }
            point.type = tp;
        }
        point.chart = (point.chart == CHART_U1) ? CHART_V1 : CHART_V2;
        point.notadummy = false;
        point.vector_field_0.clear();
        point.vector_field_1.clear();
        first_se_point_.push_back(point);
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readNodePoint
// -----------------------------------------------------------------------
bool P4VFStudy::readNodePoint(FILE *fp)
{
    double y[2];

    p4singularities::node point;

    if (fscanf(fp, "%lf %lf %d ", &(point.x0), &(point.y0), &(point.stable)) !=
            3 ||
        fscanf(fp, "%d ", &(point.chart)) != 1)
        return false;

    // change type of node if we have a gcf?
    y[0] = point.x0;
    y[1] = point.y0;

    switch (point.chart) {
    case CHART_R2:
        if (eval_term2(gcf_, y) < 0)
            point.stable *= -1;
        break;
    case CHART_U1:
        if (eval_term2(gcf_U1_, y) < 0)
            point.stable *= -1;
        break;
    case CHART_V1:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V1_, y) < 0)
            point.stable *= -1;
        break;
    case CHART_U2:
        if (eval_term2(gcf_U2_, y) < 0)
            point.stable *= -1;
        break;
    case CHART_V2:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V2_, y) < 0)
            point.stable *= -1;
        break;
    }

    first_node_point_.push_back(point);

    // if line at infinity is a line of singularities in Poincare disk:
    // duplicate singularity using symmetry
    if (singinf_ != 0 && point.chart != CHART_R2) {
        point.y0 = 0.0;
        point.chart = ((point.chart == CHART_U1) ? CHART_V1 : CHART_V2);
        point.stable = point.stable * ((dir_vec_field_ == -1) ? -1 : 1);
        first_node_point_.push_back(point);
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readStrongFocusPoint
// -----------------------------------------------------------------------
bool P4VFStudy::readStrongFocusPoint(FILE *fp)
{
    double y[2];

    p4singularities::strong_focus point;

    if (fscanf(fp, "%d %lf %lf ", &(point.stable), &(point.x0), &(point.y0)) !=
            3 ||
        fscanf(fp, "%d ", &(point.chart)) != 1)
        return false;

    // change type of node if we have a gcf?
    y[0] = point.x0;
    y[1] = point.y0;

    switch (point.chart) {
    case CHART_R2:
        if (eval_term2(gcf_, y) < 0)
            point.stable *= -1;
        break;
    case CHART_U1:
        if (eval_term2(gcf_U1_, y) < 0)
            point.stable *= -1;
        break;
    case CHART_V1:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V1_, y) < 0)
            point.stable *= -1;
        break;
    case CHART_U2:
        if (eval_term2(gcf_U2_, y) < 0)
            point.stable *= -1;
        break;
    case CHART_V2:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V2_, y) < 0)
            point.stable *= -1;
        break;
    }

    first_sf_point_.push_back(point);

    // if line at infinity is a line of singularities in Poincare disk:
    // duplicate singularity using symmetry
    if (singinf_ && point.chart != CHART_R2) {
        point.y0 = 0.0;
        point.chart = ((point.chart == CHART_U1) ? CHART_V1 : CHART_V2);
        point.stable = point.stable * ((dir_vec_field_ == -1) ? -1 : 1);
        first_sf_point_.push_back(point);
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readWeakFocusPoint
// -----------------------------------------------------------------------
bool P4VFStudy::readWeakFocusPoint(FILE *fp)
{
    p4singularities::weak_focus point;
    double y[2];

    if (fscanf(fp, "%lf %lf ", &(point.x0), &(point.y0)) != 2 ||
        fscanf(fp "%d %d ", &(point.type), &(point.chart)) != 2)
        return false;

    // change node type if we have a gcf?

    if ((point.type == STYPE_STABLE) || (point.type == STYPE_UNSTABLE)) {
        y[0] = point.x0;
        y[1] = point.x1;
        switch (point.chart) {
        case CHART_R2:
            if (eval_term2(gcf_, y) < 0)
                point.type *= -1;
            break;
        case CHART_U1:
            if (eval_term2(gcf_U1_, y) < 0)
                point.type *= -1;
            break;
        case CHART_V1:
            if ((parent_->p_ == 1) && (parent_->q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V1_, y) < 0)
                point.type *= -1;
            break;
        case CHART_U2:
            if (eval_term2(gcf_U2_, y) < 0)
                point.type *= -1;
            break;
        case CHART_V2:
            if ((parent_->p_ == 1) && (parent_->q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V2_, y) < 0)
                point.type *= -1;
            break;
        }
    }
    first_wf_point_.push_back(point);

    // if line at infinity is a line of singularities in Poincare disk:
    // duplicate singularity using symmetry
    if (singinf_ && (point.chart != CHART_R2)) {
        if (dir_vec_field_ != 1) {
            switch (point.type) {
            case STYPE_STABLE:
                point.type = STYPE_UNSTABLE;
                break;
            case STYPE_UNSTABLE:
                point.type = STYPE_STABLE;
                break;
            }
        }
        point.chart = ((point.chart == CHART_U1) ? CHART_V1 : CHART_V2);
        first_wf_point_.push_back(point);
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readDegeneratePoint
// -----------------------------------------------------------------------
bool P4VFStudy::readDegeneratePoint(FILE *fp)
{
    int n;
    p4singularities::degenerate point;

    if (fscanf(fp, "%lf %lf %lf %d ", &(point.x0), &(point.y0),
               &(point.epsilon), &n) != 4)
        return false;
    if (n) {
        if (!readBlowUpPoints(fp, point.blow_up, n))
            return false;
        point.blow_up.last().integrating_in_local_chart = true;
    }
    if (fscanf(fp, "%d ", &(point.chart)) != 1)
        return false;
    point.notadummy = true;

    first_de_point_.push_back(point);

    // if line at infinity is a line of singularities in Poincare disk:
    // duplicate singularity using symmetry
    if (singinf_ && point.chart != CHART_R2) {
        point.y0 = 0.0;
        point.chart = ((point.chart == CHART_U1) ? CHART_V1 : CHART_V2);
        point.notadummy = false;
        first_de_point_.push_back(point);
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readTransformations
// -----------------------------------------------------------------------
bool P4VFStudy::readTransformations(
    FILE *fp, std::vector<p4blowup::transformations> &trans, int n)
{
    p4blowup::transformations tr;
    for (int i = 0; i < n; i++) {
        if (fscanf(fp, "%lf %lf %d %d %d %d %d %d %d", &(tr.x0), &(tr.y0),
                   &(tr.c1), &(tr.c2), &(tr.d1), &(tr.d2), &(tr.d3), &(tr.d4),
                   &(tr.d)) != 9)
            return false;
        trans.push_back(tr);
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readTransformations
// -----------------------------------------------------------------------
bool P4VFStudy::readBlowupPoints(FILE *fp,
                                 std::vector<p4blowup::blow_up_points> &blowup,
                                 int n)
{
    p4blowup::blow_up_points bl;
    int N, typ;

    for (int i = 0; i < n; i++) {
        if (fscanf(fp, "%d ", &(bl.n)) != 1 ||
            !readTransformations(fp, bl.trans, bl.n) ||
            fscanf(fp, "%lf %lf ", &(bl.x0), &(bl.y0)) != 2 ||
            fscanf(fp, "%lf %lf %lf %lf ", &(bl.a11), &(bl.a12), &(bl.a21),
                   &(bl.a22)) != 4 ||
            !readVectorField(fp, bl.vector_field_0, bl.vector_field_1) ||
            fscanf(fp, "%d ", &N) != 1 || !readTerm1(fp, bl.sep, N) ||
            fscanf(fp, "%d ", &typ) != 1)
            return false;

        switch (typ) {
        case 1:
        case 3:
        case 9:
        case 10:
            bl.type = STYPE_UNSTABLE;
            break;
        case 2:
        case 4:
        case 11:
        case 12:
            bl.type = STYPE_STABLE;
            break;
        case 5:
        case 7:
            bl.type = STYPE_CENUNSTABLE;
            break;
        case 6:
        case 8:
            bl.type = STYPE_CENSTABLE;
            break;
        default:
            return false;
        }
        blowup.push_back(bl);
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
    char *stype;
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
        m.append(s.sprintf("%sType=%s  Dir=%-2d  original=%d", smargin, stype,
                           it.direction, it.notadummy));
        if (it.d == 0)
            m.append(s.sprintf("%sTaylor: (x,y)=(t,%s)", smargin,
                               dumpPoly1(it.separatrice, "t")));
        else
            m.append(s.sprintf("%sTaylor: (x,y)=(%s,t)", smargin,
                               dumpPoly1(separ->separatrice, "t")));
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
        m.append(s.sprintf("%s (x0,y0)=(%g,%g)  Chart %s", type, it.x0, it.y0,
                           chart));
        if (longversion) {
            m.append(s.sprintf("   Epsilon = %g  original=%d", it.epsilon,
                               it.notadummy));
            m.append(
                s.sprintf("   Transformation Matrix = [ [%g,%g], [%g,%g] ]",
                          it.a11, it.a12, it.a21, it.a22));
            m.append(s.sprintf("   Vector Field:"));
            m.append(s.sprintf("      x' = %s",
                               dumpPoly2(it.vector_field_0_, "x", "y")));
            m.append(s.sprintf("      y' = %s",
                               dumpPoly2(it.vector_field_1_, "x", "y")));
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
        m.append(s.sprintf("%s (x0,y0)=(%g,%g)  Chart %s", type, it.x0, it.y0,
                           chart));
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
        m.append(s.sprintf("%s (x0,y0)=(%g,%g)  Chart %s", type, it.x0, it.y0,
                           chart));
        if (longversion) {
            if (sf.stable == -1)
                s = "(stable)";
            else if (sf.stable == +1)
                s = "(unstable)";
            else
                s = "( ??? )";
            ss = s.toAscii();
            m.append(s.sprintf("    Stability = %d %s", sf.stable,
                               (const char *)ss));
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
        m.append(s.sprintf("%s (x0,y0)=(%g,%g)  Chart %s", type, it.x0, it.y0,
                           chart));
        if (longversion) {
            m.append(s.sprintf("    Type = %d", wf.type));
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
        m.append(s.sprintf("%s (x0,y0)=(%g,%g)  Chart %s", type, it.x0, it.y0,
                           chart));
        if (longversion) {
            if (no.stable == -1)
                s = "(stable)";
            else if (no.stable == +1)
                s = "(unstable)";
            else
                s = "( ??? )";
            ss = s.toAscii();
            m.append(s.sprintf("    Stability = %d %s", no.stable,
                               (const char *)ss));
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
        m.append(s.sprintf("%s (x0,y0)=(%g,%g)  Chart %s", type, it.x0, it.y0,
                           chart));
        if (longversion) {
            m.append(s.sprintf("   Type    = %d", se.type));
            m.append(s.sprintf("   Epsilon = %g  original=%d", se.epsilon,
                               se.notadummy));
            m.append(
                s.sprintf("   Transformation Matrix = [ [%g,%g], [%g,%g] ]",
                          se.a11, se.a12, se.a21, se.a22));
            m.append(s.sprintf("   Vector Field = "));
            m.append(s.sprintf("      x' = %s",
                               dumpPoly2(se.vector_field_0_, "x", "y")));
            m.append(s.sprintf("      y' = %s",
                               dumpPoly2(se.vector_field_1_, "x", "y")));
            m.append(s.sprintf("   Separatrices:"));
            dumpSeparatrices(m, se.separatrices, 6);
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
    dumpSingularities(m, first_saddle_point_, "SADDLE         ", false);
    dumpSingularities(m, first_se_point_, "SEMI-ELEMENTARY", false);
    dumpSingularities(m, first_node_point_, "NODE           ", false);
    dumpSingularities(m, first_wf_point_, "WEAK FOCUS     ", false);
    dumpSingularities(m, first_sf_point_, "STRONG FOCUS   ", false);
    dumpSingularities(m, first_de_point_, "DEGENERATE     ", false);
    m.append(s.sprintf(" "));
    m.append(s.sprintf("Singular points - full description"));
    m.append(s.sprintf("----------------------------------"));
    m.append(s.sprintf(" "));
    dumpSingularities(m, first_saddle_point_, "SADDLE         ", true);
    dumpSingularities(m, first_se_point_, "SEMI-ELEMENTARY", true);
    dumpSingularities(m, first_node_point_, "NODE           ", true);
    dumpSingularities(m, first_wf_point_, "WEAK FOCUS     ", true);
    dumpSingularities(m, first_sf_point_, "STRONG FOCUS   ", true);
    dumpSingularities(m, first_de_point_, "DEGENERATE     ", true);
}