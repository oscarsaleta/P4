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

#include "file_vf.h"

#include "file_paths.h"
#include "main.h"
#include "math_p4.h"
#include "math_polynom.h"
#include "p4application.h"
#include "p4settings.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

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

QInputVF *ThisVF = nullptr;

QInputVF::QInputVF() : outputWindow_(nullptr)
{
    processText_ = nullptr;
    terminateProcessButton_ = nullptr;
    clearProcessButton_ = nullptr;
    evalProcess_ = nullptr;
    reset();
}

QInputVF::~QInputVF()
{
    if (outputWindow_ != nullptr) {
        delete outputWindow_;
        outputWindow_ = nullptr;
    }
}

// -----------------------------------------------------------------------
//                  RESET
// -----------------------------------------------------------------------
void QInputVF::reset(void)
{
    filename_ = DEFAULTFILENAME;
    symbolicpackage_ = getMathPackage();
    typeofstudy_ = DEFAULTTYPE;
    numeric_ = DEFAULTNUMERIC;
    precision_ = DEFAULTPRECISION;
    precision0_ = DEFAULTPRECISION0;
    epsilon_ = DEFAULTEPSILON;
    testsep_ = DEFAULTTESTSEP;
    taylorlevel_ = DEFAULTLEVEL;
    numericlevel_ = DEFAULTNUMLEVEL;
    maxlevel_ = DEFAULTMAXLEVEL;
    weakness_ = DEFAULTWEAKNESS;
    x0_ = DEFAULTX0;
    y0_ = DEFAULTY0;
    p_ = DEFAULTP;
    q_ = DEFAULTQ;

    xdot_ = DEFAULTXDOT;
    ydot_ = DEFAULTYDOT;
    gcf_ = DEFAULTGCF;

    numparams_ = 0;
    for (int i = 0; i < MAXNUMPARAMS; i++) {
        parlabel_[i] = "";
        parvalue_[i] = "";
    }

    changed_ = false;
    evaluated_ = false;
    evaluating_ = false;
    cleared_ = true;
}

// -----------------------------------------------------------------------
//                  LOAD
// -----------------------------------------------------------------------
bool QInputVF::load(void)
{
    FILE *fp;
    QString fname;
    char scanbuf[2560];
    int i;
    int flag_numeric, flag_testsep;

    fname = getfilename();
    if (fname.length() == 0)
        return false;

    fp = fopen(QFile::encodeName(fname), "rt");
    if (fp == nullptr)
        return false;

    cleared_ = false;

    if (fscanf(fp, "%d\n", &typeofstudy_) != 1 ||
        fscanf(fp, "%d\n", &flag_numeric) != 1 ||
        fscanf(fp, "%d\n", &precision_) != 1 ||
        fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
        fscanf(fp, "%d\n", &flag_testsep) != 1 ||
        fscanf(fp, "%d\n", &taylorlevel_) != 1 ||
        fscanf(fp, "%d\n", &numericlevel_) != 1 ||
        fscanf(fp, "%d\n", &maxlevel_) != 1 ||
        fscanf(fp, "%d\n", &weakness_) != 1) {
        fclose(fp);
        return false;
    } else {
        numeric_ = ((flag_numeric == 0) ? false : true);
        epsilon_ = scanbuf;
        testsep_ = ((flag_testsep == 0) ? false : true);
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
        fclose(fp);
        return false;
    }
    xdot_ = scanbuf;
    if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
        fclose(fp);
        return false;
    }
    ydot_ = scanbuf;
    if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
        fclose(fp);
        return false;
    }
    gcf_ = scanbuf;

    if (xdot_ == "(null)")
        xdot_ = "";
    if (ydot_ == "(null)")
        ydot_ = "";
    if (gcf_ == "(null)")
        gcf_ = "";
    if (x0_ == "(null)")
        x0_ = "";
    if (y0_ == "(null)")
        y0_ = "";
    if (epsilon_ == "(null)")
        epsilon_ = "";

    if (fscanf(fp, "%d\n", &numparams_) != 1) {
        fclose(fp);
        return false;
    }
    for (i = 0; i < numparams_; i++) {
        int c;
        if (fscanf(fp, "%s", scanbuf) != 1) {
            fclose(fp);
            return false;
        }
        parlabel_[i] = scanbuf;
        while ((c = fgetc(fp)) == '\n' || c == 13)
            ;
        ungetc(c, fp);
        if (fscanf(fp, "%[^\r\n]\n", scanbuf) != 1) {
            fclose(fp);
            return false;
        }
        parvalue_[i] = scanbuf;
    }
    for (; i < MAXNUMPARAMS; i++) {
        parlabel_[i] = "";
        parvalue_[i] = "";
    }
    if (fscanf(fp, "%d\n", &precision0_) == 0)
        precision0_ = DEFAULTPRECISION0;

    fclose(fp);

    changed_ = false;
    evaluated_ = checkevaluated();
    return true;
}

// -----------------------------------------------------------------------
//                  CHECKEVALUATED
// -----------------------------------------------------------------------
bool QInputVF::checkevaluated(void)
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

    if (typeofstudy_ != TYPEOFSTUDY_INF) {
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

    if (typeofstudy_ == TYPEOFSTUDY_INF || typeofstudy_ == TYPEOFSTUDY_ALL) {
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

    return true;
}

// -----------------------------------------------------------------------
//                  SAVE
// -----------------------------------------------------------------------
bool QInputVF::save(void)
{
    FILE *fp;
    int i;
    QString fname;
    QByteArray s;
    fname = getfilename();
    if (fname.length() == 0)
        return false;

    fp = fopen(QFile::encodeName(fname), "wt");
    if (fp == nullptr)
        return false;

    fprintf(fp, "%d\n", typeofstudy_);
    fprintf(fp, "%d\n", numeric_);
    fprintf(fp, "%d\n", precision_);

    if (epsilon_.length() == 0)
        fprintf(fp, "%s\n", "(null)");
    else {
        s = epsilon_.toLatin1();
        fprintf(fp, "%s\n", (const char *)s);
    }

    fprintf(fp, "%d\n", testsep_);
    fprintf(fp, "%d\n", taylorlevel_);
    fprintf(fp, "%d\n", numericlevel_);
    fprintf(fp, "%d\n", maxlevel_);
    fprintf(fp, "%d\n", weakness_);

    if (typeofstudy_ == TYPEOFSTUDY_ONE) {
        if (x0_.length() == 0)
            fprintf(fp, "%s\n", "(null)");
        s = x0_.toLatin1();
        fprintf(fp, "%s\n", (const char *)s);
        if (y0_.length() == 0)
            fprintf(fp, "%s\n", "(null)");
        s = y0_.toLatin1();
        fprintf(fp, "%s\n", (const char *)s);
    } else {
        fprintf(fp, "%d\n", p_);
        fprintf(fp, "%d\n", q_);
    }

    if (xdot_.length() == 0)
        fprintf(fp, "%s\n", "(null)");
    else {
        s = xdot_.toLatin1();
        fprintf(fp, "%s\n", (const char *)s);
    }

    if (ydot_.length() == 0)
        fprintf(fp, "%s\n", "(null)");
    else {
        s = ydot_.toLatin1();
        fprintf(fp, "%s\n", (const char *)s);
    }

    if (gcf_.length() == 0)
        fprintf(fp, "%s\n", "(null)");
    else {
        s = gcf_.toLatin1();
        fprintf(fp, "%s\n", (const char *)s);
    }

    fprintf(fp, "%d\n", numparams_);
    for (i = 0; i < numparams_; i++) {
        if (parlabel_[i].length() == 0)
            fprintf(fp, "%s\n", "(null)");
        else {
            s = parlabel_[i].toLatin1();
            fprintf(fp, "%s\n", (const char *)s);
        }

        if (parvalue_[i].length() == 0)
            fprintf(fp, "%s\n", "(null)");
        else {
            s = parvalue_[i].toLatin1();
            fprintf(fp, "%s\n", (const char *)s);
        }
    }
    if (precision0_ != DEFAULTPRECISION0)
        fprintf(fp, "%d\n", precision0_);

    fclose(fp);
    changed_ = false;
    return true;
}

QString QInputVF::getbarefilename(void) const
{
    int slash1;
    int slash2;
    int slash3;
    int dot;
    QString fname;

    fname = filename_.trimmed();

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

QString QInputVF::getfilename(void) const
{
    int slash1;
    int slash2;
    int slash3;
    int dot;
    QString fname;

    fname = filename_.trimmed();

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
//                  GETFILENAME_xxx
// -----------------------------------------------------------------------
QString QInputVF::getfilename_finresults(void) const
{
    return getbarefilename().append("_fin.res");
}
QString QInputVF::getfilename_infresults(void) const
{
    return getbarefilename().append("_inf.res");
}
QString QInputVF::getfilename_fintable(void) const
{
    return getbarefilename().append("_fin.tab");
}
QString QInputVF::getfilename_inftable(void) const
{
    return getbarefilename().append("_inf.tab");
}
QString QInputVF::getfilename_vectable(void) const
{
    return getbarefilename().append("_vec.tab");
}
QString QInputVF::getfilename_curvetable(void) const
{
    return getbarefilename().append("_veccurve.tab");
}
QString QInputVF::getfilename_gcf(void) const
{
    return getbarefilename().append("_gcf.tab");
}
QString QInputVF::getfilename_curve(void) const
{
    return getbarefilename().append("_curve.tab");
}
QString QInputVF::getfilename_gcfresults(void) const
{
    return getbarefilename().append("_gcf.res");
} // only used in case of reduce: contains reduce output, no gcf data and is
  // deleted immediately.
QString QInputVF::getreducefilename(void) const
{
    return getbarefilename().append(".red");
}
QString QInputVF::getmaplefilename(void) const
{
    return getbarefilename().append(".txt");
}
QString QInputVF::getPrepareCurveFileName(void) const
{
    return getbarefilename().append("_curve_prep.txt");
}
QString QInputVF::getrunfilename(void) const
{
    return getbarefilename().append(".run");
}

// -----------------------------------------------------------------------
//                          FILEEXISTS
// -----------------------------------------------------------------------
bool QInputVF::fileExists(QString fname)
{
    QFile fp(fname);
    if (fp.exists())
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------
//                  PREPAREREDUCEPARAMETERS
// -----------------------------------------------------------------------
/*void QInputVF::prepareReduceParameters(QTextStream *fp)
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
//                  PREPAREMAPLEPARAMETERS
// -----------------------------------------------------------------------
void QInputVF::prepareMapleParameters(QTextStream *fp)
{
    QString s;

    *fp << "user_numeric:=" << booleanString(numeric_) << ":\n";
    *fp << "epsilon:=" << epsilon_ << ":\n";
    *fp << "test_sep:=" << booleanString(testsep_) << ":\n";
    s.sprintf("user_precision:=%d:\n", precision_);
    *fp << s;
    s.sprintf("user_precision0:=%d:\n", precision0_);
    *fp << s;
    s.sprintf("taylor_level:=%d:\n", taylorlevel_);
    *fp << s;
    s.sprintf("numeric_level:=%d:\n", numericlevel_);
    *fp << s;
    s.sprintf("max_level:=%d:\n", maxlevel_);
    *fp << s;
    s.sprintf("weakness_level:=%d:\n", weakness_);
    *fp << s;

    if (typeofstudy_ == TYPEOFSTUDY_ONE) {
        *fp << "user_p:=1:\n";
        *fp << "user_q:=1:\n";

        s = x0_.toLatin1();
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
        s.sprintf("user_p:=%d:\n", p_);
        *fp << s;
        s.sprintf("user_q:=%d:\n", q_);
        *fp << s;
    }
}

// -----------------------------------------------------------------------
//                      PREPAREREDUCEVECTORFIELD
// -----------------------------------------------------------------------
/*void QInputVF::prepareReduceVectorField(QTextStream *fp)
{
    QString myxdot;
    QString myydot;
    QString mygcf;
    QString lbl;
    QString val;
    int k;

    myxdot = convertReduceUserParameterLabels(xdot_);
    myydot = convertReduceUserParameterLabels(ydot_);
    mygcf = convertReduceUserParameterLabels(gcf_);

    *fp << "f:={" << myxdot << "," << myydot << "}$\n";
    *fp << "gcf:=" << mygcf << "$\n";

    for (k = 0; k < numparams_; k++) {
        lbl = convertReduceUserParameterLabels(parlabel_[k]);
        val = convertReduceUserParameterLabels(parvalue_[k]);

        if (lbl.length() == 0)
            continue;

        *fp << lbl << ":=" << val << "$\n";
    }
}*/

// -----------------------------------------------------------------------
//                      PREPAREMAPLEVECTORFIELD
// -----------------------------------------------------------------------
void QInputVF::prepareMapleVectorField(QTextStream *fp)
{
    QString myxdot;
    QString myydot;
    QString mygcf;
    QString lbl;
    QString val;
    int k;

    myxdot = convertMapleUserParameterLabels(xdot_);
    myydot = convertMapleUserParameterLabels(ydot_);
    mygcf = convertMapleUserParameterLabels(gcf_);

    *fp << "user_f := [ " << myxdot << ", " << myydot << " ]:\n";
    *fp << "user_gcf := " << mygcf << ":\n";

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
//                      PREPAREMAPLECURVE
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

// -----------------------------------------------------------------------
//                          CONVERTMAPLEUSERPARAMETERLABELS
// -----------------------------------------------------------------------
//
//  If the user gives a vector field with user-parameters such as "alpha" or
//  "b",
//  we add a suffix to these qualifier names and change them to "alpha_" or
//  "b_",
//  in order to avoid mixing with internal variables.
int indexOfWordInString(const QString *src, const QString *word, int start = 0)
{
    int i, j;

    while ((i = src->indexOf(*word, start)) != -1) {
        // we have found word as a substring.  The index i is an index from the
        // very beginning of string
        // (not depending of start)

        start = i + 1; // do not find this substring next loop

        // check if the substring is the beginning of a word:
        j = i;
        if (j > 0)
            if ((*src)[j - 1].isLetter() || (*src)[j - 1] == '_' ||
                (*src)[j - 1].isDigit())
                continue; // no: it is part of a bigger word, so continue...

        // check if the substring is the end of a word;
        j = i + word->length();
        if (j < src->length())
            if ((*src)[j].isLetter() || (*src)[j] == '_' || (*src)[j].isDigit())
                continue; // no: it is part of a bigger word, so continue...

        // ok: we have a word: stop looping.
        break;
    }
    return i;
}

QString QInputVF::convertMapleUserParameterLabels(QString src)
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

/*QString QInputVF::convertReduceUserParameterLabels(QString src)
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

// -----------------------------------------------------------------------
//                          BOOLEANSTRING
// -----------------------------------------------------------------------
//
// returns string representations "true" and "false" for booleans true and false
QString QInputVF::booleanString(int value) const
{
    if (value == 0) {
        /*if (symbolicpackage_ == PACKAGE_REDUCE)
            return "NIL";
        else*/
        return "false";
    } else {
        /*if (symbolicpackage_ == PACKAGE_REDUCE)
            return "'t";
        else*/
        return "true";
    }
}

#ifdef Q_OS_WIN
extern QByteArray Win_GetShortPathName(QByteArray f);
#endif

// -----------------------------------------------------------------------
//                          MAPLEPATHFORMAT
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
//                          PREPAREFILE
// -----------------------------------------------------------------------
void QInputVF::prepareFile(QTextStream *fp)
{
    QString bsaveall;
    QString name_vectab;
    QString name_inftab;
    QString name_fintab;
    QString name_infres;
    QString name_finres;
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

    user_exeprefix = "";

    /*if (symbolicpackage_ == PACKAGE_REDUCE) {
        bsaveall = booleanString(action_SaveAll);
        mainreduce = getP4ReducePath();
        mainreduce += "/";
        mainreduce += MAINREDUCEFILE;

        name_vectab = getfilename_vectable();
        name_fintab = getfilename_fintable();
        name_inftab = getfilename_inftable();
        name_finres = getfilename_finresults();
        name_infres = getfilename_infresults();

        removeFile(name_vectab);
        removeFile(name_fintab);
        removeFile(name_inftab);
        removeFile(name_infres);
        removeFile(name_finres);

        s.sprintf("all_crit_points:=%d$\n", typeofstudy_);
        s += "save_all:=" + bsaveall + "$\n";
        s += "vec_table:=\"" + name_vectab + "\"$\n";
        s += "finite_table:=\"" + name_fintab + "\"$\n";
        s += "finite_res:=\"" + name_finres + "\"$\n";
        s += "infinite_table:=\"" + name_inftab + "\"$\n";
        s += "infinite_res:=\"" + name_infres + "\"$\n";

        *fp << s;

        prepareReduceVectorField(fp);
        prepareReduceParameters(fp);

        s = "in \"" + mainreduce + "\"$\n";
        *fp << s;
    } else {*/
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
#ifdef Q_OS_WIN
    user_removecmd = "cmd /c del";
    user_exeprefix = "cmd /c ";
#else
    user_removecmd = "rm";
    user_exeprefix = "";
#endif
    mainmaple += MAINMAPLEFILE;
#ifdef Q_OS_WIN
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

    if (numeric_)
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

    removeFile(name_vectab);
    removeFile(name_fintab);
    removeFile(name_inftab);
    removeFile(name_infres);
    removeFile(name_finres);

    ba_name_vectab = maplepathformat(name_vectab);
    ba_name_fintab = maplepathformat(name_fintab);
    ba_name_inftab = maplepathformat(name_inftab);
    ba_name_finres = maplepathformat(name_finres);
    ba_name_infres = maplepathformat(name_infres);

    *fp << "all_crit_points := " << typeofstudy_ << ":\n";
    *fp << "save_all := " << bsaveall << ":\n";

    *fp << "vec_table := \"" << ba_name_vectab << "\":\n";
    *fp << "finite_table := \"" << ba_name_fintab << "\":\n";
    *fp << "finite_res := \"" << ba_name_finres << "\":\n";
    *fp << "infinite_table := \"" << ba_name_inftab << "\":\n";
    *fp << "infinite_res := \"" << ba_name_infres << "\":\n";

    prepareMapleVectorField(fp);
    prepareMapleParameters(fp);

    *fp << "try p4main() catch:\n"
           "printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
           "lastexception[2] );\n"
           "finally: closeallfiles();\n"
           "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
           "try:\n";
    /*}*/
}

// -----------------------------------------------------------------------
//                          PREPAREFILE CURVE
// -----------------------------------------------------------------------
void QInputVF::prepareCurveFile(QTextStream *fp)
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

    if (numeric_)
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
//                          EVALUATE
// -----------------------------------------------------------------------
void QInputVF::evaluate(void)
{
    /*QString filedotred;
    QString filedotrun;*/
    QString filedotmpl;
    QString s, e;
    /*QFile *fptr;
    QTextStream *fp;*/
    QProcess *proc;

    evaluatinggcf_ = false;
    evaluatingCurve_ = false;
    // possible clean up after last GCF evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepare();

    /*if (symbolicpackage_ == PACKAGE_REDUCE) {
        filedotred = getreducefilename();
        filedotrun = getrunfilename();

        fptr = new QFile(filedotrun);
        if (fptr->open(QIODevice::WriteOnly)) {
            fp = new QTextStream(fptr);
            *fp << "#!/bin/sh\n";
            *fp << getReduceExe() << " <" << filedotred << "\n";
            fp->flush();
            delete fp;
            fp = nullptr;
            fptr->close();
            delete fptr;
            fptr = nullptr;
        } else {
            delete fptr;
            fptr = nullptr;

            // cannot open???
        }

        if (processText_ == nullptr)
            createProcessWindow();
        else {
            processText_->append("\n\n------------------------------------------"
                                "-------------------------------------\n\n");
            terminateProcessButton_->setEnabled(true);
            clearProcessButton_->setEnabled(true);
        }

        proc = new QProcess(this);
        proc->setWorkingDirectory(QDir::currentPath());

        connect(proc, SIGNAL(finished(int)), p4app,
                SLOT(signalEvaluated(int)));
        connect(proc, SIGNAL(error(QProcess::ProcessError)), p4app,
                SLOT(catchProcessError(QProcess::ProcessError)));
        connect(proc, SIGNAL(readyReadStandardOutput()), this,
                SLOT(readProcessStdout()));

        processfailed_ = false;
        QString pa = "External Command: ";
        pa += "sh ";
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
            p4app->signalEvaluated(-1);
            terminateProcessButton_->setEnabled(false);
        } else {
            evalProcess_ = proc;
            evalFile_ = filedotred;
            evalFile2_ = filedotrun;
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

        /* Here a window for displaying the output text of the Maple process
         * is created */
        if (outputWindow_ == nullptr)
            createProcessWindow();
        else {
            processText_->append("\n\n--------------------------------------"
                                 "-----------------------------------------"
                                 "\n\n");
            terminateProcessButton_->setEnabled(true);
        }

        proc = new QProcess(this);

        proc->setWorkingDirectory(QDir::currentPath());

        connect(proc, SIGNAL(finished(int)), p4app, SLOT(signalEvaluated(int)));
        connect(proc, SIGNAL(errorOccurred(QProcess::ProcessError)), this,
                SLOT(catchProcessError(QProcess::ProcessError)));
        connect(proc, SIGNAL(readyReadStandardOutput()), this,
                SLOT(readProcessStdout()));

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
            p4app->signalEvaluated(-1);
            terminateProcessButton_->setEnabled(false);
        } else {
            evalProcess_ = proc;
            evalFile_ = filedotmpl;
            evalFile2_ = "";
        }
    }
    /*}*/
}

// -----------------------------------------------------------------------
//                          EVALUATE CURVE
// -----------------------------------------------------------------------
void QInputVF::evaluateCurveTable(void)
{
    QString filedotmpl;
    QString s, e;
    QProcess *proc;

    evaluatinggcf_ = false;
    evaluatingCurve_ = false;
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
            processText_->append("\n\n--------------------------------------"
                                 "-----------------------------------------"
                                 "\n\n");
            terminateProcessButton_->setEnabled(true);
        }

        proc = new QProcess(this);

        proc->setWorkingDirectory(QDir::currentPath());

        connect(proc, SIGNAL(error(QProcess::ProcessError)), p4app,
                SLOT(catchProcessError(QProcess::ProcessError)));
        connect(proc, SIGNAL(readyReadStandardOutput()), this,
                SLOT(readProcessStdout()));

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
            p4app->signalEvaluated(-1);
            terminateProcessButton_->setEnabled(false);
        } else {
            evalProcess_ = proc;
            evalFile_ = filedotmpl;
            evalFile2_ = "";
        }
    }
    //}
}

// -----------------------------------------------------------------------
//                          CATCHPROCESSERROR
// -----------------------------------------------------------------------
void QInputVF::catchProcessError(QProcess::ProcessError prerr)
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
//                          FINISHEVALUATION
// -----------------------------------------------------------------------
void QInputVF::finishEvaluation(int exitCode)
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

    //    if ( evalProcess_ != nullptr ) {
    if (processText_ != nullptr) {
        QString buf;
        buf = "\n--------------------------------------------------------------"
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
                buf = "The following error occured: " + processError_ + "\n";
            else
                buf = "";
        }
        processText_->append(buf);
    }
    //    }
    /*if (processText_ != nullptr) {
        //      processText_->hide();
        if (processText_->isActiveWindow()) {
            if (!evaluatinggcf_)
                p4startdlg->activateWindow();
            else {
                if (gcfDlg != nullptr)
                    gcfDlg->activateWindow();
            }
        }
    }*/

    if (evaluatinggcf_) {
        finishGcfEvaluation();
    }
    if (evaluatingCurve_) {
        finishCurveEvaluation();
    }
}

// -----------------------------------------------------------------------
//          FINISHGCFEVALUATION
// -----------------------------------------------------------------------
void QInputVF::finishGcfEvaluation(void)
{
    evaluatinggcf_ = false;
    if (gcfDlg_ != nullptr) {
        gcfDlg_->finishGcfEvaluation();

        //        QP4Event * e = new QP4Event(
        //        (QEvent::Type)TYPE_SIGNAL_EVALUATED, nullptr );
        //        p4app->postEvent( p4startdlg, e );
    }
}

void QInputVF::finishCurveEvaluation(void)
{
    evaluatingCurve_ = false;
    if (curveDlg_ != nullptr) {
        curveDlg_->finishCurveEvaluation();
    }
}

// -----------------------------------------------------------------------
//                          PREPARE
// -----------------------------------------------------------------------
void QInputVF::prepare(void)
{
    QString filedotred;
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
//                          PREPARE CURVE
// -----------------------------------------------------------------------
void QInputVF::prepareCurve(void)
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
//          READPROCESSSTDOUT
// -----------------------------------------------------------------------
void QInputVF::readProcessStdout(void)
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
                    t = t.mid(j + 1); // treat CR+LF as one lineend
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
//              ONCLEARBUTTON
// -----------------------------------------------------------------------
void QInputVF::onClearButton(void)
{
    if (processText_ != nullptr)
        processText_->clear();
}

// -----------------------------------------------------------------------
//              ONTERMINATEBUTTON
// -----------------------------------------------------------------------
void QInputVF::onTerminateButton(void)
{
    QString buf;
    if (evalProcess_ != nullptr) {
        if (evalProcess_->state() == QProcess::Running) {
            buf = "\n----------------------------------------------------------"
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
//          CREATEPROCESSWINDOW
// -----------------------------------------------------------------------
void QInputVF::createProcessWindow(void)
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
    if (p4smallicon!=nullptr)
        outputWindow_->setWindowIcon(*p4smallicon);

    QVBoxLayout *vLayout = new QVBoxLayout(outputWindow_);
    vLayout->setSpacing(3);
    vLayout->setContentsMargins(5, 5, 5, 5);
    QVBoxLayout *vLayout2 = new QVBoxLayout();
    vLayout2->setSpacing(3);

    processText_ = new QTextEdit(outputWindow_);
    processText_->setLineWrapMode(QTextEdit::FixedColumnWidth);
    processText_->setWordWrapMode(QTextOption::WrapAnywhere);
    processText_->setFont(*(p4app->courierFont_));
    processText_->setLineWrapColumnOrWidth(82);
    processText_->setReadOnly(true);
    processText_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vLayout2->addWidget(processText_);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(6);

    terminateProcessButton_ = new QPushButton("Terminate");
    terminateProcessButton_->setFont(*(p4app->boldFont_));
    terminateProcessButton_->setToolTip(
        "Terminates the process.  First tries to send a "
        "safe signal to the process.\nIf this does not "
        "work, then after 2 seconds, the program is "
        "abruptly terminated.");
    hLayout->addWidget(terminateProcessButton_);

    clearProcessButton_ = new QPushButton("Clear");
    clearProcessButton_->setFont((*p4app->boldFont_));
    clearProcessButton_->setToolTip("Clears this window");
    hLayout->addWidget(clearProcessButton_);

    vLayout2->addLayout(hLayout);
    vLayout->addLayout(vLayout2);

    outputWindow_->show();

    QObject::connect(terminateProcessButton_, SIGNAL(clicked()), this,
                     SLOT(onTerminateButton()));
    QObject::connect(clearProcessButton_, SIGNAL(clicked()), this,
                     SLOT(onClearButton()));
}

// -----------------------------------------------------------------------
//          EVALUATEGCF
// -----------------------------------------------------------------------
bool QInputVF::evaluateGcf(void)
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
            disconnect(proc, SIGNAL(finished(int)), p4app, 0);
            connect(proc, SIGNAL(finished(int)), p4app,
                    SLOT(signalGcfEvaluated(int)));
        } else {
            proc = new QProcess(this);
            connect(proc, SIGNAL(finished(int)), p4app,
                    SLOT(signalGcfEvaluated(int)));
            connect(proc, SIGNAL(error(QProcess::ProcessError)), p4app,
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
            p4app->signalGcfEvaluated(-1);
            terminateProcessButton_->setEnabled(false);
            return false;
        } else {
            evalProcess_ = proc;
            evalFile_ = filedotred;
            evalFile2_ = filedotrun;
            evaluatinggcf_ = true;
            return true;
        }
    } else {*/ // MAPLE MAPLE MAPLE
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
        processText_->append("\n\n------------------------------------------"
                             "-------------------------------------\n\n");
        terminateProcessButton_->setEnabled(true);
    }

    QProcess *proc;
    if (evalProcess_ != nullptr) { // re-use process of last GCF
        proc = evalProcess_;
        disconnect(proc, SIGNAL(finished(int)), p4app, 0);
        connect(proc, SIGNAL(finished(int)), p4app,
                SLOT(signalGcfEvaluated(int)));
    } else {
        proc = new QProcess(this);
        connect(proc, SIGNAL(finished(int)), p4app,
                SLOT(signalGcfEvaluated(int)));
        connect(proc, SIGNAL(error(QProcess::ProcessError)), p4app,
                SLOT(catchProcessError(QProcess::ProcessError)));
        connect(proc, SIGNAL(readyReadStandardOutput()), this,
                SLOT(readProcessStdout()));
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
        p4app->signalGcfEvaluated(-1);
        terminateProcessButton_->setEnabled(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = filedotmpl;
        evalFile2_ = "";
        evaluatinggcf_ = true;

        return true;
    }
    /*}*/
}

// -----------------------------------------------------------------------
//              PREPAREGCF
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in plane/U1/U2 charts.  This
// is only called in case of Poincare-compactification (weights p=q=1)

bool QInputVF::prepareGcf(struct term2 *f, double y1, double y2, int precision,
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

    fprintf(fp, "try FindSingularities() finally: if returnvalue=0 then "
                "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);
    /*}*/
    return true;
}

// -----------------------------------------------------------------------
//              PREPAREGCF_LYAPUNOVCYL
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))

bool QInputVF::prepareGcf_LyapunovCyl(double theta1, double theta2,
                                      int precision, int numpoints)
{
    FILE *fp;
    char buf[100];
    P4POLYNOM3 f;
    int i;

    f = VFResults.gcf_C_;

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

    fprintf(fp, "try FindSingularities() finally: if returnvalue=0 then "
                "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);
    /*}*/
    return true;
}

// -----------------------------------------------------------------------
//              PREPAREGCF_LYAPUNOVR2
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
//
// same as preparegcf, except for the "u := " and "v := " assignments,
// and the fact that one always refers to the same function VFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.

bool QInputVF::prepareGcf_LyapunovR2(int precision, int numpoints)
{
    FILE *fp;
    char buf[100];
    P4POLYNOM2 f;
    int i;

    f = VFResults.gcf_;

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

    fprintf(fp, "try FindSingularities() finally: if returnvalue=0 then "
                "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);
    /*}*/
    return true;
}

// -----------------------------------------------------------------------
//          EVALUATECURVE
// -----------------------------------------------------------------------
bool QInputVF::evaluateCurve(void)
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
        processText_->append("\n\n------------------------------------------"
                             "-------------------------------------\n\n");
        terminateProcessButton_->setEnabled(true);
    }

    QProcess *proc;
    if (evalProcess_ != nullptr) { // re-use process of last GCF
        proc = evalProcess_;
        disconnect(proc, SIGNAL(finished(int)), p4app, 0);
        connect(proc, SIGNAL(finished(int)), p4app,
                SLOT(signalCurveEvaluated(int)));
    } else {
        proc = new QProcess(this);
        connect(proc, SIGNAL(finished(int)), p4app,
                SLOT(signalCurveEvaluated(int)));
        connect(proc, SIGNAL(error(QProcess::ProcessError)), p4app,
                SLOT(catchProcessError(QProcess::ProcessError)));
        connect(proc, SIGNAL(readyReadStandardOutput()), this,
                SLOT(readProcessStdout()));
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
        p4app->signalCurveEvaluated(-1);
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
//              PREPARECURVE
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating curve in plane/U1/U2 charts.  This
// is only called in case of Poincare-compactification (weights p=q=1)
bool QInputVF::prepareCurve(struct term2 *f, double y1, double y2,
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

    fprintf(fp, "try FindSingularities() finally: if returnvalue=0 then "
                "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);

    return true;
}

// -----------------------------------------------------------------------
//              PREPARECURVE_LYAPUNOVCYL
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
bool QInputVF::prepareCurve_LyapunovCyl(double theta1, double theta2,
                                        int precision, int numpoints)
{
    FILE *fp;
    char buf[100];
    P4POLYNOM3 f;
    int i;

    f = VFResults.curve_C_;

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

    fprintf(fp, "try FindSingularities() finally: if returnvalue=0 then "
                "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);

    return true;
}

// -----------------------------------------------------------------------
//              PREPARECURVE_LYAPUNOVR2
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
//
// same as preparegcf, except for the "u := " and "v := " assignments,
// and the fact that one always refers to the same function VFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
bool QInputVF::prepareCurve_LyapunovR2(int precision, int numpoints)
{
    FILE *fp;
    char buf[100];
    P4POLYNOM2 f;
    int i;

    f = VFResults.curve_;

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

    fprintf(fp, "try FindSingularities() finally: if returnvalue=0 then "
                "`quit`(0); else `quit(1)` end if: end try:\n");

    fclose(fp);

    return true;
}