#ifndef P4INPUTVF_H
#define P4INPUTVF_H



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


namespace p4vfStudyRegions
{
struct vfRegion {
    int vfIndex;
    int *signs;
}

struct curveRegion {
    int curveIndex;
    int *signs;
}
}

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

    std::vector<QString> xdot_; //TODO:....
    std::vector<QString> ydot_;
    std::vector<QString> gcf_;
    QString curve_;
    QString isoclines_;
    QString evalFile_;
    QString evalFile2_;

    QWidget *outputWindow_;
    QProcess *evalProcess_;
    QTextEdit *processText_;
    QPushButton *terminateProcessButton_;
    QPushButton *clearProcessButton_;

    /* parameters list */
    int numparams_;
    QString parlabel_[MAXNUMPARAMS];
    QString parvalue_[MAXNUMPARAMS];

    // p5 modifications
    int numVFRegions_;
    int numCurveRegions_;
    std::vector<vfRegion> vfRegions; // TODO: are they vectors or just pointers?
    curveRegion *curveRegions;
    // --

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

    bool prepareGcf(term2 *f, double, double, int, int);
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
    bool prepareCurve(term2 *f, double, double, int, int);
    bool prepareCurve_LyapunovCyl(double, double, int, int);
    bool prepareCurve_LyapunovR2(int, int);
    bool evaluateCurve();

    /* isoclines functions */
    void evaluateIsoclinesTable();
    void prepareIsoclines();
    void prepareIsoclinesFile(QTextStream *);
    void prepareMapleIsoclines(QTextStream *);
    bool prepareIsoclines(term2 *f, double, double, int, int);
    bool prepareIsoclines_LyapunovCyl(double, double, int, int);
    bool prepareIsoclines_LyapunovR2(int, int);
    bool evaluateIsoclines();

    void createProcessWindow();

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


#endif /* P4INPUTVF_H */