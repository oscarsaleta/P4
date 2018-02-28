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

#pragma once

#include "file_tab.hpp"

#include <memory>
#include <vector>

class P4VFStudy;

class P4ParentStudy : public QObject
{
  public:
    // Constructor and destructor
    P4ParentStudy();
    //~P4ParentStudy();

    std::vector<P4VFStudy> vf_;
    // K_ will be throughout the current vector field selected
    int K_{0};

    std::vector<p4curveRegions::curveResult> separatingCurves_;

    int typeofstudy_{TYPEOFSTUDY_ALL};
    // TYPEOFVIEW_PLANE or TYPEOFVIEW_SPHERE
    int typeofview_{TYPEOFVIEW_SPHERE};
    bool plotVirtualSingularities_{DEFAULTPLOTVIRTUALSINGULARITIES};
    int p_{1};
    int q_{1};
    bool plweights_{false};  // true if p<>1 or q<>1; false if p=q=1
    double config_projection_;

    double double_p_;          // shortcuts: = (double)p
    double double_q_;          // = (double)q
    double double_p_plus_q_;   // = (double)(p+q)
    double double_p_minus_1_;  // = (double)(p-1)
    double double_q_minus_1_;  // = (double)(q-1)
    double double_q_minus_p_;  // = (double)(q-p)
    double xmin_{-1.0};        // in case of local study
    double xmax_{1.0};
    double ymin_{-1.0};
    double ymax_{1.0};

    // curves
    std::vector<p4curves::curves> arbitraryCurves_;

    // limit cycles and orbits
    std::vector<p4orbits::orbits> limCycles_;
    // int currentLimCycleIndex_;
    std::vector<p4orbits::orbits> orbits_;  // FIXME canviar ocurrències
    // int currentOrbitIndex_;

    // run-time when plotting
    // FIXME remove? canviar a int indexos?
    // std::unique_ptr<p4orbits::orbits> current_orbit_;
    // std::unique_ptr<p4orbits::orbits> current_lim_cycle_;

    double selected_ucoord_[2];

    std::vector<p4singularities::saddle> saddlePoints_;
    int selectedSaddlePointIndex_;
    std::vector<p4singularities::semi_elementary> sePoints_;
    int selectedSePointIndex_;
    std::vector<p4singularities::degenerate> dePoints_;
    int selectedDePointIndex_;

    std::vector<p4blowup::sep> seps_;
    int selectedSepIndex_;
    std::vector<p4blowup::blow_up_points> deSeps_;
    int selectedDeSepIndex_;

    int selected_sep_vfindex_;

    // current step size (during integration)
    double config_currentstep_{DEFAULT_STEPSIZE};
    // line style (dashes or points)
    bool config_dashes_{DEFAULT_LINESTYLE};
    // true for original VF, false for reduced
    bool config_kindvf_{DEFAULT_INTCONFIG};
    // number of orbits in the limit cycle window
    int config_lc_value_{DEFAULT_LCORBITS};
    // number of points in the limit cycle window
    int config_lc_numpoints_{DEFAULT_LCPOINTS};
    // maximum step size
    double config_hma_{DEFAULT_HMA};
    // minimum step size
    double config_hmi_{DEFAULT_HMI};
    // minimum step size near branches of separating curves
    double config_branchhmi_{DEFAULT_BRANCHHMI};
    // step size
    double config_step_{DEFAULT_STEPSIZE};
    // tolerance
    double config_tolerance_{DEFAULT_TOLERANCE};
    // number of points to integrate
    int config_intpoints_{DEFAULT_INTPOINTS};

    // Methods
    // void deleteOrbitPoint(P4ORBIT p);
    // void deleteLimitCycle(p4orbits::orbits *);
    // void deleteOrbit(p4orbits::orbits *);

    void resetSeparatingCurveInfo(int);
    void resetSeparatingCurveInfo();

    bool readPiecewiseData(FILE *);
    void examinePositionsOfSingularities();
    bool readTables(const QString &, bool, bool);
    void dump(const QString &basename);
    void reset();
    void setupCoordinateTransformations();
    bool readSeparatingCurve(FILE *);

    void clearVFs();

    // coordinate transformation routines, set up when starting the plot

    void (*viewcoord_to_sphere)(double, double, double *);
    bool (*sphere_to_viewcoordpair)(double *, double *, double *, double *,
                                    double *, double *);

    void (*finite_to_viewcoord)(double, double, double *);
    void (*sphere_to_viewcoord)(double, double, double, double *);
    bool (*is_valid_viewcoord)(double, double, double *);
    void (*integrate_sphere_sep)(double, double, double, double *, double *,
                                 int *, int *, int *, int *, double, double);
    void (*U1_to_sphere)(double, double, double *);
    void (*U2_to_sphere)(double, double, double *);
    void (*V1_to_sphere)(double, double, double *);
    void (*V2_to_sphere)(double, double, double *);

    void (*sphere_to_U1)(double, double, double, double *);
    void (*sphere_to_U2)(double, double, double, double *);
    void (*sphere_to_V1)(double, double, double, double *);
    void (*sphere_to_V2)(double, double, double, double *);

    void (*sphere_to_R2)(double, double, double, double *);
    void (*R2_to_sphere)(double, double, double *);
    void (*integrate_sphere_orbit)(double, double, double, double *, double *,
                                   int *, int *, double, double);
    double (*eval_lc)(double *, double, double, double);
    bool (*less2)(double *, double *);
    int (*change_dir)(double *);
};

P4ParentStudy gVFResults;
