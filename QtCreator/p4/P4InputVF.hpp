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

#include <QObject>

#include <vector>

#include <QMetaObject>
#include <QProcess>
#include <QString>

#include "custom.hpp"

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
class QProcess;
class QPushButton;
class QTextEdit;
class QTextStream;
class QWidget;

class P4ArbitraryCurveDlg;
class P4FindDlg;
class P4GcfDlg;
class P4IsoclinesDlg;
class P4ProcessWnd;

namespace p4polynom
{
struct term2;
}

namespace p4InputVFRegions
{
struct vfRegion {
    int vfIndex;
    std::vector<int> signs;
    vfRegion(int i, std::vector<int> s) : vfIndex{i}, signs{std::move(s)} {}
};

struct curveRegion {
    int curveIndex;
    std::vector<int> signs;
    curveRegion(int i, std::vector<int> s) : curveIndex{i}, signs{std::move(s)}
    {
    }
};
} // namespace p4InputVFRegions

class P4InputVF : public QObject
{
    Q_OBJECT

  public:
    P4InputVF();
    ~P4InputVF();

    //////////////////////
    // MEMBER VARIABLES //
    //////////////////////

    QString filename_{DEFAULTFILENAME};

    int symbolicpackage_{PACKAGE_MAPLE}; // 0 for reduce, 1 for maple
    int typeofstudy_;                    // 0, 1, 2, 3 = all, inf, fin, one

    // MAPLE EXECUTION PARAMETERS
    std::vector<bool> numeric_;
    std::vector<int> precision_;
    int precision0_{0};
    std::vector<QString> epsilon_;
    std::vector<bool> testsep_;
    std::vector<int> taylorlevel_;
    std::vector<int> numericlevel_;
    std::vector<int> maxlevel_;
    std::vector<int> weakness_;
    QString x0_{DEFAULTX0};
    QString y0_{DEFAULTY0};
    int p_{DEFAULTP};
    int q_{DEFAULTQ};

    // TEXT VECTOR FIELDS
    std::vector<QString> xdot_;
    std::vector<QString> ydot_;
    std::vector<QString> gcf_;

    // ARBITRARY CURVE (one for all)
    QString arbitraryCurve_;

    // ISOCLINES (one per VF)
    std::vector<QString> isoclines_;

    // EVALUATION VARIABLES
    QString evalFile_;
    QString evalFile2_;
    QProcess *evalProcess_{nullptr};
    QMetaObject::Connection *evalProcessFinishedConnection_{nullptr};

    // QT GUI ELEMENTS FIXME need to be public?
    P4ProcessWnd *outputWindow_{nullptr};
    //    QTextEdit *processText_{nullptr};
    //    QPushButton *terminateProcessButton_{nullptr};
    //    QPushButton *clearProcessButton_{nullptr};

    // PARAMETER LIST
    int numParams_{0};
    // list of parameter names
    QString parlabel_[MAXNUMPARAMS];
    // a list of parameter values for every VF (thus vector of vectors)
    std::vector<std::vector<QString>> parvalue_;

    // PIECEWISE VARIABLES: vector fields
    // indicates length of P4ParentStudy::vf_ (not really necessary but too much
    // work to get rid of atm)
    int numVF_{1};
    // same but for this->vfRegions_
    int numVFRegions_{1};
    std::vector<p4InputVFRegions::vfRegion> vfRegions_;

    // PIECEWISE VARIABLES: separating curves
    // same as above but for this->separatingCurves_
    int numSeparatingCurves_{0};
    std::vector<QString> separatingCurves_;
    std::vector<int> numPointsSeparatingCurve_;
    // same as above but for this->curveRegions_
    int numCurveRegions_{0};
    std::vector<p4InputVFRegions::curveRegion> curveRegions_;

    // PIECEWISE VARIABLES: selected regions
    int numSelected_{0};
    std::vector<int> selected_;

    // STATUS VARIABLES
    // set when data needs to be saved
    bool changed_;
    // set when data has been evaluated
    bool evaluated_;
    // set while evaluating
    bool evaluating_;
    // initial state, when records are clear
    bool cleared_;
    // true when evaluation is of GCF kind
    bool evaluatingGcf_;
    // true when evaluation is of arbitrary curve kind
    bool evaluatingArbitraryCurve_;
    // true when evaluation is of isocline kind
    bool evaluatingIsoclines_;
    // true when evaluation is of separating curve kind
    bool evaluatingPiecewiseConfig_;
    // true when process failed;
    bool processFailed_;
    // only relevant when processFailed_=true
    QString processError_;

    //////////////////////
    // MEMBER FUNCTIONS //
    //////////////////////

    // MANAGE VECTOR FIELDS
    void addVectorField();
    void deleteVectorField(int);

    // MANAGE SEPARATING CURVES
    void addSeparatingCurve();
    void deleteSeparatingCurve(int);

    // RESAMPLING
    void resampleSeparatingCurve(int);
    void resampleGcf(int);
    void resampleIsoclines(int i);

    // GET VECTOR FIELD INDEX
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

    // MANAGE REGIONS
    void markVFRegion(int, const double *);
    void unmarkVFRegion(int, const double *);
    void markCurveRegion(int, const double *);
    void unmarkCurveRegion(int, const double *);
    void clearVFMarks();
    void clearCurveMarks();
    bool isCurvePointDrawn(int, const double *);

    // COMMON ELEMENTS BETWEEN VFs
    bool hasCommonString(const std::vector<QString> &);
    bool hasCommonInt(const std::vector<int> &);
    bool hasCommonBool(const std::vector<bool> &);
    bool hasCommonParValue(int);
    QString commonString(const std::vector<QString> &);
    int commonInt(const std::vector<int> &);
    bool commonBool(const std::vector<bool> &);
    QString commonParValue(int);
    void setCommonString(std::vector<QString> &, QString);
    void setCommonInt(std::vector<int> &, int);
    void setCommonBool(std::vector<bool> &, bool);
    void setCommonParValue(int, QString);

    // FILENAMES

    // filename.inp
    QString getfilename() const;
    // filename
    QString getbarefilename() const;
    // filename_fin.res
    QString getfilename_finresults() const;
    // filename_inf.res
    QString getfilename_infresults() const;
    // filename_fin.tab
    QString getfilename_fintable() const;
    // filename_inf.tab
    QString getfilename_inftable() const;
    // filename_vec.tab
    QString getfilename_vectable() const;
    // filename_gcf.tab (temporary file)
    QString getfilename_gcf() const;
    // filename.txt
    QString getmaplefilename() const;

    // curve filenames
    // filename_veccurve.tab
    QString getfilename_arbitrarycurvetable() const;
    // filename_curve.tab (tmp)
    QString getfilename_arbitrarycurve() const;
    // filename_curve_prep.txt
    QString getPrepareArbitraryCurveFileName() const;

    // isoclines filenames
    // filename_vecisocline.tab
    QString getfilename_isoclinestable() const;
    // filename_isocline.tab (tmp file)
    QString getfilename_isoclines() const;
    // filename_isocline_prep.txt
    QString getPrepareIsoclinesFileName() const;

    // separating curve filename (filename_sepcurves.tab)
    QString getfilename_separatingcurveresults() const;

    // check if a file exists
    static bool fileExists(QString);

    void getDataFromDlg();

    // ACTIONS
    bool load();
    bool save();
    void reset(int);
    bool checkevaluated();

    // PARAMETER PREPARATION
    void prepareMapleParameters(QTextStream &, bool = false);
    void prepareMapleVectorField(QTextStream &);
    void prepareMaplePiecewiseConfig(QTextStream &);
    void prepareMapleSeparatingCurves(QTextStream &);

    // STRING UTILITIES
    inline QString booleanString(int value) const
    {
        if (value == 0)
            return "false";
        else
            return "true";
    }
    QString convertMapleUserParameterLabels(QString);
    QString convertMapleUserParametersLabelsToValues(QString);

    // EVALUATION: main execution
    void prepareFile(QTextStream &, bool); // used by Prepare()
    void prepare();
    void evaluate();
    void createProcessWindow();

    // EVALUATION: gcf
    bool prepareGcf(const std::vector<p4polynom::term2> &f, double y1,
                    double y2, int precision, int numpoints);
    bool prepareGcf_LyapunovCyl(double theta1, double theta2, int precision,
                                int numpoints, int index);
    bool prepareGcf_LyapunovR2(int precision, int numpoints, int index);
    bool evaluateGcf();

    // EVALUATION: separating curves
    void prepareSeparatingCurves();
    bool evaluateSeparatingCurves();

    // EVALUATION: arbitrary curves
    // called from P4ArbitraryCurveDlg.cpp evaluate button
    void evaluateArbitraryCurveTable();
    // called from evaluateCurveTable
    void prepareArbitraryCurve();
    // called from prepareCurve
    void prepareArbitraryCurveFile(QTextStream &);
    // called from prepareCurveFile
    void prepareMapleArbitraryCurve(QTextStream &);
    // the following are called from math_curve.cpp
    bool prepareArbitraryCurve(const std::vector<p4polynom::term2> &f,
                               double y1, double y2, int precision,
                               int numpoints);
    bool prepareArbitraryCurve_LyapunovCyl(double theta1, double theta2,
                                           int precision, int numpoints);
    bool prepareArbitraryCurve_LyapunovR2(int precision, int numpoints);
    bool evaluateArbitraryCurve();

    // EVALUATION: isoclines
    void evaluateIsoclinesTable();
    void prepareIsoclines();
    void prepareIsoclinesFile(QTextStream &);
    void prepareMapleIsoclines(QTextStream &);
    bool prepareIsoclines(const std::vector<p4polynom::term2> &f, double y1,
                          double y2, int precision, int numpoints); // FIXME
    bool prepareIsoclines_LyapunovCyl(double theta1, double theta2,
                                      int precision, int numpoints, int index);
    bool prepareIsoclines_LyapunovR2(int precision, int numpoints, int index);
    bool evaluateIsoclines();

    // get and set (observed pointers, so no memory freeing here)
    P4FindDlg *getFindDlgPtr() const { return findDlg_; }
    void setFindDlg(P4FindDlg *newdlg) { findDlg_ = newdlg; }

    P4GcfDlg *getGcfDlgPtr() const { return gcfDlg_; }
    void setGcfDlg(P4GcfDlg *newdlg) { gcfDlg_ = newdlg; }

    P4ArbitraryCurveDlg *getArbitraryCurveDlgPtr() const
    {
        return arbitraryCurveDlg_;
    }
    void setArbitraryCurveDlg(P4ArbitraryCurveDlg *newdlg)
    {
        arbitraryCurveDlg_ = newdlg;
    }

    P4IsoclinesDlg *getIsoclinesDlgPtr() const { return isoclinesDlg_; }
    void setIsoclinesDlg(P4IsoclinesDlg *newdlg) { isoclinesDlg_ = newdlg; }

  signals:
    void saveSignal();
    void loadSignal();

  public slots:
    void finishEvaluation(int);
    void catchProcessError(QProcess::ProcessError);
    void readProcessStdout();
    void onTerminateButton();
    void finishGcfEvaluation();
    void finishArbitraryCurveEvaluation();
    void finishIsoclinesEvaluation();
    void finishSeparatingCurvesEvaluation();

  private:
    // P4 GUI ELEMENTS
    P4FindDlg *findDlg_{nullptr};
    P4GcfDlg *gcfDlg_{nullptr};
    P4ArbitraryCurveDlg *arbitraryCurveDlg_{nullptr};
    P4IsoclinesDlg *isoclinesDlg_{nullptr};
};

extern P4InputVF *gThisVF;
