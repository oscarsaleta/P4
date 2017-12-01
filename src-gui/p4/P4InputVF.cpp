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

#include "P4InputVF.h"

P4InputVF *g_ThisVF = nullptr;

/*
    THIS FILE IMPLEMENTS READING AND LOADING THE VECTOR FIELD, THE PARAMETERS
   FOR REDUCE/MAPLE
    AND THE OVERALL CONFIGURATION.

    filename.inp        input vector field (read and written by P4)
    filename.red        file read by reduce
    filename.run        batch file written by P4, read by the operating system
   to execute reduce/maple

    filename_inf.res    results of search at infinity, in text form
    filename_fin.res    results of search in the finite region, in text form


    IT DOES NOT IMPLEMENT THE READING OF THE RESULTS FROM MAPLE/REDUCE (THE .TAB
   FILES).

*/

/*
    Different files:
 search at the finite region, in text form

    filename_inf.tab
    filename_fin.tab
    filename_vec.tab
*/

/*
    Structure of .inp file:

    - integer 0,1,2,3  (type of singularities: all, finite, ...)
    - parameters:
        * integer 0,1 symbolic/numeric
        * integer precision
        * string epsilon
        * integer testsep
        * integer taylorlevel
        * integer numericlevel
        * integer maxlevel
        *_ integer weakness
        * only if type of singularity is ONE:
            + string x0
            + string y0
        * otherwhise:
            + integer P
            + integer Q

    - vector field:
        * string xdot
        * string ydot
        * string gcf
        * integer numparams
        * for each parameter:
            + string label
            + string value

    - optional: integer precision0
*/
P4InputVF::P4InputVF()
{
    processText_ = nullptr;
    terminateProcessButton_ = nullptr;
    clearProcessButton_ = nullptr;
    evalProcess_ = nullptr;

    gcfDlg_ = nullptr;

    numeric_ = nullptr;
    precision_ = nullptr;
    epsilon_ = nullptr;
    testsep_ = nullptr;
    taylorlevel_ = nullptr;
    numericlevel_ = nullptr;
    maxlevel_ = nullptr;
    weakness_ = nullptr;

    numparams = 0;
    parvalue_ = nullptr;

    xdot_ = nullptr;
    ydot_ = nullptr;
    gcf_ = nullptr;

    numVF_ = 0;
    numCurves = 0;
    numPointsCurve_ = nullptr;
    curves_ = nullptr;
    numVFRegions_ = 0;
    numCurveRegions_ = 0;
    vfRegions_ = nullptr;
    curveRegions_ = nullptr;

    numSelected_ = 0;
    selected_ = nullptr;

    cleared_ = true;
    changed_ = false;
    evaluated_ = false;

    filename_ = DEFAULTFILENAME;

    reset(1);
}

P4InputVF::~P4InputVF()
{
    if (outputWindow_ != nullptr) {
        delete outputWindow_;
        outputWindow_ = nullptr;
    }
    // remove curve auxiliary files
    removeFile(getfilename_curvetable());
    removeFile(getfilename_curve());
    removeFile(getPrepareCurveFileName());
    // remove isoclines files too
    removeFile(getfilename_isoclinestable());
    removeFile(getfilename_isoclines());
    removeFile(getPrepareIsoclinesFileName());

    reset(0);
}

// -----------------------------------------------------------------------
//                  RESET
// -----------------------------------------------------------------------

void QInputVF::reset(int n)
{
    int i;

    g_VFResults.clearVFs();
    g_VFResults.vfK_ = nullptr;
    g_VFResults.K_ = 0;

    /*
        defaulttypeofview = TYPEOFVIEW_SPHERE;
        defaultxmin = -1;
        defaultxmax = 1;
        defaultymin = -1;
        defaultymax = 1;
    */
    symbolicpackage = getMathPackage();
    typeofstudy = DEFAULTTYPE;
    x0 = DEFAULTX0;
    y0 = DEFAULTY0;
    p = DEFAULTP;
    q = DEFAULTQ;

    deleteBoolList(numeric);
    deleteIntList(precision);
    deleteQStringList(epsilon, numvf);
    deleteBoolList(testsep);
    deleteIntList(taylorlevel);
    deleteIntList(numericlevel);
    deleteIntList(maxlevel);
    deleteIntList(weakness);

    if (n > 0) {
        numeric = makeNewBoolList(n, DEFAULTNUMERIC);
        precision = makeNewIntList(n, DEFAULTPRECISION);
        epsilon = makeNewQStringList(n, DEFAULTEPSILON);
        testsep = makeNewBoolList(n, DEFAULTTESTSEP);
        taylorlevel = makeNewIntList(n, DEFAULTLEVEL);
        numericlevel = makeNewIntList(n, DEFAULTNUMLEVEL);
        maxlevel = makeNewIntList(n, DEFAULTMAXLEVEL);
        weakness = makeNewIntList(n, DEFAULTWEAKNESS);
    } else {
        numeric = nullptr;
        precision = nullptr;
        epsilon = nullptr;
        testsep = nullptr;
        taylorlevel = nullptr;
        numericlevel = nullptr;
        maxlevel = nullptr;
        weakness = nullptr;
    }
    deleteQStringList(xdot, numvf);
    deleteQStringList(ydot, numvf);
    deleteQStringList(gcf, numvf);

    for (i = 0; i < numvf; i++) deleteQStringList(parvalue[i], MAXNUMPARAMS);
    free(parvalue);

    deleteQStringList(curve, numcurves);
    curve = nullptr;

    if (VFResults.curves != nullptr) {
        for (i = 0; i < numcurves; i++) VFResults.ResetCurveInfo(i);
        free(VFResults.curves);
        VFResults.curves = nullptr;
    }
    numcurves = 0;
    if (numpointscurve != nullptr) {
        free(numpointscurve);
        numpointscurve = nullptr;
    }

    if (n > 0) {
        xdot = makeNewQStringList(n, DEFAULTXDOT);
        ydot = makeNewQStringList(n, DEFAULTYDOT);
        gcf = makeNewQStringList(n, DEFAULTGCF);
    } else {
        xdot = nullptr;
        ydot = nullptr;
        gcf = nullptr;
    }
    if (n > 0) {
        parvalue = (QString ***)malloc(sizeof(QString **) * n);
        for (i = 0; i < n; i++)
            parvalue[i] = makeNewQStringList(MAXNUMPARAMS, "");
    } else
        parvalue = nullptr;

    numparams_ = 0;
    for (i = 0; i < MAXNUMPARAMS; i++) parlabel[i] = "";

    changed_ = false;
    evaluated_ = false;
    evaluating_ = false;
    evaluating_piecewiseconfig_ = false;
    cleared_ = true;

    if (curveRegions_ != nullptr) {
        for (i = 0; i < numCurveRegions_; i++)
            deleteIntList(curveRegions[i].signs);
        free(curveregions);
        curveregions = nullptr;
    }
    numcurveregions = 0;

    if (vfregions != nullptr) {
        for (i = 0; i < numvfregions; i++) deleteIntList(vfregions[i].signs);
        free(vfregions);
        vfregions = nullptr;
    }
    numvfregions = 0;

    if (n > 0) {
        numselected = 1;
        selected = (int *)malloc(sizeof(int));
        selected[0] = 0;

        if (n == 1) {
            vfregions = (VFREGION *)malloc(sizeof(VFREGION));
            vfregions->vfindex = 0;
            vfregions->signs = nullptr;
            numvfregions = 1;
        }
    } else {
        numselected = 0;
        selected = nullptr;
    }
    numvf = n;
}