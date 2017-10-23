/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  it under the terms of the GNU Lesser General Public License as published
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

#include "file_tab.h"

#include "file_vf.h"
#include "math_changedir.h"
#include "math_charts.h"
#include "math_orbits.h"
#include "math_p4.h"
#include "math_polynom.h"
#include "math_separatrice.h"
#include "p4application.h"

#include <iostream>
#include <locale.h>

#include <QFile>

QVFStudy g_VFResults;

/*
    This file contains the code to read the information from reduce/maple.
    The relevant structures for saddles etc are set up here.
*/

// -----------------------------------------------------------------------
//                              QVFStudy CONSTRUCTOR
// -----------------------------------------------------------------------
QVFStudy::QVFStudy()
{
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

    // initialize singular points structures:
    first_saddle_point_ = nullptr;
    first_se_point_ = nullptr;
    first_node_point_ = nullptr;
    first_sf_point_ = nullptr;
    first_wf_point_ = nullptr;
    first_de_point_ = nullptr;

    // initialize GCF:
    gcf_ = nullptr;
    gcf_U1_ = nullptr;
    gcf_U2_ = nullptr;
    gcf_V1_ = nullptr;
    gcf_V2_ = nullptr;
    gcf_C_ = nullptr;
    gcf_points_ = nullptr;

    // initialize limit cycles & orbits
    first_lim_cycle_ = nullptr;
    first_orbit_ = nullptr;
    current_orbit_ = nullptr;

    // initialize others
    xmin_ = -1.0;
    xmax_ = 1.0;
    ymin_ = -1.0;
    ymax_ = 1.0;
    p_ = 1;
    q_ = 1;
    typeofstudy_ = TYPEOFSTUDY_ALL;
    singinf_ = false;
    g_VFResults.dir_vec_field_ = 1;

    // initialize parameters
    // number of orbits in the limit cycle window
    config_lc_value_ = DEFAULT_LCORBITS;
    // number of points in the limit cycle window
    config_lc_numpoints_ = DEFAULT_LCPOINTS;
    // maximum step size
    config_hma_ = DEFAULT_HMA;
    // minimum step size
    config_hmi_ = DEFAULT_HMI;
    // step size
    config_step_ = DEFAULT_STEPSIZE;
    // current step size (during integration)
    config_currentstep_ = DEFAULT_STEPSIZE;
    // tolerance
    config_tolerance_ = DEFAULT_TOLERANCE;
    // projection in the case of Poincare sphere
    config_projection_ = DEFAULT_PROJECTION;
    // number of points to integrate
    config_intpoints_ = DEFAULT_INTPOINTS;
    // line style (dashes or points)
    config_dashes_ = DEFAULT_LINESTYLE;
    config_kindvf_ = DEFAULT_INTCONFIG;
}

// -----------------------------------------------------------------------
//                              QVFStudy DESTRUCTOR
// -----------------------------------------------------------------------
QVFStudy::~QVFStudy() { deleteVF(); }

// -----------------------------------------------------------------------
//                          QVFStudy::DeleteVF
// -----------------------------------------------------------------------
void QVFStudy::deleteVF()
{
    // Delete Vector Field
    delete_term2(f_vec_field_[0]);
    delete_term2(f_vec_field_[1]);
    delete_term2(vec_field_U1_[0]);
    delete_term2(vec_field_U1_[1]);
    delete_term2(vec_field_U2_[0]);
    delete_term2(vec_field_U2_[1]);
    delete_term2(vec_field_V1_[0]);
    delete_term2(vec_field_V1_[1]);
    delete_term2(vec_field_V2_[0]);
    delete_term2(vec_field_V2_[1]);
    delete_term3(vec_field_C_[0]);
    delete_term3(vec_field_C_[1]);

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
    deleteSaddle(first_saddle_point_);
    deleteSemiElementary(first_se_point_);
    deleteNode(first_node_point_);
    deleteDegenerate(first_de_point_);
    deleteStrongFocus(first_sf_point_);
    deleteWeakFocus(first_wf_point_);

    first_saddle_point_ = nullptr;
    first_se_point_ = nullptr;
    first_node_point_ = nullptr;
    first_sf_point_ = nullptr;
    first_wf_point_ = nullptr;
    first_de_point_ = nullptr;

    // Delete GCF:
    delete_term2(gcf_);
    delete_term2(gcf_U1_);
    delete_term2(gcf_U2_);
    delete_term2(gcf_V1_);
    delete_term2(gcf_V2_);
    delete_term3(gcf_C_);
    deleteOrbitPoint(gcf_points_);

    gcf_ = nullptr;
    gcf_U1_ = nullptr;
    gcf_U2_ = nullptr;
    gcf_V1_ = nullptr;
    gcf_V2_ = nullptr;
    gcf_C_ = nullptr;
    gcf_points_ = nullptr;

    // Delete curve:
    curve_vector_.clear();

    // Delete isoclines:
    isocline_vector_.clear();

    // Delete all orbits
    deleteOrbit(first_orbit_);
    first_orbit_ = nullptr;
    current_orbit_ = nullptr;

    // Delete limit cycles
    deleteLimitCycle(first_lim_cycle_);
    first_lim_cycle_ = nullptr;

    // reset others
    xmin_ = -1.0;
    xmax_ = 1.0;
    ymin_ = -1.0;
    ymax_ = 1.0;
    p_ = 1;
    q_ = 1;
    typeofstudy_ = TYPEOFSTUDY_ALL;
    singinf_ = false;
    g_VFResults.dir_vec_field_ = 1;

    lasterror_ = "";
}

// -----------------------------------------------------------------------
//                          QVFStudy::DeleteSaddle
// -----------------------------------------------------------------------
void QVFStudy::deleteSaddle(saddle *p)
{
    saddle *q;
    while (p != nullptr) {
        q = p;
        p = p->next_saddle;
        delete_term2(q->vector_field[0]);
        delete_term2(q->vector_field[1]);
        if (q->notadummy)
            deleteSeparatrices(q->separatrices); // separatrices may be nullptr
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteSemiElementary
// -----------------------------------------------------------------------
void QVFStudy::deleteSemiElementary(semi_elementary *p)
{
    semi_elementary *q;

    while (p != nullptr) {
        q = p;
        p = p->next_se;

        delete_term2(q->vector_field[0]);
        delete_term2(q->vector_field[1]);
        if (q->notadummy)
            deleteSeparatrices(q->separatrices); // separatrices may be nullptr
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteDegenerate
// -----------------------------------------------------------------------
void QVFStudy::deleteDegenerate(degenerate *p)
{
    degenerate *q;

    while (p != nullptr) {
        q = p;
        p = p->next_de;

        if (q->notadummy && (q->blow_up != nullptr))
            deleteBlowup(q->blow_up);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteSeparatrices
// -----------------------------------------------------------------------
void QVFStudy::deleteSeparatrices(sep *p)
{
    sep *q;

    while (p != nullptr) {
        q = p;
        p = p->next_sep;

        deleteOrbitPoint(q->first_sep_point);
        if (q->notadummy)
            delete_term1(q->separatrice);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteBlowup
// -----------------------------------------------------------------------
void QVFStudy::deleteBlowup(blow_up_points *b)
{
    blow_up_points *c;

    while (b != nullptr) {
        c = b;
        b = b->next_blow_up_point;
        deleteTransformations(c->trans);
        delete_term2(c->vector_field[0]);
        delete_term2(c->vector_field[1]);
        delete_term1(c->sep);
        deleteOrbitPoint(c->first_sep_point);
        delete c;
        c = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteLimitCycle
// -----------------------------------------------------------------------
void QVFStudy::deleteLimitCycle(orbits *p)
{
    deleteOrbit(p); // limit cycle is implemented as orbit.
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteOrbit
// -----------------------------------------------------------------------
void QVFStudy::deleteOrbit(orbits *p)
{
    orbits *q;

    while (p != nullptr) {
        q = p;
        p = p->next_orbit;

        deleteOrbitPoint(q->f_orbits);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadTables
// -----------------------------------------------------------------------
//
// read filename_vec.tab
// read filename_inf.tab
// read filename_fin.tab

bool QVFStudy::readTables(QString basename)
{
    FILE *fp;
    int j;
    int flag;

    setlocale(LC_ALL, "C");

    deleteVF(); // initialize structures, delete previous vector field if any
    fp = fopen(QFile::encodeName(basename + "_vec.tab"), "rt");
    if (fp == nullptr) {
        dump(basename, "Cannot open file *_vec.tab");
        deleteVF();
        return false;
    }

    if (fscanf(fp, "%d %d %d ", &typeofstudy_, &p_, &q_) != 3) {
        dump(basename, "Cannot read typeofstudy_ in *_vec.tab");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (typeofstudy_ == TYPEOFSTUDY_ONE) {
        if (fscanf(fp, "%lf %lf %lf %lf", &xmin_, &xmax_, &ymin_, &ymax_) !=
            4) {
            dump(basename, "Cannot read min-max coords in *_vec.tab");
            deleteVF();
            fclose(fp);
            return false;
        }
        p_ = q_ = 1;
        typeofview_ = TYPEOFVIEW_PLANE;
    } else
        typeofview_ = TYPEOFVIEW_SPHERE;

    plweights_ = ((p_ == 1 && q_ == 1) ? false : true);

    double_p_ = (double)p_;
    double_q_ = (double)q_;
    double_p_plus_q_ = (double)(p_ + q_);
    double_p_minus_1_ = (double)(p_ - 1);
    double_q_minus_1_ = (double)(q_ - 1);
    double_q_minus_p_ = (double)(q_ - p_);

    if (!readGCF(fp)) {
        dump(basename, "Cannot read gcf *_vec.tab");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (!readVectorField(fp, f_vec_field_)) {
        dump(basename, "Cannot read vector field in *_vec.tab");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (!readVectorField(fp, vec_field_U1_)) {
        dump(basename, "Cannot read vector field in U1-chart in *_vec.tab");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (!readVectorField(fp, vec_field_V1_)) {
        dump(basename, "Cannot read vector field in V1-chart in *_vec.tab");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (!readVectorField(fp, vec_field_U2_)) {
        dump(basename, "Cannot read vector field in U2-chart in *_vec.tab");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (!readVectorField(fp, vec_field_V2_)) {
        dump(basename, "Cannot read vector field in V2-chart in *_vec.tab");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (plweights_) {
        if (!readVectorFieldCylinder(fp, vec_field_C_)) {
            dump(basename,
                 "Cannot read vector field in Cylinder-chart in *_vec.tab");
            deleteVF();
            fclose(fp);
            return false;
        }
        singinf_ = 0;
    } else {
        if (fscanf(fp, "%d %d", &flag, &g_VFResults.dir_vec_field_) != 2) {
            dump(basename, "Cannot read sing-at-infinity flag and directions "
                           "flag in *_vec.tab");
            deleteVF();
            fclose(fp);
            return false;
        }
        singinf_ = ((flag == 0) ? false : true);
    }

    fclose(fp);

    if (typeofstudy_ != TYPEOFSTUDY_INF) {

        fp = fopen(QFile::encodeName(basename + "_fin.tab"), "rt");
        if (fp != nullptr) {
            if (!readPoints(fp)) {
                dump(basename,
                     QString(
                         "Problem reading singularity info from *_fin.tab:") +
                         lasterror_);
                deleteVF();
                fclose(fp);
                return false;
            }
            fclose(fp);
        } else {
            dump(basename, "Cannot open *_fin.tab");
            deleteVF();
            return false;
        }
    }

    if (typeofstudy_ != TYPEOFSTUDY_ONE && typeofstudy_ != TYPEOFSTUDY_FIN) {
        fp = fopen(QFile::encodeName(basename + "_inf.tab"), "rt");
        if (fp != nullptr) {
            if (p_ == 1 && q_ == 1) {
                for (j = 1; j <= 2; j++) {
                    if (!readPoints(fp)) {
                        dump(basename,
                             QString(
                                 "Cannot read singular points in *_inf.tab (") +
                                 QString::number(j) + "):" + lasterror_);
                        deleteVF();
                        fclose(fp);
                        return false;
                    }
                }
            } else {
                for (j = 1; j <= 4; j++) {
                    if (!readPoints(fp)) {
                        dump(basename,
                             QString(
                                 "Cannot read singular points in *_inf.tab (") +
                                 QString::number(j) + "):" + lasterror_);
                        deleteVF();
                        fclose(fp);
                        return false;
                    }
                }
            }
            fclose(fp);
        } else {
            dump(basename, "Cannot open *_inf.tab");
            deleteVF();
            return false;
        }
    }

    dump(basename, "all's well.");
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadGCF
// -----------------------------------------------------------------------
bool QVFStudy::readGCF(FILE *fp)
{
    int N, degree_gcf;

    if (fscanf(fp, "%d", &degree_gcf) != 1)
        return false;

    if (degree_gcf) {
        if (fscanf(fp, "%d", &N) != 1)
            return false;

        gcf_ = new term2;
        gcf_->next_term2 = nullptr;

        if (!readTerm2(fp, gcf_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        gcf_U1_ = new term2;
        gcf_U1_->next_term2 = nullptr;

        if (!readTerm2(fp, gcf_U1_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        gcf_U2_ = new term2;
        gcf_U2_->next_term2 = nullptr;

        if (!readTerm2(fp, gcf_U2_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        gcf_V1_ = new term2;
        gcf_V1_->next_term2 = nullptr;
        if (!readTerm2(fp, gcf_V1_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;
        gcf_V2_ = new term2;
        gcf_V2_->next_term2 = nullptr;
        if (!readTerm2(fp, gcf_V2_, N))
            return false;

        if (p_ != 1 || q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1)
                return false;

            gcf_C_ = new term3;
            gcf_C_->next_term3 = nullptr;
            if (!readTerm3(fp, gcf_C_, N))
                return false;
        }
    } else {
        gcf_ = nullptr;
        gcf_U1_ = nullptr;
        gcf_U2_ = nullptr;
        gcf_V1_ = nullptr;
        gcf_V2_ = nullptr;
        gcf_C_ = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadCurve
// -----------------------------------------------------------------------
bool QVFStudy::readCurve(QString basename)
{
    int N, degree_curve;
    FILE *fp = nullptr;
    setlocale(LC_ALL, "C");

    fp = fopen(QFile::encodeName(basename + "_veccurve.tab"), "rt");
    if (fp == nullptr) {
        dump(basename, "Cannot open file " + basename + "_veccurve.tab");
        return false;
    }

    curves new_curve;
    if (fscanf(fp, "%d", &degree_curve) != 1)
        return false;

    if (degree_curve > 0) {
        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.r2 = new term2;
        new_curve.r2->next_term2 = nullptr;

        if (!readTerm2(fp, new_curve.r2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.u1 = new term2;
        new_curve.u1->next_term2 = nullptr;

        if (!readTerm2(fp, new_curve.u1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.u2 = new term2;
        new_curve.u2->next_term2 = nullptr;

        if (!readTerm2(fp, new_curve.u2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.v1 = new term2;
        new_curve.v1->next_term2 = nullptr;
        if (!readTerm2(fp, new_curve.v1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;
        new_curve.v2 = new term2;
        new_curve.v2->next_term2 = nullptr;
        if (!readTerm2(fp, new_curve.v2, N))
            return false;

        if (p_ != 1 || q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1)
                return false;

            new_curve.c = new term3;
            new_curve.c->next_term3 = nullptr;
            if (!readTerm3(fp, new_curve.c, N))
                return false;
        }
    } else {
        return false; 
    }

    curve_vector_.push_back(new_curve);
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadIsoclines
// -----------------------------------------------------------------------
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
//                      QVFStudy::ReadVectorField
// -----------------------------------------------------------------------
bool QVFStudy::readVectorField(FILE *fp, P4POLYNOM2 *vf)
{
    int M, N;

    vf[0] = new term2;
    vf[0]->next_term2 = nullptr;
    vf[1] = new term2;
    vf[1]->next_term2 = nullptr;

    if (fscanf(fp, "%d", &M) != 1)
        return false;
    if (!readTerm2(fp, vf[0], M))
        return false;
    if (fscanf(fp, "%d", &N) != 1)
        return false;
    if (!readTerm2(fp, vf[1], N))
        return false;

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadVectorFieldCylinder
// -----------------------------------------------------------------------
bool QVFStudy::readVectorFieldCylinder(FILE *fp, P4POLYNOM3 *vf)
{
    int N;

    vf[0] = new term3;
    vf[0]->next_term3 = nullptr;
    vf[1] = new term3;
    vf[1]->next_term3 = nullptr;

    if (fscanf(fp, "%d", &N) != 1)
        return false;
    if (!readTerm3(fp, vf[0], N))
        return false;
    if (fscanf(fp, "%d", &N) != 1)
        return false;
    if (!readTerm3(fp, vf[1], N))
        return false;

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadPoints
// -----------------------------------------------------------------------
bool QVFStudy::readPoints(FILE *fp)
{
    int N, i, typ;

    if (fscanf(fp, "%d ", &N) != 1) {
        lasterror_ = "#sing not readable";
        return false;
    }

    for (i = 1; i <= N; i++) {
        if (fscanf(fp, "%d ", &typ) != 1) {
            lasterror_ =
                QString("sing #") + QString::number(i) + " type not readable";
            return false;
        }
        switch (typ) {
        case SADDLE:
            if (!readSaddlePoint(fp)) {
                lasterror_ = QString("sing #") + QString::number(i) +
                             " = saddle : " + lasterror_;
                return false;
            }
            break;
        case SEMI_HYPERBOLIC:
            if (!readSemiElementaryPoint(fp)) {
                lasterror_ = QString("sing #") + QString::number(i) +
                             " = semi-el : " + lasterror_;
                return false;
            }
            break;
        case NODE:
            if (!readNodePoint(fp)) {
                lasterror_ = QString("sing #") + QString::number(i) +
                             " = node : " + lasterror_;
                return false;
            }
            break;
        case STRONG_FOCUS:
            if (!readStrongFocusPoint(fp)) {
                lasterror_ = QString("sing #") + QString::number(i) +
                             " = strongfocus : " + lasterror_;
                return false;
            }
            break;
        case WEAK_FOCUS:
            if (!readWeakFocusPoint(fp)) {
                lasterror_ = QString("sing #") + QString::number(i) +
                             " = weakfocus : " + lasterror_;
                return false;
            }
            break;
        case NON_ELEMENTARY:
            if (!readDegeneratePoint(fp)) {
                lasterror_ = QString("sing #") + QString::number(i) +
                             " = degen : " + lasterror_;
                return false;
            }
            break;
        default:
            lasterror_ = QString("sing #") + QString::number(i) +
                         " type not exist (" + QString::number(typ) + ")";
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadTerm1
// -----------------------------------------------------------------------
bool QVFStudy::readTerm1(FILE *fp, P4POLYNOM1 p, int N)
{
    int i;
    P4POLYNOM1 _p;

    _p = p;

    p->next_term1 = nullptr;
    if (fscanf(fp, "%d %lf", &(p->exp), &(p->coeff)) != 2)
        return false;

    for (i = 2; i <= N; i++) {
        p->next_term1 = new term1;
        p = p->next_term1;
        p->next_term1 = nullptr;
        if (fscanf(fp, "%d %lf", &(p->exp), &(p->coeff)) != 2) {
            delete_term1(_p->next_term1);
            _p->next_term1 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadTerm2
// -----------------------------------------------------------------------
bool QVFStudy::readTerm2(FILE *fp, P4POLYNOM2 p, int N)
{
    int i;
    P4POLYNOM2 _p;

    _p = p;
    p->next_term2 = nullptr;
    if (fscanf(fp, "%d %d %lf", &(p->exp_x), &(p->exp_y), &(p->coeff)) != 3)
        return false;

    for (i = 2; i <= N; i++) {
        p->next_term2 = new term2;
        p = p->next_term2;
        p->next_term2 = nullptr;
        if (fscanf(fp, "%d %d %lf", &(p->exp_x), &(p->exp_y), &(p->coeff)) !=
            3) {
            delete_term2(_p->next_term2);
            _p->next_term2 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadTerm3
// -----------------------------------------------------------------------
bool QVFStudy::readTerm3(FILE *fp, P4POLYNOM3 p, int N)
{
    int i;
    P4POLYNOM3 _p;
    _p = p;
    p->next_term3 = nullptr;
    if (fscanf(fp, "%d %d %d %lf", &(p->exp_r), &(p->exp_Co), &(p->exp_Si),
               &(p->coeff)) != 4)
        return false;

    for (i = 2; i <= N; i++) {
        p->next_term3 = new term3;
        p = p->next_term3;
        p->next_term3 = nullptr;
        if (fscanf(fp, "%d %d %d %lf", &(p->exp_r), &(p->exp_Co), &(p->exp_Si),
                   &(p->coeff)) != 4) {
            delete_term3(_p->next_term3);
            _p->next_term3 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadSaddlePoint
// -----------------------------------------------------------------------
bool QVFStudy::readSaddlePoint(FILE *fp)
{
    int N;
    sep *sep1;
    sep *sep2;

    // make room in structure

    saddle *last;
    saddle *point;

    last = nullptr;
    point = first_saddle_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_saddle;
    }

    point = new saddle;
    point->next_saddle = nullptr;
    point->separatrices = nullptr;
    if (last == nullptr)
        first_saddle_point_ = point;
    else
        last->next_saddle = point;

    // fill structure

    if (fscanf(fp, "%lf %lf", &(point->x0), &(point->y0)) != 2) {
        return false;
    }
    if (fscanf(fp, "%lf %lf %lf %lf", &(point->a11), &(point->a12),
               &(point->a21), &(point->a22)) != 4) {
        return false;
    }

    readVectorField(fp, point->vector_field);
    if (fscanf(fp, "%d ", &(point->chart)) != 1) {
        return false;
    }
    point->separatrices = new sep;
    sep1 = point->separatrices;
    sep1->first_sep_point = nullptr;
    sep1->last_sep_point = nullptr;
    sep1->separatrice = nullptr;
    sep1->next_sep = nullptr;
    if (fscanf(fp, "%d ", &(sep1->type)) != 1) {
        return false;
    }
    if (fscanf(fp, "%d ", &N) != 1) {
        return false;
    }
    sep1->notadummy = true;
    sep1->separatrice = new term1;
    readTerm1(fp, sep1->separatrice, N);
    sep1->direction = 1;
    sep1->d = 0;
    sep1->first_sep_point = nullptr;
    sep1->last_sep_point = nullptr;
    sep1->next_sep = nullptr;

    if (point->chart == CHART_R2 || singinf_) {
        // point is finite hence we have 4 separatrices or we have a line of
        // singularities
        // at infinity and hence we have also 4 separatrices after removing the
        // line

        sep2 = new sep;
        sep1->next_sep = sep2;

        sep2->type = sep1->type;
        sep2->direction = -1;
        sep2->d = 0;
        sep2->notadummy = false;
        sep2->separatrice = sep1->separatrice;
        sep2->first_sep_point = nullptr;
        sep2->last_sep_point = nullptr;

        sep1 = sep2->next_sep = new sep;

        if (fscanf(fp, "%d", &(sep1->type)) != 1) {
            return false;
        }
        if (fscanf(fp, "%d ", &N) != 1) {
            return false;
        }

        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->direction = 1;
        sep1->d = 1;
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;

        sep2 = new sep;
        sep1->next_sep = sep2;

        sep2->type = sep1->type;
        sep2->direction = -1;
        sep2->d = 1;
        sep2->notadummy = false;
        sep2->separatrice = sep1->separatrice;
        sep2->first_sep_point = nullptr;
        sep2->last_sep_point = nullptr;
        sep2->next_sep = nullptr;
    }

    if (fscanf(fp, "%lf ", &(point->epsilon)) != 1) {
        return false;
    }
    point->notadummy = true;

    // line at infinity a line of singularities in poincare disc

    if (singinf_ && point->chart != CHART_R2) {
        last = point;
        point = new saddle;
        last->next_saddle = point;
        point->next_saddle = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = ((last->chart == CHART_U1) ? CHART_V1 : CHART_V2);
        point->notadummy = false;
        point->separatrices = last->separatrices;
        point->epsilon = last->epsilon;
        point->a11 = last->a11;
        point->a12 = last->a12;
        point->a21 = last->a21;
        point->a22 = last->a22;
        point->vector_field[0] = nullptr;
        point->vector_field[1] = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadSemiElementaryPoint
// -----------------------------------------------------------------------
bool QVFStudy::readSemiElementaryPoint(FILE *fp)
{
    // make room in structure

    semi_elementary *last;
    semi_elementary *point;
    int N;
    int s;
    sep *sep1;
    double y[2];
    bool ok;

    last = nullptr;
    point = first_se_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_se;
    }

    point = new semi_elementary;
    if (last == nullptr)
        first_se_point_ = point;
    else
        last->next_se = point;

    point->next_se = nullptr;

    if (fscanf(fp, "%lf %lf ", &(point->x0), &(point->y0)) != 2)
        return false;
    if (fscanf(fp, "%lf %lf %lf %lf ", &(point->a11), &(point->a12),
               &(point->a21), &(point->a22)) != 4)
        return false;
    readVectorField(fp, point->vector_field);
    if (fscanf(fp, "%d %d %d", &(point->type), &s, &(point->chart)) != 3)
        return false;

    switch (point->type) {
    case 1: // saddle-node
        if (s && point->chart != CHART_R2 && !singinf_) {
            // hyperbolic sep = line at infinity, not reported
            // center sep in the wrong direction
            // so no separatrices
            point->separatrices = nullptr;
        } else {
            // in the finite region (always s=1), the first sep is the center
            // sep, then the hyperbolic sep
            // at infinity, we  have s=0, so the center sep is the line at
            // infinity, not reported so only sep is
            // then the hyperbolic sep.

            point->separatrices = new sep;
            sep1 = point->separatrices;

            if (s)
                sep1->type = OT_CENT_STABLE;
            else
                sep1->type = OT_UNSTABLE;

            sep1->d = 0;
            if (((p_ == 1) && (q_ == 1) &&
                 ((point->chart == CHART_V1) || (point->chart == CHART_V2))) ||
                (point->chart == CHART_R2 || singinf_)) {
                sep1->direction = -1;
            } else {
                sep1->direction = 1;
            }

            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = nullptr;
            if (point->chart == CHART_R2 || singinf_) {
                // read second (hyperbolic) separatrix

                sep1->next_sep = new sep;
                sep1 = sep1->next_sep;
                sep1->type = OT_UNSTABLE;
                sep1->d = 1;
                sep1->direction = 1;
                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                sep1->notadummy = true;
                sep1->separatrice = new term1;
                readTerm1(fp, sep1->separatrice, N);
                sep1->first_sep_point = nullptr;
                sep1->last_sep_point = nullptr;

                // it is two-sided, so make a copy in other direction

                sep1->next_sep = new sep;
                sep1->next_sep->type = OT_UNSTABLE;
                sep1->next_sep->d = 1;
                sep1->next_sep->direction = -1;
                sep1->next_sep->notadummy = false;
                sep1->next_sep->separatrice = sep1->separatrice;
                sep1->next_sep->first_sep_point = nullptr;
                sep1->next_sep->last_sep_point = nullptr;
                sep1->next_sep->next_sep = nullptr;
            }
        }
        break;

    case 2: // saddle-node
        point->separatrices = new sep;
        sep1 = point->separatrices;
        if (s)
            sep1->type = STYPE_CENUNSTABLE;
        else
            sep1->type = STYPE_STABLE;

        sep1->d = 0;
        if ((p_ == 1) && (q_ == 1) &&
            ((point->chart == CHART_V1) || (point->chart == CHART_V2)))
            sep1->direction = -1;
        else
            sep1->direction = 1;

        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = new sep;
            sep1 = sep1->next_sep;
            sep1->type = STYPE_STABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_STABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    case 3: // saddle-node
        point->separatrices = new sep;
        sep1 = point->separatrices;
        if (s)
            sep1->type = STYPE_CENSTABLE;
        else
            sep1->type = STYPE_UNSTABLE;
        sep1->d = 0;
        if ((p_ == 1) && (q_ == 1) &&
            ((point->chart == CHART_V1) || (point->chart == CHART_V2)))
            sep1->direction = -1;
        else
            sep1->direction = 1;

        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = new sep;
            sep1 = sep1->next_sep;
            sep1->type = STYPE_UNSTABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_UNSTABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    case 4: // saddle-node
        if (s && (point->chart != CHART_R2) && !singinf_) {
            point->separatrices = nullptr;
        } else {
            point->separatrices = new sep;
            sep1 = point->separatrices;
            if (s)
                sep1->type = STYPE_CENUNSTABLE;
            else
                sep1->type = STYPE_STABLE;

            sep1->d = 0;
            if (((p_ == 1) && (q_ == 1) &&
                 ((point->chart == CHART_V1) || (point->chart == CHART_V2))) ||
                (point->chart == CHART_R2 || singinf_))
                sep1->direction = -1;
            else
                sep1->direction = 1;

            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = nullptr;
            if (point->chart == CHART_R2 || singinf_) {
                sep1->next_sep = new sep;
                sep1 = sep1->next_sep;
                sep1->type = STYPE_STABLE;
                sep1->d = 1;
                sep1->direction = 1;
                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                sep1->notadummy = true;
                sep1->separatrice = new term1;
                readTerm1(fp, sep1->separatrice, N);
                sep1->first_sep_point = nullptr;
                sep1->last_sep_point = nullptr;
                sep1->next_sep = new sep;
                sep1->next_sep->type = STYPE_STABLE;
                sep1->next_sep->d = 1;
                sep1->next_sep->direction = -1;
                sep1->next_sep->notadummy = false;
                sep1->next_sep->separatrice = sep1->separatrice;
                sep1->next_sep->first_sep_point = nullptr;
                sep1->next_sep->last_sep_point = nullptr;
                sep1->next_sep->next_sep = nullptr;
            }
        }
        break;

    case 6: // saddle
        point->separatrices = new sep;
        sep1 = point->separatrices;
        if (s)
            sep1->type = STYPE_CENUNSTABLE;
        else
            sep1->type = STYPE_STABLE;
        sep1->d = 0;
        if ((p_ == 1) && (q_ == 1) &&
            ((point->chart == CHART_V1) || (point->chart == CHART_V2)))
            sep1->direction = -1;
        else
            sep1->direction = 1;
        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_CENUNSTABLE;
            sep1->next_sep->d = 0;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = new sep;
            sep1 = sep1->next_sep->next_sep;
            sep1->type = STYPE_STABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_STABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    case 7: // saddle
        point->separatrices = new sep;
        sep1 = point->separatrices;
        if (s)
            sep1->type = STYPE_CENSTABLE;
        else
            sep1->type = STYPE_UNSTABLE;
        sep1->d = 0;
        if ((p_ == 1) && (q_ == 1) &&
            ((point->chart == CHART_V1) || (point->chart == CHART_V2)))
            sep1->direction = -1;
        else
            sep1->direction = 1;
        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_CENSTABLE;
            sep1->next_sep->d = 0;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = new sep;
            sep1 = sep1->next_sep->next_sep;
            sep1->type = STYPE_UNSTABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_UNSTABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    default:
        point->separatrices = nullptr;

        // change type of node if we have a gcf ?
        y[0] = point->x0;
        y[1] = point->y0;
        ok = false;
        switch (point->chart) {
        case CHART_R2:
            if (eval_term2(gcf_, y) < 0)
                ok = true;
            break;
        case CHART_U1:
            if (eval_term2(gcf_U1_, y) < 0)
                ok = true;
            break;
        case CHART_V1:
            if ((p_ == 1) && (q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V1_, y) < 0)
                ok = true;
            break;
        case CHART_U2:
            if (eval_term2(gcf_U2_, y) < 0)
                ok = true;
            break;
        case CHART_V2:
            if ((p_ == 1) && (q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V2_, y) < 0)
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

    if (fscanf(fp, "%lf ", &(point->epsilon)) != 1)
        return false;
    point->notadummy = true;

    // line at infinity a line of singularities in poincare disc

    if (singinf_ && (point->chart != CHART_R2)) {
        point->next_se = new semi_elementary;
        point->next_se->x0 = point->x0;
        point->next_se->y0 = 0.0;
        if (g_VFResults.dir_vec_field_ == 1)
            point->next_se->type = point->type;
        else
            switch (point->type) {
            case 1:
                point->next_se->type = 3;
                break;
            case 2:
                point->next_se->type = 4;
                break;
            case 3:
                point->next_se->type = 1;
                break;
            case 4:
                point->next_se->type = 2;
                break;
            case 6:
                point->next_se->type = 7;
                break;
            case 7:
                point->next_se->type = 6;
                break;
            default:
                point->next_se->type = point->type;
            }

        point->next_se->chart =
            (point->chart == CHART_U1) ? CHART_V1 : CHART_V2;
        point->next_se->notadummy = false;
        point->next_se->separatrices = point->separatrices;
        point->next_se->epsilon = point->epsilon;
        point->next_se->a11 = point->a11;
        point->next_se->a12 = point->a12;
        point->next_se->a21 = point->a21;
        point->next_se->a22 = point->a22;
        point->next_se->vector_field[0] = nullptr;
        point->next_se->vector_field[1] = nullptr;
        point->next_se->next_se = nullptr;
    }
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadStrongFocusPoint
// -----------------------------------------------------------------------
bool QVFStudy::readStrongFocusPoint(FILE *fp)
{
    double y[2];

    // make room in structure

    strong_focus *last;
    strong_focus *point;

    last = nullptr;
    point = first_sf_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_sf;
    }

    point = new strong_focus;
    if (last == nullptr)
        first_sf_point_ = point;
    else
        last->next_sf = point;

    point->next_sf = nullptr;

    // fill structure

    if (fscanf(fp, "%d %lf %lf ", &(point->stable), &(point->x0),
               &(point->y0)) != 3)
        return false;
    if (fscanf(fp, "%d ", &(point->chart)) != 1)
        return false;

    // change type of node if we have a gcf ?

    y[0] = point->x0;
    y[1] = point->y0;

    switch (point->chart) {
    case CHART_R2:
        if (eval_term2(gcf_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_U1:
        if (eval_term2(gcf_U1_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_V1:
        if (p_ == 1 && q_ == 1)
            y[0] = -y[0];

        if (eval_term2(gcf_V1_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_U2:
        if (eval_term2(gcf_U2_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_V2:
        if (p_ == 1 && q_ == 1)
            y[0] = -y[0];

        if (eval_term2(gcf_V2_, y) < 0)
            point->stable *= -1;
        break;
    }

    // line at infinity a line of singularities in poincare disc

    if (singinf_ && point->chart != CHART_R2) {
        last = point;
        point = new strong_focus;
        last->next_sf = point;
        point->next_sf = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = ((point->chart == CHART_U1) ? CHART_V1 : CHART_V2);
        point->stable =
            last->stable * ((g_VFResults.dir_vec_field_ == -1) ? -1 : 1);
    }

    return true;
}

bool QVFStudy::readWeakFocusPoint(FILE *fp)
{
    // make room in structure

    weak_focus *last;
    weak_focus *point;
    double y[2];

    last = nullptr;
    point = first_wf_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_wf;
    }

    point = new weak_focus;
    if (last == nullptr)
        first_wf_point_ = point;
    else
        last->next_wf = point;

    point->next_wf = nullptr;

    if (fscanf(fp, "%lf %lf ", &(point->x0), &(point->y0)) != 2)
        return false;
    if (fscanf(fp, "%d %d ", &(point->type), &(point->chart)) != 2)
        return false;

    ///* change type of node if we have a gcf ?

    if ((point->type == STYPE_STABLE) || (point->type == STYPE_UNSTABLE)) {
        y[0] = point->x0;
        y[1] = point->y0;

        switch (point->chart) {
        case CHART_R2:
            if (eval_term2(gcf_, y) < 0)
                point->type *= -1;
            break;

        case CHART_U1:
            if (eval_term2(gcf_U1_, y) < 0)
                point->type *= -1;
            break;

        case CHART_V1:
            if ((p_ == 1) && (q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V1_, y) < 0)
                point->type *= -1;
            break;

        case CHART_U2:
            if (eval_term2(gcf_U2_, y) < 0)
                point->type *= -1;
            break;

        case CHART_V2:
            if ((p_ == 1) && (q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V2_, y) < 0)
                point->type *= -1;
            break;
        }
    }

    if (singinf_ && (point->chart != CHART_R2)) {
        point->next_wf = new weak_focus;
        point->next_wf->x0 = point->x0;
        point->next_wf->y0 = 0.0;

        if (g_VFResults.dir_vec_field_ == 1)
            point->next_wf->type = point->type;
        else
            switch (point->type) {
            case STYPE_STABLE:
                point->next_wf->type = STYPE_UNSTABLE;
                break;
            case STYPE_UNSTABLE:
                point->next_wf->type = STYPE_STABLE;
                break;
            default:
                point->next_wf->type = point->type;
                break;
            }

        point->next_wf->chart =
            (point->chart == CHART_U1) ? CHART_V1 : CHART_V2;
        point = point->next_wf;
        point->next_wf = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                  QVFStudy::ReadDegeneratePoint
// -----------------------------------------------------------------------
bool QVFStudy::readDegeneratePoint(FILE *fp)
{
    int n;

    // make room in structure

    degenerate *last;
    degenerate *point;

    last = nullptr;
    point = first_de_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_de;
    }

    point = new degenerate;
    // point->next_de = nullptr;
    // point->blow_up = nullptr;
    if (last == nullptr)
        first_de_point_ = point;
    else
        last->next_de = point;

    // load structure

    if (fscanf(fp, "%lf %lf %lf %d ", &(point->x0), &(point->y0),
               &(point->epsilon), &n) != 4)
        return false;
    // point->blow_up = nullptr;
    if (n) {
        point->blow_up = new blow_up_points;
        readBlowupPoints(fp, point->blow_up, n);
        point->blow_up->blow_up_vec_field = true;
    }

    if (fscanf(fp, "%d ", &(point->chart)) != 1)
        return false;

    point->notadummy = true;

    // line at infinity a line of singularities in poincare disc

    if (singinf_ && point->chart != CHART_R2) {
        last = point;
        point = new degenerate;
        last->next_de = point;
        // point->next_de = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = (last->chart == CHART_U1) ? CHART_V1 : CHART_V2;
        point->epsilon = last->epsilon;
        point->notadummy = false;
        point->blow_up = last->blow_up;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadNodePoint
// -----------------------------------------------------------------------
bool QVFStudy::readNodePoint(FILE *fp)
{
    double y[2];

    // make room in structure

    node *last;
    node *point;

    last = nullptr;
    point = first_node_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_node;
    }

    point = new node;
    if (last == nullptr)
        first_node_point_ = point;
    else
        last->next_node = point;

    // point->next_node = nullptr;

    // load point structure

    if (fscanf(fp, "%lf %lf %d ", &(point->x0), &(point->y0),
               &(point->stable)) != 3)
        return false;
    if (fscanf(fp, "%d ", &(point->chart)) != 1)
        return false;

    // change type of node if we have a gcf ?

    y[0] = point->x0;
    y[1] = point->y0;

    switch (point->chart) {
    case CHART_R2:
        if (eval_term2(gcf_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_U1:
        if (eval_term2(gcf_U1_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_V1:
        if (p_ == 1 && q_ == 1)
            y[0] = -y[0];

        if (eval_term2(gcf_V1_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_U2:
        if (eval_term2(gcf_U2_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_V2:
        if (p_ == 1 && q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V2_, y) < 0)
            point->stable *= -1;
        break;
    }

    // if line at infinity a line of singularities in poincare disc:
    // duplicate singularity by using a symmetry

    if (singinf_ != 0 && point->chart != CHART_R2) {
        last = point;
        point = new node;
        last->next_node = point;
        point->next_node = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = (last->chart == CHART_U1) ? CHART_V1 : CHART_V2;
        // TODO: check this operation for precedence
        point->stable =
            (last->stable && (g_VFResults.dir_vec_field_ == -1)) ? -1 : 1;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadTransformations
// -----------------------------------------------------------------------
bool QVFStudy::readTransformations(FILE *fp, transformations *trans, int n)
{

    int i;

    if (fscanf(fp, "%lf %lf %d %d %d %d %d %d %d", &(trans->x0), &(trans->y0),
               &(trans->c1), &(trans->c2), &(trans->d1), &(trans->d2),
               &(trans->d3), &(trans->d4), &(trans->d)) != 9)
        return false;

    trans->next_trans = nullptr;

    for (i = 2; i <= n; i++) {
        trans->next_trans = new transformations;
        trans = trans->next_trans;

        if (fscanf(fp, "%lf %lf %d %d %d %d %d %d %d", &(trans->x0),
                   &(trans->y0), &(trans->c1), &(trans->c2), &(trans->d1),
                   &(trans->d2), &(trans->d3), &(trans->d4), &(trans->d)) != 9)
            return false;

        trans->next_trans = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadBlowupPoints
// -----------------------------------------------------------------------
bool QVFStudy::readBlowupPoints(FILE *fp, blow_up_points *b, int n)
{
    int i, N, typ;

    for (i = 1; i <= n; i++) {
        if (fscanf(fp, "%d ", &(b->n)) != 1)
            return false;

        b->trans = new transformations;

        readTransformations(fp, b->trans, b->n);
        if (fscanf(fp, "%lf %lf ", &(b->x0), &(b->y0)) != 2)
            return false;
        if (fscanf(fp, "%lf %lf %lf %lf ", &(b->a11), &(b->a12), &(b->a21),
                   &(b->a22)) != 4)
            return false;
        readVectorField(fp, b->vector_field);
        b->sep = new term1;
        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        readTerm1(fp, b->sep, N);

        if (fscanf(fp, "%d ", &typ) != 1)
            return false;
        switch (typ) {
        case 1:
            b->type = STYPE_UNSTABLE;
            break;
        case 2:
            b->type = STYPE_STABLE;
            break;
        case 3:
            b->type = STYPE_UNSTABLE;
            break;
        case 4:
            b->type = STYPE_STABLE;
            break;
        case 5:
            b->type = STYPE_CENUNSTABLE;
            break;
        case 6:
            b->type = STYPE_CENSTABLE;
            break;
        case 7:
            b->type = STYPE_CENUNSTABLE;
            break;
        case 8:
            b->type = STYPE_CENSTABLE;
            break;
        case 9:
            b->type = STYPE_UNSTABLE;
            break;
        case 10:
            b->type = STYPE_UNSTABLE;
            break;
        case 11:
            b->type = STYPE_STABLE;
            break;
        case 12:
            b->type = STYPE_STABLE;
            break;
        case 13:
            b->type = STYPE_UNSTABLE;
            break;
        case 14:
            b->type = STYPE_UNSTABLE;
            break;
        case 15:
            b->type = STYPE_STABLE;
            break;
        case 16:
            b->type = STYPE_STABLE;
            break;
        }
        b->first_sep_point = nullptr;
        b->last_sep_point = nullptr;
        b->next_blow_up_point = nullptr;

        if (i < n) {
            b->next_blow_up_point = new blow_up_points;
            b = b->next_blow_up_point;
        }
    }
    return true;
}

void QVFStudy::dumpSeparatrices(QTextEdit *m, sep *separ, int margin)
{
    QString s;
    char smargin[80];
    const char *stype;
    strcpy(smargin, "                              ");
    smargin[margin] = 0; // make number of spaces

    if (separ == nullptr) {
        DUMP(("%s/", smargin))
        return;
    }

    while (separ != nullptr) {
        switch (separ->type) {
        case STYPE_STABLE:
            stype = "Stable         ";
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

        DUMP(("%sType=%s  Dir=%-2d  original=%d", smargin, stype,
              separ->direction, separ->notadummy))
        if (separ->d == 0) {
            DUMP(("%sTaylor: (x,y)=(t,%s)", smargin,
                  dumpPoly1(separ->separatrice, "t")))
        } else {
            DUMP(("%sTaylor: (x,y)=(%s,t)", smargin,
                  dumpPoly1(separ->separatrice, "t")))
        }

        separ = separ->next_sep;
    }
}

void QVFStudy::dumpSingularities(QTextEdit *m, genericsingularity *p,
                                 const char *type, bool longversion)
{
    const char *chart;
    QString s;
    QByteArray ss;

    saddle *sa;
    degenerate *de;
    strong_focus *sf;
    weak_focus *wf;
    node *no;
    semi_elementary *se;

    while (p != nullptr) {
        switch (p->chart) {
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
        DUMP(("%s (x0,y0)=(%g,%g)  Chart %s", type, (float)(p->x0),
              (float)(p->y0), chart))
        if (longversion) {
            switch (type[0] * type[1]) {
            case 'S' * 'A':
                // saddle
                sa = (saddle *)p;
                DUMP(("   Epsilon = %g  original=%d", (float)(sa->epsilon),
                      sa->notadummy))
                DUMP(("   Transformation Matrix = [ [%g,%g], [%g,%g] ]",
                      (float)(sa->a11), (float)(sa->a12), (float)(sa->a21),
                      (float)(sa->a22)))
                DUMP(("   Vector Field:"))
                DUMP(
                    ("      x' = %s", dumpPoly2(sa->vector_field[0], "x", "y")))
                DUMP(
                    ("      y' = %s", dumpPoly2(sa->vector_field[1], "x", "y")))
                DUMP(("   Separatrices:"))
                dumpSeparatrices(m, sa->separatrices, 6);
                break;

            case 'D' * 'E':
                // degenerate
                de = (degenerate *)p;
                DUMP(("   Epsilon = %g  original=%d", (float)(de->epsilon),
                      de->notadummy))
                break;

            case 'S' * 'T':
                // strong focus
                sf = (strong_focus *)p;
                if (sf->stable == -1)
                    s = "(stable)";
                else if (sf->stable == +1)
                    s = "(unstable)";
                else
                    s = "( ??? )";

                ss = s.toLatin1();
                DUMP(("    Stability = %d %s", sf->stable, (const char *)ss))
                break;
            case 'W' * 'E':
                // weak focus
                wf = (weak_focus *)p;
                DUMP(("    Type = %d", wf->type))
                break;
            case 'N' * 'O':
                // node
                no = (node *)p;
                if (no->stable == -1)
                    s = "(stable)";
                else if (no->stable == +1)
                    s = "(unstable)";
                else
                    s = "( ??? )";
                ss = s.toLatin1();
                DUMP(("    Stability = %d %s", no->stable, (const char *)ss))
                break;
            case 'S' * 'E':
                // semi-elementary
                se = (semi_elementary *)p;
                DUMP(("   Type    = %d", se->type))
                DUMP(("   Epsilon = %g  original=%d", (float)(se->epsilon),
                      se->notadummy))
                DUMP(("   Transformation Matrix = [ [%g,%g], [%g,%g] ]",
                      (float)(se->a11), (float)(se->a12), (float)(se->a21),
                      (float)(se->a22)))
                DUMP(("   Vector Field = "))
                DUMP(
                    ("      x' = %s", dumpPoly2(se->vector_field[0], "x", "y")))
                DUMP(
                    ("      y' = %s", dumpPoly2(se->vector_field[1], "x", "y")))
                DUMP(("   Separatrices:"))
                dumpSeparatrices(m, se->separatrices, 6);
                break;
            }
            DUMP((" "))
        }
        p = p->next;
    }
}

void QVFStudy::dump(QString basename, QString info)
{
    QTextEdit *m;
    QString s;
    QByteArray ss;

    return;
    m = new QTextEdit();

    DUMP(("General Info"))
    DUMP(("------------"))
    DUMP(("  Type of study = %d", typeofstudy_))
    DUMP(("  (p,q) = (%d,%d)", p_, q_))
    DUMP(("  plweights = %d", plweights_))
    DUMP(("  doubles p,q,p+q,p-1,q-1,q-p = (%g,%g,%g,%g,%g,%g)",
          (float)double_p_, (float)double_q_, (float)double_p_plus_q_,
          (float)double_p_minus_1_, (float)double_q_minus_1_,
          (float)double_q_minus_p_))
    DUMP(("  Range x: [%g,%g] Range y: [%g,%g]", (float)xmin_, (float)xmax_,
          (float)ymin_, (float)ymax_))
    DUMP(("  Line at infinity singular? %d", singinf_))
    DUMP(("  Direction of vector field: %d", g_VFResults.dir_vec_field_))
    DUMP((" "))
    DUMP(("Vector Fields"))
    DUMP(("-------------"))
    DUMP(("  Finite chart:"))
    DUMP(("     x' = %s", dumpPoly2(f_vec_field_[0], "x", "y")))
    DUMP(("     y' = %s", dumpPoly2(f_vec_field_[1], "x", "y")))
    DUMP(("  U1 chart:"))
    DUMP(("     x' = %s", dumpPoly2(vec_field_U1_[0], "x", "y")))
    DUMP(("     y' = %s", dumpPoly2(vec_field_U1_[1], "x", "y")))
    DUMP(("  U2 chart:"))
    DUMP(("     x' = %s", dumpPoly2(vec_field_U2_[0], "x", "y")))
    DUMP(("     y' = %s", dumpPoly2(vec_field_U2_[1], "x", "y")))
    DUMP(("  V1 chart:"))
    DUMP(("     x' = %s", dumpPoly2(vec_field_V1_[0], "x", "y")))
    DUMP(("     y' = %s", dumpPoly2(vec_field_V1_[1], "x", "y")))
    DUMP(("  V2 chart:"))
    DUMP(("     x' = %s", dumpPoly2(vec_field_V2_[0], "x", "y")))
    DUMP(("     y' = %s", dumpPoly2(vec_field_V2_[1], "x", "y")))
    DUMP(("  Cylinder chart: (Co=cos(theta),Si=sin(theta))"))
    DUMP(("     r'     = %s", dumpPoly3(vec_field_C_[0], "r", "Co", "Si")))
    DUMP(("     theta' = %s", dumpPoly3(vec_field_C_[1], "r", "Co", "Si")))
    DUMP((" "))
    DUMP(("Greatest Common Factor"))
    DUMP(("----------------------"))
    DUMP(("  Finite chart:   %s", dumpPoly2(gcf_, "x", "y")))
    DUMP(("  U1 chart:       %s", dumpPoly2(gcf_U1_, "x", "y")))
    DUMP(("  U2 chart:       %s", dumpPoly2(gcf_U2_, "x", "y")))
    DUMP(("  V1 chart:       %s", dumpPoly2(gcf_V1_, "x", "y")))
    DUMP(("  V2 chart:       %s", dumpPoly2(gcf_V2_, "x", "y")))
    DUMP(("  Cylinder chart: %s", dumpPoly3(gcf_C_, "r", "Co", "Si")))
    DUMP((" "))
    DUMP(("Singular points - summary"))
    DUMP(("-------------------------"))
    DUMP((" "))
    dumpSingularities(m, (genericsingularity *)first_saddle_point_,
                      "SADDLE         ", false);
    dumpSingularities(m, (genericsingularity *)first_se_point_,
                      "SEMI-ELEMENTARY", false);
    dumpSingularities(m, (genericsingularity *)first_node_point_,
                      "NODE           ", false);
    dumpSingularities(m, (genericsingularity *)first_wf_point_,
                      "WEAK FOCUS     ", false);
    dumpSingularities(m, (genericsingularity *)first_sf_point_,
                      "STRONG FOCUS   ", false);
    dumpSingularities(m, (genericsingularity *)first_de_point_,
                      "DEGENERATE     ", false);
    DUMP((" "))
    DUMP(("Singular points - full description"))
    DUMP(("----------------------------------"))
    DUMP((" "))
    dumpSingularities(m, (genericsingularity *)first_saddle_point_,
                      "SADDLE         ", true);
    dumpSingularities(m, (genericsingularity *)first_se_point_,
                      "SEMI-ELEMENTARY", true);
    dumpSingularities(m, (genericsingularity *)first_node_point_,
                      "NODE           ", true);
    dumpSingularities(m, (genericsingularity *)first_wf_point_,
                      "WEAK FOCUS     ", true);
    dumpSingularities(m, (genericsingularity *)first_sf_point_,
                      "STRONG FOCUS   ", true);
    dumpSingularities(m, (genericsingularity *)first_de_point_,
                      "DEGENERATE     ", true);
    DUMP((" "))
    DUMP(("Default integration parameters"))
    DUMP(("------------------------------"))
    DUMP((" "))
    DUMP(("  LC Value           %d", config_lc_value_))
    DUMP(("  Max Step Size      %g", (float)config_hma_))
    DUMP(("  Min Step Size      %g", (float)config_hmi_))
    DUMP(("  Step Size          %g", (float)config_step_))
    DUMP(("  Current Step Size  %g", (float)config_currentstep_))
    DUMP(("  Tolerance          %g", (float)config_tolerance_))
    DUMP(("  Projection         %g", (float)config_projection_))
    DUMP(("  Integration Points %d", config_intpoints_))
    DUMP(("  LC NumPoints       %d", config_lc_numpoints_))
    DUMP(("  Dashes             %d", config_dashes_))

    if (config_kindvf_ == INTCONFIG_ORIGINAL)
        s = "(Original)";
    else if (config_kindvf_ == INTCONFIG_REDUCED)
        s = "(Reduced)";
    else
        s = "???";

    ss = s.toLatin1();
    DUMP(("  Kind of VF         %d %s", config_kindvf_, (const char *)ss))

    ss = basename.toLatin1() + "_fin.tab";
    QFile ffin(ss);
    if (ffin.open(QIODevice::ReadOnly)) {
        DUMP((" "))
        DUMP(("Contents of filename_fin.tab"))
        DUMP(("----------------------------"))
        DUMP((" "))
        QTextStream tfin(&ffin);
        while (!tfin.atEnd()) {
            s = tfin.readLine();
            m->append(s);
        }
        ffin.close();
    } else {
        DUMP((" "))
        DUMP(("Contents of filename_fin.tab"))
        DUMP(("----------------------------"))
        DUMP((" "))
        DUMP(("FILE DOES NOT EXIST."))
    }
    QFile finf(basename + "_inf.tab");
    if (finf.open(QIODevice::ReadOnly)) {
        DUMP((" "))
        DUMP(("Contents of filename_inf.tab"))
        DUMP(("----------------------------"))
        DUMP((" "))

        QTextStream tinf(&finf);
        while (!tinf.atEnd()) {
            s = tinf.readLine();
            m->append(s);
        }
        finf.close();
    } else {
        DUMP((" "))
        DUMP(("Contents of filename_inf.tab"))
        DUMP(("----------------------------"))
        DUMP((" "))
        DUMP(("FILE DOES NOT EXIST."))
    }
    QFile fcurve(basename + "_veccurve.tab");
    if (fcurve.open(QIODevice::ReadOnly)) {
        DUMP((" "))
        DUMP(("Contents of filename_veccurve.tab"))
        DUMP(("----------------------------"))
        DUMP((" "))

        QTextStream tcurve(&fcurve);
        while (!tcurve.atEnd()) {
            s = tcurve.readLine();
            m->append(s);
        }
        fcurve.close();
    } else {
        DUMP((" "))
        DUMP(("Contents of filename_veccurve.tab"))
        DUMP(("----------------------------"))
        DUMP((" "))
        DUMP(("FILE DOES NOT EXIST."))
    }
    DUMP((" "))
    ss = info.toLatin1();
    DUMP(("%s", (const char *)ss))

    m->setFont(*(g_p4app->courierFont_));
    m->setReadOnly(true);
    m->resize(640, 480);
    setP4WindowTitle(m, "DUMP SCREEN");
    m->show();
}

void QVFStudy::setupCoordinateTransformations(void)
{
    if (!plweights_) {
        U1_to_sphere = U1_to_psphere;
        U2_to_sphere = U2_to_psphere;
        V1_to_sphere = VV1_to_psphere;
        V2_to_sphere = VV2_to_psphere;
        sphere_to_R2 = psphere_to_R2;
        R2_to_sphere = R2_to_psphere;
        sphere_to_U1 = psphere_to_U1;
        sphere_to_U2 = psphere_to_U2;
        sphere_to_V1 = psphere_to_VV1;
        sphere_to_V2 = psphere_to_VV2;

        integrate_sphere_sep = integrate_poincare_sep;
        integrate_sphere_orbit = integrate_poincare_orbit;
        eval_lc = eval_lc_poincare;
        less2 = less_poincare;
        change_dir = change_dir_poincare;

        switch (typeofview_) {
        case TYPEOFVIEW_SPHERE:
            viewcoord_to_sphere = ucircle_psphere;
            sphere_to_viewcoord = psphere_ucircle;
            finite_to_viewcoord = finite_ucircle;
            is_valid_viewcoord = isvalid_psphereviewcoord;
            sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
            break;

        case TYPEOFVIEW_PLANE:
            viewcoord_to_sphere = R2_to_psphere;
            sphere_to_viewcoord = psphere_to_R2;
            finite_to_viewcoord = identitytrf_R2;
            is_valid_viewcoord = isvalid_R2viewcoord;
            sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
            break;

        case TYPEOFVIEW_U1:
            viewcoord_to_sphere = xyrevU1_to_psphere;
            sphere_to_viewcoord = psphere_to_xyrevU1;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord = isvalid_U1viewcoord;
            sphere_to_viewcoordpair = psphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_U2:
            viewcoord_to_sphere = U2_to_psphere;
            sphere_to_viewcoord = psphere_to_U2;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord = isvalid_U2viewcoord;
            sphere_to_viewcoordpair = psphere_to_viewcoordpair_discontinuousy;
            break;

        case TYPEOFVIEW_V1:
            viewcoord_to_sphere = xyrevV1_to_psphere;
            sphere_to_viewcoord = psphere_to_xyrevV1;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord = isvalid_V1viewcoord;
            sphere_to_viewcoordpair = psphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_V2:
            viewcoord_to_sphere = V2_to_psphere;
            sphere_to_viewcoord = psphere_to_V2;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord = isvalid_V2viewcoord;
            sphere_to_viewcoordpair = psphere_to_viewcoordpair_discontinuousy;
            break;
        }
    } else {
        U1_to_sphere = U1_to_plsphere;
        U2_to_sphere = U2_to_plsphere;
        V1_to_sphere = V1_to_plsphere;
        V2_to_sphere = V2_to_plsphere;
        sphere_to_R2 = plsphere_to_R2;
        R2_to_sphere = R2_to_plsphere;
        sphere_to_U1 = plsphere_to_U1;
        sphere_to_U2 = plsphere_to_U2;
        sphere_to_V1 = plsphere_to_V1;
        sphere_to_V2 = plsphere_to_V2;

        integrate_sphere_sep = integrate_lyapunov_sep;
        integrate_sphere_orbit = integrate_lyapunov_orbit;
        eval_lc = eval_lc_lyapunov;
        less2 = less_lyapunov;
        change_dir = change_dir_lyapunov;

        switch (typeofview_) {
        case TYPEOFVIEW_SPHERE:
            viewcoord_to_sphere = annulus_plsphere;
            sphere_to_viewcoord = plsphere_annulus;
            finite_to_viewcoord = finite_annulus;
            is_valid_viewcoord = isvalid_plsphereviewcoord;
            sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
            break;
        case TYPEOFVIEW_PLANE:
            viewcoord_to_sphere = R2_to_plsphere;
            sphere_to_viewcoord = plsphere_to_R2;
            finite_to_viewcoord = identitytrf_R2;
            is_valid_viewcoord = isvalid_R2viewcoord;
            sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
            break;
        case TYPEOFVIEW_U1:
            viewcoord_to_sphere = xyrevU1_to_plsphere;
            sphere_to_viewcoord = plsphere_to_xyrevU1;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord = isvalid_U1viewcoord;
            sphere_to_viewcoordpair = plsphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_U2:
            viewcoord_to_sphere = U2_to_plsphere;
            sphere_to_viewcoord = plsphere_to_U2;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord = isvalid_U2viewcoord;
            sphere_to_viewcoordpair = plsphere_to_viewcoordpair_discontinuousy;
            break;

        case TYPEOFVIEW_V1:
            viewcoord_to_sphere = xyrevV1_to_plsphere;
            sphere_to_viewcoord = plsphere_to_xyrevV1;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord = isvalid_V1viewcoord;
            sphere_to_viewcoordpair = plsphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_V2:
            viewcoord_to_sphere = V2_to_plsphere;
            sphere_to_viewcoord = plsphere_to_V2;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord = isvalid_V2viewcoord;
            sphere_to_viewcoordpair = plsphere_to_viewcoordpair_discontinuousy;
            break;
        }
    }
}
