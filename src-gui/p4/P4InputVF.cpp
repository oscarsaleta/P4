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

#include "P4InputVF.hpp"

#include <memory>

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QMetaObject>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QTextEdit>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>

#include "P4Application.hpp"
#include "P4ArbitraryCurveDlg.hpp"
#include "P4Event.hpp"
#include "P4FindDlg.hpp"
#include "P4GcfDlg.hpp"
#include "P4IsoclinesDlg.hpp"
#include "P4ParentStudy.hpp"
#include "P4ProcessWnd.hpp"
#include "P4StartDlg.hpp"
#include "P4VFStudy.hpp"
#include "file_paths.hpp"
#include "main.hpp"
#include "math_charts.hpp"
#include "math_p4.hpp"
#include "math_polynom.hpp"
#include "math_regions.hpp"
#include "p4settings.hpp"

// NOTE: the code for reduce implementations can be found in older
// versions of P4, it is removed from 7.0.2 onwards

/*
    THIS FILE IMPLEMENTS READING AND LOADING THE VECTOR FIELD, THE PARAMETERS
   FOR REDUCE/MAPLE AND THE OVERALL CONFIGURATION.

    filename.inp        input vector field (read and written by P4)

    filename_inf.res    results of search at infinity, in text form
    filename_fin.res    results of search in the finite region, in text form

    IT DOES NOT IMPLEMENT THE READING OF THE RESULTS FROM MAPLE/REDUCE (THE .TAB
   FILES).
*/

/*
    Different files: search at the finite region, in text form

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
    // vfRegions_.emplace_back(0, std::vector<int>{});
    reset(1);
}

// -----------------------------------------------------------------------
//          DESTRUCTOR
// -----------------------------------------------------------------------
P4InputVF::~P4InputVF()
{
    // TODO moure això al final de l'avaluació de les corbes/isoclines?
    // remove curve auxiliary files
    removeFile(getfilename_arbitrarycurvetable());
    removeFile(getfilename_arbitrarycurve());
    removeFile(getPrepareArbitraryCurveFileName());
    // remove isoclines files too
    removeFile(getfilename_isoclinestable());
    removeFile(getfilename_isoclines());
    removeFile(getPrepareIsoclinesFileName());
}

// -----------------------------------------------------------------------
//          P4InputVF::reset
// -----------------------------------------------------------------------
void P4InputVF::reset(int n)
{
    int i;

    gVFResults.clearVFs();
    gVFResults.K_ = 0;

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

    parvalue_.clear();
    numParams_ = 0;
    for (i = 0; i < MAXNUMPARAMS; i++)
        parlabel_[i] = "";

    isoclines_.clear();

    // refill vectors with default values
    for (i = 0; i < n; i++) {
        numeric_.push_back(DEFAULTNUMERIC);
        precision_.push_back(DEFAULTPRECISION);
        precision0_ = DEFAULTPRECISION0;
        epsilon_.push_back(QString(DEFAULTEPSILON));
        testsep_.push_back(DEFAULTTESTSEP);
        taylorlevel_.push_back(DEFAULTLEVEL);
        numericlevel_.push_back(DEFAULTNUMLEVEL);
        maxlevel_.push_back(DEFAULTMAXLEVEL);
        weakness_.push_back(DEFAULTWEAKNESS);
        xdot_.push_back(QString(DEFAULTXDOT));
        ydot_.push_back(QString(DEFAULTYDOT));
        gcf_.push_back(QString(DEFAULTGCF));
        parvalue_.push_back(std::vector<QString>{});
        for (int j = 0; j < MAXNUMPARAMS; j++)
            parvalue_.back().push_back(QString{});
    }

    separatingCurves_.clear();
    numSeparatingCurves_ = 0;
    numPointsSeparatingCurve_.clear();
    gVFResults.resetSeparatingCurveInfo();

    curveRegions_.clear();
    numCurveRegions_ = 0;
    vfRegions_.clear();
    numVFRegions_ = 0;

    changed_ = false;
    evaluated_ = false;
    evaluating_ = false;
    cleared_ = true;
    evaluatingGcf_ = false;
    evaluatingArbitraryCurve_ = false;
    evaluatingIsoclines_ = false;
    evaluatingPiecewiseConfig_ = false;
    processFailed_ = false;

    if (n > 0) {
        numSelected_ = 1;
        selected_.push_back(0);

        if (n == 1) {
            vfRegions_.emplace_back(0, std::vector<int>{});
            numVFRegions_ = 1;
        }
    } else {
        numSelected_ = 0;
        selected_.clear();
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
    char scanbuf[2560];
    int c;
    int flag_numeric, flag_testsep, aux;

    QString fname{getfilename()};
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
            fscanf(fp, "%d\n", &flag_numeric) != 1 ||
            fscanf(fp, "%d\n", &aux) != 1 ||
            fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
            fscanf(fp, "%d\n", &flag_testsep) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            numeric_[0] = ((flag_numeric == 0) ? false : true);
            precision_[0] = aux;
            epsilon_[0] = scanbuf;
            testsep_[0] = ((flag_testsep == 0) ? false : true);
        }
        if (fscanf(fp, "%d\n", &taylorlevel_[0]) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        if (fscanf(fp, "%d\n", &numericlevel_[0]) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        if (fscanf(fp, "%d\n", &maxlevel_[0]) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        if (fscanf(fp, "%d\n", &weakness_[0]) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }

        if (typeofstudy_ == P4TypeOfStudy::typeofstudy_one) {
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
        xdot_[0] = scanbuf;
        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        ydot_[0] = scanbuf;
        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        gcf_[0] = scanbuf;

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

        if (fscanf(fp, "%d\n", &numParams_) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }

        std::vector<QString> auxvec;
        for (unsigned int i = 0; i < numParams_; i++) {
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
            auxvec.emplace_back(scanbuf);
        }
        parvalue_.clear();
        parvalue_.push_back(auxvec);

        /*for (i = numParams_; i < MAXNUMPARAMS; i++) {
            parlabel_[i] = QString{};
            parvalue_[0][i].push_back(QString{});
        }*/

        if (fscanf(fp, "%d\n", &precision0_) == 0)
            precision0_ = DEFAULTPRECISION0;
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
        if (_typeofstudy == P4TypeOfStudy::typeofstudy_one) {
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

        if (fscanf(fp, "%d\n", &n) != 1 || n <= 0) {
            reset(1);
            fclose(fp);
            return false;
        }

        reset(n);
        x0_ = _x0;
        y0_ = _y0;
        p_ = _p;
        q_ = _q;
        typeofstudy_ = _typeofstudy;

        gVFResults.resetSeparatingCurveInfo();

        if (fscanf(fp, "%u\n", &numSeparatingCurves_) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }

        if (numSeparatingCurves_ > 0) {
            int npoints;
            for (unsigned int k = 0; k < numSeparatingCurves_; k++) {
                if (fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
                    fscanf(fp, "%d\n", &npoints) != 1) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                separatingCurves_.emplace_back(scanbuf);
                numPointsSeparatingCurve_.push_back(std::move(npoints));
            }
        } else {
            separatingCurves_.clear();
            numPointsSeparatingCurve_.clear();
        }

        if (fscanf(fp, "%u\n", &numParams_) != 1 || numParams_ > MAXNUMPARAMS) {
            reset(1);
            fclose(fp);
            return false;
        }
        for (unsigned int i = 0; i < numParams_; i++) {
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            parlabel_[i] = QString{scanbuf};
        }
        for (unsigned int i = numParams_; i < MAXNUMPARAMS; i++)
            parlabel_[i] = QString{};

        if (fscanf(fp, "%u\n", &numVFRegions_) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        for (unsigned int i = 0; i < numVFRegions_; i++) {
            // read index
            int indx;
            if (fscanf(fp, "%d", &indx) != 1 || indx < 0 ||
                indx >= static_cast<int>(numVF_)) {
                reset(1);
                fclose(fp);
                return false;
            }
            // read signs
            std::vector<int> sgns;
            int aux;
            for (unsigned int k = 0; k < numSeparatingCurves_; k++) {
                if (fscanf(fp, "%d", &aux) != 1 || (aux != 1 && aux != -1)) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                sgns.push_back(std::move(aux));
            }
            // sgns could be an empty list
            vfRegions_.emplace_back(indx, std::move(sgns));
        }

        if (fscanf(fp, "%d\n", &numCurveRegions_) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        for (unsigned int i = 0; i < numCurveRegions_; i++) {
            // read index
            int indx;
            if (fscanf(fp, "%d", &indx) != 1 || indx < 0 ||
                indx >= static_cast<int>(numSeparatingCurves_)) {
                reset(1);
                fclose(fp);
                return false;
            }
            // read signs
            std::vector<int> sgns;
            int aux;
            for (unsigned int k = 0; k < numSeparatingCurves_; k++) {
                if (fscanf(fp, "%d", &aux) != 1 || aux > 1 || aux < -1) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                sgns.push_back(std::move(aux));
            }
            // sgns could be an empty list
            curveRegions_.emplace_back(indx, std::move(sgns));
        }

        for (unsigned int k = 0; k < numVF_; k++) {
            if (fscanf(fp, "%d\n", &flag_numeric) != 1 ||
                fscanf(fp, "%d\n", &aux) != 1 ||
                fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
                fscanf(fp, "%d\n", &flag_testsep) != 1) {
                reset(1);
                fclose(fp);
                return false;
            } else {
                bool value{(flag_numeric == 0) ? false : true};
                numeric_.push_back(std::move(value));
                precision_.push_back(std::move(aux));
                epsilon_.emplace_back(scanbuf);
                value = ((flag_testsep == 0) ? false : true);
                testsep_.push_back(std::move(value));
            }
            if (fscanf(fp, "%d\n", &aux) != 1) {
                reset(1);
                fclose(fp);
                return false;
            } else {
                taylorlevel_.push_back(std::move(aux));
            }
            if (fscanf(fp, "%d\n", &aux) != 1) {
                reset(1);
                fclose(fp);
                return false;
            } else {
                numericlevel_.push_back(std::move(aux));
            }
            if (fscanf(fp, "%d\n", &aux) != 1) {
                reset(1);
                fclose(fp);
                return false;
            } else {
                maxlevel_.push_back(std::move(aux));
            }
            if (fscanf(fp, "%d\n", &aux) != 1) {
                reset(1);
                fclose(fp);
                return false;
            } else {
                weakness_.push_back(std::move(aux));
            }

            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            xdot_.emplace_back(scanbuf);
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            ydot_.emplace_back(scanbuf);
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            gcf_.emplace_back(scanbuf);

            if (xdot_[k] == "(null)")
                xdot_[k] = "";
            if (ydot_[k] == "(null)")
                ydot_[k] = "";
            if (gcf_[k] == "(null)")
                gcf_[k] = "";
            if (epsilon_[k] == "(null)")
                epsilon_[k] = "";

            std::vector<QString> auxvec;
            for (unsigned int i = 0; i < numParams_; i++) {
                if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                auxvec.emplace_back(scanbuf);
            }
            parvalue_.push_back(auxvec);
            /*for (i = numParams_; i < MAXNUMPARAMS; i++)
                parvalue_[k].push_back(QString{});*/
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

    auto fi = std::make_unique<QFileInfo>(getfilename());
    if (!fi->exists())
        return false;
    auto dt = fi->lastModified();

    auto fivec = std::make_unique<QFileInfo>(getbarefilename() + "_vec.tab");
    if (!fivec->exists())
        return false;
    auto dtvec = fivec->lastModified();
    if (dtvec.secsTo(dt) > 0 || dtvec.daysTo(dt) > 0)
        return false;

    if (typeofstudy_ != P4TypeOfStudy::typeofstudy_inf) {
        auto fifin =
            std::make_unique<QFileInfo>(getbarefilename() + "_fin.tab");
        if (!fifin->exists())
            return false;
        auto dtfin = fifin->lastModified();
        if (dtfin.secsTo(dt) > 0 || dtfin.daysTo(dt) > 0)
            return false;
    }

    if (typeofstudy_ == P4TypeOfStudy::typeofstudy_inf ||
        typeofstudy_ == P4TypeOfStudy::typeofstudy_all) {
        auto fiinf =
            std::make_unique<QFileInfo>(getbarefilename() + "_inf.tab");
        if (!fiinf->exists())
            return false;
        auto dtinf = fiinf->lastModified();
        if (dtinf.secsTo(dt) > 0 || dtinf.daysTo(dt) > 0)
            return false;
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::save
// -----------------------------------------------------------------------
bool P4InputVF::save()
{
    QSettings settings{getbarefilename().append(".conf"),
                       QSettings::NativeFormat};
    settings.clear();
    emit saveSignal();

    auto fname = getfilename();
    if (fname.isEmpty())
        return false;

    QFile file{QFile::encodeName(getfilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out{&file};

        out << "P5\n";
        out << typeofstudy_ << "\n";
        if (typeofstudy_ == P4TypeOfStudy::typeofstudy_one) {
            if (x0_.isEmpty())
                out << "(null)\n";
            else
                out << x0_ << "\n";
            if (y0_.isEmpty())
                out << "(null)\n";
            else
                out << y0_ << "\n";
        } else {
            out << p_ << "\n";
            out << q_ << "\n";
        }

        out << numVF_ << "\n";
        out << numSeparatingCurves_ << "\n";
        for (unsigned int i = 0; i < numSeparatingCurves_; i++) {
            if (separatingCurves_[i].isEmpty())
                out << "(null)\n";
            else
                out << separatingCurves_[i] << "\n";
            out << numPointsSeparatingCurve_[i] << "\n";
        }

        out << numParams_ << "\n";
        for (unsigned int i = 0; i < numParams_; i++) {
            if (parlabel_[i].isEmpty())
                out << "(null)\n";
            else
                out << parlabel_[i] << "\n";
        }

        out << numVFRegions_ << "\n";
        for (unsigned int i = 0; i < numVFRegions_; i++) {
            out << vfRegions_[i].vfIndex;
            for (unsigned int k = 0; k < numSeparatingCurves_; k++)
                out << vfRegions_[i].signs[k];
            out << "\n";
        }

        out << numCurveRegions_ << "\n";
        for (unsigned int i = 0; i < numCurveRegions_; i++) {
            out << curveRegions_[i].curveIndex;
            for (unsigned int k = 0; k < numSeparatingCurves_; k++)
                out << curveRegions_[i].signs[k];
            out << "\n";
        }

        for (unsigned int i = 0; i < numVF_; i++) {
            out << numeric_[i] << "\n";
            out << precision_[i] << "\n";
            if (epsilon_[i].isEmpty())
                out << "(null)\n";
            else
                out << epsilon_[i] << "\n";
            out << testsep_[i] << "\n";
            out << taylorlevel_[i] << "\n";
            out << numericlevel_[i] << "\n";
            out << maxlevel_[i] << "\n";
            out << weakness_[i] << "\n";
            if (xdot_[i].isEmpty())
                out << "(null)\n";
            else
                out << xdot_[i] << "\n";
            if (ydot_[i].isEmpty())
                out << "(null)\n";
            else
                out << ydot_[i] << "\n";
            if (gcf_[i].isEmpty())
                out << "(null)\n";
            else
                out << gcf_[i] << "\n";

            for (unsigned int k = 0; k < numParams_; k++) {
                if (parvalue_[i][k].isEmpty())
                    out << "(null)\n";
                else
                    out << parvalue_[i][k] << "\n";
            }
        }
        if (precision0_ != DEFAULTPRECISION0)
            out << precision0_ << "\n";

        out.flush();
        file.close();
        changed_ = false;
        return true;
    }
    return false;
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
    QString fname{filename_.trimmed()};

    if (fname.isEmpty())
        return "";

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
    QString fname{filename_.trimmed()};

    if (fname.isEmpty())
        return "";

    slash1 = fname.lastIndexOf('/');
    slash2 = fname.lastIndexOf('\\');
    slash3 = fname.lastIndexOf(':');
    dot = fname.lastIndexOf('.');

    if (dot <= slash1 && dot <= slash2 && dot <= slash3) {
        // there is no dot present, or at least not in the
        // last part of the file name.

        return fname.append(".inp");
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

QString P4InputVF::getmaplefilename() const
{
    return getbarefilename().append(".txt");
}

// curve filenames
QString P4InputVF::getfilename_arbitrarycurvetable() const
{
    return getbarefilename().append("_veccurve.tab");
}

QString P4InputVF::getfilename_arbitrarycurve() const
{
    return getbarefilename().append("_curve.tab");
}

QString P4InputVF::getPrepareArbitraryCurveFileName() const
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

// separating curve filenames
QString P4InputVF::getfilename_separatingcurveresults() const
{
    return getbarefilename().append("_sepcurves.tab");
}

// -----------------------------------------------------------------------
//          P4InputVF::fileExists
// -----------------------------------------------------------------------
bool P4InputVF::fileExists(QString fname)
{
    QFile file{QFile::encodeName(std::move(fname))};
    return file.exists();
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleParameters
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleParameters(QTextStream &fp, bool forArbitraryCurves)
{
    QString s;

    if (!forArbitraryCurves) {
        fp << "user_numeric_pieces := [ ";
        for (unsigned int i = 0; i < numVF_; i++) {
            fp << booleanString(numeric_[i]);
            if (i == numVF_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }

        fp << "epsilon_pieces := [ ";
        for (unsigned int i = 0; i < numVF_; i++) {
            fp << epsilon_[i];
            if (i == numVF_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }

        fp << "test_sep_pieces := [ ";
        for (unsigned int i = 0; i < numVF_; i++) {
            fp << booleanString(testsep_[i]);
            if (i == numVF_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }

        fp << "user_precision_pieces := [ ";
        for (unsigned int i = 0; i < numVF_; i++) {
            fp << precision_[i];
            if (i == numVF_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }

        fp << "taylor_level_pieces := [ ";
        for (unsigned int i = 0; i < numVF_; i++) {
            fp << taylorlevel_[i];
            if (i == numVF_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }

        fp << "numeric_level_pieces := [ ";
        for (unsigned int i = 0; i < numVF_; i++) {
            fp << numericlevel_[i];
            if (i == numVF_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }

        fp << "max_level_pieces := [ ";
        for (unsigned int i = 0; i < numVF_; i++) {
            fp << maxlevel_[i];
            if (i == numVF_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }

        fp << "weakness_level_pieces := [ ";
        for (unsigned int i = 0; i < numVF_; i++) {
            fp << weakness_[i];
            if (i == numVF_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }

        fp << "user_precision0 := " << precision0_ << ":\n";
    } else { // for curves: use only first VF for parameters
        fp << "user_numeric := " << booleanString(numeric_[0]) << ":\n";
        fp << "epsilon := " << epsilon_[0] << ":\n";
        fp << "test_sep := " << booleanString(testsep_[0]) << ":\n";
        s.sprintf("user_precision := %d:\n", precision_[0]);
        fp << s;
        s.sprintf("user_precision0 := %d:\n", precision0_);
        fp << s;
        s.sprintf("taylor_level := %d:\n", taylorlevel_[0]);
        fp << s;
        s.sprintf("numeric_level := %d:\n", numericlevel_[0]);
        fp << s;
        s.sprintf("max_level := %d:\n", maxlevel_[0]);
        fp << s;
        s.sprintf("weakness_level := %d:\n", weakness_[0]);
        fp << s;
    }

    if (typeofstudy_ == P4TypeOfStudy::typeofstudy_one) {
        fp << "user_p := 1:\n";
        fp << "user_q := 1:\n";

        fp << "x0 := " << x0_ << ":\n";
        fp << "y0 := " << y0_ << ":\n";
        s.sprintf("x_min := x0+(%8.5g):\n", static_cast<double>(X_MIN));
        fp << s;
        s.sprintf("x_max := x0+(%8.5g):\n", static_cast<double>(X_MAX));
        fp << s;
        s.sprintf("y_min := y0+(%8.5g):\n", static_cast<double>(Y_MIN));
        fp << s;
        s.sprintf("y_max := y0+(%8.5g):\n", static_cast<double>(Y_MAX));
        fp << s;
    } else {
        s.sprintf("user_p := %d:\n", p_);
        fp << s;
        s.sprintf("user_q := %d:\n", q_);
        fp << s;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleVectorField
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleVectorField(QTextStream &fp)
{
    QString myxdot, myydot, mygcf, lbl, val;

    fp << "user_f_pieces := [ ";
    for (unsigned int i = 0; i < numVF_; i++) {
        myxdot = convertMapleUserParameterLabels(xdot_[i]);
        myydot = convertMapleUserParameterLabels(ydot_[i]);
        fp << "[" << myxdot << "," << myydot << "]";
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "user_gcf_pieces := [ ";
    for (unsigned int i = 0; i < numVF_; i++) {
        mygcf = convertMapleUserParameterLabels(gcf_[i]);
        fp << mygcf;
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "user_parameters := [ ";
    for (unsigned int k = 0; k < numParams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);
        fp << lbl;
        if (k == numParams_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }
    if (numParams_ == 0)
        fp << " ]:\n";

    for (unsigned int k = 0; k < numParams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);
        if (lbl.length() == 0)
            continue;
        fp << lbl << "_pieces := [ ";
        for (unsigned int i = 0; i < numVF_; i++) {
            val = convertMapleUserParameterLabels(parvalue_[i][k]);
            if (!numeric_[i])
                fp << val;
            else
                fp << "evalf(" << val << ")";
            if (i == numVF_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleSeparatingCurves
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleSeparatingCurves(QTextStream &fp)
{
    QString s;

    fp << "user_curves := [ ";
    if (numSeparatingCurves_ > 0)
        for (unsigned int i = 0; i < numSeparatingCurves_; i++) {
            s = convertMapleUserParameterLabels(separatingCurves_[i]);
            fp << s;
            if (i == numSeparatingCurves_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }
    else
        fp << "]:\n";

    fp << "user_numpointscurves :=[ ";
    if (numSeparatingCurves_ > 0)
        for (unsigned int i = 0; i < numSeparatingCurves_; i++) {
            fp << numPointsSeparatingCurve_[i];
            if (i == numSeparatingCurves_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }
    else
        fp << "]:\n";
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleArbitraryCurve
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleArbitraryCurve(QTextStream &fp)
{
    QString mycurve{convertMapleUserParameterLabels(arbitraryCurve_)};
    fp << "user_curve := " << mycurve << ":\n";

    QString lbl, val;
    for (unsigned int k = 0; k < numParams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);
        // FIXME: s'ha d'iterar per tots els parvalues?
        val = convertMapleUserParameterLabels(parvalue_[0][k]);

        if (lbl.length() == 0)
            continue;

        if (!commonBool(numeric_)) {
            fp << lbl << " := " << val << ":\n";
        } else {
            fp << lbl << " := evalf( " << val << " ):\n";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleisoclines
// -----------------------------------------------------------------------
// FIXME: ha de ser una llista. mirar el p5 a veure com son els altres
// parametres a la gui
void P4InputVF::prepareMapleIsoclines(QTextStream &fp)
{
    QString myisoclines;
    QString lbl;
    QString val;

    fp << "user_isoclines := [";
    for (auto s : isoclines_) {
        myisoclines = convertMapleUserParameterLabels(s);
        fp << s; // FIXME
    }

    for (unsigned int k = 0; k < numParams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);
        // FIXME: en aquest cas segur q està malament perquè hem de fer-ho amb
        // cada VF per separat (cada VF té les seves isoclines)
        val = convertMapleUserParameterLabels(parvalue_[0][k]);

        if (lbl.length() == 0)
            continue;

        if (!numeric_[0]) {
            fp << lbl << " := " << val << ":\n";
        } else {
            fp << lbl << " := evalf( " << val << " ):\n";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMaplePiecewiseConfig
// -----------------------------------------------------------------------
void P4InputVF::prepareMaplePiecewiseConfig(QTextStream &fp)
{
    fp << "vfregions := [ ";
    if (numVFRegions_ == 0)
        fp << "]:\n";
    else {
        for (unsigned int i = 0; i < numVFRegions_; i++) {
            fp << "[" << vfRegions_[i].vfIndex << ", [ ";
            for (unsigned int k = 0; k < numSeparatingCurves_; k++) {
                fp << vfRegions_[i].signs[k];
                if (k < numSeparatingCurves_ - 1)
                    fp << ",";
            }
            if (i == numVFRegions_ - 1)
                fp << "] ] ]:\n";
            else
                fp << "] ],\n    ";
        }
    }

    fp << "curveregions := [ ";
    if (numCurveRegions_ == 0)
        fp << "]:\n";
    else {
        for (unsigned int i = 0; i < numCurveRegions_; i++) {
            fp << "[" << curveRegions_[i].curveIndex << ", [ ";
            for (unsigned int k = 0; k < numSeparatingCurves_; k++) {
                fp << curveRegions_[i].signs[k];
                if (k < numSeparatingCurves_ - 1)
                    fp << ",";
            }
            if (i == numCurveRegions_ - 1)
                fp << "] ] ]:\n";
            else
                fp << "] ],\n    ";
        }
    }
}

// -----------------------------------------------------------------------
//          indexOfWordInString
// -----------------------------------------------------------------------
//  If the user gives a vector field with user-parameters such as "alpha" or
//  "b", we add a suffix to these qualifier names and change them to "alpha_" or
//  "b_", in order to avoid mixing with internal variables.
static int indexOfWordInString(const QString &src, const QString &word,
                               int start = 0)
{
    int i, j;

    while ((i = src.indexOf(word, start)) != -1) {
        // we have found word as a substring.  The index i is an index from the
        // very beginning of string (not depending of start)

        start = i + 1; // do not find this substring next loop

        // check if the substring is the beginning of a word:
        j = i;
        if (j > 0)
            if (src[j - 1].isLetter() || src[j - 1] == '_' ||
                src[j - 1].isDigit())
                continue; // no: it is part of a bigger word, so continue...

        // check if the substring is the end of a word;
        j = i + word.length();
        if (j < src.length())
            if (src[j].isLetter() || src[j] == '_' || src[j].isDigit())
                continue; // no: it is part of a bigger word, so continue...

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
    QString s{std::move(src)};
    QString t;
    QString p, newlabel;
    int i;

    for (unsigned int k = 0; k < numParams_; k++) {
        p = parlabel_[k];
        if (p.length() == 0)
            continue;

        newlabel = p + "_";

        t = "";
        while (1) {
            i = indexOfWordInString(s, p);
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
    int i;
    QString t, p, newlabel;
    QString s{std::move(src)};
    for (unsigned int k = 0; k < numParams_; k++) {
        p = parlabel_[k];
        if (p.length() == 0)
            continue;
        // FIXME: ficar un index com parametre per indicar quin VF fem servir?
        newlabel = parvalue_[0][k]; // this is the difference: we take the value
        t = "";
        while (1) {
            i = indexOfWordInString(s, p);
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

#ifdef Q_OS_WIN
extern QByteArray win_GetShortPathName(QByteArray f);
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

    ba_fname = win_GetShortPathName(ba_fname);
#endif
    ba_fname.replace("\\", "\\\\");
    return ba_fname;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareFile
// -----------------------------------------------------------------------

void P4InputVF::prepareFile(QTextStream &fp, bool prepareforcurves)
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

    fp << "restart;\n";
    fp << "read( \"" << ba_mainmaple << "\" );\n";
    fp << "user_bindir := \"" << ba_user_bindir << "\":\n";
    fp << "user_tmpdir := \"" << ba_user_tmpdir << "\":\n";
    fp << "user_lypexe := \"" << user_lypexe << "\":\n";
    fp << "user_lypexe_mpf := \"" << user_lypexe_mpf << "\":\n";
    fp << "user_sepexe := \"" << user_sepexe << "\":\n";
    fp << "user_exeprefix := \"" << user_exeprefix << "\":\n";
    fp << "user_platform := \"" << user_platform << "\":\n";
    fp << "user_sumtablepath := \"" << ba_user_sumtablepath << "\":\n";
    fp << "user_removecmd := \"" << user_removecmd << "\":\n";
    fp << "user_simplify := " << user_simplify << ":\n";
    fp << "user_simplifycmd := " << user_simplifycmd << ":\n";

    bsaveall = booleanString(gActionSaveAll);

    name_vectab = getfilename_vectable();
    name_fintab = getfilename_fintable();
    name_inftab = getfilename_inftable();
    name_finres = getfilename_finresults();
    name_infres = getfilename_infresults();
    name_curves = getfilename_separatingcurveresults();

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

    fp << "all_crit_points := " << typeofstudy_ << ":\n";
    fp << "save_all := " << bsaveall << ":\n";

    fp << "vec_table := \"" << ba_name_vectab << "\":\n";
    fp << "finite_table := \"" << ba_name_fintab << "\":\n";
    fp << "finite_res := \"" << ba_name_finres << "\":\n";
    fp << "infinite_table := \"" << ba_name_inftab << "\":\n";
    fp << "infinite_res := \"" << ba_name_infres << "\":\n";
    fp << "user_curvesfile := \"" << ba_name_curves << "\":\n";

    prepareMapleVectorField(fp);
    prepareMapleParameters(fp);
    prepareMapleSeparatingCurves(fp);
    prepareMaplePiecewiseConfig(fp);

    if (prepareforcurves) {
        fp << "user_precision := 8:\n"
              "try findAllSeparatingCurves() catch:\n"
              "  printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
              "lastexception[2] );\n"
              "finally:\n"
              "  closeallfiles();\n"
              "  if normalexit=0 then\n"
              "    `quit`(0);\n"
              "  else\n"
              "    `quit(1)`\n"
              "  end if:\n"
              "end try:\n";
    } else {
        fp << "try p4main() catch:\n"
              "  printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
              "lastexception[2] );\n"
              "finally:\n"
              "  closeallfiles();\n"
              "  if normalexit=0 then\n"
              "    `quit`(0);\n"
              "  else\n"
              "    `quit(1)`\n"
              "  end if:\n"
              "end try:\n";
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareArbitraryCurveFile
// -----------------------------------------------------------------------
void P4InputVF::prepareArbitraryCurveFile(QTextStream &fp)
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

    fp << "restart;\n";
    fp << "read( \"" << ba_mainmaple << "\" );\n";
    fp << "user_bindir := \"" << ba_user_bindir << "\":\n";
    fp << "user_tmpdir := \"" << ba_user_tmpdir << "\":\n";
    fp << "user_exeprefix := \"" << user_exeprefix << "\":\n";
    fp << "user_platform := \"" << user_platform << "\":\n";
    fp << "user_removecmd := \"" << user_removecmd << "\":\n";
    fp << "user_simplify := " << user_simplify << ":\n";
    fp << "user_simplifycmd := " << user_simplifycmd << ":\n";
    fp << "all_crit_points := " << typeofstudy_ << ":\n";

    name_curvetab = getfilename_arbitrarycurvetable();
    removeFile(name_curvetab);
    ba_name_curvetab = maplepathformat(name_curvetab);
    fp << "curve_table := \"" << ba_name_curvetab << "\":\n";

    prepareMapleArbitraryCurve(fp);
    // true because it is for arbitrary curve
    prepareMapleParameters(fp, true);

    fp << "try prepareArbitraryCurve() catch:\n"
          "printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
          "lastexception[2] );\n"
          "finally: closeallfiles();\n"
          "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
          "try:\n";
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclinesFile
// -----------------------------------------------------------------------
void P4InputVF::prepareIsoclinesFile(QTextStream &fp)
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

    fp << "restart;\n";
    fp << "read( \"" << ba_mainmaple << "\" );\n";
    fp << "user_bindir := \"" << ba_user_bindir << "\":\n";
    fp << "user_tmpdir := \"" << ba_user_tmpdir << "\":\n";
    fp << "user_exeprefix := \"" << user_exeprefix << "\":\n";
    fp << "user_platform := \"" << user_platform << "\":\n";
    fp << "user_removecmd := \"" << user_removecmd << "\":\n";
    fp << "user_simplify := " << user_simplify << ":\n";
    fp << "user_simplifycmd := " << user_simplifycmd << ":\n";
    fp << "all_crit_points := " << typeofstudy_ << ":\n";

    name_isoclinestab = getfilename_isoclinestable();
    removeFile(name_isoclinestab);
    ba_name_isoclinestab = maplepathformat(name_isoclinestab);
    fp << "isoclines_table := \"" << ba_name_isoclinestab << "\":\n";

    prepareMapleIsoclines(fp);
    prepareMapleParameters(fp);

    fp << "try prepareIsoclines() catch:\n"
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
    QString filedotmpl;
    QString s, e;

    // possible clean up after last GCF evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepare();

    filedotmpl = getmaplefilename();

    s = getMapleExe();
    if (s.isEmpty())
        s = "";
    else
        s = s.append(" \"").append(filedotmpl).append("\"");

    if (outputWindow_ == nullptr)
        createProcessWindow();
    else {
        outputWindow_->appendText(
            "\n\n--------------------------------------------------------------"
            "-----------------\n\n");
        outputWindow_->enableTerminateProcessButton(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    auto proc = new QProcess{this};
    proc->setWorkingDirectory(QDir::currentPath());

    evalProcessFinishedConnection_ =
        new QMetaObject::Connection{QObject::connect(
            proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
            gP4app, &P4Application::signalEvaluated)};
    QObject::connect(proc, &QProcess::readyReadStandardOutput, this,
                     &P4InputVF::readProcessStdout);
#ifdef QT_QPROCESS_OLD
    QObject::connect(proc,
                     static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                         &QProcess::error),
                     this, &P4InputVF::catchProcessError);
#else
    QObject::connect(proc, &QProcess::errorOccurred, this,
                     &P4InputVF::catchProcessError);
#endif

    processFailed_ = false;
    QString pa = "External Command: " + getMapleExe() + " " + filedotmpl;
    outputWindow_->appendText(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);

    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processFailed_ = true;
        delete proc;
        proc = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        gP4app->signalEvaluated(-1);
        outputWindow_->enableTerminateProcessButton(false);
    } else {
        evalProcess_ = proc;
        evalFile_ = std::move(filedotmpl);
        evalFile2_ = "";
        evaluating_ = true;
        evaluatingGcf_ = false;
        evaluatingPiecewiseConfig_ = false;
        evaluatingArbitraryCurve_ = false;
        evaluatingIsoclines_ = false;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateArbitraryCurveTable
// -----------------------------------------------------------------------
void P4InputVF::evaluateArbitraryCurveTable()
{
    // possible clean up after last evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepareArbitraryCurve();
    auto filedotmpl = getPrepareArbitraryCurveFileName();

    auto s = getMapleExe();
    if (s.isEmpty())
        s = "";
    else
        s = s.append(" \"").append(filedotmpl).append("\"");

    /* Here a window for displaying the output text of the Maple process
     * is created */
    if (outputWindow_ == nullptr)
        createProcessWindow();
    else {
        outputWindow_->appendText(
            "\n\n-----------------------------------------"
            "---------------------"
            "-----------------\n\n");
        outputWindow_->enableTerminateProcessButton(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    auto proc = new QProcess{this};
    proc->setWorkingDirectory(QDir::currentPath());
    evalProcessFinishedConnection_ =
        new QMetaObject::Connection{QObject::connect(
            proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
            gP4app, &P4Application::signalCurveEvaluated)};
    QObject::connect(proc, &QProcess::readyReadStandardOutput, this,
                     &P4InputVF::readProcessStdout);
#ifdef QT_QPROCESS_OLD
    QObject::connect(proc,
                     static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                         &QProcess::error),
                     gP4app, &P4Application::catchProcessError);
#else
    QObject::connect(proc, &QProcess::errorOccurred, gP4app,
                     &P4Application::catchProcessError);
#endif

    processFailed_ = false;
    QString pa = "External Command: " + getMapleExe() + " " + filedotmpl;
    outputWindow_->appendText(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);

    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processFailed_ = true;
        delete proc;
        proc = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        gP4app->signalEvaluated(-1);
        outputWindow_->enableTerminateProcessButton(false);
    } else {
        evalProcess_ = proc;
        evalFile_ = std::move(filedotmpl);
        evalFile2_ = "";
        evaluating_ = true;
        evaluatingGcf_ = false;
        evaluatingPiecewiseConfig_ = false;
        evaluatingArbitraryCurve_ = true;
        evaluatingIsoclines_ = false;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateIsoclinesTable
// -----------------------------------------------------------------------
void P4InputVF::evaluateIsoclinesTable()
{
    evaluatingGcf_ = false;
    evaluatingArbitraryCurve_ = false;
    evaluatingIsoclines_ = false;
    // possible clean up after last Curve evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepareIsoclines();
    auto filedotmpl = getPrepareIsoclinesFileName();

    auto s = getMapleExe();
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
            outputWindow_->appendText(
                "\n\n--------------------------------------"
                "-----------------------------------------"
                "\n\n");
            outputWindow_->enableTerminateProcessButton(true);
            outputWindow_->show();
            outputWindow_->raise();
        }

        QProcess *proc;
        if (evalProcess_ != nullptr) { // re-use process of last GCF
            proc = evalProcess_;
            QObject::disconnect(*evalProcessFinishedConnection_);
            evalProcessFinishedConnection_ =
                new QMetaObject::Connection{QObject::connect(
                    proc,
                    static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                    gP4app, &P4Application::signalCurveEvaluated)};
        } else {
            proc = new QProcess{this};
            evalProcessFinishedConnection_ =
                new QMetaObject::Connection{QObject::connect(
                    proc,
                    static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                    gP4app, &P4Application::signalCurveEvaluated)};
            QObject::connect(proc, &QProcess::readyReadStandardOutput, this,
                             &P4InputVF::readProcessStdout);
#ifdef QT_QPROCESS_OLD
            QObject::connect(
                proc,
                static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                    &QProcess::error),
                gP4app, &P4Application::catchProcessError);
#else
            QObject::connect(proc, &QProcess::errorOccurred, gP4app,
                             &P4Application::catchProcessError);
#endif
        }

        proc->setWorkingDirectory(QDir::currentPath());

        processFailed_ = false;
        QString pa{"External Command: "};
        pa += getMapleExe();
        pa += " ";
        pa += filedotmpl;
        outputWindow_->appendText(pa);
        proc->start(getMapleExe(), QStringList(filedotmpl),
                    QIODevice::ReadWrite);

        if (proc->state() != QProcess::Running &&
            proc->state() != QProcess::Starting) {
            processFailed_ = true;
            delete proc;
            proc = nullptr;
            delete evalProcess_;
            evalProcess_ = nullptr;
            evalFile_ = "";
            evalFile2_ = "";
            gP4app->signalEvaluated(-1);
            outputWindow_->enableTerminateProcessButton(false);
        } else {
            evalProcess_ = proc;
            evalFile_ = std::move(filedotmpl);
            evalFile2_ = "";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::catchProcessError
// -----------------------------------------------------------------------
void P4InputVF::catchProcessError(QProcess::ProcessError prerr)
{
    processFailed_ = true;
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
    if (!evalFile_.isNull() && evalFile_ != "") {
        removeFile(evalFile_);
        evalFile_ = "";
    }

    if (!evalFile2_.isNull() && evalFile2_ != "") {
        removeFile(evalFile2_);
        evalFile2_ = "";
    }

    if (outputWindow_ != nullptr)
        outputWindow_->enableTerminateProcessButton(false);

    if (evalProcess_ != nullptr) {
        if (outputWindow_ != nullptr) {
            outputWindow_->show();
            outputWindow_->raise();
            QString buf{"\n------------------------------------------------"
                        "------------"
                        "-------------------\n"};
            outputWindow_->appendText(buf);
            if (evalProcess_ != nullptr) {
                if (evalProcess_->state() == QProcess::Running) {
                    evalProcess_->terminate();
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
                    QTimer::singleShot(5000, evalProcess_, SLOT(kill));
#else
                    QTimer::singleShot(5000, evalProcess_, &QProcess::kill);
#endif
                    buf = "Kill signal sent to process.\n";
                } else {
                    if (!processFailed_)
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
                if (processFailed_)
                    buf =
                        "The following error occured: " + processError_ + "\n";
                else
                    buf = "";
            }
            outputWindow_->appendText(buf);
        }
    }

    if (evaluatingGcf_)
        finishGcfEvaluation();
    if (evaluatingArbitraryCurve_)
        finishArbitraryCurveEvaluation();
    if (evaluatingIsoclines_)
        finishIsoclinesEvaluation();
    if (evaluatingPiecewiseConfig_)
        finishSeparatingCurvesEvaluation();
}

// -----------------------------------------------------------------------
//          P4InputVF::finishGcfEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishGcfEvaluation()
{
    evaluatingGcf_ = false;
    if (gcfDlg_ != nullptr) {
        gcfDlg_->finishGcfEvaluation();
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::finishArbitraryCurveEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishArbitraryCurveEvaluation()
{
    evaluatingArbitraryCurve_ = false;
    if (arbitraryCurveDlg_ != nullptr) {
        arbitraryCurveDlg_->finishArbitraryCurveEvaluation();
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
    auto filedotmpl = getmaplefilename();
    QFile file{QFile::encodeName(filedotmpl)};
    if (file.open(QFile::WriteOnly)) {
        QTextStream fp{&file};
        prepareFile(fp, false);
        fp.flush();
        file.close();
    }
    // what if P4 cannot open the file?
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareArbitraryCurve
// -----------------------------------------------------------------------
void P4InputVF::prepareArbitraryCurve()
{
    auto filedotmpl = getPrepareArbitraryCurveFileName();
    QFile file{QFile::encodeName(filedotmpl)};
    if (file.open(QFile::WriteOnly)) {
        QTextStream fp{&file};
        prepareArbitraryCurveFile(fp);
        fp.flush();
        file.close();
    }
    // what if P4 cannot open the file?
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclines
// -----------------------------------------------------------------------
void P4InputVF::prepareIsoclines()
{
    auto filedotmpl = getPrepareIsoclinesFileName();
    QFile file{QFile::encodeName(filedotmpl)};
    if (file.open(QFile::WriteOnly)) {
        QTextStream fp{&file};
        prepareIsoclinesFile(fp);
        fp.flush();
        file.close();
    }
    // what if P4 cannot open the file?
}

// -----------------------------------------------------------------------
//          P4InputVF::readProcessStdout
// -----------------------------------------------------------------------
void P4InputVF::readProcessStdout()
{
    QByteArray t;
    QByteArray line;
    int i, j;

    if (evalProcess_ == nullptr || outputWindow_ == nullptr)
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
            outputWindow_->appendText(line);
            i = t.indexOf('\n');
            j = t.indexOf('\r');
        }
        if (t.length() != 0)
            outputWindow_->appendText(t);
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
            QString buf{"\n------------------------------------------------"
                        "------------"
                        "-------------------\n"};
            outputWindow_->appendText(buf);
            evalProcess_->terminate();
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
            QTimer::singleShot(2000, evalProcess_, SLOT(kill));
#else
            QTimer::singleShot(2000, evalProcess_, &QProcess::kill);
#endif
            buf = "Kill signal sent to process.\n";
            outputWindow_->appendText(buf);
            processFailed_ = true;
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

    outputWindow_ = new P4ProcessWnd{};

    QObject::connect(outputWindow_, &P4ProcessWnd::terminateSignal, this,
                     &P4InputVF::onTerminateButton);
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateGcf
// -----------------------------------------------------------------------
bool P4InputVF::evaluateGcf()
{
    auto filedotmpl = getmaplefilename();

    // QString s = getMapleExe().append("
    // \"").append(filedotmpl).append("\"");

    if (outputWindow_ == nullptr)
        createProcessWindow();
    else {
        outputWindow_->appendText(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        outputWindow_->enableTerminateProcessButton(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    QProcess *proc;
    if (evalProcess_ != nullptr) { // re-use process of last GCF
        proc = evalProcess_;
        QObject::disconnect(*evalProcessFinishedConnection_);
        evalProcessFinishedConnection_ =
            new QMetaObject::Connection{QObject::connect(
                proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                gP4app, &P4Application::signalCurveEvaluated)};
    } else {
        proc = new QProcess{this};
        QObject::connect(
            proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
            gP4app, &P4Application::signalCurveEvaluated);
#ifdef QT_QPROCESS_OLD
        QObject::connect(
            proc,
            static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                &QProcess::error),
            gP4app, &P4Application::catchProcessError);
#else
        QObject::connect(proc, &QProcess::errorOccurred, gP4app,
                         &P4Application::catchProcessError);
#endif
        QObject::connect(proc, &QProcess::readyReadStandardOutput, this,
                         &P4InputVF::readProcessStdout);
    }

    proc->setWorkingDirectory(QDir::currentPath());

    processFailed_ = false;
    QString pa = "External Command: ";
    pa += getMapleExe();
    pa += " ";
    pa += filedotmpl;
    outputWindow_->appendText(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);
    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processFailed_ = true;
        delete proc;
        proc = nullptr;
        delete evalProcess_;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        gP4app->signalCurveEvaluated(-1);
        outputWindow_->enableTerminateProcessButton(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = std::move(filedotmpl);
        evalFile2_ = "";
        evaluatingGcf_ = true;
        evaluating_ = true;
        evaluatingPiecewiseConfig_ = false;
        return true;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareGcf
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in plane/U1/U2 charts.  This
// is only called in case of Poincare-compactification (weights p=q=1)
bool P4InputVF::prepareGcf(P4Polynom::term2 *f, double y1, double y2,
                           int precision, int numpoints)
{
    QFile file{QFile::encodeName(getmaplefilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out{&file};

        auto ba_mainmaple = maplepathformat(getP4MaplePath()
                                                .append(QDir::separator())
                                                .append(MAINMAPLEGCFFILE));

        auto user_file = getfilename_gcf();
        auto ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := " << -1.0 << ":\n";
        out << "user_x2 := " << 1.0 << ":\n";
        out << "user_y1 := " << y1 << ":\n";
        out << "user_y2 := " << y2 << ":\n";
        out << "u := x:\n";
        out << "v := y:\n";
        out << "user_f := ";

        int i;
        char buf[100];
        for (i = 0; f != nullptr; i++) {
            out << printterm2(buf, f, (i == 0) ? true : false, "x", "y");
            f = f->next_term2;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareGcf_LyapunovCyl
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification
// (weights (p,q)!=(1,1))
bool P4InputVF::prepareGcf_LyapunovCyl(double theta1, double theta2,
                                       int precision, int numpoints, int index)
{
    QFile file{QFile::encodeName(getmaplefilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out{&file};

        auto f = gVFResults.vf_[index]->gcf_C_;

        auto ba_mainmaple = maplepathformat(getP4MaplePath()
                                                .append(QDir::separator())
                                                .append(MAINMAPLEGCFFILE));

        auto user_file = getfilename_gcf();
        removeFile(user_file);
        auto ba_user_file = maplepathformat(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := " << 0.0 << ":\n";
        out << "user_x2 := " << 1.0 << ":\n";
        out << "user_y1 := " << theta1 << ":\n";
        out << "user_y2 := " << theta2 << ":\n";
        out << "u := cos(y):\n";
        out << "v := sin(y):\n";
        out << "user_f := ";

        int i;
        char buf[100];
        for (i = 0; f != nullptr; i++) {
            out << printterm3(buf, f, (i == 0) ? true : false, "x", "U", "V");
            f = f->next_term3;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareGcf_LyapunovR2
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights
// (p,q)
// !=(1,1))
//
// same as preparegcf, except for the "u := " and "v := " assignments,
// and the fact that one always refers to the same function gVFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
bool P4InputVF::prepareGcf_LyapunovR2(int precision, int numpoints, int index)
{
    QFile file{QFile::encodeName(getmaplefilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out{&file};

        auto f = gVFResults.vf_[index]->gcf_;

        auto ba_mainmaple = maplepathformat(getP4MaplePath()
                                                .append(QDir::separator())
                                                .append(MAINMAPLEGCFFILE));

        auto user_file = getfilename_gcf();
        auto ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 0.0:\n" << 0.0;
        out << "user_x2 := 1.0:\n" << 1.0;
        out << "user_y1 := 0.0:\n" << 0.0;
        out << "user_y2 := " << TWOPI << ":\n";

        out << "u := x*cos(y):\n";
        out << "v := x*sin(y):\n";
        out << "user_f := ";

        int i;
        char buf[100];

        for (i = 0; f != nullptr; i++) {
            out << printterm2(buf, f, (i == 0) ? true : false, "U", "V");
            f = f->next_term2;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateArbitraryCurve
// -----------------------------------------------------------------------
bool P4InputVF::evaluateArbitraryCurve()
{
    auto filedotmpl = getmaplefilename();
    auto s = getMapleExe();
    if (s.isEmpty()) {
        s = "";
    } else {
        s = s.append(" \"").append(filedotmpl).append("\"");
    }

    if (outputWindow_ == nullptr)
        createProcessWindow();
    else {
        outputWindow_->appendText(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        outputWindow_->enableTerminateProcessButton(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    QProcess *proc;
    // re-use process of last GCF
    if (evalProcess_ != nullptr) {
        proc = evalProcess_;
        QObject::disconnect(*evalProcessFinishedConnection_);
        evalProcessFinishedConnection_ =
            new QMetaObject::Connection{QObject::connect(
                proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                gP4app, &P4Application::signalCurveEvaluated)};
    } else {
        proc = new QProcess{this};
        evalProcessFinishedConnection_ =
            new QMetaObject::Connection{QObject::connect(
                proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                gP4app, &P4Application::signalCurveEvaluated)};
#ifdef QT_QPROCESS_OLD
        QObject::connect(
            proc,
            static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                &QProcess::error),
            gP4app, &P4Application::catchProcessError);
#else
        QObject::connect(proc, &QProcess::errorOccurred, gP4app,
                         &P4Application::catchProcessError);
#endif
        QObject::connect(proc, &QProcess::readyReadStandardOutput, this,
                         &P4InputVF::readProcessStdout);
    }

    proc->setWorkingDirectory(QDir::currentPath());

    processFailed_ = false;
    QString pa = "External Command: ";
    pa += getMapleExe();
    pa += " ";
    pa += filedotmpl;
    outputWindow_->appendText(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);
    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processFailed_ = true;
        delete proc;
        proc = nullptr;
        delete evalProcess_;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        gP4app->signalCurveEvaluated(-1);
        outputWindow_->enableTerminateProcessButton(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = std::move(filedotmpl);
        evalFile2_ = "";
        evaluatingArbitraryCurve_ = true;
        return true;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareArbitraryCurve
// -----------------------------------------------------------------------
// Prepare files in case of calculating curve in plane/U1/U2 charts.  This
// is only called in case of Poincare-compactification (weights p=q=1)
bool P4InputVF::prepareArbitraryCurve(const std::vector<P4Polynom::term2> &f,
                                      double y1, double y2, int precision,
                                      int numpoints)
{
    QFile file{QFile::encodeName(getmaplefilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out{&file};

        auto mainmaple =
            getP4MaplePath().append(QDir::separator()).append(MAINMAPLEGCFFILE);
        auto ba_mainmaple = maplepathformat(mainmaple);

        auto user_file = getfilename_arbitrarycurve();
        auto ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := -1.0:\n";
        out << "user_x2 := 1.0:\n";
        out << "user_y1 := " << y1 << ":\n";
        out << "user_y2 := " << y2 << ":\n";
        out << "u := x:\n";
        out << "v := y:\n";
        out << "user_f := ";

        int i{0};
        char buf[100];
        for (auto const &it : f) {
            out << printterm2(buf, &it, (i == 0) ? true : false, "x", "y");
            i++;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareArbitraryCurve_LyapunovCyl
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.
// This is only called in case of Poincare-Lyapunov compactification
// (weights (p,q)
// !=(1,1))
bool P4InputVF::prepareArbitraryCurve_LyapunovCyl(double theta1, double theta2,
                                                  int precision, int numpoints)
{
    QFile file{QFile::encodeName(getmaplefilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out{&file};

        auto const &f = gVFResults.arbitraryCurves_.back().c;

        auto mainmaple =
            getP4MaplePath().append(QDir::separator()).append(MAINMAPLEGCFFILE);
        auto ba_mainmaple = maplepathformat(mainmaple);

        QString user_platform{USERPLATFORM};

        auto user_file = getfilename_arbitrarycurve();
        auto ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 0.0:\n";
        out << "user_x2 := 1.0:\n";
        out << "user_y1 := " << theta1 << ":\n";
        out << "user_y2 := " << theta2 << ":\n";
        out << "u := cos(y):\n";
        out << "v := sin(y):\n";
        out << "user_f := ";

        int i{0};
        char buf[100];
        for (auto const &it : f) {
            out << printterm3(buf, &it, (i == 0) ? true : false, "x", "U", "V");
            i++;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
               "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareArbitraryCurve_LyapunovR2
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.
// This is only called in case of Poincare-Lyapunov compactification
// (weights (p,q)
// !=(1,1))
//
// same as preparegcf, except for the "u := " and "v := " assignments,
// and the fact that one always refers to the same function gVFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
bool P4InputVF::prepareArbitraryCurve_LyapunovR2(int precision, int numpoints)
{
    QFile file{QFile::encodeName(getmaplefilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out{&file};

        auto const &f = gVFResults.arbitraryCurves_.back().r2;

        auto mainmaple =
            getP4MaplePath().append(QDir::separator()).append(MAINMAPLEGCFFILE);
        auto ba_mainmaple = maplepathformat(mainmaple);

        QString user_platform{USERPLATFORM};

        auto user_file = getfilename_arbitrarycurve();
        auto ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 0.0:\n";
        out << "user_x2 := 1.0:\n";
        out << "user_y1 := 0.0:\n";
        out << "user_y2 := " << TWOPI << ":\n";
        out << "u := x*cos(y):\n";
        out << "v := x*sin(y):\n";
        out << "user_f := ";

        int i{0};
        char buf[100];
        for (auto const &it : f) {
            out << printterm2(buf, &it, (i == 0) ? true : false, "U", "V");
            i++;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
               "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateIsoclines
// -----------------------------------------------------------------------
bool P4InputVF::evaluateIsoclines()
{
    evaluatingGcf_ = false;
    // evaluating

    auto filedotmpl = getmaplefilename();
    auto s = getMapleExe();
    s = s.append(" ");
    if (filedotmpl.contains(' ')) {
        s = s.append("\"");
        s = s.append(filedotmpl);
        s = s.append("\"");
    } else
        s = s.append(filedotmpl);

    if (outputWindow_ == nullptr)
        createProcessWindow();
    else {
        outputWindow_->appendText(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        outputWindow_->enableTerminateProcessButton(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    QProcess *proc;
    if (evalProcess_ != nullptr) { // re-use process of last GCF
        proc = evalProcess_;
        QObject::disconnect(*evalProcessFinishedConnection_);
        evalProcessFinishedConnection_ =
            new QMetaObject::Connection{QObject::connect(
                proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                gP4app, &P4Application::signalCurveEvaluated)};
    } else {
        proc = new QProcess{this};
        evalProcessFinishedConnection_ =
            new QMetaObject::Connection{QObject::connect(
                proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                gP4app, &P4Application::signalCurveEvaluated)};
#ifdef QT_QPROCESS_OLD
        QObject::connect(
            proc,
            static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                &QProcess::error),
            gP4app, &P4Application::catchProcessError);
#else
        QObject::connect(proc, &QProcess::errorOccurred, gP4app,
                         &P4Application::catchProcessError);
#endif
        QObject::connect(proc, &QProcess::readyReadStandardOutput, this,
                         &P4InputVF::readProcessStdout);
    }

    proc->setWorkingDirectory(QDir::currentPath());

    processFailed_ = false;
    QString pa = "External Command: ";
    pa += getMapleExe();
    pa += " ";
    pa += filedotmpl;
    outputWindow_->appendText(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);
    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processFailed_ = true;
        delete proc;
        proc = nullptr;
        delete evalProcess_;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        gP4app->signalCurveEvaluated(-1);
        outputWindow_->enableTerminateProcessButton(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = std::move(filedotmpl);
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
bool P4InputVF::prepareIsoclines(const std::vector<P4Polynom::term2> &f,
                                 double y1, double y2, int precision,
                                 int numpoints)
{
    QFile file{QFile::encodeName(getmaplefilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out{&file};

        auto ba_mainmaple = maplepathformat(getP4MaplePath()
                                                .append(QDir::separator())
                                                .append(MAINMAPLEGCFFILE));

        auto user_file = getfilename_isoclines();
        auto ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 1.0:\n";
        out << "user_x2 := -1.0:\n";
        out << "user_y1 := " << y1 << ":\n";
        out << "user_y2 := " << y2 << ":\n";
        out << "u := x:\n";
        out << "v := y:\n";
        out << "user_f := ";

        int i{0};
        char buf[100];
        for (auto const &it : f) {
            out << printterm2(buf, &it, (i == 0) ? true : false, "x", "y");
            i++;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclines_LyapunovCyl
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.
// This is only called in case of Poincare-Lyapunov compactification
// (weights (p,q)!=(1,1))
bool P4InputVF::prepareIsoclines_LyapunovCyl(double theta1, double theta2,
                                             int precision, int numpoints,
                                             int index)
{
    QFile file{QFile::encodeName(getmaplefilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out{&file};

        auto const &f = gVFResults.vf_[index]->isocline_vector_.back().c;

        auto ba_mainmaple = maplepathformat(getP4MaplePath()
                                                .append(QDir::separator())
                                                .append(MAINMAPLEGCFFILE));

        auto user_file = getfilename_isoclines();
        auto ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 0.0:\n";
        out << "user_x2 := 1.0:\n";
        out << "user_y1 := " << theta1 << ":\n";
        out << "user_y2 := " << theta2 << ":\n";
        out << "u := cos(y):\n";
        out << "v := sin(y):\n";
        out << "user_f := ";

        int i{0};
        char buf[100];
        for (auto const &it : f) {
            out << printterm3(buf, &it, (i == 0) ? true : false, "x", "U", "V");
            i++;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
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
// and the fact that one always refers to the same function gVFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
bool P4InputVF::prepareIsoclines_LyapunovR2(int precision, int numpoints,
                                            int index)
{
    QFile file{QFile::encodeName(getmaplefilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out{&file};

        auto const &f = gVFResults.vf_[index]->isocline_vector_.back().r2;

        auto ba_mainmaple = maplepathformat(getP4MaplePath()
                                                .append(QDir::separator())
                                                .append(MAINMAPLEGCFFILE));

        auto user_file = getfilename_isoclines();
        auto ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 0.0:\n";
        out << "user_x2 := 1.0:\n";
        out << "user_y1 := 0.0:\n";
        out << "user_y2 := " << TWOPI << ":\n";
        out << "u := x*cos(y):\n";
        out << "v := x*sin(y):\n";
        out << "user_f := ";

        int i{0};
        char buf[100];
        for (auto const &it : f) {
            out << printterm2(buf, &it, (i == 0) ? true : false, "U", "V");
            i++;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//              COMMON SETTINGS ROUTINES
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonString
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonString(const std::vector<QString> &lst)
{
    auto j0 = lst[selected_[0]];
    for (auto i : selected_) {
        if (j0.compare(lst[i]))
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonInt
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonInt(const std::vector<int> &lst)
{
    for (unsigned int i = 1; i < numSelected_; i++) {
        if (lst[selected_[i]] != lst[selected_[0]])
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonBool
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonBool(const std::vector<bool> &lst)
{
    for (unsigned int i = 1; i < numSelected_; i++) {
        if (lst[selected_[i]] != lst[selected_[0]])
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonParValue
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonParValue(int index)
{
    for (unsigned int i = 1; i < numSelected_; i++) {
        if (parvalue_[selected_[0]][index].compare(
                parvalue_[selected_[i]][index]))
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::commonString
// -----------------------------------------------------------------------
QString P4InputVF::commonString(const std::vector<QString> &lst)
{
    return lst[selected_[0]];
}

// -----------------------------------------------------------------------
//          P4InputVF::commonInt
// -----------------------------------------------------------------------
int P4InputVF::commonInt(const std::vector<int> &lst)
{
    return lst[selected_[0]];
}

// -----------------------------------------------------------------------
//          P4InputVF::commonBool
// -----------------------------------------------------------------------
bool P4InputVF::commonBool(const std::vector<bool> &lst)
{
    return lst[selected_[0]];
}

// -----------------------------------------------------------------------
//          P4InputVF::commonParValue
// -----------------------------------------------------------------------
QString P4InputVF::commonParValue(int index)
{
    return parvalue_[selected_[0]][index];
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonString
// -----------------------------------------------------------------------
void P4InputVF::setCommonString(std::vector<QString> &lst, QString val)
{
    for (unsigned int i = 0; i < numSelected_; i++)
        lst[selected_[i]] = val;
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonInt
// -----------------------------------------------------------------------
void P4InputVF::setCommonInt(std::vector<int> &lst, int val)
{
    for (unsigned int i = 0; i < numSelected_; i++)
        lst[selected_[i]] = val;
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonBool
// -----------------------------------------------------------------------
void P4InputVF::setCommonBool(std::vector<bool> &lst, bool val)
{
    for (unsigned int i = 0; i < numSelected_; i++) {
        lst[selected_[i]] = val;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonParvalue
// -----------------------------------------------------------------------
void P4InputVF::setCommonParValue(int index, QString val)
{
    for (unsigned int i = 0; i < numSelected_; i++) {
        parvalue_[selected_[i]][index] = val;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::addVectorField
// -----------------------------------------------------------------------
void P4InputVF::addVectorField()
{
    int i;

    if (!gVFResults.vf_.empty()) {
        gVFResults.clearVFs();
        gVFResults.K_ = 0;
    }

    xdot_.push_back(QString(DEFAULTXDOT));
    ydot_.push_back(QString(DEFAULTYDOT));
    gcf_.push_back(QString(DEFAULTGCF));
    epsilon_.push_back(QString(DEFAULTEPSILON));
    numeric_.push_back(DEFAULTNUMERIC);
    testsep_.push_back(DEFAULTTESTSEP);
    precision_.push_back(DEFAULTPRECISION);
    precision0_ = DEFAULTPRECISION0;
    taylorlevel_.push_back(DEFAULTLEVEL);
    numericlevel_.push_back(DEFAULTNUMLEVEL);
    maxlevel_.push_back(DEFAULTMAXLEVEL);
    weakness_.push_back(DEFAULTWEAKNESS);
    parvalue_.push_back(std::vector<QString>{});
    for (i = 0; i < MAXNUMPARAMS; i++)
        parvalue_.back().push_back(QString{});

    if (hasCommonString(xdot_))
        xdot_[numVF_] = commonString(xdot_);
    if (hasCommonString(ydot_))
        ydot_[numVF_] = commonString(ydot_);
    if (hasCommonString(gcf_))
        gcf_[numVF_] = commonString(gcf_);
    if (hasCommonString(epsilon_))
        epsilon_[numVF_] = commonString(epsilon_);
    if (hasCommonBool(numeric_))
        numeric_[numVF_] = commonBool(numeric_);
    if (hasCommonBool(testsep_))
        testsep_[numVF_] = commonBool(testsep_);
    if (hasCommonInt(precision_))
        precision_[numVF_] = commonInt(precision_);
    if (hasCommonInt(taylorlevel_))
        taylorlevel_[numVF_] = commonInt(taylorlevel_);
    if (hasCommonInt(numericlevel_))
        numericlevel_[numVF_] = commonInt(numericlevel_);
    if (hasCommonInt(maxlevel_))
        maxlevel_[numVF_] = commonInt(maxlevel_);
    if (hasCommonInt(weakness_))
        weakness_[numVF_] = commonInt(weakness_);

    for (i = 0; i < MAXNUMPARAMS; i++)
        if (hasCommonParValue(i))
            parvalue_[numVF_][i] = commonParValue(i);

    numVF_++;
}

// -----------------------------------------------------------------------
//          P4InputVF::deleteVectorField
// -----------------------------------------------------------------------
void P4InputVF::deleteVectorField(unsigned int index)
{
    // first disconnect from other structures
    if (!gVFResults.vf_.empty()) {
        gVFResults.clearVFs();
        gVFResults.K_ = 0;
    }

    for (unsigned int k = 0; k < numVFRegions_; k++) {
        if (vfRegions_[k].vfIndex == index) {
            vfRegions_.erase(std::begin(vfRegions_) + k);
            numVFRegions_--;
            k--;
            if (numVFRegions_ == 0) {
                vfRegions_.clear();
            }
        } else if (vfRegions_[k].vfIndex > index)
            vfRegions_[k].vfIndex--;
    }

    for (int k = 0; k < static_cast<int>(numSelected_); k++) {
        if (selected_[k] == index) {
            if (k < static_cast<int>(numSelected_) - 1)
                selected_.erase(std::begin(selected_) + k);
            numSelected_--;
            k--;
            if (numSelected_ == 0) {
                numSelected_ = 1;
                selected_[0] = index;
                if (selected_[0] >= numVF_ - 1) {
                    if (numVF_ < 2) {
                        selected_[0] = 0;
                    } else {
                        selected_[0] = numVF_ - 2;
                    }
                }
                break;
            }
        } else if (selected_[k] > index)
            selected_[k]--;
    }

    if (numVF_ == 1) {
        // delete last one: replace by default startup values
        xdot_[0] = DEFAULTXDOT;
        ydot_[0] = DEFAULTYDOT;
        gcf_[0] = DEFAULTGCF;
        epsilon_[0] = DEFAULTEPSILON;
        numeric_[0] = DEFAULTNUMERIC;
        precision_[0] = DEFAULTPRECISION;
        testsep_[0] = DEFAULTTESTSEP;
        taylorlevel_[0] = DEFAULTLEVEL;
        numericlevel_[0] = DEFAULTNUMLEVEL;
        maxlevel_[0] = DEFAULTMAXLEVEL;
        weakness_[0] = DEFAULTWEAKNESS;
        for (int k = 0; k < MAXNUMPARAMS; k++) {
            parvalue_[0][k] = QString{};
            parlabel_[k] = QString{};
        }

        numParams_ = 0;
        p_ = DEFAULTP;
        q_ = DEFAULTQ;
        // symbolicpackage = PACKAGE_MAPLE;
        typeofstudy_ = DEFAULTTYPE;
        x0_ = DEFAULTX0;
        y0_ = DEFAULTY0;
    } else {
        xdot_.erase(std::begin(xdot_) + index);
        ydot_.erase(std::begin(ydot_) + index);
        gcf_.erase(std::begin(gcf_) + index);
        epsilon_.erase(std::begin(epsilon_) + index);
        parvalue_.erase(std::begin(parvalue_) + index); // TODO: funcionarà?

        numVF_--;
    }
    if (numVF_ == 1 && numVFRegions_ == 0 && numSeparatingCurves_ == 0) {
        numVFRegions_ = 1;
        vfRegions_.emplace_back(0, std::vector<int>{});
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::addSeparatingCurve
// -----------------------------------------------------------------------
void P4InputVF::addSeparatingCurve()
{
    if (!gVFResults.separatingCurves_.empty()) {
        gVFResults.resetSeparatingCurveInfo();
    }

    separatingCurves_.push_back(QString{});
    numPointsSeparatingCurve_.push_back(DEFAULT_CURVEPOINTS);
    numSeparatingCurves_++;

    for (unsigned int i = 0; i < numVFRegions_; i++)
        vfRegions_[i].signs.push_back(1);

    for (unsigned int i = 0; i < numCurveRegions_; i++)
        curveRegions_[i].signs.push_back(1);
}

// -----------------------------------------------------------------------
//          P4InputVF::deleteSeparatingCurve
// -----------------------------------------------------------------------
void P4InputVF::deleteSeparatingCurve(int index)
{
    if (index < 0 || index >= static_cast<int>(numSeparatingCurves_))
        return;

    if (!gVFResults.separatingCurves_.empty()) {
        gVFResults.resetSeparatingCurveInfo();
    }

    if (numSeparatingCurves_ == 1) {
        // special case
        numSeparatingCurves_ = 0;
        separatingCurves_.clear();
        numPointsSeparatingCurve_.clear();

        for (unsigned int i = 0; i < numVFRegions_; i++)
            vfRegions_[i].signs.clear();
        for (unsigned int i = 0; i < numCurveRegions_; i++)
            curveRegions_[i].signs.clear();
        if (numVFRegions_ == 0 && numVF_ == 1) {
            numVFRegions_ = 1;
            vfRegions_.clear();
            vfRegions_.push_back(
                p4InputVFRegions::vfRegion{0, std::vector<int>{}});
        }
        return;
    }

    separatingCurves_.erase(std::begin(separatingCurves_) + index);
    numSeparatingCurves_--;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareSeparatingCurves
// -----------------------------------------------------------------------
void P4InputVF::prepareSeparatingCurves()
{
    QFile file{QFile::encodeName(getmaplefilename())};
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream fp{&file};
        prepareFile(fp, true);
        fp.flush();
        file.close();
    }
    // what if P4 cannot open the file?
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateSeparatingCurves
// -----------------------------------------------------------------------

bool P4InputVF::evaluateSeparatingCurves()
{
    evaluatingGcf_ = false;
    evaluatingPiecewiseConfig_ = true;

    // possible clean up after last GCF evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepareSeparatingCurves();

    QString filedotmpl{getmaplefilename()};

    QString s{getMapleExe()};
    if (s.isEmpty())
        s = "";
    else
        s = s.append(" \"").append(filedotmpl).append("\"");

    if (outputWindow_ == nullptr)
        createProcessWindow();
    else {
        outputWindow_->appendText(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        outputWindow_->enableTerminateProcessButton(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    auto proc = new QProcess{this};

    proc->setWorkingDirectory(QDir::currentPath());

    evalProcessFinishedConnection_ =
        new QMetaObject::Connection{QObject::connect(
            proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
            gP4app, &P4Application::signalSeparatingCurvesEvaluated)};
#ifdef QT_QPROCESS_OLD
    QObject::connect(proc,
                     static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                         &QProcess::error),
                     gP4app, &P4Application::catchProcessError);
#else
    QObject::connect(proc, &QProcess::errorOccurred, gP4app,
                     &P4Application::catchProcessError);
#endif
    QObject::connect(proc, &QProcess::readyReadStandardOutput, this,
                     &P4InputVF::readProcessStdout);

    processFailed_ = false;
    QString pa{"External Command: " + getMapleExe() + " " + filedotmpl};
    outputWindow_->appendText(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);

    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processFailed_ = true;
        delete proc;
        proc = nullptr;
        delete evalProcess_;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        gP4app->signalEvaluated(-1);
        outputWindow_->enableTerminateProcessButton(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = std::move(filedotmpl);
        evalFile2_ = "";
        evaluating_ = true;
        evaluatingPiecewiseConfig_ = true;
        evaluatingGcf_ = false;
        return true;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::finishSeparatingCurvesEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishSeparatingCurvesEvaluation()
{
    evaluatingPiecewiseConfig_ = false;
    auto e = new P4Event{
        static_cast<QEvent::Type>(TYPE_SIGNAL_SEPARATINGCURVESEVALUATED),
        nullptr};
    gP4app->postEvent(gP4startDlg, e);
}

// -----------------------------------------------------------------------
//          P4InputVF::markVFRegion
// -----------------------------------------------------------------------
void P4InputVF::markVFRegion(unsigned int index, const double *p)
{
    int i;

    if (gVFResults.separatingCurves_.empty())
        return;

    for (i = numVFRegions_ - 1; i >= 0; i--) {
        if (isInsideRegion_sphere(vfRegions_[i].signs, p))
            break;
    }
    if (i < 0) {
        std::vector<int> sgns;
        if (numSeparatingCurves_ > 0)
            // sgns.reserve(sizeof(int) * numSeparatingCurves_);
            for (auto const &curveResult : gVFResults.separatingCurves_) {
                if (eval_curve(curveResult, p) < 0)
                    sgns.push_back(-1);
                else
                    sgns.push_back(+1);
            }
        vfRegions_.emplace_back(-1, sgns);
        numVFRegions_++;
        i = numVFRegions_ - 1;
    }
    vfRegions_[i].vfIndex = index;
}

// -----------------------------------------------------------------------
//          P4InputVF::unmarkVFRegion
// -----------------------------------------------------------------------
void P4InputVF::unmarkVFRegion(unsigned int index, const double *p)
{
    int i;

    if (gVFResults.separatingCurves_.empty())
        return;

    for (i = numVFRegions_ - 1; i >= 0; i--) {
        if (isInsideRegion_sphere(vfRegions_[i].signs, p))
            break;
    }
    if (i < 0)
        return; // region cannot be found???

    if (vfRegions_[i].vfIndex != index)
        return; // region does not have the corresponding index

    vfRegions_.erase(std::begin(vfRegions_) + i);
    if (numVFRegions_ == 1) {
        vfRegions_.clear();
    }
    numVFRegions_--;
}

// -----------------------------------------------------------------------
//          P4InputVF::markCurveRegion
// -----------------------------------------------------------------------
void P4InputVF::markCurveRegion(unsigned int index, const double *p)
{
    std::vector<int> signs;

    if (numSeparatingCurves_ == 0 || gVFResults.separatingCurves_.empty())
        return;

    for (unsigned int i = 0; i < numSeparatingCurves_; i++) {
        if (i == index)
            signs.push_back(0);
        else if (eval_curve(gVFResults.separatingCurves_[i], p) < 0)
            signs.push_back(-1);
        else
            signs.push_back(1);
    }
    for (auto const &curve : curveRegions_) {
        if (signs == curve.signs && index == curve.curveIndex) {
            // curve mark already exist
            return;
        }
    }

    curveRegions_.emplace_back(index, signs);
    numCurveRegions_++;
    resampleSeparatingCurve(index);
}

// -----------------------------------------------------------------------
//          P4InputVF::unmarkCurveRegion
// -----------------------------------------------------------------------
void P4InputVF::unmarkCurveRegion(unsigned int index, const double *p)
{
    std::vector<int> signs;

    if (numSeparatingCurves_ == 0 || gVFResults.separatingCurves_.empty())
        return;

    // NOTE: es pot fer així? reservar i accedir com si existissin?
    signs.reserve(sizeof(int) * numSeparatingCurves_);
    for (int i = numSeparatingCurves_ - 1; i >= 0; i--) {
        if (i == static_cast<int>(index))
            signs[i] = 0;
        else if (eval_curve(gVFResults.separatingCurves_[i], p) < 0)
            signs[i] = -1;
        else
            signs[i] = 1;
    }

    for (unsigned int i = 0; i < numCurveRegions_; i++) {
        if (signs == curveRegions_[i].signs &&
            index == curveRegions_[i].curveIndex) {
            // curve mark exists
            curveRegions_.erase(std::begin(curveRegions_) + i);
            numCurveRegions_--;
            resampleSeparatingCurve(index);
            break;
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::clearVFMarks
// -----------------------------------------------------------------------
void P4InputVF::clearVFMarks()
{
    vfRegions_.clear();
    numVFRegions_ = 0;
}

// -----------------------------------------------------------------------
//          P4InputVF::clearCurveMarks
// -----------------------------------------------------------------------
void P4InputVF::clearCurveMarks()
{
    curveRegions_.clear();
    numCurveRegions_ = 0;
}

// -----------------------------------------------------------------------
//          P4InputVF::isCurvePointDrawn
// -----------------------------------------------------------------------
bool P4InputVF::isCurvePointDrawn(unsigned int index, const double *pcoord)
{
    if (numSeparatingCurves_ == 0)
        return true;
    if (gVFResults.separatingCurves_.empty())
        return false; // NOTE: sure? if there are no separating curves there
                      // are no regions so all poinst should be drawn...

    for (auto const &curve : curveRegions_) {
        if (curve.curveIndex == index) {
            for (int k = numSeparatingCurves_ - 1; k >= 0; k--) {
                if (k == static_cast<int>(index))
                    continue;
                if (eval_curve(gVFResults.separatingCurves_[k], pcoord) < 0) {
                    if (curve.signs[k] > 0)
                        return false;
                } else {
                    if (curve.signs[k] < 0)
                        return false;
                }
            }
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::resampleSeparatingCurve
// -----------------------------------------------------------------------
void P4InputVF::resampleSeparatingCurve(int i)
{
    if (gVFResults.separatingCurves_.empty())
        return;

    for (auto &sep : gVFResults.separatingCurves_[i].points) {
        if (isCurvePointDrawn(i, sep.pcoord))
            sep.color = P4ColourSettings::colour_separating_curve;
        else
            sep.color = P4ColourSettings::colour_shaded_curve;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::resampleGcf
// -----------------------------------------------------------------------
// Erases the GCF points of a vector field that lie outside of the region
// where this vector field is assigned
void P4InputVF::resampleGcf(int i)
{
    if (gVFResults.separatingCurves_.empty() || gVFResults.vf_.empty())
        return;

    auto &sep = gVFResults.vf_[i]->gcf_points_;
    while (sep != nullptr) {
        if (getVFIndex_sphere(sep->pcoord) != i) {
            auto sepx = sep;
            sep = sep->nextpt;
            delete sepx;
            if (sep != nullptr)
                sep->dashes = 0;
        } else
            sep = sep->nextpt;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::resampleIsoclines
// -----------------------------------------------------------------------
// Erases the isoclines points of a vector field that lie outside of the
// region where this vector field is assigned
void P4InputVF::resampleIsoclines(int i)
{
    if (gVFResults.separatingCurves_.empty() || gVFResults.vf_.empty())
        return;

    for (auto &isoc : gVFResults.vf_[i]->isocline_vector_) {
        for (auto it = std::begin(isoc.points); it != std::end(isoc.points);
             ++it) {
            it->dashes = 0;
            if (getVFIndex_sphere(it->pcoord) != i)
                isoc.points.erase(it);
        }
    }
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
//          GET VF INDEX FUNCTIONS
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

/* In this file, routines are developped to check if a point belongs to a
 * given region, and to find the associated vector field.
 *
 * In order to avoid calculations with large numbers, when a point is close
 * to infinity, the calculations are not done in usual coordinates, but
 * instead in one of the charts near infinity.
 *
 * Therefore, a lot of variants have been developed with care.
 */

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_R2
// ---------------------------------------------------------------------
// Given a coordinate ucoord=(x0,y0), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The point is given in standard coordinates (in the finite region)
// and is assumed to be in a ball with small radius.
int P4InputVF::getVFIndex_R2(const double *ucoord)
{
    for (auto i : vfRegions_) {
        if (isInsideRegion_R2(i.signs, ucoord))
            return i.vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_sphere
// ---------------------------------------------------------------------
// Given a coordinate pcoord=(X,Y,Z), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The point is given in Poincare-sphere coordinates (X,Y,Z), or
// Poincare-Lyapunov coordinates (X,Y,Z) depending on the setting of the
// system
int P4InputVF::getVFIndex_sphere(const double *pcoord)
{
    if (!gVFResults.plweights_) {
        return getVFIndex_psphere(pcoord);
    } else {
        return getVFIndex_plsphere(pcoord);
    }
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_psphere
// ---------------------------------------------------------------------
// Given a coordinate pcoord=(X,Y,Z), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The point is given in Poincare-sphere coordinates (X,Y,Z), where
// (X,Y) lies on the unit circle and Z is (the inverse of) a radial
// coordinate.
//
// Since we do not know if the point is close to infinity or not, and
// if it is, in which chart it lies, we need to first find that out.
int P4InputVF::getVFIndex_psphere(const double *pcoord)
{
    double ucoord[2], theta;
    if (pcoord[2] > ZCOORD) {
        psphere_to_R2(pcoord[0], pcoord[1], pcoord[2], ucoord);
        return getVFIndex_R2(ucoord);
    } else {
        theta = std::atan2(fabs(pcoord[1]), fabs(pcoord[0]));
        if (theta < PI_DIV4 && theta > -PI_DIV4) {
            if (pcoord[0] > 0) {
                psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                return getVFIndex_U1(ucoord);
            } else {
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                return getVFIndex_V1(ucoord);
            }
        } else {
            if (pcoord[1] > 0) {
                psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                return getVFIndex_U2(ucoord);
            } else {
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                return getVFIndex_V2(ucoord);
            }
        }
    }
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_plsphere
// ---------------------------------------------------------------------
// Given a coordinate pcoord=(X,Y,Z), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The point is given in Poincare-sphere coordinates (X,Y,Z).  Such
// coordinates can be of two forms:
//
//      (X,Y,Z) = (0,u,v)  --> (x,y) = (u,v)
//      (X,Y,Z) = (1,u,v)  --> (x,y) = (cos(v)/u^p, sin(v)/u^q)
//
// The first form is used for points inside the unit circle; the second
// for points near infinity.  In the above form, the relation with the
// normal (x,y)-coordinates in the plane are shown.
int P4InputVF::getVFIndex_plsphere(const double *pcoord)
{
    double ucoord[2];

    ucoord[0] = pcoord[1];
    ucoord[1] = pcoord[2];

    if (pcoord[0])
        return getVFIndex_R2(ucoord);
    else
        return getVFIndex_cyl(ucoord);
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_cyl
// ---------------------------------------------------------------------
// Given a coordinate y=(r,theta), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// This is only used in the Poincare-Lyapunov setting, in a study
// near infinity.
//
// The point is given in cylindrical coordinates (r,theta).  The
// r-coordinate is a (inverse) radial coordinate, the theta is an angular
// coordinate.
int P4InputVF::getVFIndex_cyl(const double *y)
{
    for (auto i : vfRegions_) {
        if (isInsideRegion_cyl(i.signs, y))
            return i.vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_U1
// ---------------------------------------------------------------------
// Given a coordinate y=(z1,z2), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The relation with original coordinates is:
//
//          (x,y) = (1/z2,z1/z2),           z2 > 0.
//
// If z2<0, we are inside the chart VV1.
int P4InputVF::getVFIndex_U1(const double *y)
{
    for (auto i : vfRegions_) {
        if (isInsideRegion_U1(i.signs, y))
            return i.vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_V1
// ---------------------------------------------------------------------
// Given a coordinate y=(z1,z2), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The relation with original coordinates is:
//
//          (x,y) = (-1/z2,z1/z2),           z2 > 0.
//
// If z2<0, we are inside the chart UU1.
int P4InputVF::getVFIndex_V1(const double *y)
{
    for (auto i : vfRegions_) {
        if (isInsideRegion_V1(i.signs, y))
            return i.vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_U2
// ---------------------------------------------------------------------
// Given a coordinate y=(z1,z2), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The relation with original coordinates is:
//
//          (x,y) = (z1/z2,1/z2),           z2 > 0.
//
// If z2<0, we are inside the chart VV2.
int P4InputVF::getVFIndex_U2(const double *y)
{
    for (auto i : vfRegions_) {
        if (isInsideRegion_U2(i.signs, y))
            return i.vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_V2
// ---------------------------------------------------------------------
//
// Given a coordinate y=(z1,z2), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The relation with original coordinates is:
//
//          (x,y) = (z1/z2,-1/z2),           z2 > 0.
//
// If z2<0, we are inside the chart UU2.
int P4InputVF::getVFIndex_V2(const double *y)
{
    for (auto i : vfRegions_) {
        if (isInsideRegion_V2(i.signs, y))
            return i.vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//  getVFIndex_UU1, getVFIndex_UU2, getVFIndex_VV1, getVFIndex_VV2
// ---------------------------------------------------------------------
//
// When integration starts, for example in a U1 chart in a point (z1,z2)
// very close to infinity (close to z2=0, but z2>0), it may sometimes happen
// that the line of infinity is crossed (to a point z2<0).  The resulting
// point lies in the U1 chart, but in a wrong part of it.  We say that
// it lies in the VV1-part: this point will be mapped to the symmetric point
// in the V1-chart.
//
// So:
//
//   * a point in the UU1 chart lies in the wrong part of the V1-chart, and
//   is in fact a U1-point
//   * a point in the UU2 chart lies in the wrong part of the V2-chart, and
//   is in fact a U2-point
//   * a point in the VV1 chart lies in the wrong part of the U1-chart, and
//   is in fact a V1-point
//   * a point in the VV2 chart lies in the wrong part of the U2-chart, and
//   is in fact a V2-point
//
// The relation between the symmetric charts is simple:
//              (z1,z2) ---> (-z1,-z2).
//
// Note: these charts are only used when p=q=1.
int P4InputVF::getVFIndex_UU1(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_U1(y);
}

int P4InputVF::getVFIndex_UU2(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_U2(y);
}

int P4InputVF::getVFIndex_VV1(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_V1(y);
}

int P4InputVF::getVFIndex_VV2(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_V2(y);
}

// ---------------------------------------------------------------------
//          getDataFromDlg
// ---------------------------------------------------------------------
void P4InputVF::getDataFromDlg()
{
    if (findDlg_ != nullptr)
        findDlg_->getDataFromDlg();
}
