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

// -----------------------------------------------------------------------
//          CONSTRUCTOR
// -----------------------------------------------------------------------
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

// -----------------------------------------------------------------------
//          DESTRUCTOR
// -----------------------------------------------------------------------
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
//          P4InputVF::reset
// -----------------------------------------------------------------------
void P4InputVF::reset(int n)
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
    deleteQStringList(epsilon, numVF_);
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
    deleteQStringList(xdot_, numVF_);
    deleteQStringList(ydot_, numVF_);
    deleteQStringList(gcf_, numVF_);

    for (i = 0; i < numVF_; i++) deleteQStringList(parvalue_[i], MAXNUMPARAMS);
    free(parvalue_);

    deleteQStringList(curves_, numCurves_);
    curves_ = nullptr;

    g_VFResults.resetCurveInfo();
    numCurves_ = 0;

    if (numPointsCurve_ != nullptr) {
        free(numPointsCurve_);
        numPointsCurve_ = nullptr;
    }

    if (n > 0) {
        xdot_ = makeNewQStringList(n, DEFAULTXDOT);
        ydot_ = makeNewQStringList(n, DEFAULTYDOT);
        gcf_ = makeNewQStringList(n, DEFAULTGCF);
    } else {
        xdot_ = nullptr;
        ydot_ = nullptr;
        gcf_ = nullptr;
    }
    if (n > 0) {
        parvalue_ = (QString ***)malloc(sizeof(QString **) * n);
        for (i = 0; i < n; i++)
            parvalue_[i] = makeNewQStringList(MAXNUMPARAMS, "");
    } else
        parvalue_ = nullptr;

    numparams_ = 0;
    for (i = 0; i < MAXNUMPARAMS; i++) parlabel[i] = "";

    changed_ = false;
    evaluated_ = false;
    evaluating_ = false;
    evaluating_piecewiseconfig_ = false;
    cleared_ = true;

    if (curveRegions_ != nullptr) {
        for (i = 0; i < numCurveRegions_; i++)
            deleteIntList(curveRegions_[i].signs);
        free(curveRegions_);
        curveRegions_ = nullptr;
    }
    numCurveRegions_ = 0;

    if (vfRegions_ != nullptr) {
        for (i = 0; i < numVFRegions_; i++) deleteIntList(vfRegions_[i].signs);
        free(vfRegions_);
        vfRegions_ = nullptr;
    }
    numVFRegions_ = 0;

    if (n > 0) {
        numSelected_ = 1;
        selected_ = (int *)malloc(sizeof(int));
        selected_[0] = 0;

        if (n == 1) {
            vfRegions_ = (p4VFStudyRegions::vfRegion *)malloc(
                sizeof(p4VFStudyRegions::vfRegion));
            vfRegions_->vfIndex = 0;
            vfRegions_->signs = nullptr;
            numVFRegions_ = 1;
        }
    } else {
        numSelected_ = 0;
        selected_ = nullptr;
    }
    numVF_ = n;
}

// -----------------------------------------------------------------------
//          P4InputVF::load
// -----------------------------------------------------------------------
bool P4InputVF::load()
{
    emit loadSignal();

    FILE *fp;
    QString fname;
    char scanbuf[2560];
    int i, k, c;
    int flag_numeric, flag_testsep;

    fname = getfilename();
    if (fname.length() == 0)
        return false;

    fp = fopen(QFile::encodeName(fname), "rt");
    if (fp == nullptr)
        return false;

    cleared_ = false;

    fscanf(fp, "%[^\n]\n", scanbuf);
    if (strcmp(scanbuf, "P5")) {
        // compatibility mode

        reset(1);
        fclose(fp);

        fp = fopen(QFile::encodeName(fname), "rt");

        if (fscanf(fp, "%d\n", &typeofstudy_) != 1 ||
            fscanf(fp, "%d\n", flag_numeric) != 1 ||
            fscanf(fp, "%d\n", precision_) != 1 ||
            fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
            fscanf(fp, "%d\n", flag_testsep) != 1 ||
            fscanf(fp, "%d\n", taylorlevel_) != 1 ||
            fscanf(fp, "%d\n", numericlevel_) != 1 ||
            fscanf(fp, "%d\n", maxlevel_) != 1 ||
            fscanf(fp, "%d\n", weakness_) != 1) {
            reset(1);
            fclose(fp);
            return false
        } else {
            *numeric_ = ((flag_numeric == 0) ? false : true);
            *(epsilon_[0]) = scanbuf != 1;
            *testsep_ = ((flag_testsep == 0) ? false : true);
        }

        if (typeofstudy_ == TYPEOFSTUDY_ONE) {
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                fclose(fp);
                return false;
            }
            x0_ = scanbuf;
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                fclose(fp);
                return false;
            }
            y0_ = scanbuf;
            p_ = 1;
            q_ = 1;
        } else {
            x0_ = DEFAULTX0;
            y0_ = DEFAULTY0;
            if (fscanf(fp, "%d\n", &p_) != 1 || fscanf(fp, "%d\n", &q_) != 1) {
                fclose(fp);
                return false;
            }
        }

        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        *(xdot_[0]) = scanbuf;
        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        *(ydot_[0]) = scanbuf;
        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        *(gcf_[0]) = scanbuf;

        if (*(xdot_[0]) == "(null)")
            *(xdot_[0]) = "";
        if (*(ydot_[0]) == "(null)")
            *(ydot_[0]) = "";
        if (*(gcf_[0]) == "(null)")
            *(gcf_[0]) = "";
        if (x0_ == "(null)")
            x0_ = "";
        if (y0_ == "(null)")
            y0_ = "";
        if (*(epsilon_[0]) == "(null)")
            *(epsilon_[0]) = "";

        if (fscanf(fp, "%d\n", &numparams_) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        for (i = 0; i < numparams_; i++) {
            if (fscanf(fp, "%s", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            parlabel_[i] = scanbuf;
            while ((c = fgetc(fp)) == '\n')
                ;
            ungetc(c, fp);
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            *(parvalue_[0][i]) = scanbuf;
        }
        for (; i < MAXNUMPARAMS; i++) {
            parlabel_[i] = "";
            *(parvalue_[0][i]) = "";
        }
    } else {
        QString _x0;
        QString _y0;
        int _typeofstudy;
        int _p;
        int _q;
        int n;

        if (fscanf(fp, "%d\n", &_typeofstudy) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        if (_typeofstudy == TYPEOFSTUDY_ONE) {
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            _x0 = scanbuf;
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            _y0 = scanbuf;
            if (_x0 == "(null)")
                _x0 = "";
            if (_y0 == "(null)")
                _y0 = "";
            _p = 1;
            _q = 1;
        } else {
            _x0 = DEFAULTX0;
            _y0 = DEFAULTY0;
            if (fscanf(fp, "%d\n", &_p) != 1 || fscanf(fp, "%d\n", &_q) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
        }

        if (fscanf(fp, "%d\n", &n) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        if (n <= 0) {
            reset(1);
            fclose(fp);
            return false;
        }
        reset(n);
        numVF_ = n;
        x0_ = _x0;
        y0_ = _y0;
        p_ = _p;
        q_ = _q;
        typeofstudy_ = _typeofstudy;

        g_VFResults.resetCurveInfo();

        if (fscanf(fp, "%d\n", &numCurves_) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        if (numCurves_ < 0) {
            numCurves_ = 0;
            reset(1);
            fclose(fp);
            return false;
        }

        if (numCurves_ > 0) {
            curves_ = (QString **)malloc(sizeof(QString *) * numCurves_);
            numPointsCurve_ = (int *)malloc(sizeof(int) * numCurves_);
            for (k = 0; k < numCurves_; k++) {
                curves_[k] = new QString("");
                numPointsCurve_[k] = DEFAULT_CURVEPOINTS;
            }
            for (k = 0; k < numCurves_; k++) {
                if (fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
                    fscanf(fp, "%d\n", numPointsCurve_ + k) != 1) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                *(curves_[k]) = scanbuf;
            }
        } else {
            curves_ = nullptr;
            numPointsCurve_ = nullptr;
        }

        if (fscanf(fp, "%d\n", &numparams_) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        if (numparams_ < 0 || numparams_ > MAXNUMPARAMS) {
            reset(1);
            fclose(fp);
            return false;
        }
        for (i = 0; i < numparams_; i++) {
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            parlabel_[i] = scanbuf;
        }
        for (; i < MAXNUMPARAMS; i++) parlabel_[i] = "";

        if (fscanf(fp, "%d\n", &numVFRegions_) != 1 || numVFRegions_ < 0) {
            reset(1);
            fclose(fp);
            return false;
        }
        if (numVFRegions_ > 0) {
            vfRegions_ = (p4VFStudyRegions::vfRegion *)malloc(
                sizeof(p4VFStudyRegions::vfRegion) * numVFRegions_);
            for (i = 0; i < numVFRegions_; i++) {
                if (numCurves_ == 0)
                    vfRegions_[i].signs = nullptr;
                else
                    vfRegions_[i].signs =
                        (int *)malloc(sizeof(int) * numCurves_);
            }
            for (i = 0; i < numVFRegions_; i++) {
                if (fscanf(fp, "%d", &(vfRegions_[i].vfIndex)) != 1) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                if (vfRegions_[i].vfIndex < 0 ||
                    vfRegions_[i].vfIndex >= numVF_) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                for (k = 0; k < numCurves_; k++) {
                    if (fscanf(fp, "%d", &(vfRegions_[i].signs[k])) != 1) {
                        reset(1);
                        fclose(fp);
                        return false;
                    }
                    if (vfRegions_[i].signs[k] != 1 &&
                        vfRegions_[i].signs[k] != -1) {
                        reset(1);
                        fclose(fp);
                        return false;
                    }
                }
            }
        } else
            vfRegions_ = nullptr;

        if (fscanf(fp, "%d\n", &numCurveRegions_) != 1 ||
            numCurveRegions_ < 0) {
            reset(1);
            fclose(fp);
            return false;
        }
        if (numCurveRegions_ > 0) {
            curveRegions_ = (p4VFStudyRegions::curveRegion *)malloc(
                sizeof(p4VFStudyRegions::curveRegion) * numCurveRegions_);
            for (i = 0; i < numCurveRegions_; i++) {
                if (numCurves_ == 0)
                    curveRegions_[i].signs = nullptr;
                else
                    curveRegions_[i].signs =
                        (int *)malloc(sizeof(int) * numCurves_);
            }
            for (i = 0; i < numCurveRegions_; i++) {
                if (fscanf(fp, "%d", &(curveRegions_[i].curveIndex)) != 1) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                if (curveRegions_[i].curveIndex < 0 ||
                    curveRegions_[i].curveIndex >= numCurves_) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                for (k = 0; k < numCurves_; k++) {
                    if (fscanf(fp, "%d", &(curveRegions_[i].signs[k])) != 1) {
                        reset(1);
                        fclose(fp);
                        return false;
                    }
                    if (curveRegions_[i].signs[k] > 1 ||
                        curveRegions_[i].signs[k] < -1) {
                        reset(1);
                        fclose(fp);
                        return false;
                    }
                }
            }
        } else
            curveRegions_ = nullptr;

        for (k = 0; k < numVF_; k++) {
            if (fscanf(fp, "%d\n", flag_numeric) != 1 ||
                fscanf(fp, "%d\n", precision_ + k) != 1 ||
                fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
                fscanf(fp, "%d\n", flag_testsep) != 1 ||
                fscanf(fp, "%d\n", taylorlevel_ + k) != 1 ||
                fscanf(fp, "%d\n", numericlevel_ + k) != 1 ||
                fscanf(fp, "%d\n", maxlevel_ + k) != 1 ||
                fscanf(fp, "%d\n", weakness_ + k) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            *(epsilon_[k]) = scanbuf;
            numeric_[k] = ((flag_numeric == 0) ? false : true);
            testsep_[k] = ((flag_testsep == 0) ? false : true);

            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            *(xdot_[k]) = scanbuf;
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            *(ydot_[k]) = scanbuf;
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            *(gcf_[k]) = scanbuf;

            if (*(xdot_[k]) == "(null)")
                *(xdot_[k]) = "";
            if (*(ydot_[k]) == "(null)")
                *(ydot_[k]) = "";
            if (*(gcf_[k]) == "(null)")
                *(gcf_[k]) = "";
            if (*(epsilon_[k]) == "(null)")
                *(epsilon_[k]) = "";

            for (i = 0; i < numparams_; i++) {
                if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                *(parvalue_[k][i]) = scanbuf;
            }
            for (; i < MAXNUMPARAMS; i++) *(parvalue_[k][i]) = "";
        }
    }

    fclose(fp);

    changed_ = false;
    cleared_ = false;
    evaluated_ = checkevaluated();
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::checkevaluated
// -----------------------------------------------------------------------
bool P4InputVF::checkevaluated()
{
    // if the evaluate files are already found on disc, then set the flag to
    // true.

    QFileInfo *fi;
    QFileInfo *fifin;
    QFileInfo *fiinf;
    QFileInfo *fivec;

    QDateTime dt;
    QDateTime dtfin;
    QDateTime dtinf;
    QDateTime dtvec;

    fi = new QFileInfo(getfilename());
    dt = fi->lastModified();
    delete fi;
    fi = nullptr;

    fivec = new QFileInfo(getbarefilename() + "_vec.tab");
    if (fivec->isFile() == false) {
        delete fivec;
        fivec = nullptr;
        return false;
    }
    dtvec = fivec->lastModified();
    if (dtvec.secsTo(dt) > 0 || dtvec.daysTo(dt) > 0)
        return false;

    if (typeofstudy != TYPEOFSTUDY_INF) {
        fifin = new QFileInfo(getbarefilename() + "_fin.tab");
        if (fifin->isFile() == false) {
            delete fifin;
            fifin = nullptr;
            return false;
        }
        dtfin = fifin->lastModified();
        delete fifin;
        fifin = nullptr;
        if (dtfin.secsTo(dt) > 0 || dtfin.daysTo(dt) > 0)
            return false;
    }

    if (typeofstudy == TYPEOFSTUDY_INF || typeofstudy == TYPEOFSTUDY_ALL) {
        fiinf = new QFileInfo(getbarefilename() + "_inf.tab");
        if (fiinf->isFile() == false) {
            delete fiinf;
            fiinf = nullptr;
            return false;
        }
        dtinf = fiinf->lastModified();
        delete fiinf;
        fiinf = nullptr;
        if (dtinf.secsTo(dt) > 0 || dtinf.daysTo(dt) > 0)
            return false;
    }

    if (fi != nullptr) {
        delete fi;
        fi = nullptr;
    }
    if (fifin != nullptr) {
        delete fifin;
        fifin = nullptr;
    }
    if (fiinf != nullptr) {
        delete fiinf;
        fiinf = nullptr;
    }
    if (fivec != nullptr) {
        delete fivec;
        fivec = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::save
// -----------------------------------------------------------------------
bool P4InputVF::save()
{
    QSettings settings(getbarefilename().append(".conf"),
                       QSettings::NativeFormat);
    settings.clear();
    emit saveSignal();

    FILE *fp;
    int k, i;
    QString fname;
    QByteArray s;
    fname = getfilename();
    if (fname.length() == 0)
        return false;

    fp = fopen(QFile::encodeName(fname), "wt");
    if (fp == nullptr)
        return false;

    fprintf(fp, "P5\n");

    fprintf(fp, "%d\n", typeofstudy_);
    if (typeofstudy_ == TYPEOFSTUDY_ONE) {
        if (x0_.length() == 0)
            fprintf(fp, "%s\n", "(null)");
        s = x0_.toAscii();
        fprintf(fp, "%s\n", (const char *)s);
        if (y0_.length() == 0)
            fprintf(fp, "%s\n", "(null)");
        s = y0_.toAscii();
        fprintf(fp, "%s\n", (const char *)s);
    } else {
        fprintf(fp, "%d\n", p_);
        fprintf(fp, "%d\n", q_);
    }
    fprintf(fp, "%d\n", numVF_);
    fprintf(fp, "%d\n", numCurves_);
    for (k = 0; k < numCurves_; k++) {
        if (curves_[k]->length() == 0)
            fprintf(fp, "%s\n", "(null)");
        else {
            s = curves_[k]->toAscii();
            fprintf(fp, "%s\n", (const char *)s);
        }
        fprintf(fp, "%d\n", numPointsCurve_[k]);
    }

    fprintf(fp, "%d\n", numparams_);
    for (i = 0; i < numparams_; i++) {
        if (parlabel_[i].length() == 0)
            fprintf(fp, "%s\n", "(null)");
        else {
            s = parlabel_[i].toAscii();
            fprintf(fp, "%s\n", (const char *)s);
        }
    }

    fprintf(fp, "%d\n", numVFRegions_);
    for (i = 0; i < numVFRegions_; i++) {
        fprintf(fp, "%d", vfRegions_[i].vfIndex);
        for (k = 0; k < numCurves_; k++)
            fprintf(fp, " %d", vfRegions_[i].signs[k]);
        fprintf(fp, "\n");
    }

    fprintf(fp, "%d\n", numCurveRegions_);
    for (i = 0; i < numCurveRegions_; i++) {
        fprintf(fp, "%d", curveregions[i].curveIndex);
        for (k = 0; k < numCurves_; k++)
            fprintf(fp, " %d", curveregions[i].signs[k]);
        fprintf(fp, "\n");
    }

    for (k = 0; k < numVF_; k++) {
        fprintf(fp, "%d\n", numeric_[k]);
        fprintf(fp, "%d\n", precision_[k]);
        if (epsilon_[k]->length() == 0)
            fprintf(fp, "%s\n", "(null)");
        else {
            s = epsilon_[k]->toAscii();
            fprintf(fp, "%s\n", (const char *)s);
        }

        fprintf(fp, "%d\n", testsep_[k]);
        fprintf(fp, "%d\n", taylorlevel_[k]);
        fprintf(fp, "%d\n", numericlevel_[k]);
        fprintf(fp, "%d\n", maxlevel_[k]);
        fprintf(fp, "%d\n", weakness_[k]);
        if (xdot_[k]->length() == 0)
            fprintf(fp, "%s\n", "(null)");
        else {
            s = xdot_[k]->toAscii();
            fprintf(fp, "%s\n", (const char *)s);
        }

        if (ydot_[k]->length() == 0)
            fprintf(fp, "%s\n", "(null)");
        else {
            s = ydot_[k]->toAscii();
            fprintf(fp, "%s\n", (const char *)s);
        }

        if (gcf_[k]->length() == 0)
            fprintf(fp, "%s\n", "(null)");
        else {
            s = gcf_[k]->toAscii();
            fprintf(fp, "%s\n", (const char *)s);
        }

        for (i = 0; i < numparams_; i++) {
            if (parvalue_[k][i]->length() == 0)
                fprintf(fp, "%s\n", "(null)");
            else {
                s = parvalue_[k][i]->toAscii();
                fprintf(fp, "%s\n", (const char *)s);
            }
        }
    }
    fclose(fp);
    changed_ = false;
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::getbarefilename
// -----------------------------------------------------------------------
QString P4InputVF::getbarefilename(void) const
{
    int slash1;
    int slash2;
    int slash3;
    int dot;
    QString fname;

    fname = filename.trimmed();

    if (fname.length() == 0) {
        return "";
    }

    slash1 = fname.lastIndexOf('/');
    slash2 = fname.lastIndexOf('\\');
    slash3 = fname.lastIndexOf(':');
    dot = fname.lastIndexOf('.');

    if (dot <= slash1 && dot <= slash2 && dot <= slash3) {
        // there is no dot present, or at least not in the
        // last part of the file name.
        return fname;
    }

    return fname.left(dot);
}

// -----------------------------------------------------------------------
//          P4InputVF::getbarefilename
// -----------------------------------------------------------------------
QString P4InputVF::getfilename(void) const
{
    int slash1;
    int slash2;
    int slash3;
    int dot;
    QString fname;

    fname = filename.trimmed();

    if (fname.length() != 0) {
        slash1 = fname.lastIndexOf('/');
        slash2 = fname.lastIndexOf('\\');
        slash3 = fname.lastIndexOf(':');
        dot = fname.lastIndexOf('.');

        if (dot <= slash1 && dot <= slash2 && dot <= slash3) {
            // there is no dot present, or at least not in the
            // last part of the file name.

            return fname.append(".inp");
        }
    }
    return fname;
}

// -----------------------------------------------------------------------
//          P4InputVF::getfilename_xxx
// -----------------------------------------------------------------------
QString P4InputVF::getfilename_finresults(void) const
{
    return getbarefilename().append("_fin.res");
}
QString P4InputVF::getfilename_infresults(void) const
{
    return getbarefilename().append("_inf.res");
}
QString P4InputVF::getfilename_fintable(void) const
{
    return getbarefilename().append("_fin.tab");
}
QString P4InputVF::getfilename_inftable(void) const
{
    return getbarefilename().append("_inf.tab");
}
QString P4InputVF::getfilename_vectable(void) const
{
    return getbarefilename().append("_vec.tab");
}
QString P4InputVF::getfilename_gcf(void) const
{
    return getbarefilename().append("_gcf.tab");
}
// only used in case of reduce: contains reduce output, no gcf data and is
// deleted immediately.
/*QString QInputVF::getfilename_gcfresults(void) const
{
    return getbarefilename().append("_gcf.res");
}*/
QString P4InputVF::getfilename_curveresults(void) const
{
    return getbarefilename().append("_curves.tab");
}
/*QString QInputVF::getreducefilename(void) const
{
    return getbarefilename().append(".red");
}*/
QString P4InputVF::getmaplefilename(void) const
{
    return getbarefilename().append(".txt");
}
QString P4InputVF::getrunfilename(void) const
{
    return getbarefilename().append(".run");
}
// curve filenames
QString P4InputVF::getfilename_curvetable(void) const
{
    return getbarefilename().append("_veccurve.tab");
}
QString P4InputVF::getfilename_curve(void) const
{
    return getbarefilename().append("_curve.tab");
}
QString P4InputVF::getPrepareCurveFileName(void) const
{
    return getbarefilename().append("_curve_prep.txt");
}
// isoclines filenames
QString P4InputVF::getfilename_isoclinestable(void) const
{
    return getbarefilename().append("_vecisoclines.tab");
}
QString P4InputVF::getfilename_isoclines(void) const
{
    return getbarefilename().append("_isoclines.tab");
}
QString P4InputVF::getPrepareIsoclinesFileName(void) const
{
    return getbarefilename().append("_isoclines_prep.txt");
}

// -----------------------------------------------------------------------
//          P4InputVF::fileExists
// -----------------------------------------------------------------------
bool P4InputVF::fileExists(QString fname)
{
    QFile fp(fname);
    if (fp.exists())
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareReduceParameters
// -----------------------------------------------------------------------
/*void P4InputVF::prepareReduceParameters(QTextStream *fp)
{
    QString s;
    int _testsep = (testsep == 0) ? 1 : 0; // inverse meaning in reduce???

    s.sprintf("numeric:=%d$\n", numeric_);
    *fp << s;
    s.sprintf("user_precision:=%d$\n", precision);
    *fp << s;
    *fp << "epsilon:=" << epsilon_ << "$\n";
    s.sprintf("test_sep:=%d$\n", _testsep);
    *fp << s;
    s.sprintf("taylor_level:=%d$\n", taylorlevel_);
    *fp << s;
    s.sprintf("numeric_level:=%d$\n", numericlevel_);
    *fp << s;
    s.sprintf("max_level:=%d$\n", maxlevel_);
    *fp << s;
    s.sprintf("weakness_level:=%d$\n", weakness);
    *fp << s;

    if (typeofstudy_ == TYPEOFSTUDY_ONE) {
        s.sprintf("p:=1$\nq:=1$\n");
        *fp << s;
        *fp << "x0:=" << x0_ << "$\n";
        *fp << "y0:=" << y0_ << "$\n";
        s.sprintf("x_min:=x0+(%f)$\n", (float)(X_MIN));
        *fp << s;
        s.sprintf("x_max:=x0+(%f)$\n", (float)(X_MAX));
        *fp << s;
        s.sprintf("y_min:=y0+(%f)$\n", (float)(Y_MIN));
        *fp << s;
        s.sprintf("y_max:=y0+(%f)$\n", (float)(Y_MAX));
        *fp << s;
    } else {
        s.sprintf("p:=%d$\n", p);
        *fp << s;
        s.sprintf("q:=%d$\n", q);
        *fp << s;
    }
}*/

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleParameters
// -----------------------------------------------------------------------

void P4InputVF::prepareMapleParameters(QTextStream *fp)
{
    QString s;
    int i;

    *fp << "user_numeric_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        *fp << booleanString(numeric_[i]);
        if (i == numVF_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }

    *fp << "epsilon_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        *fp << (*(epsilon_[i]));
        if (i == numVF_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }

    *fp << "test_sep_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        *fp << booleanString(testsep_[i]);
        if (i == numVF_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }

    *fp << "user_precision_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        *fp << precision_[i];
        if (i == numVF_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }

    *fp << "taylor_level_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        *fp << taylorlevel_[i];
        if (i == numVF_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }

    *fp << "numeric_level_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        *fp << numericlevel_[i];
        if (i == numVF_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }

    *fp << "max_level_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        *fp << maxlevel_[i];
        if (i == numVF_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }

    *fp << "weakness_level_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        *fp << weakness_[i];
        if (i == numVF_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }

    if (typeofstudy == TYPEOFSTUDY_ONE) {
        *fp << "user_p := 1:\n";
        *fp << "user_q := 1:\n";

        s = x0.toAscii();
        *fp << "x0 := " << x0_ << ":\n";
        *fp << "y0 := " << y0_ << ":\n";
        s.sprintf("x_min := x0+(%f):\n", (float)(X_MIN));
        *fp << s;
        s.sprintf("x_max := x0+(%f):\n", (float)(X_MAX));
        *fp << s;
        s.sprintf("y_min := y0+(%f):\n", (float)(Y_MIN));
        *fp << s;
        s.sprintf("y_max := y0+(%f):\n", (float)(Y_MAX));
        *fp << s;
    } else {
        s.sprintf("user_p := %d:\n", p_);
        *fp << s;
        s.sprintf("user_q := %d:\n", q_);
        *fp << s;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareReduceCurves
// -----------------------------------------------------------------------
/*P5
void P4InputVF::prepareReduceCurves( QTextStream * fp )
{
}
*/

// -----------------------------------------------------------------------
//          P4InputVF::prepareReduceVectorField
// -----------------------------------------------------------------------
/*P5
void P4InputVF::prepareReduceVectorField( QTextStream * fp )
{
    QString myxdot;
    QString myydot;
    QString mygcf;
    QString lbl;
    QString val;
    int k;

    myxdot = ConvertReduceUserParameterLabels( xdot );
    myydot = ConvertReduceUserParameterLabels( ydot );
    mygcf = ConvertReduceUserParameterLabels( gcf );

    *fp << "f:={" << myxdot << "," << myydot << "}$\n";
    *fp << "gcf:=" << mygcf << "$\n";

    for( k = 0; k < numparams; k++ )
    {
        lbl = ConvertReduceUserParameterLabels( parlabel[k] );
        val = ConvertReduceUserParameterLabels( parvalue[k] );

        if( lbl.length() == 0 )
            continue;

        *fp << lbl << ":=" << val << "$\n";
    }
}
*/

// -----------------------------------------------------------------------
//          P4InputVF::prepareReducePiecewiseConfig
// -----------------------------------------------------------------------
/*P5
void P4InputVF::prepareReducePiecewiseConfig( QTextStream * fp )
{

    ???

}
*/

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleVectorField
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleVectorField(QTextStream *fp)
{
    QString myxdot;
    QString myydot;
    QString mygcf;
    QString lbl;
    QString val;
    int k, i;

    *fp << "user_f_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        myxdot =
            convertMapleUserParameterLabels(*(xdot_[i]));  // TODO: corregir
        myydot = convertMapleUserParameterLabels(*(ydot_[i]));
        *fp << "[" << myxdot << "," << myydot << "]";
        if (i == numVF_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }

    *fp << "user_gcf_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        mygcf = convertMapleUserParameterLabels(*(gcf_[i]));
        *fp << mygcf;
        if (i == numVF_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }

    *fp << "user_parameters := [ ";
    for (k = 0; k < numparams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);
        *fp << lbl;
        if (k == numparams_ - 1)
            *fp << " ]:\n";
        else
            *fp << ", ";
    }
    if (numparams_ == 0)
        *fp << " ]:\n";

    for (k = 0; k < numparams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);

        if (lbl.length() == 0)
            continue;

        *fp << lbl << "_pieces := [ ";

        for (i = 0; i < numVF_; i++) {
            val = convertMapleUserParameterLabels(*(parvalue_[i][k]));
            if (!numeric_[i])
                *fp << val;
            else
                *fp << "evalf(" << val << ")";
            if (i == numVF_ - 1)
                *fp << " ]:\n";
            else
                *fp << ", ";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleCurves (P5 version)
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleCurves(QTextStream *fp)
{
    int i;
    QString s;

    *fp << "user_curves := [ ";
    if (numCurves_ > 0)
        for (i = 0; i < numCurves_; i++) {
            s = convertMapleUserParameterLabels(*(curves_[i]));
            *fp << s;
            if (i == numCurves_ - 1)
                *fp << " ]:\n";
            else
                *fp << ", ";
        }
    else
        *fp << "]:\n";

    *fp << "user_numpointscurves :=[ ";
    if (numCurves_ > 0)
        for (i = 0; i < numCurves_; i++) {
            *fp << numPointsCurve_[i];
            if (i == numCurves_ - 1)
                *fp << " ]:\n";
            else
                *fp << ", ";
        }
    else
        *fp << "]:\n";
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleCurve (P4 version)
// -----------------------------------------------------------------------
void QInputVF::prepareMapleCurve(QTextStream *fp)
{
    QString mycurve;
    QString lbl;
    QString val;
    int k;

    mycurve = convertMapleUserParameterLabels(curve_);
    *fp << "user_curve := " << mycurve << ":\n";

    for (k = 0; k < numparams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);
        val = convertMapleUserParameterLabels(parvalue_[k]);

        if (lbl.length() == 0)
            continue;

        if (!numeric_) {
            *fp << lbl << " := " << val << ":\n";
        } else {
            *fp << lbl << " := evalf( " << val << " ):\n";
        }
    }
}