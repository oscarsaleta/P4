/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
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
#include "win_gcf.h"

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

class QInputVF : public QObject
{
    Q_OBJECT
  public:
    QString filename;

    int symbolicpackage; // 0 for reduce, 1 for maple
    int typeofstudy;     // 0, 1, 2, 3 = all, inf, fin, one

    bool numeric;
    int precision;
    int precision0;
    QString epsilon;
    bool testsep;
    int taylorlevel;
    int numericlevel;
    int maxlevel;
    int weakness;
    QString x0;
    QString y0;
    int p;
    int q;

    QString xdot;
    QString ydot;
    QString gcf;
    QString curve;
    QString EvalFile;
    QString EvalFile2;

    QProcess *EvalProcess;
    QTextEdit *ProcessText;
    QPushButton *ProcessButton;
    QPushButton *ProcessClearButton;

    int numparams;
    QString parlabel[MAXNUMPARAMS];
    QString parvalue[MAXNUMPARAMS];

    bool changed;         // set when data needs to be saved
    bool evaluated;       // set when data has been evaluated
    bool evaluating;      // set while evaluating
    bool cleared;         // initial state, when records are clear
    bool evaluatinggcf;   // true when evaluation is of GCF kind
    bool evaluatingCurve;
    bool processfailed;   // true when process failed;
    QString processError; // only relevant when processfailed=true
    QGcfDlg *gcfDlg;
    QCurveDlg *curveDlg;

  public:
    QInputVF();

    QString getfilename(void) const;            // filename.inp
    QString getbarefilename(void) const;        // filename
    QString getfilename_finresults(void) const; // filename_fin.res
    QString getfilename_infresults(void) const; // filename_inf.res
    QString getfilename_fintable(void) const;   // filename_fin.tab
    QString getfilename_inftable(void) const;   // filename_inf.tab
    QString getfilename_vectable(void) const;   // filename_vec.tab
    QString getfilename_gcf(void) const; // filename_gcf.tab (temporary file)
    QString getfilename_gcfresults(void)
        const; // filename_gcf.res (temporary file, only used in case of reduce)
    QString getfilename_curve(void) const; // filename_curve.tab (temporary file)
    QString getreducefilename(void) const; // filename.red
    QString getmaplefilename(void) const;  // filename.mpl
    QString getrunfilename(void) const;    // filename.run

    bool load(void);
    bool save(void);
    void reset(void);
    bool checkevaluated(void);

    static bool FileExists(QString);

    void prepareReduceParameters(QTextStream *);
    void prepareReduceVectorField(QTextStream *);
    void prepareMapleParameters(QTextStream *);
    void prepareMapleVectorField(QTextStream *);
    QString booleanString(int value) const;
    QString convertMapleUserParameterLabels(QString);
    QString convertReduceUserParameterLabels(QString);

    void prepareFile(QTextStream *); // used by Prepare()

    void prepare(void);
    void evaluate(void);

    bool prepareGcf(struct term2 *f, double, double, int, int);
    bool prepareGcf_LyapunovCyl(double, double, int, int);
    bool prepareGcf_LyapunovR2(int, int);
    bool evaluateGcf(void);
    
    bool prepareCurve(struct term2 *f, double, double, int, int);
    bool prepareCurve_LyapunovCyl(double, double, int, int);
    bool prepareCurve_LyapunovR2(int, int);
    bool evaluateCurve(void);

    void createProcessWindow(void);

  public slots:
    void finishEvaluation(int);
    void catchProcessError(QProcess::ProcessError);
    void ReadProcessStdout(void);
    void onTerminateButton(void);
    void onClearButton(void);
    void finishGcfEvaluation(void);
};

extern QInputVF *ThisVF;

#endif /* FILE_VF_H */
