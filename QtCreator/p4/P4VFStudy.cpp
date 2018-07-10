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
    delete f_vec_field_[0];
    delete f_vec_field_[1];
    delete vec_field_U1_[0];
    delete vec_field_U1_[1];
    delete vec_field_U2_[0];
    delete vec_field_U2_[1];
    delete vec_field_V1_[0];
    delete vec_field_V1_[1];
    delete vec_field_V2_[0];
    delete vec_field_V2_[1];
    delete vec_field_C_[0];
    delete vec_field_C_[1];

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

    // Delete singular points
    delete firstSaddlePoint_;
    delete firstSePoint_;
    delete firstNodePoint_;
    delete firstDePoint_;
    delete firstSfPoint_;
    delete firstWfPoint_;

    firstSaddlePoint_ = nullptr;
    firstSePoint_ = nullptr;
    firstNodePoint_ = nullptr;
    firstDePoint_ = nullptr;
    firstSfPoint_ = nullptr;
    firstWfPoint_ = nullptr;

    // Delete GCF:
    delete gcf_;
    delete gcf_U1_;
    delete gcf_U2_;
    delete gcf_V1_;
    delete gcf_V2_;
    delete gcf_C_;
    delete gcf_points_;

    gcf_ = nullptr;
    gcf_U1_ = nullptr;
    gcf_U2_ = nullptr;
    gcf_V1_ = nullptr;
    gcf_V2_ = nullptr;
    gcf_C_ = nullptr;
    gcf_points_ = nullptr;

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
    if (!readGCF(fpvec) || !readVectorField(fpvec, f_vec_field_) ||
        !readVectorField(fpvec, vec_field_U1_) ||
        !readVectorField(fpvec, vec_field_V1_) ||
        !readVectorField(fpvec, vec_field_U2_) ||
        !readVectorField(fpvec, vec_field_V2_))
        return false;

    if (parent_->plweights_) {
        if (!readVectorFieldCylinder(fpvec, vec_field_C_))
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
#ifndef Q_OS_DARWIN
    setlocale(LC_ALL, "C");
#endif

    fp = fopen(QFile::encodeName(basename + "_vecisoclines.tab"), "rt");
    if (fp == nullptr) {
        // dump(basename, "Cannot open file " + basename + "_vecisoclines.tab");
        return false;
    }

    P4Curves::isoclines new_isocline;
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
bool P4VFStudy::readVectorField(FILE *fp, P4Polynom::term2 *vf[2])
{
    int M, N;

    vf[0] = new P4Polynom::term2;
    vf[1] = new P4Polynom::term2;

    if (fscanf(fp, "%d", &M) != 1 || M < 0 || !readTerm2(fp, vf[0], M) ||
        fscanf(fp, "%d", &N) != 1 || N < 0 || !readTerm2(fp, vf[1], N))
        return false;

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readVectorFieldCylinder
// -----------------------------------------------------------------------
bool P4VFStudy::readVectorFieldCylinder(FILE *fp, P4Polynom::term3 *vf[2])
{
    int M, N;

    vf[0] = new P4Polynom::term3;
    vf[1] = new P4Polynom::term3;

    if (fscanf(fp, "%d", &M) != 1 || M < 0 || !readTerm3(fp, vf[0], M) ||
        fscanf(fp, "%d", &N) != 1 || N < 0 || !readTerm3(fp, vf[1], N))
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
        case P4SingularityType::saddle:
            if (!readSaddlePoint(fp))
                return false;
            break;
        case P4SingularityType::semi_hyperbolic:
            if (!readSemiElementaryPoint(fp))
                return false;
            break;
        case P4SingularityType::node:
            if (!readNodePoint(fp))
                return false;
            break;
        case P4SingularityType::strong_focus:
            if (!readStrongFocusPoint(fp))
                return false;
            break;
        case P4SingularityType::weak_focus:
            if (!readWeakFocusPoint(fp))
                return false;
            break;
        case P4SingularityType::non_elementary:
            if (!readDegeneratePoint(fp))
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

    P4Blowup::sep *sep2{nullptr};
    P4Singularities::saddle *last{nullptr}, *point{firstSaddlePoint_};

    while (point != nullptr) {
        last = point;
        point = point->next_saddle;
    }
    point = new P4Singularities::saddle;
    if (last == nullptr)
        firstSaddlePoint_ = point;
    else
        last->next_saddle = point;

    // Create sep with:
    //   type: 0 (will be read from file)
    //   direction: 1
    //   d: 0
    //   notadummy: true
    //   separatrice: new P4Polynom::term1 (will be read from file)
    //   first_sep_point: nullptr
    //   last_sep_point: nullptr
    //   next_sep: nullptr
    point->separatrices =
        new P4Blowup::sep{0, 1, 0, true, new P4Polynom::term1};
    auto sep1 = point->separatrices;

    // fill structure
    if (fscanf(fp, "%lf %lf", &(point->x0), &(point->y0)) != 2 ||
        fscanf(fp, "%lf %lf %lf %lf", &(point->a11), &(point->a12),
               &(point->a21), &(point->a22)) != 4 ||
        !readVectorField(fp, point->vector_field) ||
        fscanf(fp, "%d ", &(point->chart)) != 1 ||
        fscanf(fp, "%d ", &(sep1->type)) != 1 || fscanf(fp, "%d ", &N) != 1 ||
        N < 0 || !readTerm1(fp, sep1->separatrice, N)) {
        delete point;
        point = nullptr;
        return false;
    }

    if (point->chart == P4Charts::chart_R2 || singinf_) {
        // point is finite hence we have 4 separatrices or we have a line of
        // singularities at infinity and hence we have also 4 separatrices if we
        // delete the line

        // Create sep (will not outline each member in the constructor anymore)
        sep2 = new P4Blowup::sep{sep1->type, -1, 0, false, sep1->separatrice};
        sep1->next_sep = sep2;

        sep1 = sep2->next_sep =
            new P4Blowup::sep{0, 1, 1, true, new P4Polynom::term1};
        if (fscanf(fp, "%d", &(sep1->type)) != 1 || fscanf(fp, "%d", &N) != 1 ||
            N < 0 || !readTerm1(fp, sep1->separatrice, N)) {
            delete point;
            point = nullptr;
            return false;
        }

        sep2 = new P4Blowup::sep{sep1->type, -1, 1, false, sep1->separatrice};
        sep1->next_sep = sep2;
    }

    if (fscanf(fp, "%lf ", &(point->epsilon)) != 1) {
        delete point;
        point = nullptr;
        return false;
    }

    // line at infinity a line of singularities in poincare disc

    if (singinf_ && point->chart != P4Charts::chart_R2) {
        last = point;
        point = new P4Singularities::saddle{last->x0,
                                            0.0,
                                            ((last->chart == P4Charts::chart_U1)
                                                 ? P4Charts::chart_V1
                                                 : P4Charts::chart_V2),
                                            0,
                                            last->epsilon,
                                            false,
                                            last->a11,
                                            last->a12,
                                            last->a21,
                                            last->a22,
                                            last->separatrices};
        last->next_saddle = point;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readSemiElementaryPoint
// -----------------------------------------------------------------------
bool P4VFStudy::readSemiElementaryPoint(FILE *fp)
{
    int s, N, typ, dir;
    double y[2];
    bool ok;

    P4Singularities::semi_elementary *last{nullptr}, *point{firstSePoint_};
    P4Blowup::sep *sep1{nullptr};

    while (point != nullptr) {
        last = point;
        point = point->next_se;
    }
    point = new P4Singularities::semi_elementary;
    if (last == nullptr)
        firstSePoint_ = point;
    else
        last->next_se = point;

    if (fscanf(fp, "%lf %lf ", &(point->x0), &(point->y0)) != 2 ||
        fscanf(fp, "%lf %lf %lf %lf ", &(point->a11), &(point->a12),
               &(point->a21), &(point->a22)) != 4 ||
        !readVectorField(fp, point->vector_field) ||
        fscanf(fp, "%d %d %d", &(point->type), &s, &(point->chart)) != 3) {
        delete point;
        point = nullptr;
        return false;
    }

    if ((parent_->p_ == 1) && (parent_->q_ == 1) &&
        ((point->chart == P4Charts::chart_V1) ||
         (point->chart == P4Charts::chart_V2))) {
        dir = -1;
    } else {
        dir = 1;
    }

    switch (point->type) {

    case 1: // saddle-node
        if (!s || (point->chart == P4Charts::chart_R2)) {

            if (s) {
                typ = P4SeparatriceType::cent_stable;
            } else {
                typ = P4SeparatriceType::unstable;
            }

            point->separatrices =
                new P4Blowup::sep{typ, dir, 0, true, new P4Polynom::term1};
            sep1 = point->separatrices;

            if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
                !readTerm1(fp, sep1->separatrice, N)) {
                delete point;
                point = nullptr;
                return false;
            }

            if (point->chart == P4Charts::chart_R2) {
                sep1->next_sep =
                    new P4Blowup::sep{P4SeparatriceType::unstable, 1, 1, true,
                                      new P4Polynom::term1};
                sep1 = sep1->next_sep;
                if (!readTerm1(fp, sep1->separatrice, N)) {
                    delete point;
                    point = nullptr;
                    return false;
                }
                sep1->next_sep =
                    new P4Blowup::sep{P4SeparatriceType::unstable, -1, 1, false,
                                      sep1->separatrice};
            }
        }
        break;

    case 2: // saddle-node
        if (s) {
            typ = P4SeparatriceType::cent_unstable;
        } else {
            typ = P4SeparatriceType::stable;
        }

        point->separatrices =
            new P4Blowup::sep{typ, dir, 0, true, new P4Polynom::term1};
        sep1 = point->separatrices;

        if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
            !readTerm1(fp, sep1->separatrice, N)) {
            delete point;
            point = nullptr;
            return false;
        }

        if (point->chart == P4Charts::chart_R2) {
            sep1->next_sep = new P4Blowup::sep{P4SeparatriceType::stable, 1, 1,
                                               true, new P4Polynom::term1};
            sep1 = sep1->next_sep;
            if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
                !readTerm1(fp, sep1->separatrice, N)) {
                delete point;
                point = nullptr;
                return false;
            }
            sep1->next_sep = new P4Blowup::sep{P4SeparatriceType::stable, -1, 1,
                                               false, sep1->separatrice};
        }
        break;

    case 3: // saddle-node
        if (s) {
            typ = P4SeparatriceType::cent_stable;
        } else {
            typ = P4SeparatriceType::unstable;
        }

        point->separatrices =
            new P4Blowup::sep{typ, dir, 0, true, new P4Polynom::term1};
        sep1 = point->separatrices;

        if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
            !readTerm1(fp, sep1->separatrice, N)) {
            delete point;
            point = nullptr;
            return false;
        }

        if (point->chart == P4Charts::chart_R2) {
            sep1->next_sep = new P4Blowup::sep{P4SeparatriceType::unstable, 1,
                                               1, true, new P4Polynom::term1};
            sep1 = sep1->next_sep;
            if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
                !readTerm1(fp, sep1->separatrice, N)) {
                delete point;
                point = nullptr;
                return false;
            }
            sep1->next_sep = new P4Blowup::sep{P4SeparatriceType::unstable, -1,
                                               1, false, sep1->separatrice};
        }
        break;

    case 4: // saddle-node
        if (!s || (point->chart == P4Charts::chart_R2)) {

            if (s) {
                typ = P4SeparatriceType::cent_unstable;
            } else {
                typ = P4SeparatriceType::stable;
            }

            point->separatrices =
                new P4Blowup::sep{typ, dir, 0, true, new P4Polynom::term1};
            sep1 = point->separatrices;

            if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
                !readTerm1(fp, sep1->separatrice, N)) {
                delete point;
                point = nullptr;
                return false;
            }

            if (point->chart == P4Charts::chart_R2) {
                sep1->next_sep =
                    new P4Blowup::sep{P4SeparatriceType::stable, 1, 1, true,
                                      new P4Polynom::term1};
                sep1 = sep1->next_sep;
                if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
                    !readTerm1(fp, sep1->separatrice, N)) {
                    delete point;
                    point = nullptr;
                    return false;
                }

                sep1->next_sep = new P4Blowup::sep{
                    P4SeparatriceType::stable, -1, 1, false, sep1->separatrice};
            }
        }
        break;

    case 6: // saddle
        if (s) {
            typ = P4SeparatriceType::cent_unstable;
        } else {
            typ = P4SeparatriceType::stable;
        }

        point->separatrices =
            new P4Blowup::sep{typ, dir, 0, true, new P4Polynom::term1};
        sep1 = point->separatrices;

        if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
            !readTerm1(fp, sep1->separatrice, N)) {
            delete point;
            point = nullptr;
            return false;
        }

        if (point->chart == P4Charts::chart_R2) {
            sep1->next_sep = new P4Blowup::sep{P4SeparatriceType::cent_unstable,
                                               -1, 0, false, sep1->separatrice};
            sep1->next_sep->next_sep = new P4Blowup::sep{
                P4SeparatriceType::stable, 1, 1, true, new P4Polynom::term1};
            sep1 = sep1->next_sep->next_sep;

            if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
                !readTerm1(fp, sep1->separatrice, N)) {
                delete point;
                point = nullptr;
                return false;
            }

            sep1->next_sep = new P4Blowup::sep{P4SeparatriceType::stable, -1, 1,
                                               false, sep1->separatrice};
        }
        break;

    case 7: // saddle
        if (s) {
            typ = P4SeparatriceType::cent_stable;
        } else {
            typ = P4SeparatriceType::unstable;
        }

        point->separatrices =
            new P4Blowup::sep{typ, dir, 0, true, new P4Polynom::term1};
        sep1 = point->separatrices;

        if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
            !readTerm1(fp, sep1->separatrice, N)) {
            delete point;
            point = nullptr;
            return false;
        }

        if (point->chart == P4Charts::chart_R2) {
            sep1->next_sep = new P4Blowup::sep{P4SeparatriceType::cent_stable,
                                               -1, 0, false, sep1->separatrice};
            sep1->next_sep->next_sep = new P4Blowup::sep{
                P4SeparatriceType::unstable, 1, 1, true, new P4Polynom::term1};
            sep1 = sep1->next_sep->next_sep;

            if (fscanf(fp, "%d", &N) != 1 || N < 0 ||
                !readTerm1(fp, sep1->separatrice, N)) {
                delete point;
                point = nullptr;
                return false;
            }

            sep1->next_sep = new P4Blowup::sep{P4SeparatriceType::unstable, -1,
                                               1, false, sep1->separatrice};
        }
        break;

    default:
        point->separatrices = nullptr;

        // change type of node if we have a gcf ?
        y[0] = point->x0;
        y[1] = point->y0;
        ok = false;

        switch (point->chart) {
        case P4Charts::chart_R2:
            if (eval_term2(gcf_, y) < 0)
                ok = true;
            break;
        case P4Charts::chart_U1:
            if (eval_term2(gcf_U1_, y) < 0)
                ok = true;
            break;
        case P4Charts::chart_V1:
            if ((parent_->p_ == 1) && (parent_->q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V1_, y) < 0)
                ok = true;
            break;
        case P4Charts::chart_U2:
            if (eval_term2(gcf_U2_, y) < 0)
                ok = true;
            break;
        case P4Charts::chart_V2:
            if ((parent_->p_ == 1) && (parent_->q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_U2_, y) < 0)
                ok = true;
            break;
        }
        if (ok) {
            if (point->type == 5)
                point->type = 8;
            else
                point->type = 5;
        }
    }

    if (fscanf(fp, "%lf ", &(point->epsilon)) != 1) {
        delete point;
        point = nullptr;
        return false;
    }
    point->notadummy = true;

    // line at infinity a line of singularities in poincare disc
    if (singinf_ && (point->chart != P4Charts::chart_R2)) {
        if (dir_vec_field_ == 1) {
            typ = point->type;
        } else {
            switch (point->type) {
            case 1:
                typ = 3;
                break;
            case 2:
                typ = 4;
                break;
            case 3:
                typ = 1;
                break;
            case 4:
                typ = 2;
                break;
            case 6:
                typ = 7;
                break;
            case 7:
                typ = 6;
                break;
            default:
                typ = point->type;
            }
        }

        point->next_se = new P4Singularities::semi_elementary{
            point->x0,
            0.0,
            (point->chart == P4Charts::chart_U1) ? P4Charts::chart_V1
                                                 : P4Charts::chart_V2,
            0,
            point->epsilon,
            false,
            point->a11,
            point->a12,
            point->a21,
            point->a22,
            typ};
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readNodePoint
// -----------------------------------------------------------------------
bool P4VFStudy::readNodePoint(FILE *fp)
{
    double y[2];

    P4Singularities::node *last{nullptr}, *point{firstNodePoint_};

    while (point != nullptr) {
        last = point;
        point = point->next_node;
    }

    point = new P4Singularities::node;
    if (last == nullptr)
        firstNodePoint_ = point;
    else
        last->next_node = point;

    // load point structure

    if (fscanf(fp, "%lf %lf %d ", &(point->x0), &(point->y0),
               &(point->stable)) != 3 ||
        fscanf(fp, "%d ", &(point->chart)) != 1) {
        delete point;
        point = nullptr;
        return false;
    }

    // change type of node if we have a gcf ?
    y[0] = point->x0;
    y[1] = point->y0;

    switch (point->chart) {
    case P4Charts::chart_R2:
        if (eval_term2(gcf_, y) < 0)
            point->stable *= -1;
        break;
    case P4Charts::chart_U1:
        if (eval_term2(gcf_U1_, y) < 0)
            point->stable *= -1;
        break;
    case P4Charts::chart_V1:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V1_, y) < 0)
            point->stable *= -1;
        break;
    case P4Charts::chart_U2:
        if (eval_term2(gcf_U2_, y) < 0)
            point->stable *= -1;
        break;
    case P4Charts::chart_V2:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V2_, y) < 0)
            point->stable *= -1;
        break;
    }

    // if line at infinity a line of singularities in poincare disc:
    // duplicate singularity by using a symmetry
    if (singinf_ && point->chart != P4Charts::chart_R2) {
        last = point;
        point = new P4Singularities::node{
            last->x0, 0.0,
            (last->chart == P4Charts::chart_U1) ? P4Charts::chart_V1
                                                : P4Charts::chart_V2,
            0, last->stable * ((dir_vec_field_ == -1) ? -1 : 1)};
        last->next_node = point;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readStrongFocusPoint
// -----------------------------------------------------------------------
bool P4VFStudy::readStrongFocusPoint(FILE *fp)
{
    double y[2];

    P4Singularities::strong_focus *last{nullptr}, *point{firstSfPoint_};

    while (point != nullptr) {
        last = point;
        point = point->next_sf;
    }

    point = new P4Singularities::strong_focus;
    if (last == nullptr)
        firstSfPoint_ = point;
    else
        last->next_sf = point;

    // fill structure
    if (fscanf(fp, "%d %lf %lf ", &(point->stable), &(point->x0),
               &(point->y0)) != 3 ||
        fscanf(fp, "%d ", &(point->chart)) != 1) {
        delete point;
        point = nullptr;
        return false;
    }

    // change type of node if we have a gcf ?

    y[0] = point->x0;
    y[1] = point->y0;

    switch (point->chart) {
    case P4Charts::chart_R2:
        if (eval_term2(gcf_, y) < 0)
            point->stable *= -1;
        break;
    case P4Charts::chart_U1:
        if (eval_term2(gcf_U1_, y) < 0)
            point->stable *= -1;
        break;
    case P4Charts::chart_V1:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V1_, y) < 0)
            point->stable *= -1;
        break;
    case P4Charts::chart_U2:
        if (eval_term2(gcf_U2_, y) < 0)
            point->stable *= -1;
        break;
    case P4Charts::chart_V2:
        if (parent_->p_ == 1 && parent_->q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V2_, y) < 0)
            point->stable *= -1;
        break;
    }

    // line at infinity a line of singularities in poincare disc
    if (singinf_ && point->chart != P4Charts::chart_R2) {
        last = point;
        point = new P4Singularities::strong_focus{
            last->x0, 0.0,
            ((point->chart == P4Charts::chart_U1) ? P4Charts::chart_V1
                                                  : P4Charts::chart_V2),
            0, last->stable * ((dir_vec_field_ == -1) ? -1 : 1)};
        last->next_sf = point;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readWeakFocusPoint
// -----------------------------------------------------------------------
bool P4VFStudy::readWeakFocusPoint(FILE *fp)
{
    double y[2];
    int typ;

    P4Singularities::weak_focus *last{nullptr}, *point{firstWfPoint_};

    while (point != nullptr) {
        last = point;
        point = point->next_wf;
    }

    point = new P4Singularities::weak_focus;
    if (last == nullptr)
        firstWfPoint_ = point;
    else
        last->next_wf = point;

    // fill structure
    if (fscanf(fp, "%lf %lf ", &(point->x0), &(point->y0)) != 2 ||
        fscanf(fp, "%d %d ", &(point->type), &(point->chart)) != 2) {
        delete point;
        point = nullptr;
        return false;
    }

    // change type of node if we have a gcf ?

    if ((point->type == P4SingularityStability::stable) ||
        (point->type == P4SingularityStability::unstable)) {

        y[0] = point->x0;
        y[1] = point->y0;

        switch (point->chart) {
        case P4Charts::chart_R2:
            if (eval_term2(gcf_, y) < 0)
                point->type *= -1;
            break;
        case P4Charts::chart_U1:
            if (eval_term2(gcf_U1_, y) < 0)
                point->type *= -1;
            break;
        case P4Charts::chart_V1:
            if (parent_->p_ == 1 && parent_->q_ == 1)
                y[0] = -y[0];
            if (eval_term2(gcf_V1_, y) < 0)
                point->type *= -1;
            break;
        case P4Charts::chart_U2:
            if (eval_term2(gcf_U2_, y) < 0)
                point->type *= -1;
            break;
        case P4Charts::chart_V2:
            if (parent_->p_ == 1 && parent_->q_ == 1)
                y[0] = -y[0];
            if (eval_term2(gcf_V2_, y) < 0)
                point->type *= -1;
            break;
        }
    }

    // line at infinity a line of singularities in poincare disc
    if (singinf_ && point->chart != P4Charts::chart_R2) {
        if (dir_vec_field_ == 1) {
            typ = point->type;

        } else {
            switch (point->type) {
            case P4SingularityStability::stable:
                typ = P4SingularityStability::unstable;
                break;
            case P4SingularityStability::unstable:
                typ = P4SingularityStability::stable;
                break;
            default:
                typ = point->type;
                break;
            }
        }
        point->next_wf = new P4Singularities::weak_focus{
            point->x0, 0.0,
            ((point->chart == P4Charts::chart_U1) ? P4Charts::chart_V1
                                                  : P4Charts::chart_V2),
            1, typ};
        point = point->next_wf;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readDegeneratePoint
// -----------------------------------------------------------------------
bool P4VFStudy::readDegeneratePoint(FILE *fp)
{
    int n;

    // make room in structure
    P4Singularities::degenerate *last{nullptr}, *point{firstDePoint_};

    while (point != nullptr) {
        last = point;
        point = point->next_de;
    }

    point = new P4Singularities::degenerate;
    if (last == nullptr)
        firstDePoint_ = point;
    else
        last->next_de = point;

    // load structure
    if (fscanf(fp, "%lf %lf %lf %d ", &(point->x0), &(point->y0),
               &(point->epsilon), &n) != 4) {
        delete point;
        point = nullptr;
        return false;
    }

    if (n) {
        point->blow_up = new P4Blowup::blow_up_points;
        readBlowupPoints(fp, point->blow_up, n);
        point->blow_up->integrating_in_local_chart = true;
    }

    if (fscanf(fp, "%d ", &(point->chart)) != 1) {
        delete point;
        point = nullptr;
        return false;
    }
    point->notadummy = true;

    // line at infinity a line of singularities in poincare disc
    if (singinf_ && point->chart != P4Charts::chart_R2) {
        last = point;
        point = new P4Singularities::degenerate{
            last->x0,
            0.0,
            ((last->chart == P4Charts::chart_U1) ? P4Charts::chart_V1
                                                 : P4Charts::chart_V2),
            1,
            last->epsilon,
            false,
            last->blow_up};
        last->next_de = point;
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readTransformations
// -----------------------------------------------------------------------
bool P4VFStudy::readTransformations(FILE *fp, P4Blowup::transformations *trans,
                                    int n)
{
    if (fscanf(fp, "%lf %lf %d %d %d %d %d %d %d", &(trans->x0), &(trans->y0),
               &(trans->c1), &(trans->c2), &(trans->d1), &(trans->d2),
               &(trans->d3), &(trans->d4), &(trans->d)) != 9) {
        delete trans;
        trans = nullptr;
        return false;
    }

    for (int i = 2; i <= n; i++) {
        trans = new P4Blowup::transformations;
        trans = trans->next_trans;
        if (fscanf(fp, "%lf %lf %d %d %d %d %d %d %d", &(trans->x0),
                   &(trans->y0), &(trans->c1), &(trans->c2), &(trans->d1),
                   &(trans->d2), &(trans->d3), &(trans->d4),
                   &(trans->d)) != 9) {
            delete trans;
            trans = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4VFStudy::readBlowupPoints
// -----------------------------------------------------------------------
bool P4VFStudy::readBlowupPoints(FILE *fp, P4Blowup::blow_up_points *b, int n)
{
    int N, typ;

    for (int i = 1; i <= n; i++) {
        b->trans = new P4Blowup::transformations;
        b->sep = new P4Polynom::term1;
        if (fscanf(fp, "%d ", &(b->n)) != 1 ||
            !readTransformations(fp, b->trans, b->n) ||
            fscanf(fp, "%lf %lf ", &(b->x0), &(b->y0)) != 2 ||
            fscanf(fp, "%lf %lf %lf %lf ", &(b->a11), &(b->a12), &(b->a21),
                   &(b->a22)) != 4 ||
            !readVectorField(fp, b->vector_field) ||
            fscanf(fp, "%d ", &N) != 1 || !readTerm1(fp, b->sep, N) ||
            fscanf(fp, "%d ", &typ) != 1) {
            return false;
        }

        switch (typ) {
        case 1:
        case 3:
        case 9:
        case 10:
            b->type = P4SeparatriceType::unstable;
            break;
        case 2:
        case 4:
        case 11:
        case 12:
            b->type = P4SeparatriceType::stable;
            break;
        case 5:
        case 7:
            b->type = P4SeparatriceType::cent_unstable;
            break;
        case 6:
        case 8:
            b->type = P4SeparatriceType::cent_stable;
            break;
        default:
            return false;
        }

        if (i < n) {
            b->next_blow_up_point = new P4Blowup::blow_up_points;
            b = b->next_blow_up_point;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          DUMP FUNCTIONS
// -----------------------------------------------------------------------

void P4VFStudy::dumpSeparatrices(QTextEdit &m, P4Blowup::sep *separ, int margin)
{
    QString s;
    char smargin[80];
    std::string stype;
    strcpy(smargin, "                              ");
    smargin[margin] = 0;

    if (separ == nullptr) {
        m.append(s.sprintf("%s/", smargin));
        return;
    }

    while (separ != nullptr) {
        switch (separ->type) {
        case P4SeparatriceType::stable:
            stype = "stable         ";
            break;
        case P4SeparatriceType::unstable:
            stype = "unstable       ";
            break;
        case P4SeparatriceType::cent_stable:
            stype = "center-stable  ";
            break;
        case P4SeparatriceType::cent_unstable:
            stype = "center-unstable";
            break;
        default:
            stype = "???????????????";
            break;
        }
        m.append(s.sprintf("%sType=%s  Dir=%-2d  original=%d", smargin,
                           stype.data(), separ->direction, separ->notadummy));
        if (separ->d == 0) {
            m.append(s.sprintf("%sTaylor: (x,y)=(t,%s)", smargin,
                               dumpPoly1(separ->separatrice, "t")));
        } else {
            m.append(s.sprintf("%sTaylor: (x,y)=(%s,t)", smargin,
                               dumpPoly1(separ->separatrice, "t")));
        }
        separ = separ->next_sep;
    }
}

// dump saddle singularities
void P4VFStudy::dumpSingularities(QTextEdit &m, P4Singularities::saddle *p,
                                  bool longversion)
{
    const char *chart;
    QString s;

    while (p != nullptr) {
        switch (p->chart) {
        case P4Charts::chart_R2:
            chart = "R2";
            break;
        case P4Charts::chart_U1:
            chart = "U1";
            break;
        case P4Charts::chart_U2:
            chart = "U2";
            break;
        case P4Charts::chart_V1:
            chart = "V1";
            break;
        case P4Charts::chart_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(s.sprintf("SADDLE:\t(x0,y0)=(%g,%g)  Chart %s", p->x0, p->y0,
                           chart));
        if (longversion) {
            m.append(s.sprintf("   Epsilon = %g  original=%d", p->epsilon,
                               p->notadummy));
            m.append(
                s.sprintf("   Transformation Matrix = [ [%g,%g], [%g,%g] ]",
                          p->a11, p->a12, p->a21, p->a22));
            m.append(s.sprintf("   Vector Field:"));
            m.append(s.sprintf("      x' = %s",
                               dumpPoly2(p->vector_field[0], "x", "y")));
            m.append(s.sprintf("      y' = %s",
                               dumpPoly2(p->vector_field[1], "x", "y")));
            m.append(s.sprintf("   Separatrices:"));
            dumpSeparatrices(m, p->separatrices, 6);
            m.append(s.sprintf(" "));
        }
        p = p->next_saddle;
    }
}

// dump degenerate singularities
void P4VFStudy::dumpSingularities(QTextEdit &m, P4Singularities::degenerate *p,
                                  bool longversion)
{
    const char *chart;
    QString s;

    while (p != nullptr) {
        switch (p->chart) {
        case P4Charts::chart_R2:
            chart = "R2";
            break;
        case P4Charts::chart_U1:
            chart = "U1";
            break;
        case P4Charts::chart_U2:
            chart = "U2";
            break;
        case P4Charts::chart_V1:
            chart = "V1";
            break;
        case P4Charts::chart_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(s.sprintf("DEGENERATE:\t(x0,y0)=(%g,%g)  Chart %s", p->x0,
                           p->y0, chart));
        if (longversion) {
            m.append(s.sprintf("   Epsilon = %g  original=%d", p->epsilon,
                               p->notadummy));
            m.append(s.sprintf(" "));
        }
        p = p->next_de;
    }
}

// dump strong focus singularities
void P4VFStudy::dumpSingularities(QTextEdit &m,
                                  P4Singularities::strong_focus *p,
                                  bool longversion)
{
    const char *chart;
    QString s;
    QByteArray ss;

    while (p != nullptr) {
        switch (p->chart) {
        case P4Charts::chart_R2:
            chart = "R2";
            break;
        case P4Charts::chart_U1:
            chart = "U1";
            break;
        case P4Charts::chart_U2:
            chart = "U2";
            break;
        case P4Charts::chart_V1:
            chart = "V1";
            break;
        case P4Charts::chart_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(s.sprintf("STRONG FOCUS:\t(x0,y0)=(%g,%g)  Chart %s", p->x0,
                           p->y0, chart));
        if (longversion) {
            if (p->stable == -1)
                s = "(stable)";
            else if (p->stable == +1)
                s = "(unstable)";
            else
                s = "( ??? )";
            ss = s.toLatin1();
            m.append(
                s.sprintf("    Stability = %d %s", p->stable, ss.constData()));
            m.append(s.sprintf(" "));
        }
        p = p->next_sf;
    }
}

// dump weak focus singularities
void P4VFStudy::dumpSingularities(QTextEdit &m, P4Singularities::weak_focus *p,
                                  bool longversion)
{
    const char *chart;
    QString s;

    while (p != nullptr) {
        switch (p->chart) {
        case P4Charts::chart_R2:
            chart = "R2";
            break;
        case P4Charts::chart_U1:
            chart = "U1";
            break;
        case P4Charts::chart_U2:
            chart = "U2";
            break;
        case P4Charts::chart_V1:
            chart = "V1";
            break;
        case P4Charts::chart_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(s.sprintf("WEAK FOCUS:\t(x0,y0)=(%g,%g)  Chart %s", p->x0,
                           p->y0, chart));
        if (longversion) {
            m.append(s.sprintf("    Type = %d", p->type));
            m.append(s.sprintf(" "));
        }
        p = p->next_wf;
    }
}

// dump node singularities
void P4VFStudy::dumpSingularities(QTextEdit &m, P4Singularities::node *p,
                                  bool longversion)
{
    const char *chart;
    QString s;
    QByteArray ss;

    while (p != nullptr) {
        switch (p->chart) {
        case P4Charts::chart_R2:
            chart = "R2";
            break;
        case P4Charts::chart_U1:
            chart = "U1";
            break;
        case P4Charts::chart_U2:
            chart = "U2";
            break;
        case P4Charts::chart_V1:
            chart = "V1";
            break;
        case P4Charts::chart_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(
            s.sprintf("NODE:\t(x0,y0)=(%g,%g)  Chart %s", p->x0, p->y0, chart));
        if (longversion) {
            if (p->stable == -1)
                s = "(stable)";
            else if (p->stable == +1)
                s = "(unstable)";
            else
                s = "( ??? )";
            ss = s.toLatin1();
            m.append(
                s.sprintf("    Stability = %d %s", p->stable, ss.constData()));
            m.append(s.sprintf(" "));
        }
        p = p->next_node;
    }
}

// dump semi elementary singularities
void P4VFStudy::dumpSingularities(QTextEdit &m,
                                  P4Singularities::semi_elementary *p,
                                  bool longversion)
{
    const char *chart;
    QString s;
    QByteArray ss;

    while (p != nullptr) {
        switch (p->chart) {
        case P4Charts::chart_R2:
            chart = "R2";
            break;
        case P4Charts::chart_U1:
            chart = "U1";
            break;
        case P4Charts::chart_U2:
            chart = "U2";
            break;
        case P4Charts::chart_V1:
            chart = "V1";
            break;
        case P4Charts::chart_V2:
            chart = "V2";
            break;
        default:
            chart = "?";
            break;
        }
        m.append(s.sprintf("SEMI ELEMENTARY:\t(x0,y0)=(%g,%g)  Chart %s", p->x0,
                           p->y0, chart));
        if (longversion) {
            m.append(s.sprintf("   Type    = %d", p->type));
            m.append(s.sprintf("   Epsilon = %g  original=%d", p->epsilon,
                               p->notadummy));
            m.append(
                s.sprintf("   Transformation Matrix = [ [%g,%g], [%g,%g] ]",
                          p->a11, p->a12, p->a21, p->a22));
            m.append(s.sprintf("   Vector Field = "));
            m.append(s.sprintf("      x' = %s",
                               dumpPoly2(p->vector_field[0], "x", "y")));
            m.append(s.sprintf("      y' = %s",
                               dumpPoly2(p->vector_field[1], "x", "y")));
            m.append(s.sprintf("   Separatrices:"));
            dumpSeparatrices(m, p->separatrices, 6);
            m.append(s.sprintf(" "));
        }
        p = p->next_se;
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
    m.append(s.sprintf("     x' = %s", dumpPoly2(f_vec_field_[0], "x", "y")));
    m.append(s.sprintf("     y' = %s", dumpPoly2(f_vec_field_[1], "x", "y")));
    m.append(s.sprintf("  U1 chart:"));
    m.append(s.sprintf("     x' = %s", dumpPoly2(vec_field_U1_[0], "x", "y")));
    m.append(s.sprintf("     y' = %s", dumpPoly2(vec_field_U1_[1], "x", "y")));
    m.append(s.sprintf("  U2 chart:"));
    m.append(s.sprintf("     x' = %s", dumpPoly2(vec_field_U2_[0], "x", "y")));
    m.append(s.sprintf("     y' = %s", dumpPoly2(vec_field_U2_[1], "x", "y")));
    m.append(s.sprintf("  V1 chart:"));
    m.append(s.sprintf("     x' = %s", dumpPoly2(vec_field_V1_[0], "x", "y")));
    m.append(s.sprintf("     y' = %s", dumpPoly2(vec_field_V1_[1], "x", "y")));
    m.append(s.sprintf("  V2 chart:"));
    m.append(s.sprintf("     x' = %s", dumpPoly2(vec_field_V2_[0], "x", "y")));
    m.append(s.sprintf("     y' = %s", dumpPoly2(vec_field_V2_[1], "x", "y")));
    m.append(s.sprintf("  Cylinder chart: (Co=cos(theta),Si=sin(theta))"));
    m.append(s.sprintf("     r'     = %s",
                       dumpPoly3(vec_field_C_[0], "r", "Co", "Si")));
    m.append(s.sprintf("     theta' = %s",
                       dumpPoly3(vec_field_C_[1], "r", "Co", "Si")));
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
    dumpSingularities(m, firstSaddlePoint_, false);
    dumpSingularities(m, firstSePoint_, false);
    dumpSingularities(m, firstNodePoint_, false);
    dumpSingularities(m, firstWfPoint_, false);
    dumpSingularities(m, firstSfPoint_, false);
    dumpSingularities(m, firstDePoint_, false);
    m.append(s.sprintf(" "));
    m.append(s.sprintf("Singular points - full description"));
    m.append(s.sprintf("----------------------------------"));
    m.append(s.sprintf(" "));
    dumpSingularities(m, firstSaddlePoint_, true);
    dumpSingularities(m, firstSePoint_, true);
    dumpSingularities(m, firstNodePoint_, true);
    dumpSingularities(m, firstWfPoint_, true);
    dumpSingularities(m, firstSfPoint_, true);
    dumpSingularities(m, firstDePoint_, true);
}
