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

#include "P4ParentStudy.hpp"

#include <QFile>

#include <locale.h>

#include "P4VFStudy.hpp"
#include "math_changedir.hpp"
#include "math_charts.hpp"
#include "math_orbits.hpp"
#include "math_p4.hpp"
#include "math_polynom.hpp"
#include "math_regions.hpp"
#include "math_separatingcurves.hpp"
#include "math_separatrice.hpp"

// -----------------------------------------------------------------------
//                              P4ParentStudy CONSTRUCTOR
// -----------------------------------------------------------------------
P4ParentStudy::P4ParentStudy()
{
    selected_ucoord_[0] = selected_ucoord_[1] = 0;

    double_p_ = p_;
    double_q_ = q_;
    double_p_plus_q_ = p_ + q_;
    double_p_minus_1_ = p_ - 1;
    double_q_minus_1_ = q_ - 1;
    double_q_minus_p_ = q_ - p_;

    setupCoordinateTransformations();
}

// -----------------------------------------------------------------------
//                      P4ParentStudy DESTRUCTOR
// -----------------------------------------------------------------------
// P4ParentStudy::~P4ParentStudy() { reset(); }

// -----------------------------------------------------------------------
//          P4ParentStudy::reset
// -----------------------------------------------------------------------
void P4ParentStudy::reset()
{
    int i;
    vf_.clear();
    // vfK_ = nullptr;
    K_ = 0;

    xmin_ = -1.0;
    xmax_ = 1.0;
    ymin_ = -1.0;
    ymax_ = 1.0;
    p_ = 1;
    q_ = 1;
    plweights_ = false;
    typeofstudy_ = TYPEOFSTUDY_ALL;
    typeofview_ = TYPEOFVIEW_SPHERE;
    config_projection_ = DEFAULT_PROJECTION;
    double_p_ = p_;
    double_q_ = q_;
    double_p_plus_q_ = p_ + q_;
    double_p_minus_1_ = p_ - 1;
    double_q_minus_1_ = q_ - 1;
    double_q_minus_p_ = q_ - p_;
    config_lc_value_ = DEFAULT_LCORBITS;
    config_lc_numpoints_ = DEFAULT_LCPOINTS;
    config_currentstep_ = DEFAULT_STEPSIZE;
    config_dashes_ = DEFAULT_LINESTYLE;
    config_kindvf_ = DEFAULT_INTCONFIG;
    plotVirtualSingularities_ = DEFAULTPLOTVIRTUALSINGULARITIES;

    // delete orbits
    orbits_.clear();
    // current_orbit_ = nullptr;

    // delete limit cycles
    limCycles_.clear();
    // current_lim_cycle_ = nullptr;

    selected_ucoord_[0] = selected_ucoord_[1] = 0;
    saddlePoints_.clear();
    selectedSaddlePointIndex_ = -1;
    sePoints_.clear();
    selectedSePointIndex_ = -1;
    dePoints_.clear();
    selectedDePointIndex_ = -1;
    seps_.clear();
    selectedSepIndex_ = -1;
    deSeps_.clear();
    selectedDeSepIndex_ = -1;

    separatingCurves_.clear();

    config_hma_ = DEFAULT_HMA;
    config_hmi_ = DEFAULT_HMI;
    config_branchhmi_ = DEFAULT_BRANCHHMI;
    config_step_ = DEFAULT_STEPSIZE;
    config_tolerance_ = DEFAULT_TOLERANCE;
    config_intpoints_ = DEFAULT_INTPOINTS;

    setupCoordinateTransformations();
}

// -----------------------------------------------------------------------
//          P4ParentStudy::readPiecewiseData
// -----------------------------------------------------------------------
bool P4ParentStudy::readPiecewiseData(FILE *fp)
{
    int j, k, v;
    if (gThisVF->numSeparatingCurves_ == 0)
        return true;
    if (fscanf(fp, "%d\n", &v) != 1)
        return false;
    if (v != gThisVF->numVFRegions_)
        return false;

    if (gThisVF->numVFRegions_ > 0) {
        for (j = 0; j < gThisVF->numVFRegions_; j++) {
            if (fscanf(fp, "%d", &v) != 1 || v != gThisVF->vfRegions_[j].vfIndex)
                return false;
            for (k = 0; k < gThisVF->numSeparatingCurves_; k++) {
                if (fscanf(fp, "%d", &v) != 1 ||
                    v != gThisVF->vfRegions_[j].signs[k])
                    return false;
            }
            fscanf(fp, "\n");
        }
    }

    if (fscanf(fp, "%d\n", &v) != 1 || v != gThisVF->numCurveRegions_)
        return false;
    if (gThisVF->numCurveRegions_ > 0) {
        for (j = 0; j < gThisVF->numCurveRegions_; j++) {
            if (fscanf(fp, "%d", &v) != 1 ||
                v != gThisVF->curveRegions_[j].curveIndex)
                return false;
            for (k = 0; k < gThisVF->numSeparatingCurves_; k++) {
                if (fscanf(fp, "%d", &v) != 1 ||
                    v != gThisVF->curveRegions_[j].signs[k])
                    return false;
            }
            fscanf(fp, "\n");
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4ParentStudy::readTables
// -----------------------------------------------------------------------
bool P4ParentStudy::readTables(const QString &basename, bool evalpiecewisedata,
                               bool onlytry)
{
    FILE *fpvec;
    FILE *fpinf;
    FILE *fpfin;
    FILE *fpcurv;
    int j, v;
    int p, q, prec, numcurves, numvf;

    setlocale(LC_ALL, "C");

    if (evalpiecewisedata) {
        /*
            The curves result file contains:

            - Identifier P5
            - p, q, precision, numcurves
            - For each curve:
                . curve polynomial in (x,y) in finite chart
                . curve polynomial in U1 chart
                . curve polynomial in U2 chart
                . curve polynomial in V1 chart
                . curve polynomial in V2 chart
                . (only when $(p,q)\not=(1,1)$: curve polynomial in cylindrical
           coordinates . Plot data in 5 different charts.
        */

        separatingCurves_.clear();

        fpcurv = fopen(QFile::encodeName(basename + "_curves.tab"), "rt");
        if (fpcurv == nullptr)
            return false;

        if (fscanf(fpcurv, "p5\n%d %d %d %d\n", &p, &q, &prec, &numcurves) !=
            4) {
            fclose(fpcurv);
            return false;
        }
        if (numcurves != gThisVF->numSeparatingCurves_ || p != gThisVF->p_ ||
            q != gThisVF->q_) {
            if (onlytry) {
                fclose(fpcurv);
                return false;
            }
            reset();
            fclose(fpcurv);
            return false;
        }
        p_ = p;
        q_ = q;

        plweights_ = ((p_ == 1 && q_ == 1) ? false : true);
        double_p_ = static_cast<double>(p_);
        double_q_ = static_cast<double>(q_);
        double_p_plus_q_ = static_cast<double>(p_ + q_);
        double_p_minus_1_ = static_cast<double>(p_ - 1);
        double_q_minus_1_ = static_cast<double>(q_ - 1);
        double_q_minus_p_ = static_cast<double>(q_ - p_);

        /*P5 Store precision _prec */

        for (j = 0; j < gThisVF->numSeparatingCurves_; j++) {
            if (!readSeparatingCurve(fpcurv)) {
                separatingCurves_.clear();
                fclose(fpcurv);
                return false;
            }
            if (!readSeparatingCurvePoints(fpcurv, separatingCurves_[j].points,
                                           j)) {
                separatingCurves_.clear();
                fclose(fpcurv);
                return false;
            }
        }

        for (j = 0; j < gThisVF->numSeparatingCurves_; j++)
            gThisVF->resampleSeparatingCurve(j);

        fclose(fpcurv);
        // dump( basename );
        return true;
    }

    reset(); // initialize structures, delete previous vector field if any

    fpvec = fopen(QFile::encodeName(basename + "_vec.tab"), "rt");
    if (fpvec == nullptr)
        return false;

    if (fscanf(fpvec, "P5\n%d %d\n", &numvf, &v) != 2) {
        fclose(fpvec);
        return false;
    }
    if (v != gThisVF->numSeparatingCurves_ || numvf != gThisVF->numVF_) {
        fclose(fpvec);
        return false;
    }

    // allocate room for all vector fields
    // FIXME
    // vf_ = (QVFStudy **)realloc(vf_, sizeof(QVFStudy *) * gThisVF->numVF_);
    // vf_.reserve(sizeof(std::shared_ptr<P4VFStudy>) * gThisVF->numVF_);
    for (j = 0; j < gThisVF->numVF_; j++) {
        vf_.emplace_back(std::make_unique<P4VFStudy>(this));
    }

    if (fscanf(fpvec, "%d\n%d\n%d\n", &typeofstudy_, &p, &q) != 3) {
        reset();
        fclose(fpvec);
        return false;
    }

    if (p != gThisVF->p_ || q != gThisVF->q_) {
        reset();
        fclose(fpvec);
        return false;
    }
    p_ = p;
    q_ = q;

    plweights_ = ((p_ == 1 && q_ == 1) ? false : true);
    double_p_ = static_cast<double>(p_);
    double_q_ = static_cast<double>(q_);
    double_p_plus_q_ = static_cast<double>(p_ + q_);
    double_p_minus_1_ = static_cast<double>(p_ - 1);
    double_q_minus_1_ = static_cast<double>(q_ - 1);
    double_q_minus_p_ = static_cast<double>(q_ - p_);

    if (typeofstudy_ == TYPEOFSTUDY_ONE) {
        if (fscanf(fpvec, "%lf %lf %lf %lf", &xmin_, &xmax_, &ymin_, &ymax_) !=
            4) {
            reset();
            fclose(fpvec);
            return false;
        }
        p_ = q_ = 1;
        typeofview_ = TYPEOFVIEW_PLANE;
    } else
        typeofview_ = TYPEOFVIEW_SPHERE;

    // read the separating curves
    for (j = 0; j < gThisVF->numSeparatingCurves_; j++) {
        separatingCurves_.clear();
        if (!readSeparatingCurve(fpvec)) {
            reset();
            fclose(fpvec);
            return false;
        }
    }

    if (!readPiecewiseData(fpvec)) {
        reset();
        fclose(fpvec);
        return false;
    }

    if (typeofstudy_ != TYPEOFSTUDY_INF) {
        fpfin = fopen(QFile::encodeName(basename + "_fin.tab"), "rt");
        if (fpfin == nullptr) {
            fclose(fpvec);
            reset();
            return false;
        }
    } else
        fpfin = nullptr;

    if (typeofstudy_ != TYPEOFSTUDY_ONE && typeofstudy_ != TYPEOFSTUDY_FIN) {
        fpinf = fopen(QFile::encodeName(basename + "_inf.tab"), "rt");
        if (fpinf == nullptr) {
            fclose(fpvec);
            if (fpfin != nullptr)
                fclose(fpfin);
            reset();
            return false;
        }
    } else
        fpinf = nullptr;

    for (j = 0; j < gThisVF->numVF_; j++) {
        if (!vf_[j]->readTables(fpvec, fpfin, fpinf)) {
            reset();
            if (fpinf != nullptr)
                fclose(fpinf);
            if (fpfin != nullptr)
                fclose(fpfin);
            fclose(fpvec);
            return false;
        }
    }

    if (fpinf != nullptr)
        fclose(fpinf);
    if (fpfin != nullptr)
        fclose(fpfin);
    fclose(fpvec);

    readTables(basename, true, true); // try to read the piecewise curve points
                                      // as well if they are present on disk
    // dump(basename);
    examinePositionsOfSingularities();
    return true;
}

// -----------------------------------------------------------------------
//          P4ParentStudy::readArbitraryCurve
// -----------------------------------------------------------------------
// P4 version, this is for visualizing a curve on the plane
bool P4ParentStudy::readArbitraryCurve(QString basename)
{
    int N, degree_curve;
    setlocale(LC_ALL, "C");

    FILE *fp = fopen(QFile::encodeName(basename + "_veccurve.tab"), "rt");
    if (fp == nullptr) {
        // TODO: this is the P4VFStudy::dump
        // dump(basename, "Cannot open file " + basename + "_veccurve.tab");
        return false;
    }

    p4curves::curves new_curve;
    if (fscanf(fp, "%d", &degree_curve) != 1 || degree_curve < 0)
        return false;
    if (degree_curve == 0)
        return true;

    if (degree_curve > 0) {
        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm2(fp, new_curve.r2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm2(fp, new_curve.u1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm2(fp, new_curve.u2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm2(fp, new_curve.v1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm2(fp, new_curve.v2, N))
            return false;

        if (p_ != 1 || q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1 || N < 0)
                return false;
            if (!readTerm3(fp, new_curve.c, N))
                return false;
        }
    }

    arbitraryCurves_.push_back(std::move(new_curve));
    return true;
}

// -----------------------------------------------------------------------
//          P4ParentStudy::readSeparatingCurve
// -----------------------------------------------------------------------
bool P4ParentStudy::readSeparatingCurve(FILE *fp)
{
    int N, degree_sep;
    p4curves::curves dummy;

    setlocale(LC_ALL, "C");

    if (fscanf(fp, "%d", &degree_sep) != 1 || degree_sep < 0)
        return false;
    if (degree_sep == 0)
        return true;

    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;
    if (!readTerm2(fp, dummy.r2, N))
        return false;

    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;
    if (!readTerm2(fp, dummy.u1, N))
        return false;

    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;
    if (!readTerm2(fp, dummy.u2, N))
        return false;

    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;
    if (!readTerm2(fp, dummy.v1, N))
        return false;

    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;
    if (!readTerm2(fp, dummy.v2, N))
        return false;

    if (plweights_) {
        if (fscanf(fp, "%d", &N) != 1 || N < 0)
            return false;
        if (!readTerm3(fp, dummy.c, N))
            return false;
    }

    separatingCurves_.push_back(std::move(dummy));
    return true;
}

// -----------------------------------------------------------------------
//          P4ParentStudy::setupCoordinateTransformations
// -----------------------------------------------------------------------
void P4ParentStudy::setupCoordinateTransformations()
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

// -----------------------------------------------------------------------
//          P4ParentStudy::readPiecewiseData
// -----------------------------------------------------------------------
bool P4ParentStudy::readPiecewiseData(FILE *fp)
{
    int j, k, v;

    if (gThisVF->numSeparatingCurves_ == 0)
        return true;

    if (fscanf(fp, "%d\n", &v) != 1)
        return false;

    if (v != gThisVF->numVFRegions_)
        return false;

    if (gThisVF->numVFRegions_ > 0) {
        for (j = 0; j < gThisVF->numVFRegions_; j++) {
            if (fscanf(fp, "%d", &v) != 1)
                return false;
            if (v != gThisVF->vfRegions_[j].vfIndex)
                return false;
            for (k = 0; k < gThisVF->numSeparatingCurves_; k++) {
                if (fscanf(fp, "%d", &v) != 1)
                    return false;
                if (v != gThisVF->vfRegions_[j].signs[k])
                    return false;
            }
            fscanf(fp, "\n");
        }
    }

    if (fscanf(fp, "%d\n", &v) != 1)
        return false;
    if (v != gThisVF->numCurveRegions_)
        return false;
    if (gThisVF->numCurveRegions_ > 0) {
        for (j = 0; j < gThisVF->numCurveRegions_; j++) {
            if (fscanf(fp, "%d", &v) != 1)
                return false;
            if (v != gThisVF->curveRegions_[j].curveIndex)
                return false;
            for (k = 0; k < gThisVF->numSeparatingCurves_; k++) {
                if (fscanf(fp, "%d", &v) != 1)
                    return false;
                if (v != gThisVF->curveRegions_[j].signs[k])
                    return false;
            }
            fscanf(fp, "\n");
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4ParentStudy::deleteVFs
// -----------------------------------------------------------------------
void P4ParentStudy::clearVFs() { vf_.clear(); }

// -----------------------------------------------------------------------
//          P4ParentStudy::examinePositionsOfSingularities
// -----------------------------------------------------------------------
void P4ParentStudy::examinePositionsOfSingularities()
{
    int i;
    std::vector<positionitem> positions;
    int numpositions{0};

    for (i = 0; i < gThisVF->numVF_; i++) {
        for (auto &sing : vf_[i]->saddlePoints_)
            markSingularity(sing, positions, numpositions, i, plweights_);
        for (auto &sing : vf_[i]->sePoints_)
            markSingularity(sing, positions, numpositions, i, plweights_);
        for (auto &sing : vf_[i]->nodePoints_)
            markSingularity(sing, positions, numpositions, i, plweights_);
        for (auto &sing : vf_[i]->sfPoints_)
            markSingularity(sing, positions, numpositions, i, plweights_);
        for (auto &sing : vf_[i]->wfPoints_)
            markSingularity(sing, positions, numpositions, i, plweights_);
        for (auto &sing : vf_[i]->dePoints_)
            markSingularity(sing, positions, numpositions, i, plweights_);
    }
}

// -----------------------------------------------------------------------
//          P4ParentStudy::resetSeparatingCurveInfo
// -----------------------------------------------------------------------
void P4ParentStudy::resetSeparatingCurveInfo(int i)
{
    separatingCurves_.erase(std::begin(separatingCurves_) + i);
}

// -----------------------------------------------------------------------
//          P4ParentStudy::resetSeparatingCurveInfo
// -----------------------------------------------------------------------
void P4ParentStudy::resetSeparatingCurveInfo() { separatingCurves_.clear(); }
