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
    numCurves_ = 0;
    numPointsCurve_ = nullptr;
    curves_ = nullptr;
    numVFRegions_ = 0;
    numCurveRegions_ = 0;

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

    // symbolicpackage_ = PACKAGE_MAPLE;//getMathPackage();
    typeofstudy_ = DEFAULTTYPE;
    x0_ = DEFAULTX0;
    y0_ = DEFAULTY0;
    p_ = DEFAULTP;
    q_ = DEFAULTQ;

    // clear current vectors
    numeric_.clear();
    precision_.clear();
    epsilon_.clear();
    testsep_.clear();
    taylorlevel_.clear();
    numericlevel_.clear();
    maxlevel_.clear();
    weakness_.clear();

    xdot_.clear();
    ydot_.clear();
    gcf_.clear();

    // refill vectors with default values
    for (i = 0; i < n; i++) {
        numeric_.push_back(DEFAULTNUMERIC);
        precision_.push_back(DEFAULTPRECISION);
        epsilon_.push_back(QString(DEFAULTEPSILON));
        testsep_.push_back(DEFAULTTESTSEP);
        taylorlevel_.push_back(DEFAULTLEVEL);
        numericlevel_.push_back(DEFAULTNUMLEVEL);
        maxlevel_.push_back(DEFAULTMAXLEVEL);
        weakness_.push_back(DEFAULTWEAKNESS);
        xdot_.push_back(QString(DEFAULTXDOT));
        ydot_.push_back(QString(DEFAULTYDOT));
        gcf_.push_back(QString(DEFAULTGCF));
    }

    parvalue_.clear();
    parvalue_.swap(std::vector < std::vector<QString>());

    curves_.clear();
    g_VFResults.resetCurveInfo();  // TODO: mirar si he de canviar alguna cosa
    numCurves_ = 0;

    numPointsCurve_.clear();

    if (n > 0) {
        parvalue_ = (QString ***)malloc(sizeof(QString **) * n);
        for (i = 0; i < n; i++)
            parvalue_[i] = makeNewQStringList(MAXNUMPARAMS, "");
    } else
        parvalue_ = nullptr;

    numparams_ = 0;
    for (i = 0; i < MAXNUMPARAMS; i++) parlabel_[i] = "";

    changed_ = false;
    evaluated_ = false;
    evaluating_ = false;
    evaluating_piecewiseconfig_ = false;
    cleared_ = true;

    curveRegions_.clear();
    numCurveRegions_ = 0;

    vfRegions_.clear();
    numVFRegions_ = 0;

    if (n > 0) {
        numSelected_ = 1;
        selected_.push_back(0);

        if (n == 1) {
            p4VFStudyRegions::vfRegion region(0, nullptr);
            vfRegions_.push_back(region);
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

        int aux;
        if (fscanf(fp, "%d\n", &typeofstudy_) != 1 ||
            fscanf(fp, "%d\n", &flag_numeric) != 1 ||
            fscanf(fp, "%d\n", &aux) != 1 ||
            fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
            fscanf(fp, "%d\n", &flag_testsep) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            bool value;
            value = ((flag_numeric == 0) ? false : true);
            numeric_.push_back(value);
            precision_.push_back(aux);
            epsilon_.push_back(QString(scanbuf));
            value = ((flag_testsep == 0) ? false : true);
            testsep_.push_back(value);
        }
        if (fscanf(fp, "%d\n", &aux) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            taylorlevel_.push_back(aux);
        }
        if (fscanf(fp, "%d\n", &aux) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            numericlevel_.push_back(aux);
        }
        if (fscanf(fp, "%d\n", &aux) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            maxlevel_.push_back(aux);
        }
        if (fscanf(fp, "%d\n", &aux) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            weakness_.push_back(aux);
        }

        if (typeofstudy_ == TYPEOFSTUDY_ONE) {
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            x0_ = scanbuf;
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
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
                reset(1);
                fclose(fp);
                return false;
            }
        }

        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        xdot_.push_back(QString(scanbuf));
        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        ydot_.push_back(QString(scanbuf));
        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        gcf_.push_back(QString(scanbuf));

        if (xdot_[0] == "(null)")
            xdot_[0] = "";
        if (ydot_[0] == "(null)")
            ydot_[0] = "";
        if (gcf_[0] == "(null)")
            gcf_[0] = "";
        if (x0_ == "(null)")
            x0_ = "";
        if (y0_ == "(null)")
            y0_ = "";
        if (epsilon_[0] == "(null)")
            epsilon_[0] = "";

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
            parlabel_.push_back(QString(scanbuf));
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
        for (i = 0; i < numVFRegions_; i++) {
            // read index
            int indx;
            if (fscanf(fp, "%d", &indx) != 1 || indx < 0 || indx >= numVF_) {
                reset(1);
                fclose(fp);
                return false;
            }
            // read signs
            std::vector<int> sgns;
            int aux;
            for (k = 0; k < numCurves_; k++) {
                if (fscanf(fp, "%d", &aux) != 1 || (aux != 1 && aux != -1)) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                sgns.push_back(aux);
            }
            // sgns could be an empty list
            p4VFStudyRegions::vfRegion region(indx, sgns);
            vfRegions_.push_back(region);
        }

        if (fscanf(fp, "%d\n", &numCurveRegions_) != 1 ||
            numCurveRegions_ < 0) {
            reset(1);
            fclose(fp);
            return false;
        }
        for (i = 0; i < numCurveRegions_; i++) {
            // read index
            int indx;
            if (fscanf(fp, "%d", &indx) != 1 || indx < 0 ||
                indx >= numCurves_) {
                reset(1);
                fclose(fp);
                return false;
            }
            // read signs
            std::vector<int> sgns;
            int aux;
            for (k = 0; k < numCurves_; k++) {
                if (fscanf(fp, "%d", &aux) != 1 || aux > 1 || aux < -1))
                    {
                        reset(1);
                        fclose(fp);
                        return false;
                    }
                sgns.push_back(aux);
            }
            // sgns could be an empty list
            p4VFStudyRegions::curveRegion region(indx, sgns);
            curveRegions_.push_back(region);
        }

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
QString P4InputVF::getbarefilename() const
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
QString P4InputVF::getfilename() const
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
QString P4InputVF::getfilename_finresults() const
{
    return getbarefilename().append("_fin.res");
}
QString P4InputVF::getfilename_infresults() const
{
    return getbarefilename().append("_inf.res");
}
QString P4InputVF::getfilename_fintable() const
{
    return getbarefilename().append("_fin.tab");
}
QString P4InputVF::getfilename_inftable() const
{
    return getbarefilename().append("_inf.tab");
}
QString P4InputVF::getfilename_vectable() const
{
    return getbarefilename().append("_vec.tab");
}
QString P4InputVF::getfilename_gcf() const
{
    return getbarefilename().append("_gcf.tab");
}
// only used in case of reduce: contains reduce output, no gcf data and is
// deleted immediately.
/*QString P4InputVF::getfilename_gcfresults() const
{
    return getbarefilename().append("_gcf.res");
}*/
QString P4InputVF::getfilename_curveresults() const
{
    return getbarefilename().append("_curves.tab");
}
/*QString P4InputVF::getreducefilename() const
{
    return getbarefilename().append(".red");
}*/
QString P4InputVF::getmaplefilename() const
{
    return getbarefilename().append(".txt");
}
QString P4InputVF::getrunfilename() const
{
    return getbarefilename().append(".run");
}
// curve filenames
QString P4InputVF::getfilename_curvetable() const
{
    return getbarefilename().append("_veccurve.tab");
}
QString P4InputVF::getfilename_curve() const
{
    return getbarefilename().append("_curve.tab");
}
QString P4InputVF::getPrepareCurveFileName() const
{
    return getbarefilename().append("_curve_prep.txt");
}
// isoclines filenames
QString P4InputVF::getfilename_isoclinestable() const
{
    return getbarefilename().append("_vecisoclines.tab");
}
QString P4InputVF::getfilename_isoclines() const
{
    return getbarefilename().append("_isoclines.tab");
}
QString P4InputVF::getPrepareIsoclinesFileName() const
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
        lbl = ConvertReduceUserParameterLabels( parlabel_[k] );
        val = ConvertReduceUserParameterLabels( parvalue_[k] );

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
void P4InputVF::prepareMapleCurve(QTextStream *fp)
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

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleisoclines
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleIsoclines(QTextStream *fp)
{
    QString myisoclines;
    QString lbl;
    QString val;
    int k;

    myisoclines = convertMapleUserParameterLabels(isoclines_);
    *fp << "user_isoclines := " << myisoclines << ":\n";

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

// -----------------------------------------------------------------------
//          P4InputVF::prepareMaplePiecewiseConfig
// -----------------------------------------------------------------------

void P4InputVF::prepareMaplePiecewiseConfig(QTextStream *fp)
{
    int i, k;

    *fp << "vfregions := [ ";
    if (numVFRegions_ == 0)
        *fp << "]:\n";
    else {
        for (i = 0; i < numVFRegions_; i++) {
            *fp << "[" << vfregions[i].vfindex << ", [ ";
            for (k = 0; k < numCurves_; k++) {
                *fp << vfregions[i].signs[k];
                if (k < numCurves_ - 1)
                    *fp << ",";
            }
            if (i == numVFRegions_ - 1)
                *fp << "] ] ]:\n";
            else
                *fp << "] ],\n    ";
        }
    }

    *fp << "curveregions := [ ";
    if (numCurveRegions_ == 0)
        *fp << "]:\n";
    else {
        for (i = 0; i < numCurveRegions_; i++) {
            *fp << "[" << curveRegions_[i].curveindex << ", [ ";
            for (k = 0; k < numCurves_; k++) {
                *fp << curveRegions_[i].signs[k];
                if (k < numCurves_ - 1)
                    *fp << ",";
            }
            if (i == numCurveRegions_ - 1)
                *fp << "] ] ]:\n";
            else
                *fp << "] ],\n    ";
        }
    }
}

// -----------------------------------------------------------------------
//          indexOfWordInString
// -----------------------------------------------------------------------
//
//  If the user gives a vector field with user-parameters such as "alpha" or
//  "b", we add a suffix to these qualifier names and change them to "alpha_" or
//  "b_", in order to avoid mixing with internal variables.
static int indexOfWordInString(const QString *src, const QString *word,
                               int start = 0)
{
    int i, j;

    while ((i = src->indexOf(*word, start)) != -1) {
        // we have found word as a substring.  The index i is an index from the
        // very beginning of string (not depending of start)

        start = i + 1;  // do not find this substring next loop

        // check if the substring is the beginning of a word:
        j = i;
        if (j > 0)
            if ((*src)[j - 1].isLetter() || (*src)[j - 1] == '_' ||
                (*src)[j - 1].isDigit())
                continue;  // no: it is part of a bigger word, so continue...

        // check if the substring is the end of a word;
        j = i + word->length();
        if (j < src->length())
            if ((*src)[j].isLetter() || (*src)[j] == '_' || (*src)[j].isDigit())
                continue;  // no: it is part of a bigger word, so continue...

        // ok: we have a word: stop looping.
        break;
    }
    return i;
}

// -----------------------------------------------------------------------
//          P4InputVF::convertMapleUserParameterLabels
// -----------------------------------------------------------------------
QString P4InputVF::convertMapleUserParameterLabels(QString src)
{
    QString s;
    QString t;
    QString p, newlabel;
    int i, k;

    s = src;
    for (k = 0; k < numparams_; k++) {
        p = parlabel_[k];
        newlabel = p + "_";

        if (p.length() == 0)
            continue;

        t = "";
        while (1) {
            i = indexOfWordInString(&s, &p);
            if (i == -1)
                break;

            t += s.left(i);
            t += newlabel;
            s = s.mid(i + p.length());
        }
        s = t + s;
    }

    return s;
}

// -----------------------------------------------------------------------
//          P4InputVF::convertMapleUserParametersLabelsToValues
// -----------------------------------------------------------------------
QString P4InputVF::convertMapleUserParametersLabelsToValues(QString src)
{
    QString t, p, newlabel;
    int i, k;
    QString s = src;
    for (k = 0; k < numparams_; k++) {
        p = parlabel_[k];
        newlabel = parvalue_[k];
        if (p.length() == 0)
            continue;
        t = "";
        while (1) {
            i = indexOfWordInString(&s, &p);
            if (i == -1)
                break;
            t += s.left(i);
            t += newlabel;
            s = s.mid(i + p.length());
        }
        s = t + s;
    }
    return s;
}

    // -----------------------------------------------------------------------
    //          P4InputVF::convertReduceUserParameterLabels
    // -----------------------------------------------------------------------
    /*QString P4InputVF::convertReduceUserParameterLabels(QString src)
    {
        QString s;
        QString t;
        QString p, newlabel;
        int i, k;

        s = src;
        for (k = 0; k < numparams_; k++) {
            p = parlabel_[k];
            newlabel = p + "_";

            if (p.length() == 0)
                continue;

            t = "";
            while (1) {
                i = indexOfWordInString(&s, &p);
                if (i == -1)
                    break;

                t += s.left(i);
                t += newlabel;
                s = s.mid(i + p.length());
            }
            s = t + s;
        }

        return s;
    }*/

#ifdef Q_OS_WIN
extern QByteArray Win_GetShortPathName(QByteArray f);
#endif

// -----------------------------------------------------------------------
//          maplepathformat
// -----------------------------------------------------------------------
//
// replace all backslashes \ by \\ (ANSI C format)
//
// Under windows, avoid spaces by using the short path form.

QByteArray maplepathformat(const QString fname)
{
    QByteArray ba_fname;

    ba_fname = QFile::encodeName(fname);
#ifdef Q_OS_WIN
    if (ba_fname.length() == 0) {
        return ba_fname;
    }
    if (ba_fname[ba_fname.length() - 1] != '\\' &&
        ba_fname[ba_fname.length() - 1] != '/') {
        // last character is not a path separator

        // no need to convert to short form
        // (this is to avoid error: conversion to short forms does not work
        // for files that possibly do not yet exist)

        ba_fname.replace("\\", "\\\\");
        return ba_fname;
    }

    ba_fname = Win_GetShortPathName(ba_fname);
#endif
    ba_fname.replace("\\", "\\\\");
    return ba_fname;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareFile
// -----------------------------------------------------------------------

void P4InputVF::prepareFile(QTextStream *fp, bool prepareforcurves)
{
    QString bsaveall;
    QString name_vectab;
    QString name_inftab;
    QString name_fintab;
    QString name_infres;
    QString name_finres;
    QString name_curves;
    QString mainreduce;
    QString s;

    QString mainmaple;
    QString user_bindir;
    QString user_tmpdir;
    QString user_lypexe;
    QString user_lypexe_mpf;
    QString user_sepexe;
    QString user_platform;
    QString user_removecmd;
    QString user_simplify;
    QString user_simplifycmd;
    QString user_sumtablepath;
    QString user_exeprefix;

    QByteArray ba_mainmaple;
    QByteArray ba_user_bindir;
    QByteArray ba_user_tmpdir;
    QByteArray ba_user_sumtablepath;
    QByteArray ba_name_vectab;
    QByteArray ba_name_fintab;
    QByteArray ba_name_inftab;
    QByteArray ba_name_finres;
    QByteArray ba_name_infres;
    QByteArray ba_name_curves;

    user_exeprefix = "";

    /*if( symbolicpackage == PACKAGE_REDUCE )
    {
        bsaveall = booleanString( action_SaveAll );
        mainreduce = getP4ReducePath();
        mainreduce += "/";
        mainreduce += MAINREDUCEFILE;

        name_vectab = getfilename_vectable();
        name_fintab = getfilename_fintable();
        name_inftab = getfilename_inftable();
        name_finres = getfilename_finresults();
        name_infres = getfilename_infresults();
        name_curves = getfilename_curveresults();

        if( prepareforcurves )
            RemoveFile( name_curves );
        else
        {
            RemoveFile( name_vectab );
            RemoveFile( name_fintab );
            RemoveFile( name_inftab );
            RemoveFile( name_infres );
            RemoveFile( name_finres );
        }

        s.sprintf( "all_crit_points:=%d$\n", typeofstudy );
        s += "save_all:=" + bsaveall + "$\n";
        s += "vec_table:=\"" + name_vectab + "\"$\n";
        s += "finite_table:=\"" + name_fintab + "\"$\n";
        s += "finite_res:=\"" + name_finres + "\"$\n";
        s += "infinite_table:=\"" + name_inftab + "\"$\n";
        s += "infinite_res:=\"" + name_infres + "\"$\n";

        *fp << s;

        PrepareReduceVectorField( fp );
        PrepareReduceParameters( fp );
        PrepareReduceCurves( fp );
        PrepareReducePiecewiseConfig( fp );

        s = "in \"" + mainreduce + "\"$\n";
        *fp << s;
    }
    else
    {*/
    mainmaple = getP4MaplePath();
    user_bindir = getP4BinPath();
    user_tmpdir = getP4TempPath();
    user_sumtablepath = getP4SumTablePath();

    mainmaple += QDir::separator();
    if (user_bindir != "")
        user_bindir += QDir::separator();
    if (user_tmpdir != "")
        user_tmpdir += QDir::separator();
    if (user_sumtablepath != "")
        user_sumtablepath += QDir::separator();
    user_platform = USERPLATFORM;
#ifdef Q_WS_WIN
    user_removecmd = "cmd /c del";
    user_exeprefix = "cmd /c ";
#else
    user_removecmd = "rm";
    user_exeprefix = "";
#endif
    mainmaple += MAINMAPLEFILE;
#ifdef Q_WS_WIN
    user_lypexe = "lyapunov.exe";
    user_lypexe_mpf = "lyapunov_mpf.exe";
    user_sepexe = "separatrice.exe";
#else
    user_lypexe = "lyapunov";
    user_lypexe_mpf = "lyapunov_mpf";
    user_sepexe = "separatrice";
#endif

    ba_mainmaple = maplepathformat(mainmaple);
    ba_user_bindir = maplepathformat(user_bindir);
    ba_user_tmpdir = maplepathformat(user_tmpdir);
    ba_user_sumtablepath = maplepathformat(user_sumtablepath);

    if (numeric_[0])
        user_simplify = "false";
    else
        user_simplify = "true";

    user_simplifycmd = MAPLE_SIMPLIFY_EXPRESSIONS;

    *fp << "restart;\n";
    *fp << "read( \"" << ba_mainmaple << "\" );\n";
    *fp << "user_bindir := \"" << ba_user_bindir << "\":\n";
    *fp << "user_tmpdir := \"" << ba_user_tmpdir << "\":\n";
    *fp << "user_lypexe := \"" << user_lypexe << "\":\n";
    *fp << "user_lypexe_mpf := \"" << user_lypexe_mpf << "\":\n";
    *fp << "user_sepexe := \"" << user_sepexe << "\":\n";
    *fp << "user_exeprefix := \"" << user_exeprefix << "\":\n";
    *fp << "user_platform := \"" << user_platform << "\":\n";
    *fp << "user_sumtablepath := \"" << ba_user_sumtablepath << "\":\n";
    *fp << "user_removecmd := \"" << user_removecmd << "\":\n";
    *fp << "user_simplify := " << user_simplify << ":\n";
    *fp << "user_simplifycmd := " << user_simplifycmd << ":\n";

    bsaveall = booleanString(action_SaveAll);

    name_vectab = getfilename_vectable();
    name_fintab = getfilename_fintable();
    name_inftab = getfilename_inftable();
    name_finres = getfilename_finresults();
    name_infres = getfilename_infresults();
    name_curves = getfilename_curveresults();

    if (prepareforcurves) {
        removeFile(name_curves);
    } else {
        removeFile(name_vectab);
        removeFile(name_fintab);
        removeFile(name_inftab);
        removeFile(name_infres);
        removeFile(name_finres);
    }

    ba_name_vectab = maplepathformat(name_vectab);
    ba_name_fintab = maplepathformat(name_fintab);
    ba_name_inftab = maplepathformat(name_inftab);
    ba_name_finres = maplepathformat(name_finres);
    ba_name_infres = maplepathformat(name_infres);
    ba_name_curves = maplepathformat(name_curves);

    *fp << "all_crit_points := " << typeofstudy_ << ":\n";
    *fp << "save_all := " << bsaveall << ":\n";

    *fp << "vec_table := \"" << ba_name_vectab << "\":\n";
    *fp << "finite_table := \"" << ba_name_fintab << "\":\n";
    *fp << "finite_res := \"" << ba_name_finres << "\":\n";
    *fp << "infinite_table := \"" << ba_name_inftab << "\":\n";
    *fp << "infinite_res := \"" << ba_name_infres << "\":\n";
    *fp << "user_curvesfile := \"" << ba_name_curves << "\":\n";

    prepareMapleVectorField(fp);
    prepareMapleParameters(fp);
    prepareMapleCurves(fp);
    prepareMaplePiecewiseConfig(fp);

    if (prepareforcurves) {
        *fp << "user_precision := 8:\ntry FindAllCurves() catch:\n"
               "printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
               "lastexception[2] );\n"
               "finally: closeallfiles();\n"
               "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
               "try:\n";
    } else {
        *fp << "try p5main() catch:\n"
               "printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
               "lastexception[2] );\n"
               "finally: closeallfiles();\n"
               "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
               "try:\n";
    }
    //}
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurveFile
// -----------------------------------------------------------------------
void P4InputVF::prepareCurveFile(QTextStream *fp)
{
    QString name_curvetab;
    QString s;

    QString mainmaple;
    QString user_bindir;
    QString user_tmpdir;
    QString user_platform;
    QString user_removecmd;
    QString user_simplify;
    QString user_simplifycmd;
    QString user_sumtablepath;
    QString user_exeprefix;

    QByteArray ba_mainmaple;
    QByteArray ba_user_bindir;
    QByteArray ba_user_tmpdir;
    QByteArray ba_name_curvetab;

    user_exeprefix = "";

    mainmaple = getP4MaplePath();
    user_bindir = getP4BinPath();
    user_tmpdir = getP4TempPath();
    user_sumtablepath = getP4SumTablePath();

    mainmaple += QDir::separator();
    if (user_bindir != "")
        user_bindir += QDir::separator();
    if (user_tmpdir != "")
        user_tmpdir += QDir::separator();
    user_platform = USERPLATFORM;
#ifdef Q_OS_WIN
    user_removecmd = "cmd /c del";
    user_exeprefix = "cmd /c ";
#else
    user_removecmd = "rm";
    user_exeprefix = "";
#endif
    mainmaple += MAINMAPLEFILE;

    ba_mainmaple = maplepathformat(mainmaple);
    ba_user_bindir = maplepathformat(user_bindir);
    ba_user_tmpdir = maplepathformat(user_tmpdir);

    if (numeric_[0])
        user_simplify = "false";
    else
        user_simplify = "true";

    user_simplifycmd = MAPLE_SIMPLIFY_EXPRESSIONS;

    *fp << "restart;\n";
    *fp << "read( \"" << ba_mainmaple << "\" );\n";
    *fp << "user_bindir := \"" << ba_user_bindir << "\":\n";
    *fp << "user_tmpdir := \"" << ba_user_tmpdir << "\":\n";
    *fp << "user_exeprefix := \"" << user_exeprefix << "\":\n";
    *fp << "user_platform := \"" << user_platform << "\":\n";
    *fp << "user_removecmd := \"" << user_removecmd << "\":\n";
    *fp << "user_simplify := " << user_simplify << ":\n";
    *fp << "user_simplifycmd := " << user_simplifycmd << ":\n";
    *fp << "all_crit_points := " << typeofstudy_ << ":\n";

    name_curvetab = getfilename_curvetable();
    removeFile(name_curvetab);
    ba_name_curvetab = maplepathformat(name_curvetab);
    *fp << "curve_table := \"" << ba_name_curvetab << "\":\n";

    prepareMapleCurve(fp);
    prepareMapleParameters(fp);

    *fp << "try prepareCurve() catch:\n"
           "printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
           "lastexception[2] );\n"
           "finally: closeallfiles();\n"
           "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
           "try:\n";
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclinesFile
// -----------------------------------------------------------------------
void P4InputVF::prepareIsoclinesFile(QTextStream *fp)
{
    QString name_isoclinestab;
    QString s;

    QString mainmaple;
    QString user_bindir;
    QString user_tmpdir;
    QString user_platform;
    QString user_removecmd;
    QString user_simplify;
    QString user_simplifycmd;
    QString user_sumtablepath;
    QString user_exeprefix;

    QByteArray ba_mainmaple;
    QByteArray ba_user_bindir;
    QByteArray ba_user_tmpdir;
    QByteArray ba_name_isoclinestab;

    user_exeprefix = "";

    mainmaple = getP4MaplePath();
    user_bindir = getP4BinPath();
    user_tmpdir = getP4TempPath();
    user_sumtablepath = getP4SumTablePath();

    mainmaple += QDir::separator();
    if (user_bindir != "")
        user_bindir += QDir::separator();
    if (user_tmpdir != "")
        user_tmpdir += QDir::separator();
    user_platform = USERPLATFORM;
#ifdef Q_OS_WIN
    user_removecmd = "cmd /c del";
    user_exeprefix = "cmd /c ";
#else
    user_removecmd = "rm";
    user_exeprefix = "";
#endif
    mainmaple += MAINMAPLEFILE;

    ba_mainmaple = maplepathformat(mainmaple);
    ba_user_bindir = maplepathformat(user_bindir);
    ba_user_tmpdir = maplepathformat(user_tmpdir);

    if (numeric_[0])
        user_simplify = "false";
    else
        user_simplify = "true";

    user_simplifycmd = MAPLE_SIMPLIFY_EXPRESSIONS;

    *fp << "restart;\n";
    *fp << "read( \"" << ba_mainmaple << "\" );\n";
    *fp << "user_bindir := \"" << ba_user_bindir << "\":\n";
    *fp << "user_tmpdir := \"" << ba_user_tmpdir << "\":\n";
    *fp << "user_exeprefix := \"" << user_exeprefix << "\":\n";
    *fp << "user_platform := \"" << user_platform << "\":\n";
    *fp << "user_removecmd := \"" << user_removecmd << "\":\n";
    *fp << "user_simplify := " << user_simplify << ":\n";
    *fp << "user_simplifycmd := " << user_simplifycmd << ":\n";
    *fp << "all_crit_points := " << typeofstudy_ << ":\n";

    name_isoclinestab = getfilename_isoclinestable();
    removeFile(name_isoclinestab);
    ba_name_isoclinestab = maplepathformat(name_isoclinestab);
    *fp << "isoclines_table := \"" << ba_name_isoclinestab << "\":\n";

    prepareMapleIsoclines(fp);
    prepareMapleParameters(fp);

    *fp << "try prepareIsoclines() catch:\n"
           "printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
           "lastexception[2] );\n"
           "finally: closeallfiles();\n"
           "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
           "try:\n";
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluate
// -----------------------------------------------------------------------

void P4InputVF::evaluate()
{
    // QString filedotred;
    // QString filedotrun;
    QString filedotmpl;
    QString s, e;
    // QFile *fptr;
    // QTextStream *fp;
    QProcess *proc;

    evaluatinggcf_ = false;
    evaluatingCurve_ = false;
    evaluatingIsoclines_ = false;

    // possible clean up after last GCF evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepare();

    /*if (symbolicpackage == PACKAGE_REDUCE) {
        filedotred = getreducefilename();
        filedotrun = getrunfilename();

        fptr = new QFile(filedotrun);
        if (fptr->open(QIODevice::WriteOnly)) {
            fp = new QTextStream(fptr);
            *fp << "#!/bin/sh\n";
            *fp << GetReduceExe() << " <" << filedotred << "\n";
            fp->flush();
            delete fp;
            fptr->close();
            delete fptr;
            fptr = nullptr;
        } else {
            delete fptr;
            fptr = nullptr;

            // cannot open???
        }

        if (ProcessText == nullptr)
            CreateProcessWindow();
        else {
            ProcessText->append(
                "\n\n----------------------------------------------------------"
                "---------------------\n\n");
            ProcessText->show();
            ProcessButton->setEnabled(true);
            ProcessClearButton->setEnabled(true);
        }

        proc = new QProcess(this);
        proc->setWorkingDirectory(QDir::currentPath());

        connect(proc, SIGNAL(finished(int)), g_p4app,
                SLOT(Signal_Evaluated(int)));
        connect(proc, SIGNAL(error(QProcess::ProcessError)), g_p4app,
                SLOT(cathProcessError(QProcess::ProcessError)));
        connect(proc, SIGNAL(readyReadStandardOutput()), this,
                SLOT(ReadProcessStdout()));

        processfailed = false;
        QString pa = "External Command: ";
        pa += "sh ";
        pa += filedotrun;
        ProcessText->append(pa);
        proc->start("sh", QStringList(filedotrun), QIODevice::ReadWrite);
        if (proc->state() != QProcess::Running &&
            proc->state() != QProcess::Starting) {
            processfailed = true;
            delete proc;
            proc = nullptr;
            EvalProcess = nullptr;
            EvalFile = "";
            EvalFile2 = "";
            g_p4app->Signal_Evaluated(-1);
            ProcessButton->setEnabled(false);
        } else {
            EvalProcess = proc;
            EvalFile = filedotred;
            EvalFile2 = filedotrun;
            evaluating = true;
            evaluating_gcf = false;
            evaluating_piecewiseconfig = false;
        }
    } else {*/
    filedotmpl = getmaplefilename();

    s = getMapleExe();
    if (s == "" || s.isNull()) {
        s = "";
    } else {
        s = s.append(" ");
        if (filedotmpl.contains(' ')) {
            s = s.append("\"");
            s = s.append(filedotmpl);
            s = s.append("\"");
        } else
            s = s.append(filedotmpl);

        if (outputWindow_ == nullptr || processText_ == nullptr)
            createProcessWindow();
        else {
            processText_->append(
                "\n\n--------------------------------------"
                "-----------------------------------------"
                "\n\n");
            terminateProcessButton_->setEnabled(true);
            outputWindow_->show();
            outputWindow_->raise();
        }

        proc = new QProcess(this);

        proc->setWorkingDirectory(QDir::currentPath());

        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &QP4Application::signalEvaluated);
        connect(proc, &QProcess::readyReadStandardOutput, this,
                &P4InputVF::readProcessStdout);
#ifdef QT_QPROCESS_OLD
        connect(proc,
                static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                    &QProcess::error),
                this, &P4InputVF::catchProcessError);
#else
        connect(proc, &QProcess::errorOccurred, this,
                &P4InputVF::catchProcessError);
#endif

        processfailed_ = false;
        QString pa = "External Command: ";
        pa += getMapleExe();
        pa += " ";
        pa += filedotmpl;
        processText_->append(pa);
        proc->start(getMapleExe(), QStringList(filedotmpl),
                    QIODevice::ReadWrite);

        if (proc->state() != QProcess::Running &&
            proc->state() != QProcess::Starting) {
            processfailed_ = true;
            delete proc;
            proc = nullptr;
            evalProcess_ = nullptr;
            evalFile_ = "";
            evalFile2_ = "";
            g_p4app->signalEvaluated(-1);
            terminateProcessButton_->setEnabled(false);
        } else {
            evalProcess_ = proc;
            evalFile_ = filedotmpl;
            evalFile2_ = "";
            evaluating_ = true;
            evaluatinggcf_ = false;
            evaluatingPiecewiseConfig_ = false;
        }
    }
    //}
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateCurveTable
// -----------------------------------------------------------------------
void P4InputVF::evaluateCurveTable()
{
    QString filedotmpl;
    QString s, e;
    QProcess *proc;

    evaluatinggcf_ = false;
    evaluatingCurve_ = false;
    evaluatingIsoclines_ = false;

    // possible clean up after last Curve evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepareCurve();
    filedotmpl = getPrepareCurveFileName();

    s = getMapleExe();
    if (s.isNull()) {
        s = "";
    } else {
        s = s.append(" ");
        if (filedotmpl.contains(' ')) {
            s = s.append("\"");
            s = s.append(filedotmpl);
            s = s.append("\"");
        } else
            s = s.append(filedotmpl);

        /* Here a window for displaying the output text of the Maple process
         * is created */
        if (outputWindow_ == nullptr)
            createProcessWindow();
        else {
            processText_->append(
                "\n\n--------------------------------------"
                "-----------------------------------------"
                "\n\n");
            terminateProcessButton_->setEnabled(true);
            outputWindow_->show();
            outputWindow_->raise();
        }

        // proc = new QProcess(this);

        // QProcess *proc;
        if (evalProcess_ != nullptr) {  // re-use process of last GCF
            proc = evalProcess_;
            disconnect(proc, SIGNAL(finished(int)), g_p4app, 0);
            connect(proc,
                    static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                    g_p4app, &QP4Application::signalCurveEvaluated);
        } else {
            proc = new QProcess(this);
            proc->setWorkingDirectory(QDir::currentPath());
            connect(proc,
                    static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                    g_p4app, &QP4Application::signalCurveEvaluated);
            connect(proc, &QProcess::readyReadStandardOutput, this,
                    &P4InputVF::readProcessStdout);
#ifdef QT_QPROCESS_OLD
            connect(proc,
                    static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                        &QProcess::error),
                    g_p4app, &QP4Application::catchProcessError);
#else
            connect(proc, &QProcess::errorOccurred, g_p4app,
                    &QP4Application::catchProcessError);
#endif
        }

        processfailed_ = false;
        QString pa = "External Command: ";
        pa += getMapleExe();
        pa += " ";
        pa += filedotmpl;
        processText_->append(pa);
        proc->start(getMapleExe(), QStringList(filedotmpl),
                    QIODevice::ReadWrite);

        if (proc->state() != QProcess::Running &&
            proc->state() != QProcess::Starting) {
            processfailed_ = true;
            delete proc;
            proc = nullptr;
            evalProcess_ = nullptr;
            evalFile_ = "";
            evalFile2_ = "";
            g_p4app->signalEvaluated(-1);
            terminateProcessButton_->setEnabled(false);
        } else {
            evalProcess_ = proc;
            evalFile_ = filedotmpl;
            evalFile2_ = "";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateIsoclinesTable
// -----------------------------------------------------------------------
void P4InputVF::evaluateIsoclinesTable()
{
    QString filedotmpl;
    QString s, e;
    // QProcess *proc;

    evaluatinggcf_ = false;
    evaluatingCurve_ = false;
    evaluatingIsoclines_ = false;
    // possible clean up after last Curve evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepareIsoclines();
    filedotmpl = getPrepareIsoclinesFileName();

    s = getMapleExe();
    if (s.isNull()) {
        s = "";
    } else {
        s = s.append(" ");
        if (filedotmpl.contains(' ')) {
            s = s.append("\"");
            s = s.append(filedotmpl);
            s = s.append("\"");
        } else
            s = s.append(filedotmpl);

        /* Here a window for displaying the output text of the Maple process
         * is created */
        if (outputWindow_ == nullptr || processText_ == nullptr)
            createProcessWindow();
        else {
            processText_->append(
                "\n\n--------------------------------------"
                "-----------------------------------------"
                "\n\n");
            terminateProcessButton_->setEnabled(true);
            outputWindow_->show();
            outputWindow_->raise();
        }

        QProcess *proc;
        if (evalProcess_ != nullptr) {  // re-use process of last GCF
            proc = evalProcess_;
            disconnect(proc, SIGNAL(finished(int)), g_p4app, 0);
            connect(proc,
                    static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                    g_p4app, &QP4Application::signalCurveEvaluated);
        } else {
            proc = new QProcess(this);
            connect(proc,
                    static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                    g_p4app, &QP4Application::signalCurveEvaluated);
            connect(proc, &QProcess::readyReadStandardOutput, this,
                    &P4InputVF::readProcessStdout);
#ifdef QT_QPROCESS_OLD
            connect(proc,
                    static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                        &QProcess::error),
                    g_p4app, &QP4Application::catchProcessError);
#else
            connect(proc, &QProcess::errorOccurred, g_p4app,
                    &QP4Application::catchProcessError);
#endif
        }

        proc->setWorkingDirectory(QDir::currentPath());

        processfailed_ = false;
        QString pa = "External Command: ";
        pa += getMapleExe();
        pa += " ";
        pa += filedotmpl;
        processText_->append(pa);
        proc->start(getMapleExe(), QStringList(filedotmpl),
                    QIODevice::ReadWrite);

        if (proc->state() != QProcess::Running &&
            proc->state() != QProcess::Starting) {
            processfailed_ = true;
            delete proc;
            proc = nullptr;
            evalProcess_ = nullptr;
            evalFile_ = "";
            evalFile2_ = "";
            g_p4app->signalEvaluated(-1);
            terminateProcessButton_->setEnabled(false);
        } else {
            evalProcess_ = proc;
            evalFile_ = filedotmpl;
            evalFile2_ = "";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::catchProcessError
// -----------------------------------------------------------------------
void P4InputVF::catchProcessError(QProcess::ProcessError prerr)
{
    processfailed_ = true;
    switch (prerr) {
        case QProcess::FailedToStart:
            processError_ = "Failed to start";
            break;
        case QProcess::Crashed:
            processError_ = "Crash";
            break;
        case QProcess::Timedout:
            processError_ = "Time-out";
            break;
        case QProcess::WriteError:
        case QProcess::ReadError:
        case QProcess::UnknownError:
        default:
            processError_ = "Unknown error";
            break;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::finishEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishEvaluation(int exitCode)
{
    UNUSED(exitCode);
    if (!evalFile_.isNull() && evalFile_ != "") {
        removeFile(evalFile_);
        evalFile_ = "";
    }

    if (!evalFile2_.isNull() && evalFile2_ != "") {
        removeFile(evalFile2_);
        evalFile2_ = "";
    }

    if (terminateProcessButton_ != nullptr)
        terminateProcessButton_->setEnabled(false);

    if (evalProcess_ != nullptr) {
        if (processText_ != nullptr) {
            outputWindow_->show();
            outputWindow_->raise();
            QString buf;
            buf =
                "\n----------------------------------------------------------"
                "----"
                "-----------------\n";
            processText_->append(buf);
            if (evalProcess_ != nullptr) {
                if (evalProcess_->state() == QProcess::Running) {
                    evalProcess_->terminate();
                    QTimer::singleShot(5000, evalProcess_, SLOT(kill()));
                    buf = "Kill signal sent to process.\n";
                } else {
                    if (!processfailed_)
                        buf.sprintf("The process finished normally (%d)\n",
                                    evalProcess_->exitCode());
                    else {
                        buf.sprintf("The process stopped abnormally (%d : ",
                                    evalProcess_->exitCode());
                        buf += processError_;
                        buf += ")\n";
                    }
                }
            } else {
                if (processfailed_)
                    buf =
                        "The following error occured: " + processError_ + "\n";
                else
                    buf = "";
            }
            processText_->append(buf);
        }
    }
    /*if (processText_ != nullptr) {
        //      processText_->hide();
        if (processText_->isActiveWindow()) {
            if (!evaluatinggcf_)
                g_p4stardlg->activateWindow();
            else {
                if (gcfDlg != nullptr)
                    gcfDlg->activateWindow();
            }
        }
    }*/

    if (evaluatinggcf_)
        finishGcfEvaluation();
    if (evaluatingCurve_)
        finishCurveEvaluation();
    if (evaluatingIsoclines_)
        finishIsoclinesEvaluation();
    if (evaluatingPiecewiseConfig_)
        finishCurvesEvaluation();
}

// -----------------------------------------------------------------------
//          P4InputVF::finishGcfEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishGcfEvaluation()
{
    evaluatinggcf_ = false;
    if (gcfDlg_ != nullptr) {
        gcfDlg_->finishGcfEvaluation();

        //        QP4Event * e = new QP4Event(
        //        (QEvent::Type)TYPE_SIGNAL_EVALUATED, nullptr );
        //        g_p4app->postEvent( g_p4stardlg, e );
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::finishCurveEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishCurveEvaluation()
{
    evaluatingCurve_ = false;
    if (curveDlg_ != nullptr) {
        curveDlg_->finishCurveEvaluation();
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::finishIsoclinesEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishIsoclinesEvaluation()
{
    evaluatingIsoclines_ = false;
    if (isoclinesDlg_ != nullptr) {
        isoclinesDlg_->finishIsoclinesEvaluation();
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepare
// -----------------------------------------------------------------------
void P4InputVF::prepare()
{
    // QString filedotred;
    QString filedotmpl;
    QFile *fptr;
    QTextStream *fp;

    /*if (symbolicpackage_ == PACKAGE_REDUCE) {
        filedotred = getreducefilename();
        fptr = new QFile(filedotred);
        if (fptr->open(QIODevice::WriteOnly)) {
            fp = new QTextStream(fptr);
            prepareFile(fp);
            fp->flush();
            delete fp;
            fp = nullptr;
            fptr->close();
            delete fptr;
            fptr = nullptr;
        } else {
            delete fptr;
            fptr = nullptr;

            // cannot open ???
        }
    } else {*/
    filedotmpl = getmaplefilename();

    fptr = new QFile(filedotmpl);
    if (fptr->open(QIODevice::WriteOnly)) {
        fp = new QTextStream(fptr);
        prepareFile(fp);
        fp->flush();
        delete fp;
        fp = nullptr;
        fptr->close();
        delete fptr;
        fptr = nullptr;
    } else {
        delete fptr;
        fptr = nullptr;

        // cannot open?
    }
    /*}*/
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurve
// -----------------------------------------------------------------------
void P4InputVF::prepareCurve()
{
    QString filedotmpl;
    QFile *fptr;
    QTextStream *fp;

    filedotmpl = getPrepareCurveFileName();

    fptr = new QFile(filedotmpl);
    if (fptr->open(QIODevice::WriteOnly)) {
        fp = new QTextStream(fptr);
        prepareCurveFile(fp);
        fp->flush();
        delete fp;
        fp = nullptr;
        fptr->close();
        delete fptr;
        fptr = nullptr;
    } else {
        delete fptr;
        fptr = nullptr;

        // cannot open?
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclines
// -----------------------------------------------------------------------
void P4InputVF::prepareIsoclines()
{
    QString filedotmpl;
    QFile *fptr;
    QTextStream *fp;

    filedotmpl = getPrepareIsoclinesFileName();

    fptr = new QFile(filedotmpl);
    if (fptr->open(QIODevice::WriteOnly)) {
        fp = new QTextStream(fptr);
        prepareIsoclinesFile(fp);
        fp->flush();
        delete fp;
        fp = nullptr;
        fptr->close();
        delete fptr;
        fptr = nullptr;
    } else {
        delete fptr;
        fptr = nullptr;

        // cannot open?
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::readProcessStdout
// -----------------------------------------------------------------------
void P4InputVF::readProcessStdout()
{
    QByteArray t;
    QByteArray line;
    int i, j;

    if (evalProcess_ == nullptr || processText_ == nullptr)
        return;

    while (1) {
        t = evalProcess_->readAllStandardOutput();
        if (t.length() == 0)
            break;

        i = t.indexOf('\n');
        j = t.indexOf('\r');
        while (i >= 0 || j >= 0) {
            if ((j < i && j != -1) || i == -1) {
                line = t.left(j);
                if (i == j + 1)
                    t = t.mid(j + 2);
                else
                    t = t.mid(j + 1);  // treat CR+LF as one lineend
            } else {
                line = t.left(i);
                t = t.mid(i + 1);
            }
            processText_->append(line);
            i = t.indexOf('\n');
            j = t.indexOf('\r');
        }
        if (t.length() != 0)
            processText_->append(t);
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::onTerminateButton
// -----------------------------------------------------------------------
void P4InputVF::onTerminateButton()
{
    QString buf;
    if (evalProcess_ != nullptr) {
        if (evalProcess_->state() == QProcess::Running) {
            buf =
                "\n----------------------------------------------------------"
                "---------------------\n";
            processText_->append(buf);
            evalProcess_->terminate();
            QTimer::singleShot(2000, evalProcess_, SLOT(kill()));
            buf = "Kill signal sent to process.\n";
            processText_->append(buf);
            processfailed_ = true;
            processError_ = "Terminated by user";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::createProcessWindow
// -----------------------------------------------------------------------
void P4InputVF::createProcessWindow()
{
    if (outputWindow_ != nullptr) {
        if (outputWindow_->isVisible() == false)
            outputWindow_->show();
        outputWindow_->activateWindow();
        outputWindow_->raise();
        return;
    }

    outputWindow_ = new QWidget(nullptr);
    outputWindow_->setWindowTitle("Output window");
    outputWindow_->resize(530, 344);
    if (g_p4smallicon != nullptr)
        outputWindow_->setWindowIcon(*g_p4smallicon);

    QVBoxLayout *vLayout = new QVBoxLayout(outputWindow_);
    vLayout->setSpacing(3);
    vLayout->setContentsMargins(5, 5, 5, 5);
    QVBoxLayout *vLayout2 = new QVBoxLayout();
    vLayout2->setSpacing(3);

    processText_ = new QTextEdit(outputWindow_);
    processText_->setLineWrapMode(QTextEdit::FixedColumnWidth);
    processText_->setWordWrapMode(QTextOption::WrapAnywhere);
    processText_->setFont(*(g_p4app->courierFont_));
    processText_->setLineWrapColumnOrWidth(82);
    processText_->setReadOnly(true);
    processText_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vLayout2->addWidget(processText_);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(6);

    hLayout->addStretch();
    terminateProcessButton_ = new QPushButton("Terminate");
    terminateProcessButton_->setFont(*(g_p4app->boldFont_));
    terminateProcessButton_->setToolTip(
        "Terminates the process.  First tries to send a "
        "safe signal to the process.\nIf this does not "
        "work, then after 2 seconds, the program is "
        "abruptly terminated.");
    hLayout->addWidget(terminateProcessButton_);

    clearProcessButton_ = new QPushButton("Clear");
    clearProcessButton_->setFont((*g_p4app->boldFont_));
    clearProcessButton_->setToolTip("Clears this window");
    hLayout->addWidget(clearProcessButton_);

    vLayout2->addLayout(hLayout);
    vLayout->addLayout(vLayout2);

    outputWindow_->show();

    QObject::connect(terminateProcessButton_, &QPushButton::clicked, this,
                     &P4InputVF::onTerminateButton);
    QObject::connect(clearProcessButton_, &QPushButton::clicked, this, [=]() {
        if (processText_ != nullptr)
            processText_->clear();
    });
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateGcf
// -----------------------------------------------------------------------
bool P4InputVF::evaluateGcf()
{
    /*QString filedotred;
    QString filedotrun;*/
    QString filedotmpl;
    QString filedotgcf;
    QString s;

    /*if (symbolicpackage_ == PACKAGE_REDUCE) {
        filedotred = getreducefilename();
        filedotrun = getrunfilename();
        QString filedotgcfresults = getfilename_gcfresults();
        QFile *fptr = new QFile(filedotrun);
        if (fptr->open(QIODevice::WriteOnly)) {
            QTextStream *fps;
            fps = new QTextStream(fptr);
            *fps << "#!/bin/sh\n";
            *fps << getReduceExe() << ".psl"
                 << " <" << filedotred << ">" << filedotgcfresults << "\n";
            fps->flush();
            delete fps;
            fps = nullptr;
            fptr->close();
            delete fptr;
            fptr = nullptr;
        }

        if (processText_ == nullptr)
            createProcessWindow();
        else {
            processText_->append("\n\n------------------------------------------"
                                "-------------------------------------\n\n");
            terminateProcessButton_->setEnabled(true);
        }

        QProcess *proc;

        if (evalProcess_ != nullptr) { // re-use process of last GCF
            proc = evalProcess_;
            disconnect(proc, SIGNAL(finished(int)), g_p4app, 0);
            connect(proc, SIGNAL(finished(int)), g_p4app,
                    SLOT(signalCurveEvaluated(int)));
        } else {
            proc = new QProcess(this);
            connect(proc, SIGNAL(finished(int)), g_p4app,
                    SLOT(signalCurveEvaluated(int)));
            connect(proc, SIGNAL(error(QProcess::ProcessError)), g_p4app,
                    SLOT(catchProcessError(QProcess::ProcessError)));
            connect(proc, SIGNAL(readyReadStandardOutput()), this,
                    SLOT(readProcessStdout()));
        }

        proc->setWorkingDirectory(QDir::currentPath());

        processfailed_ = false;

        // syntax:  %s/psl/bpsl -td %d -f %s/reduce.img <%s >%s"
        //                      %s = reduce directory (getenv("reduce"))
        //                      %d = memory
        //                      %s = reduce directory (getenv("reduce"))
        //                      %s = filedotred
        //                      %s = getfilename_gcfresults

        QString pa = "External Command: sh ";
        pa += filedotrun;
        processText_->append(pa);
        proc->start("sh", QStringList(filedotrun), QIODevice::ReadWrite);

        if (proc->state() != QProcess::Running &&
            proc->state() != QProcess::Starting) {
            processfailed_ = true;
            delete proc;
            proc = nullptr;
            evalProcess_ = nullptr;
            evalFile_ = "";
            evalFile2_ = "";
            g_p4app->signalCurveEvaluated(-1);
            terminateProcessButton_->setEnabled(false);
            return false;
        } else {
            evalProcess_ = proc;
            evalFile_ = filedotred;
            evalFile2_ = filedotrun;
            evaluatinggcf_ = true;
            return true;
        }
    } else {*/  // MAPLE MAPLE MAPLE
    filedotmpl = getmaplefilename();

    s = getMapleExe();
    s = s.append(" ");
    if (filedotmpl.contains(' ')) {
        s = s.append("\"");
        s = s.append(filedotmpl);
        s = s.append("\"");
    } else
        s = s.append(filedotmpl);

    if (processText_ == nullptr)
        createProcessWindow();
    else {
        processText_->append(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        terminateProcessButton_->setEnabled(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    QProcess *proc;
    if (evalProcess_ != nullptr) {  // re-use process of last GCF
        proc = evalProcess_;
        disconnect(proc, SIGNAL(finished(int)), g_p4app, 0);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &QP4Application::signalCurveEvaluated);
    } else {
        proc = new QProcess(this);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &QP4Application::signalCurveEvaluated);
#ifdef QT_QPROCESS_OLD
        connect(proc,
                static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                    &QProcess::error),
                g_p4app, &QP4Application::catchProcessError);
#else
        connect(proc, &QProcess::errorOccurred, g_p4app,
                &QP4Application::catchProcessError);
#endif
        connect(proc, &QProcess::readyReadStandardOutput, this,
                &P4InputVF::readProcessStdout);
    }

    proc->setWorkingDirectory(QDir::currentPath());

    processfailed_ = false;
    QString pa = "External Command: ";
    pa += getMapleExe();
    pa += " ";
    pa += filedotmpl;
    processText_->append(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);
    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processfailed_ = true;
        delete proc;
        proc = nullptr;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        g_p4app->signalCurveEvaluated(-1);
        terminateProcessButton_->setEnabled(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = filedotmpl;
        evalFile2_ = "";
        evaluatinggcf_ = true;
        evaluating_ = true;
        evaluatingPiecewiseConfig_ = false;

        return true;
    }
    /*}*/
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareGcf
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in plane/U1/U2 charts.  This
// is only called in case of Poincare-compactification (weights p=q=1)

bool P4InputVF::prepareGcf(P4POLYNOM2 f, double y1, double y2, int precision,
                           int numpoints)
{
    FILE *fp;
    int i;
    char buf[100];

    /*if (symbolicpackage_ == PACKAGE_REDUCE) {
        QString filedotred;
        QString userfilered;

        filedotred = getreducefilename();
        fp = fopen(QFile::encodeName(filedotred), "w");

        userfilered = getfilename_gcf();
        removeFile(userfilered);

        fprintf(fp, "load gnuplot; \n");
        fprintf(fp, "symbolic procedure plot!-filename();\n");
        fprintf(fp, "plot!-files!*:=\"%s\";\n",
                (const char *)(QFile::encodeName(userfilered)));
        fprintf(fp, "lisp setq(plotcommand!*,\"cat\");\n");
        fprintf(fp, "precision %d$\n", precision);
        fprintf(fp, "f:=");
        for (i = 0; f != nullptr; i++) {
            fprintf(fp, "%s",
                    printterm2(buf, f, (i == 0) ? true : false, "x", "y"));
            f = f->next_term2;
        }
        if (i == 0)
            fprintf(fp, "0$\n");
        else
            fprintf(fp, "$\n");
        fprintf(fp, "on rounded$\n");
        fprintf(fp, "if deg(f,x)=0 then f:=f*(x^2+1)$\n");
        fprintf(fp, "if deg(f,y)=0 then f:=f*(y^2+1)$\n");
        fprintf(fp, "plot(f=0,x=(-1 .. 1),y=(%f .. %f),", (float)y1, (float)y2);
        fprintf(fp, "points=%d)$\n", numpoints);
        fclose(fp);
    } else {*/
    QString mainmaple;
    QString user_platform;
    QString user_file;
    QString filedotmpl;
    QByteArray ba_mainmaple;
    QByteArray ba_user_file;

    filedotmpl = getmaplefilename();

    fp = fopen(QFile::encodeName(filedotmpl), "w");
    if (fp == nullptr)
        return false;

    mainmaple = getP4MaplePath();
    mainmaple += QDir::separator();

    user_platform = USERPLATFORM;
    mainmaple += MAINMAPLEGCFFILE;

    ba_mainmaple = maplepathformat(mainmaple);
    user_file = getfilename_gcf();
    removeFile(user_file);
    ba_user_file = maplepathformat(user_file);

    fprintf(fp, "restart;\n");
    fprintf(fp, "read( \"%s\" );\n", (const char *)ba_mainmaple);
    fprintf(fp, "user_file := \"%s\":\n", (const char *)ba_user_file);
    fprintf(fp, "user_numpoints := %d:\n", numpoints);
    fprintf(fp, "Digits := %d:\n", precision);
    fprintf(fp, "user_x1 := %g:\n", (float)(-1.0));
    fprintf(fp, "user_x2 := %g:\n", (float)1.0);
    fprintf(fp, "user_y1 := %g:\n", (float)y1);
    fprintf(fp, "user_y2 := %g:\n", (float)y2);
    fprintf(fp, "u := %s:\n", "x");
    fprintf(fp, "v := %s:\n", "y");
    fprintf(fp, "user_f := ");
    for (i = 0; f != nullptr; i++) {
        fprintf(fp, "%s",
                printterm2(buf, f, (i == 0) ? true : false, "x", "y"));
        f = f->next_term2;
    }
    if (i == 0)
        fprintf(fp, "0:\n");
    else
        fprintf(fp, ":\n");

    fprintf(fp,
            "try FindSingularities() finally: if returnvalue=0 then "
            "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);
    /*}*/
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareGcf_LyapunovCyl
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))

bool P4InputVF::prepareGcf_LyapunovCyl(double theta1, double theta2,
                                       int precision, int numpoints, int index)
{
    FILE *fp;
    char buf[100];
    P4POLYNOM3 f;
    int i;

    f = g_VFResults.vf_[index]->gcf_C_;

    /*if (symbolicpackage_ == PACKAGE_REDUCE) {
        QString filedotred;
        QString userfilered;

        filedotred = getreducefilename();
        fp = fopen(QFile::encodeName(filedotred), "w");

        userfilered = getfilename_gcf();
        removeFile(userfilered);

        fprintf(fp, "load gnuplot; \n");
        fprintf(fp, "symbolic procedure plot!-filename();\n");
        fprintf(fp, "plot!-files!*:=\"%s\";\n",
                (const char *)QFile::encodeName(userfilered));
        fprintf(fp, "lisp setq(plotcommand!*,\"cat\");\n");

        fprintf(fp, "precision %d$\n", precision);
        fprintf(fp, "Co:=cos(y)$\n");
        fprintf(fp, "Si:=sin(y)$\n");
        fprintf(fp, "f:=");

        for (i = 0; f != nullptr; i++) {
            fprintf(fp, "%s", printterm3(buf, f, (i == 0) ? true : false, "x",
                                         "Co", "Si"));
            f = f->next_term3;
        }
        if (i == 0)
            fprintf(fp, "0$\n");
        else
            fprintf(fp, "$\n");

        fprintf(fp, "on rounded$\n");
        fprintf(fp, "if deg(f,x)=0 then f:=f*(x^2+1)$\n");
        fprintf(fp, "if deg(f,y)=0 then f:=f*(y^2+1)$\n");
        fprintf(fp, "plot(f=0,x=(0 .. 1),y=(%f .. %f),", (float)theta1,
                (float)theta2);
        fprintf(fp, "points=%d)$\n", numpoints);

        fclose(fp);
    } else {*/
    QString mainmaple;
    QString user_platform;
    QString user_file;
    QString filedotmpl;
    QByteArray ba_mainmaple;
    QByteArray ba_user_file;

    filedotmpl = getmaplefilename();

    fp = fopen(QFile::encodeName(filedotmpl), "w");
    if (fp == nullptr)
        return false;

    mainmaple = getP4MaplePath();
    mainmaple += QDir::separator();

    user_platform = USERPLATFORM;
    mainmaple += MAINMAPLEGCFFILE;

    ba_mainmaple = maplepathformat(mainmaple);
    user_file = getfilename_gcf();
    removeFile(user_file);
    ba_user_file = maplepathformat(user_file);

    fprintf(fp, "restart;\n");
    fprintf(fp, "read( \"%s\" );\n", (const char *)ba_mainmaple);
    fprintf(fp, "user_file := \"%s\":\n", (const char *)ba_user_file);
    fprintf(fp, "user_numpoints := %d:\n", numpoints);
    fprintf(fp, "Digits := %d:\n", precision);
    fprintf(fp, "user_x1 := %g:\n", (float)0.0);
    fprintf(fp, "user_x2 := %g:\n", (float)1.0);
    fprintf(fp, "user_y1 := %g:\n", (float)theta1);
    fprintf(fp, "user_y2 := %g:\n", (float)theta2);
    fprintf(fp, "u := %s:\n", "cos(y)");
    fprintf(fp, "v := %s:\n", "sin(y)");
    fprintf(fp, "user_f := ");

    for (i = 0; f != nullptr; i++) {
        fprintf(fp, "%s",
                printterm3(buf, f, (i == 0) ? true : false, "x", "U", "V"));
        f = f->next_term3;
    }
    if (i == 0)
        fprintf(fp, "0:\n");
    else
        fprintf(fp, ":\n");

    fprintf(fp,
            "try FindSingularities() finally: if returnvalue=0 then "
            "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);
    /*}*/
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareGcf_LyapunovR2
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
//
// same as preparegcf, except for the "u := " and "v := " assignments,
// and the fact that one always refers to the same function g_VFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.

bool P4InputVF::prepareGcf_LyapunovR2(int precision, int numpoints, int index)
{
    FILE *fp;
    char buf[100];
    P4POLYNOM2 f;
    int i;

    f = g_VFResults.vf_[index]->gcf_;

    /*if (symbolicpackage_ == PACKAGE_REDUCE) {
        QString filedotred;
        QString userfilered;

        filedotred = getreducefilename();
        fp = fopen(QFile::encodeName(filedotred), "w");

        userfilered = getfilename_gcf();
        removeFile(userfilered);

        fprintf(fp, "load gnuplot; \n");
        fprintf(fp, "symbolic procedure plot!-filename();\n");
        fprintf(fp, "plot!-files!*:=\"%s\";\n",
                (const char *)QFile::encodeName(userfilered));
        fprintf(fp, "lisp setq(plotcommand!*,\"cat\");\n");

        fprintf(fp, "precision %d$\n", precision);
        fprintf(fp, "u:=x*cos(y)$\n");
        fprintf(fp, "v:=x*sin(y)$\n");
        fprintf(fp, "f:=");
        for (i = 0; f != nullptr; i++) {
            fprintf(fp, "%s",
                    printterm2(buf, f, (i == 0) ? true : false, "u", "v"));
            f = f->next_term2;
        }
        if (i == 0)
            fprintf(fp, "0$\n");
        else
            fprintf(fp, "$\n");
        fprintf(fp, "on rounded$\n");
        fprintf(fp, "if deg(f,x)=0 then f:=f*(x^2+1)$\n");
        fprintf(fp, "if deg(f,y)=0 then f:=f*(y^2+1)$\n");
        fprintf(fp, "plot(f=0,x=(0 .. 1),y=(0 .. 2*pi),");
        fprintf(fp, "points=%d)$\n", numpoints);
        fclose(fp);
    } else {*/
    QString mainmaple;
    QString user_platform;
    QString user_file;
    QString filedotmpl;
    QByteArray ba_mainmaple;
    QByteArray ba_user_file;

    filedotmpl = getmaplefilename();

    fp = fopen(QFile::encodeName(filedotmpl), "w");
    if (fp == nullptr)
        return false;

    mainmaple = getP4MaplePath();
    mainmaple += QDir::separator();

    user_platform = USERPLATFORM;
    mainmaple = mainmaple.append(MAINMAPLEGCFFILE);

    ba_mainmaple = maplepathformat(mainmaple);
    user_file = getfilename_gcf();
    removeFile(user_file);
    ba_user_file = maplepathformat(user_file);

    fprintf(fp, "restart;\n");
    fprintf(fp, "read( \"%s\" );\n", (const char *)ba_mainmaple);
    fprintf(fp, "user_file := \"%s\":\n", (const char *)ba_user_file);
    fprintf(fp, "user_numpoints := %d:\n", numpoints);
    fprintf(fp, "Digits := %d:\n", precision);
    fprintf(fp, "user_x1 := %g:\n", (float)0.0);
    fprintf(fp, "user_x2 := %g:\n", (float)1.0);
    fprintf(fp, "user_y1 := %g:\n", (float)0.0);
    fprintf(fp, "user_y2 := %g:\n", (float)TWOPI);
    fprintf(fp, "u := %s:\n", "x*cos(y)");
    fprintf(fp, "v := %s:\n", "x*sin(y)");
    fprintf(fp, "user_f := ");

    for (i = 0; f != nullptr; i++) {
        fprintf(fp, "%s",
                printterm2(buf, f, (i == 0) ? true : false, "U", "V"));
        f = f->next_term2;
    }
    if (i == 0)
        fprintf(fp, "0:\n");
    else
        fprintf(fp, ":\n");

    fprintf(fp,
            "try FindSingularities() finally: if returnvalue=0 then "
            "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);
    /*}*/
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateCurve
// -----------------------------------------------------------------------
bool P4InputVF::evaluateCurve()
{
    QString filedotmpl;
    QString s;

    filedotmpl = getmaplefilename();

    s = getMapleExe();
    s = s.append(" ");
    if (filedotmpl.contains(' ')) {
        s = s.append("\"");
        s = s.append(filedotmpl);
        s = s.append("\"");
    } else
        s = s.append(filedotmpl);

    if (processText_ == nullptr)
        createProcessWindow();
    else {
        processText_->append(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        terminateProcessButton_->setEnabled(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    QProcess *proc;
    // re-use process of last GCF
    if (evalProcess_ != nullptr) {
        proc = evalProcess_;
        disconnect(proc, SIGNAL(finished(int)), g_p4app, 0);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &QP4Application::signalCurveEvaluated);
    } else {
        proc = new QProcess(this);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &QP4Application::signalCurveEvaluated);
#ifdef QT_QPROCESS_OLD
        connect(proc,
                static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                    &QProcess::error),
                g_p4app, &QP4Application::catchProcessError);
#else
        connect(proc, &QProcess::errorOccurred, g_p4app,
                &QP4Application::catchProcessError);
#endif
        connect(proc, &QProcess::readyReadStandardOutput, this,
                &P4InputVF::readProcessStdout);
    }

    proc->setWorkingDirectory(QDir::currentPath());

    processfailed_ = false;
    QString pa = "External Command: ";
    pa += getMapleExe();
    pa += " ";
    pa += filedotmpl;
    processText_->append(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);
    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processfailed_ = true;
        delete proc;
        proc = nullptr;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        g_p4app->signalCurveEvaluated(-1);
        terminateProcessButton_->setEnabled(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = filedotmpl;
        evalFile2_ = "";
        evaluatingCurve_ = true;

        return true;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurve
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating curve in plane/U1/U2 charts.  This
// is only called in case of Poincare-compactification (weights p=q=1)
bool P4InputVF::prepareCurve(P4POLYNOM2 f, double y1, double y2, int precision,
                             int numpoints)
{
    FILE *fp;
    int i;
    char buf[100];

    QString mainmaple;
    QString user_platform;
    QString user_file;
    QString filedotmpl;
    QByteArray ba_mainmaple;
    QByteArray ba_user_file;

    filedotmpl = getmaplefilename();

    fp = fopen(QFile::encodeName(filedotmpl), "w");
    if (fp == nullptr)
        return false;

    mainmaple = getP4MaplePath();
    mainmaple += QDir::separator();

    user_platform = USERPLATFORM;
    mainmaple += MAINMAPLEGCFFILE;

    ba_mainmaple = maplepathformat(mainmaple);
    user_file = getfilename_curve();
    removeFile(user_file);
    ba_user_file = maplepathformat(user_file);

    fprintf(fp, "restart;\n");
    fprintf(fp, "read( \"%s\" );\n", (const char *)ba_mainmaple);
    fprintf(fp, "user_file := \"%s\":\n", (const char *)ba_user_file);
    fprintf(fp, "user_numpoints := %d:\n", numpoints);
    fprintf(fp, "Digits := %d:\n", precision);
    fprintf(fp, "user_x1 := %g:\n", (float)(-1.0));
    fprintf(fp, "user_x2 := %g:\n", (float)1.0);
    fprintf(fp, "user_y1 := %g:\n", (float)y1);
    fprintf(fp, "user_y2 := %g:\n", (float)y2);
    fprintf(fp, "u := %s:\n", "x");
    fprintf(fp, "v := %s:\n", "y");
    fprintf(fp, "user_f := ");
    for (i = 0; f != nullptr; i++) {
        fprintf(fp, "%s",
                printterm2(buf, f, (i == 0) ? true : false, "x", "y"));
        f = f->next_term2;
    }
    if (i == 0)
        fprintf(fp, "0:\n");
    else
        fprintf(fp, ":\n");

    fprintf(fp,
            "try FindSingularities() finally: if returnvalue=0 then "
            "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);

    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurve_LyapunovCyl
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
bool P4InputVF::prepareCurve_LyapunovCyl(double theta1, double theta2,
                                         int precision, int numpoints,
                                         int index)
{
    FILE *fp;
    char buf[100];
    P4POLYNOM3 f;
    int i;

    f = g_VFResults.vf_[index]->curve_vector_.back().c;

    QString mainmaple;
    QString user_platform;
    QString user_file;
    QString filedotmpl;
    QByteArray ba_mainmaple;
    QByteArray ba_user_file;

    filedotmpl = getmaplefilename();

    fp = fopen(QFile::encodeName(filedotmpl), "w");
    if (fp == nullptr)
        return false;

    mainmaple = getP4MaplePath();
    mainmaple += QDir::separator();

    user_platform = USERPLATFORM;
    mainmaple += MAINMAPLEGCFFILE;

    ba_mainmaple = maplepathformat(mainmaple);
    user_file = getfilename_curve();
    removeFile(user_file);
    ba_user_file = maplepathformat(user_file);

    fprintf(fp, "restart;\n");
    fprintf(fp, "read( \"%s\" );\n", (const char *)ba_mainmaple);
    fprintf(fp, "user_file := \"%s\":\n", (const char *)ba_user_file);
    fprintf(fp, "user_numpoints := %d:\n", numpoints);
    fprintf(fp, "Digits := %d:\n", precision);
    fprintf(fp, "user_x1 := %g:\n", (float)0.0);
    fprintf(fp, "user_x2 := %g:\n", (float)1.0);
    fprintf(fp, "user_y1 := %g:\n", (float)theta1);
    fprintf(fp, "user_y2 := %g:\n", (float)theta2);
    fprintf(fp, "u := %s:\n", "cos(y)");
    fprintf(fp, "v := %s:\n", "sin(y)");
    fprintf(fp, "user_f := ");

    for (i = 0; f != nullptr; i++) {
        fprintf(fp, "%s",
                printterm3(buf, f, (i == 0) ? true : false, "x", "U", "V"));
        f = f->next_term3;
    }
    if (i == 0)
        fprintf(fp, "0:\n");
    else
        fprintf(fp, ":\n");

    fprintf(fp,
            "try FindSingularities() finally: if returnvalue=0 then "
            "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);

    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurve_LyapunovR2
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
//
// same as preparegcf, except for the "u := " and "v := " assignments,
// and the fact that one always refers to the same function g_VFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
bool P4InputVF::prepareCurve_LyapunovR2(int precision, int numpoints, int index)
{
    FILE *fp;
    char buf[100];
    P4POLYNOM2 f;
    int i;

    f = g_VFResults.vf_[index]->curve_vector_.back().r2;

    QString mainmaple;
    QString user_platform;
    QString user_file;
    QString filedotmpl;
    QByteArray ba_mainmaple;
    QByteArray ba_user_file;

    filedotmpl = getmaplefilename();

    fp = fopen(QFile::encodeName(filedotmpl), "w");
    if (fp == nullptr)
        return false;

    mainmaple = getP4MaplePath();
    mainmaple += QDir::separator();

    user_platform = USERPLATFORM;
    mainmaple = mainmaple.append(MAINMAPLEGCFFILE);

    ba_mainmaple = maplepathformat(mainmaple);
    user_file = getfilename_curve();
    removeFile(user_file);
    ba_user_file = maplepathformat(user_file);

    fprintf(fp, "restart;\n");
    fprintf(fp, "read( \"%s\" );\n", (const char *)ba_mainmaple);
    fprintf(fp, "user_file := \"%s\":\n", (const char *)ba_user_file);
    fprintf(fp, "user_numpoints := %d:\n", numpoints);
    fprintf(fp, "Digits := %d:\n", precision);
    fprintf(fp, "user_x1 := %g:\n", (float)0.0);
    fprintf(fp, "user_x2 := %g:\n", (float)1.0);
    fprintf(fp, "user_y1 := %g:\n", (float)0.0);
    fprintf(fp, "user_y2 := %g:\n", (float)TWOPI);
    fprintf(fp, "u := %s:\n", "x*cos(y)");
    fprintf(fp, "v := %s:\n", "x*sin(y)");
    fprintf(fp, "user_f := ");

    for (i = 0; f != nullptr; i++) {
        fprintf(fp, "%s",
                printterm2(buf, f, (i == 0) ? true : false, "U", "V"));
        f = f->next_term2;
    }
    if (i == 0)
        fprintf(fp, "0:\n");
    else
        fprintf(fp, ":\n");

    fprintf(fp,
            "try FindSingularities() finally: if returnvalue=0 then "
            "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);

    return true;
}
// -----------------------------------------------------------------------
//          P4InputVF::evaluateIsoclines
// -----------------------------------------------------------------------
bool P4InputVF::evaluateIsoclines()
{
    QString filedotmpl;
    QString s;

    filedotmpl = getmaplefilename();

    s = getMapleExe();
    s = s.append(" ");
    if (filedotmpl.contains(' ')) {
        s = s.append("\"");
        s = s.append(filedotmpl);
        s = s.append("\"");
    } else
        s = s.append(filedotmpl);

    if (processText_ == nullptr)
        createProcessWindow();
    else {
        processText_->append(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        terminateProcessButton_->setEnabled(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    QProcess *proc;
    if (evalProcess_ != nullptr) {  // re-use process of last GCF
        proc = evalProcess_;
        disconnect(proc, SIGNAL(finished(int)), g_p4app, 0);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &QP4Application::signalCurveEvaluated);
    } else {
        proc = new QProcess(this);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &QP4Application::signalCurveEvaluated);
#ifdef QT_QPROCESS_OLD
        connect(proc,
                static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                    &QProcess::error),
                g_p4app, &QP4Application::catchProcessError);
#else
        connect(proc, &QProcess::errorOccurred, g_p4app,
                &QP4Application::catchProcessError);
#endif
        connect(proc, &QProcess::readyReadStandardOutput, this,
                &P4InputVF::readProcessStdout);
    }

    proc->setWorkingDirectory(QDir::currentPath());

    processfailed_ = false;
    QString pa = "External Command: ";
    pa += getMapleExe();
    pa += " ";
    pa += filedotmpl;
    processText_->append(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);
    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processfailed_ = true;
        delete proc;
        proc = nullptr;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        g_p4app->signalCurveEvaluated(-1);
        terminateProcessButton_->setEnabled(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = filedotmpl;
        evalFile2_ = "";
        evaluatingIsoclines_ = true;

        return true;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclines
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating isoclines in plane/U1/U2 charts.
// This is only called in case of Poincare-compactification (weights p=q=1)
bool P4InputVF::prepareIsoclines(P4POLYNOM2 f, double y1, double y2,
                                 int precision, int numpoints)
{
    FILE *fp;
    int i;
    char buf[100];

    QString mainmaple;
    QString user_platform;
    QString user_file;
    QString filedotmpl;
    QByteArray ba_mainmaple;
    QByteArray ba_user_file;

    filedotmpl = getmaplefilename();

    fp = fopen(QFile::encodeName(filedotmpl), "w");
    if (fp == nullptr)
        return false;

    mainmaple = getP4MaplePath();
    mainmaple += QDir::separator();

    user_platform = USERPLATFORM;
    mainmaple += MAINMAPLEGCFFILE;

    ba_mainmaple = maplepathformat(mainmaple);
    user_file = getfilename_isoclines();
    removeFile(user_file);
    ba_user_file = maplepathformat(user_file);

    fprintf(fp, "restart;\n");
    fprintf(fp, "read( \"%s\" );\n", (const char *)ba_mainmaple);
    fprintf(fp, "user_file := \"%s\":\n", (const char *)ba_user_file);
    fprintf(fp, "user_numpoints := %d:\n", numpoints);
    fprintf(fp, "Digits := %d:\n", precision);
    fprintf(fp, "user_x1 := %g:\n", (float)(-1.0));
    fprintf(fp, "user_x2 := %g:\n", (float)1.0);
    fprintf(fp, "user_y1 := %g:\n", (float)y1);
    fprintf(fp, "user_y2 := %g:\n", (float)y2);
    fprintf(fp, "u := %s:\n", "x");
    fprintf(fp, "v := %s:\n", "y");
    fprintf(fp, "user_f := ");
    for (i = 0; f != nullptr; i++) {
        fprintf(fp, "%s",
                printterm2(buf, f, (i == 0) ? true : false, "x", "y"));
        f = f->next_term2;
    }
    if (i == 0)
        fprintf(fp, "0:\n");
    else
        fprintf(fp, ":\n");

    fprintf(fp,
            "try FindSingularities() finally: if returnvalue=0 then "
            "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);

    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclines_LyapunovCyl
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
bool P4InputVF::prepareIsoclines_LyapunovCyl(double theta1, double theta2,
                                             int precision, int numpoints,
                                             int index)
{
    FILE *fp;
    char buf[100];
    P4POLYNOM3 f;
    int i;

    for (int j = 0; j < 2; j++) {
        f = g_VFResults.vf_[index]->isocline_vector_.back().c;

        QString mainmaple;
        QString user_platform;
        QString user_file;
        QString filedotmpl;
        QByteArray ba_mainmaple;
        QByteArray ba_user_file;

        filedotmpl = getmaplefilename();

        fp = fopen(QFile::encodeName(filedotmpl), "w");
        if (fp == nullptr)
            return false;

        mainmaple = getP4MaplePath();
        mainmaple += QDir::separator();

        user_platform = USERPLATFORM;
        mainmaple += MAINMAPLEGCFFILE;

        ba_mainmaple = maplepathformat(mainmaple);
        user_file = getfilename_isoclines();
        removeFile(user_file);
        ba_user_file = maplepathformat(user_file);

        fprintf(fp, "restart;\n");
        fprintf(fp, "read( \"%s\" );\n", (const char *)ba_mainmaple);
        fprintf(fp, "user_file := \"%s\":\n", (const char *)ba_user_file);
        fprintf(fp, "user_numpoints := %d:\n", numpoints);
        fprintf(fp, "Digits := %d:\n", precision);
        fprintf(fp, "user_x1 := %g:\n", (float)0.0);
        fprintf(fp, "user_x2 := %g:\n", (float)1.0);
        fprintf(fp, "user_y1 := %g:\n", (float)theta1);
        fprintf(fp, "user_y2 := %g:\n", (float)theta2);
        fprintf(fp, "u := %s:\n", "cos(y)");
        fprintf(fp, "v := %s:\n", "sin(y)");
        fprintf(fp, "user_f := ");

        for (i = 0; f != nullptr; i++) {
            fprintf(fp, "%s",
                    printterm3(buf, f, (i == 0) ? true : false, "x", "U", "V"));
            f = f->next_term3;
        }
        if (i == 0)
            fprintf(fp, "0:\n");
        else
            fprintf(fp, ":\n");

        fprintf(fp,
                "try FindSingularities() finally: if returnvalue=0 then "
                "`quit`(0); else `quit(1)` end if: end try:\n");

        fclose(fp);
    }
    return true;
}

// -----------------------------------------------------------------------
//              PREPAREISOCLINES_LYAPUNOVR2
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating an isocline in charts near infinity.
// This is only called in case of Poincare-Lyapunov compactification
// (weights (p,q) !=(1,1))
//
// same as preparegcf, except for the "u := " and "v := " assignments,
// and the fact that one always refers to the same function g_VFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
bool P4InputVF::prepareIsoclines_LyapunovR2(int precision, int numpoints,
                                            int index)
{
    FILE *fp;
    char buf[100];
    P4POLYNOM2 f;
    int i;

    for (int j = 0; j < 2; j++) {
        f = g_VFResults.vf_[index]->isocline_vector_.back().r2;

        QString mainmaple;
        QString user_platform;
        QString user_file;
        QString filedotmpl;
        QByteArray ba_mainmaple;
        QByteArray ba_user_file;

        filedotmpl = getmaplefilename();

        fp = fopen(QFile::encodeName(filedotmpl), "w");
        if (fp == nullptr)
            return false;

        mainmaple = getP4MaplePath();
        mainmaple += QDir::separator();

        user_platform = USERPLATFORM;
        mainmaple = mainmaple.append(MAINMAPLEGCFFILE);

        ba_mainmaple = maplepathformat(mainmaple);
        user_file = getfilename_isoclines();
        removeFile(user_file);
        ba_user_file = maplepathformat(user_file);

        fprintf(fp, "restart;\n");
        fprintf(fp, "read( \"%s\" );\n", (const char *)ba_mainmaple);
        fprintf(fp, "user_file := \"%s\":\n", (const char *)ba_user_file);
        fprintf(fp, "user_numpoints := %d:\n", numpoints);
        fprintf(fp, "Digits := %d:\n", precision);
        fprintf(fp, "user_x1 := %g:\n", (float)0.0);
        fprintf(fp, "user_x2 := %g:\n", (float)1.0);
        fprintf(fp, "user_y1 := %g:\n", (float)0.0);
        fprintf(fp, "user_y2 := %g:\n", (float)TWOPI);
        fprintf(fp, "u := %s:\n", "x*cos(y)");
        fprintf(fp, "v := %s:\n", "x*sin(y)");
        fprintf(fp, "user_f := ");

        for (i = 0; f != nullptr; i++) {
            fprintf(fp, "%s",
                    printterm2(buf, f, (i == 0) ? true : false, "U", "V"));
            f = f->next_term2;
        }
        if (i == 0)
            fprintf(fp, "0:\n");
        else
            fprintf(fp, ":\n");

        fprintf(fp,
                "try FindSingularities() finally: if returnvalue=0 then "
                "`quit`(0); else `quit(1)` end if: end try:\n");

        fclose(fp);
    }
    return true;
}

// -----------------------------------------------------------------------
//              COMMON SETTINGS ROUTINES
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonString
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonString(std::vector<QString>)
{
    int i, j, j0;
    j0 = selected_[0];
    for (i = 1; i < numSelected_; i++) {
        j = selected_[i];
        if (lst[j0]->compare(*(lst[j])))
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonInt
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonInt(int *lst)
{
    int i, j, j0;
    j0 = selected_[0];
    for (i = 1; i < numSelected_; i++) {
        j = selected_[i];
        if (lst[j] != lst[j0])
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonBool
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonBool(bool *lst)
{
    int i, j, j0;
    j0 = selected_[0];
    for (i = 1; i < numSelected_; i++) {
        j = selected_[i];
        if (lst[j] != lst[j0])
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonParvalue
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonParvalue(int index)
{
    int i, j, j0;
    j0 = selected_[0];
    for (i = 1; i < numSelected_; i++) {
        j = selected_[i];
        if (parvalue_[j0][index]->compare(*(parvalue_[j][index])))
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::commonString
// -----------------------------------------------------------------------
QString P4InputVF::commonString(QString **lst) { return *(lst[selected_[0]]); }

// -----------------------------------------------------------------------
//          P4InputVF::commonInt
// -----------------------------------------------------------------------
int P4InputVF::commonInt(int *lst) { return lst[selected_[0]]; }

// -----------------------------------------------------------------------
//          P4InputVF::commonBoolean
// -----------------------------------------------------------------------
bool P4InputVF::commonBoolean(bool *lst) { return lst[selected_[0]]; }

// -----------------------------------------------------------------------
//          P4InputVF::commonParvalue
// -----------------------------------------------------------------------
QString P4InputVF::commonParvalue(int index)
{
    return *(parvalue_[selected_[0]][index]);
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonString
// -----------------------------------------------------------------------
void P4InputVF::setCommonString(QString **lst, QString val)
{
    int i, j;
    for (i = 0; i < numSelected_; i++) {
        j = selected_[i];
        *(lst[j]) = val;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonInt
// -----------------------------------------------------------------------
void P4InputVF::setCommonInt(int *lst, int val)
{
    int i, j;
    for (i = 0; i < numSelected_; i++) {
        j = selected_[i];
        lst[j] = val;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonBool
// -----------------------------------------------------------------------
void P4InputVF::setCommonBool(bool *lst, bool val)
{
    int i, j;
    for (i = 0; i < numSelected_; i++) {
        j = selected_[i];
        lst[j] = val;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonParvalue
// -----------------------------------------------------------------------
void P4InputVF::setCommonParvalue(int index, QString val)
{
    int i, j;
    for (i = 0; i < numSelected_; i++) {
        j = selected_[i];
        *(parvalue_[j][index]) = val;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::addVectorField
// -----------------------------------------------------------------------
void P4InputVF::addVectorField()
{
    int i;

    if (!VFResults.vf_.empty()) {
        VFResults.clearVFs();
        VFResults.vfK_ = nullptr;
        VFResults.K_ = 0;
    }

    // TODO: treure reallocs i implementar-ho com vectors
    // xdot = (QString **)realloc(xdot, sizeof(QString *) * (numVF_ + 1));
    // ydot = (QString **)realloc(ydot, sizeof(QString *) * (numVF_ + 1));
    // gcf = (QString **)realloc(gcf, sizeof(QString *) * (numVF_ + 1));
    // epsilon = (QString **)realloc(epsilon, sizeof(QString *) * (numVF_ + 1));
    // numeric = (bool *)realloc(numeric, sizeof(bool) * (numVF_ + 1));
    // precision = (int *)realloc(precision, sizeof(int) * (numVF_ + 1));
    // testsep = (bool *)realloc(testsep, sizeof(bool) * (numVF_ + 1));
    // taylorlevel = (int *)realloc(taylorlevel, sizeof(int) * (numVF_ + 1));
    // numericlevel = (int *)realloc(numericlevel, sizeof(int) * (numVF_ + 1));
    // maxlevel = (int *)realloc(maxlevel, sizeof(int) * (numVF_ + 1));
    // weakness = (int *)realloc(weakness, sizeof(int) * (numVF_ + 1));
    parvalue = (QString ***)realloc(parvalue, sizeof(QString *) * (numVF_ + 1));

    xdot_.push_back(QString(DEFAULTXDOT));
    ydot_.push_back(QString(DEFAULTYDOT));
    gcf_.push_back(QString(DEFAULTGCF));
    epsilon_.push_back(QString(DEFAULTEPSILON));
    numeric_.push_back(DEFAULTNUMERIC);
    testsep_.push_back(DEFAULTTESTSEP);
    precision_.push_back(DEFAULTPRECISION);
    taylorlevel_.push_back(DEFAULTLEVEL);
    numericlevel_.push_back(DEFAULTNUMLEVEL);
    maxlevel_.push_back(DEFAULTMAXLEVEL);
    weakness_.push_back(DEFAULTWEAKNESS);
    parvalue[numVF_] = (QString **)malloc(sizeof(QString *) * MAXNUMPARAMS);
    for (i = 0; i < MAXNUMPARAMS; i++) parvalue[numVF_][i] = new QString("");

    if (hasCommonString(xdot_))
        xdot[numVF_] = commonString(xdot);
    if (hasCommonString(ydot))
        *(ydot[numVF_]) = commonString(ydot);
    if (hasCommonString(gcf))
        *(gcf[numVF_]) = commonString(gcf);
    if (hasCommonString(epsilon))
        *(epsilon[numVF_]) = commonString(epsilon);
    if (hasCommonBool(numeric))
        numeric[numVF_] = commonBool(numeric);
    if (hasCommonBool(testsep))
        testsep[numVF_] = commonBool(testsep);
    if (hasCommonInt(precision))
        precision[numVF_] = commonInt(precision);
    if (hasCommonInt(taylorlevel))
        taylorlevel[numVF_] = commonInt(taylorlevel);
    if (hasCommonInt(numericlevel))
        numericlevel[numVF_] = commonInt(numericlevel);
    if (hasCommonInt(maxlevel))
        maxlevel[numVF_] = commonInt(maxlevel);
    if (hasCommonInt(weakness))
        weakness[numVF_] = commonInt(weakness);

    for (i = 0; i < MAXNUMPARAMS; i++)
        if (HasCommonParvalue(i))
            *(parvalue[numVF_][i]) = CommonParvalue(i);

    numvf++;
}

// -----------------------------------------------------------------------
//          P4InputVF::deleteVectorField
// -----------------------------------------------------------------------
void P4InputVF::deleteVectorField(int index)
{
    // first disconnect from other structures

    int k, i;

    if (!VFResults.vf_.empty()) {
        VFResults.clearVFs();
        VFResults.vfK_ = nullptr;
        VFResults.K_ = 0;
    }

    for (k = 0; k < numVFRegions_; k++) {
        if (vfRegions_[k].vfIndex == index) {
            free(vfRegions_[k].signs);
            if (k < numVFRegions_ - 1)
                memmove(vfRegions_ + k, vfRegions_ + k + 1,
                        sizeof(VFREGION) * (numVFRegions_ - k - 1));
            numVFRegions_--;
            k--;
            if (numVFRegions_ == 0) {
                free(vfRegions_);
                vfRegions_ = NULL;
            }
        } else if (vfRegions_[k].vfIndex > index)
            vfRegions_[k].vfIndex--;
    }
    for (k = 0; k < numSelected_; k++) {
        if (selected[k] == index) {
            if (k < numSelected_ - 1)
                memmove(selected + k, selected + k + 1,
                        sizeof(int) * (numSelected_ - k - 1));
            numSelected_--;
            k--;
            if (numSelected_ == 0) {
                numSelected_ = 1;
                selected[0] = index;
                if (selected[0] >= numvf - 1)
                    selected[0] = numvf - 2;
                if (selected[0] < 0)
                    selected[0] = 0;
                break;
            }
        } else if (selected[k] > index)
            selected[k]--;
    }

    if (numvf == 1) {
        // delete last one: replace by default startup values

        *(xdot[0]) = DEFAULTXDOT;
        *(ydot[0]) = DEFAULTYDOT;
        *(gcf[0]) = DEFAULTGCF;
        *(epsilon[0]) = DEFAULTEPSILON;
        *numeric = DEFAULTNUMERIC;
        *precision = DEFAULTPRECISION;
        *testsep = DEFAULTTESTSEP;
        *taylorlevel = DEFAULTLEVEL;
        *numericlevel = DEFAULTNUMLEVEL;
        *maxlevel = DEFAULTMAXLEVEL;
        *weakness = DEFAULTWEAKNESS;
        for (k = 0; k < MAXNUMPARAMS; k++) *(parvalue[0][k]) = "";

        for (k = 0; k < MAXNUMPARAMS; k++) parlabel[k] = "";

        numparams = 0;
        p = DEFAULTP;
        q = DEFAULTQ;
        symbolicpackage = PACKAGE_MAPLE;
        typeofstudy = DEFAULTTYPE;
        x0 = DEFAULTX0;
        y0 = DEFAULTY0;
        /*
                defaulttypeofview = TYPEOFVIEW_SPHERE;
                defaultxmin = X_MIN;
                defaultxmax = X_MAX;
                defaultymin = Y_MIN;
                defaultymax = Y_MAX;
        */
    } else {
        delete xdot[index];
        delete ydot[index];
        delete gcf[index];
        delete epsilon[index];
        for (k = 0; k < MAXNUMPARAMS; k++) delete parvalue[index][k];
        free(parvalue[index]);

        if (index < numvf - 1) {
            // do some memory moving

            memmove(xdot + index, xdot + index + 1,
                    sizeof(QString *) * (numvf - index - 1));
            memmove(ydot + index, ydot + index + 1,
                    sizeof(QString *) * (numvf - index - 1));
            memmove(gcf + index, gcf + index + 1,
                    sizeof(QString *) * (numvf - index - 1));
            memmove(epsilon + index, epsilon + index + 1,
                    sizeof(QString *) * (numvf - index - 1));
            memmove(numeric + index, numeric + index + 1,
                    sizeof(bool) * (numvf - index - 1));
            memmove(testsep + index, testsep + index + 1,
                    sizeof(bool) * (numvf - index - 1));
            memmove(precision + index, precision + index + 1,
                    sizeof(int) * (numvf - index - 1));
            memmove(taylorlevel + index, taylorlevel + index + 1,
                    sizeof(int) * (numvf - index - 1));
            memmove(numericlevel + index, numericlevel + index + 1,
                    sizeof(int) * (numvf - index - 1));
            memmove(maxlevel + index, maxlevel + index + 1,
                    sizeof(int) * (numvf - index - 1));
            memmove(weakness + index, weakness + index + 1,
                    sizeof(int) * (numvf - index - 1));
            memmove(parvalue + index, parvalue + index + 1,
                    sizeof(QString **) * (numvf - index - 1));
        }
        numvf--;
    }

    if (numvf == 1 && numvfregions == 0 && numcurves == 0) {
        numvfregions = 1;
        vfregions = (VFREGION *)malloc(sizeof(VFREGION));
        vfregions->vfindex = 0;
        vfregions->signs = NULL;
    }
}