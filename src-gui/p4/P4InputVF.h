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

#include "custom.h"

#include <memory>

#include <QObject>
#include <QProcess>
#include <QString>

#define TYPEOFSTUDY_ALL 0
#define TYPEOFSTUDY_FIN 1
#define TYPEOFSTUDY_INF 2
#define TYPEOFSTUDY_ONE 3

#define PACKAGE_MAPLE 1
#define PACKAGE_REDUCE 0

/* Check Qt version for compatibility with QProcess::errorOccurred */
#if QT_VERSION_MINOR < 6
#define QT_QPROCESS_OLD
#endif

// forward class and struct declarations
class QWidget;
class QProcess;
class QTextEdit;
class QPushButton;
class QTextStream;

struct term2;

namespace p4InputVFRegions
{
struct vfRegion {
    int vfIndex;
    std::vector<int> signs;
    vfRegion(int i, std::vector<int> s) : vfIndex{i}, signs{s} {}
}

struct curveRegion {
    int curveIndex;
    std::vector<int> signs;
    curveRegion(int i, std::vector<int> s) : curveIndex{i}, signs{s} {}
}
}

class P4InputVF : public QObject
{
    Q_OBJECT
   public:
    P4InputVF();
    //~P4InputVF();

    QString filename_;

    int symbolicpackage_;  // 0 for reduce, 1 for maple
    int typeofstudy_;      // 0, 1, 2, 3 = all, inf, fin, one

    std::vector<bool> numeric_;
    std::vector<int> precision_;
    std::vector<int> precision0_;
    std::vector<QString> epsilon_;
    std::vector<bool> testsep_;
    std::vector<int> taylorlevel_;
    std::vector<int> numericlevel_;
    std::vector<int> maxlevel_;
    std::vector<int> weakness_;
    QString x0_;
    QString y0_;
    int p_;
    int q_;

    std::vector<QString> xdot_;
    std::vector<QString> ydot_;
    std::vector<QString> gcf_;
    // curve_ is the original, curves_ is P5. There is only one curve regardless
    // of how many regions we have
    QString curve_;
    std::vector<QString> isoclines_;
    QString evalFile_;
    QString evalFile2_;

    std::unique_ptr<QWidget> outputWindow_;
    std::unique_ptr<QProcess> evalProcess_;
    std::unique_ptr<QTextEdit> processText_;
    std::unique_ptr<QPushButton> terminateProcessButton_;
    std::unique_ptr<QPushButton> clearProcessButton_;

    /* parameters list */
    int numparams_;
    QString parlabel_[MAXNUMPARAMS];
    std::vector<std::vector<QString>> parvalue_;

    bool changed_;        // set when data needs to be saved
    bool evaluated_;      // set when data has been evaluated
    bool evaluating_;     // set while evaluating
    bool cleared_;        // initial state, when records are clear
    bool evaluatinggcf_;  // true when evaluation is of GCF kind
    bool evaluatingCurve_;
    bool evaluatingIsoclines_;
    bool processfailed_;    // true when process failed;
    QString processError_;  // only relevant when processfailed=true

    std::unique_ptr<P4FindDlg> findDlg_;
    std::unique_ptr<QGcfDlg> gcfDlg_;
    std::unique_ptr<QCurveDlg> curveDlg_;
    std::unique_ptr<QIsoclinesDlg> isoclinesDlg_;

    QString getfilename() const;             // filename.inp
    QString getbarefilename() const;         // filename
    QString getfilename_finresults() const;  // filename_fin.res
    QString getfilename_infresults() const;  // filename_inf.res
    QString getfilename_fintable() const;    // filename_fin.tab
    QString getfilename_inftable() const;    // filename_inf.tab
    QString getfilename_vectable() const;    // filename_vec.tab
    QString getfilename_gcf() const;  // filename_gcf.tab (temporary file)
    // QString getfilename_gcfresults() const; // filename_gcf.res (temp file,
    // only for reduce)
    QString getreducefilename() const;  // filename.red
    QString getmaplefilename() const;   // filename.txt
    QString getrunfilename() const;     // filename.run
    // curve filenames
    QString getfilename_curvetable() const;   // filename_veccurve.tab
    QString getfilename_curve() const;        // filename_curve.tab (tmp file)
    QString getPrepareCurveFileName() const;  // filename_curve_prep.txt
    // isoclines filenames
    QString getfilename_isoclinestable() const;  // filename_vecisocline.tab
    QString getfilename_isoclines() const;  // filename_isocline.tab (tmp file)
    QString getPrepareIsoclinesFileName() const;  // filename_isocline_prep.txt

    void getDataFromDlg();

    bool load();
    bool save();
    void reset(int);
    bool checkevaluated();

    static bool fileExists(QString);

    void prepareMapleParameters(QTextStream &);
    void prepareMapleVectorField(QTextStream &);
    inline QString booleanString(int value) const
    {
        if (value == 0)
            return "false";
        else
            return "true";
    }
    QString convertMapleUserParameterLabels(QString);
    QString convertMapleUserParametersLabelsToValues(QString);

    void prepareFile(QTextStream &);  // used by Prepare()

    void prepare();
    void evaluate();

    // FIXME
    bool prepareGcf(std::vector<p4polynom::term2> f, double y1, double y2,
                    int precision, int numpoints);
    bool prepareGcf_LyapunovCyl(double theta1, double theta2, int precision,
                                int numpoints, int index);
    bool prepareGcf_LyapunovR2(int precision, int numpoints, int index);
    bool evaluateGcf();

    /* curve functions */
    // called from win_curve.cpp evaluate button
    void evaluateCurveTable();
    // called from evaluateCurveTable
    void prepareCurve();
    // called from prepareCurve
    void prepareCurveFile(QTextStream &);
    // called from prepareCurveFile
    void prepareMapleCurve(QTextStream &);
    // the following are called from math_curve.cpp
    bool prepareCurve(std::vector<p4polynom::term2> f, double y1, double y2,
                      int precision, int numpoints);  // FIXME
    bool prepareCurve_LyapunovCyl(double theta1, double theta2, int precision,
                                  int numpoints, int index);
    bool prepareCurve_LyapunovR2(int precision, int numpoints, int index);
    bool evaluateCurve();

    /* isoclines functions */
    void evaluateIsoclinesTable();
    void prepareIsoclines();
    void prepareIsoclinesFile(QTextStream &);
    void prepareMapleIsoclines(QTextStream &);
    bool prepareIsoclines(std::vector<p4polynom::term2> f, double y1, double y2,
                          int precision,
                          int numpoints);  // FIXME
    bool prepareIsoclines_LyapunovCyl(double theta1, double theta2,
                                      int precision, int numpoints, int index);
    bool prepareIsoclines_LyapunovR2(int precision, int numpoints, int index);
    bool evaluateIsoclines();

    void createProcessWindow();

    // p5 modifications --------------------------------------------------------
    int numVF_;
    int numVFRegions_;
    std::vector<p4InputVFRegions::vfRegion> vfRegions;

    int numCurves_;
    int numCurveRegions_;
    std::vector<p4InputVFRegions::curveRegion> curveRegions;

    std::vector<int> numPointsCurve_;
    std::vector<QString> curves_;

    int numSelected_;
    std::vector<int> selected_;

    bool evaluatingPiecewiseConfig_;

    void addVectorField();
    void deleteVectorField();
    void addSeparatingCurve();
    void deleteSeparatingCurve();
    void resampleCurve();

    void prepareCurves();
    bool evaluateCurves();

    // TODO
    int getVFIndex_R2(const double *);
    int getVFIndex_U1(const double *);
    int getVFIndex_U2(const double *);
    int getVFIndex_V1(const double *);
    int getVFIndex_V2(const double *);
    int getVFIndex_UU1(const double *);
    int getVFIndex_UU2(const double *);
    int getVFIndex_VV1(const double *);
    int getVFIndex_VV2(const double *);
    int getVFIndex_sphere(const double *);
    int getVFIndex_psphere(const double *);
    int getVFIndex_plsphere(const double *);
    int getVFIndex_cyl(const double *);

    void markVFRegion(int, const double *);
    void unmarkVFRegion(int, const double *);
    void markCurveRegion(int, const double *);
    void unmarkCurveRegion(int, const double *);
    void clearVFMarks();
    void clearCurveMarks();
    bool isCurvePointDrawn(int, const double *);
    void resampleGcf(int);

    bool hasCommonString(QString **);
    bool hasCommonInt(int *);
    bool hasCommonBool(bool *);
    bool hasCommonParvalue(int);
    QString commonString(QString **);
    int commonInt(int *);
    bool commonBool(bool *);
    QString commonParvalue(int);
    void setCommonString(QString **, QString);
    void setCommonInt(int *, int);
    void setCommonBool(bool *, bool);
    void setCommonParvalue(int, QString);

    QString getfilename_curveresults(void) const;  // filename_curves.res

    void prepareMaplePiecewiseConfig(QTextStream &);
    // P5 function, the original one is prepareMapleCurve (singular)
    void prepareMapleCurves(QTextStream &);
    // -------------------------------------------------------------------------

   signals:
    void saveSignal();
    void loadSignal();

   public slots:
    void finishEvaluation(int);
    void catchProcessError(QProcess::ProcessError);
    void readProcessStdout();
    void onTerminateButton();
    void finishGcfEvaluation();
    void finishCurveEvaluation();
    void finishIsoclinesEvaluation();
};

extern std::unique_ptr<P4InputVF> g_ThisVF;
