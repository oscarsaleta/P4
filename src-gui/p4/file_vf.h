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

#ifndef FILE_VF_H
#define FILE_VF_H

#include "custom.h"
#include "win_curve.h"
#include "win_find.h"
#include "win_gcf.h"
#include "win_isoclines.h"

#include <QObject>
#include <QProcess>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QTextStream>

#define TYPEOFSTUDY_ALL 0
#define TYPEOFSTUDY_FIN 1
#define TYPEOFSTUDY_INF 2
#define TYPEOFSTUDY_ONE 3

#define PACKAGE_MAPLE 1
#define PACKAGE_REDUCE 0

/* Check Qt version for compatibility with QProcess::errorOccurred */
#if QT_VERSION_MINOR < 6
    #warning "Old Qt version"
    #define QT_QPROCESS_OLD
#endif

class QInputVF : public QObject
{
    Q_OBJECT
  public:
    QInputVF();
    ~QInputVF();

    QString filename_;

    int symbolicpackage_; // 0 for reduce, 1 for maple
    int typeofstudy_;     // 0, 1, 2, 3 = all, inf, fin, one

    bool numeric_;
    int precision_;
    int precision0_;
    QString epsilon_;
    bool testsep_;
    int taylorlevel_;
    int numericlevel_;
    int maxlevel_;
    int weakness_;
    QString x0_;
    QString y0_;
    int p_;
    int q_;

    QString xdot_;
    QString ydot_;
    QString gcf_;
    QString curve_;
    QString isoclines_;
    QString evalFile_;
    QString evalFile2_;

    QWidget *outputWindow_;
    QProcess *evalProcess_;
    QTextEdit *processText_;
    QPushButton *terminateProcessButton_;
    QPushButton *clearProcessButton_;

    int numparams_;
    QString parlabel_[MAXNUMPARAMS];
    QString parvalue_[MAXNUMPARAMS];

    bool changed_;       // set when data needs to be saved
    bool evaluated_;     // set when data has been evaluated
    bool evaluating_;    // set while evaluating
    bool cleared_;       // initial state, when records are clear
    bool evaluatinggcf_; // true when evaluation is of GCF kind
    bool evaluatingCurve_;
    bool evaluatingIsoclines_;
    bool processfailed_;   // true when process failed;
    QString processError_; // only relevant when processfailed=true

    QFindDlg *findDlg_;
    QGcfDlg *gcfDlg_;
    QCurveDlg *curveDlg_;
    QIsoclinesDlg *isoclinesDlg_;

    QString getfilename() const;            // filename.inp
    QString getbarefilename() const;        // filename
    QString getfilename_finresults() const; // filename_fin.res
    QString getfilename_infresults() const; // filename_inf.res
    QString getfilename_fintable() const;   // filename_fin.tab
    QString getfilename_inftable() const;   // filename_inf.tab
    QString getfilename_vectable() const;   // filename_vec.tab
    QString getfilename_gcf() const;        // filename_gcf.tab (temporary file)
    // QString getfilename_gcfresults() const; // filename_gcf.res (temp file,
    // only for reduce)
    QString getreducefilename() const; // filename.red
    QString getmaplefilename() const;  // filename.txt
    QString getrunfilename() const;    // filename.run
    // curve filenames
    QString getfilename_curvetable() const;  // filename_veccurve.tab
    QString getfilename_curve() const;       // filename_curve.tab (tmp file)
    QString getPrepareCurveFileName() const; // filename_curve_prep.txt
    // isoclines filenames
    QString getfilename_isoclinestable() const; // filename_vecisocline.tab
    QString getfilename_isoclines() const; // filename_isocline.tab (tmp file)
    QString getPrepareIsoclinesFileName() const; // filename_isocline_prep.txt

    void getDataFromDlg();

    bool load();
    bool save();
    void reset();
    bool checkevaluated();

    static bool fileExists(QString);

    // void prepareReduceParameters(QTextStream *);
    // void prepareReduceVectorField(QTextStream *);
    void prepareMapleParameters(QTextStream *);
    void prepareMapleVectorField(QTextStream *);
    inline QString booleanString(int value) const
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
    QString convertMapleUserParameterLabels(QString);
    QString convertMapleUserParametersLabelsToValues(QString);
    // QString convertReduceUserParameterLabels(QString);

    void prepareFile(QTextStream *); // used by Prepare()

    void prepare();
    void evaluate();

    bool prepareGcf(P4POLYNOM2 f, double, double, int, int);
    bool prepareGcf_LyapunovCyl(double, double, int, int);
    bool prepareGcf_LyapunovR2(int, int);
    bool evaluateGcf();

    /* curve functions */
    // called from win_curve.cpp evaluate button
    void evaluateCurveTable();
    // called from evaluateCurveTable
    void prepareCurve();
    // called from prepareCurve
    void prepareCurveFile(QTextStream *);
    // called from prepareCurveFile
    void prepareMapleCurve(QTextStream *);
    // the following are called from math_curve.cpp
    bool prepareCurve(P4POLYNOM2 f, double, double, int, int);
    bool prepareCurve_LyapunovCyl(double, double, int, int);
    bool prepareCurve_LyapunovR2(int, int);
    bool evaluateCurve();

    /* isoclines functions */
    void evaluateIsoclinesTable();
    void prepareIsoclines();
    void prepareIsoclinesFile(QTextStream *);
    void prepareMapleIsoclines(QTextStream *);
    bool prepareIsoclines(P4POLYNOM2 f, double, double, int, int);
    bool prepareIsoclines_LyapunovCyl(double, double, int, int);
    bool prepareIsoclines_LyapunovR2(int, int);
    bool evaluateIsoclines();

    void createProcessWindow();

  public slots:
    void finishEvaluation(int);
    void catchProcessError(QProcess::ProcessError);
    void readProcessStdout();
    void onTerminateButton();
    void finishGcfEvaluation();
    void finishCurveEvaluation();
    void finishIsoclinesEvaluation();
};

extern QInputVF *g_ThisVF;

#endif /* FILE_VF_H */
