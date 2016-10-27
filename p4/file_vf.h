#ifndef FILE_VF_H
#define FILE_VF_H


#include <QObject>
#include <QProcess>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QTextStream>

#include "custom.h"
#include "win_gcf.h"


#define TYPEOFSTUDY_ALL     0
#define TYPEOFSTUDY_FIN     1
#define TYPEOFSTUDY_INF     2
#define TYPEOFSTUDY_ONE     3

#define PACKAGE_MAPLE       1
#define PACKAGE_REDUCE      0


class QInputVF : public QObject
{
Q_OBJECT
public:
    QString filename;

    int symbolicpackage;        // 0 for reduce, 1 for maple
    int typeofstudy;            // 0, 1, 2, 3 = all, inf, fin, one

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
    QString EvalFile;
    QString EvalFile2;
    
    QProcess * EvalProcess;
    QTextEdit * ProcessText;
    QPushButton * ProcessButton;
    QPushButton * ProcessClearButton;

    int numparams;
    QString parlabel[MAXNUMPARAMS];
    QString parvalue[MAXNUMPARAMS];

    bool changed;               // set when data needs to be saved
    bool evaluated;             // set when data has been evaluated
    bool evaluating;            // set while evaluating
    bool cleared;               // initial state, when records are clear
    bool evaluatinggcf;         // true when evaluation is of GCF kind
    bool processfailed;         // true when process failed;
    QString processError;       // only relevant when processfailed=true
    QGcfDlg * gcfDlg;

public:
    QInputVF();
    
    QString getfilename( void ) const;              // filename.inp
    QString getbarefilename( void ) const;          // filename
    QString getfilename_finresults( void ) const;   // filename_fin.res
    QString getfilename_infresults( void ) const;   // filename_inf.res
    QString getfilename_fintable( void ) const;     // filename_fin.tab
    QString getfilename_inftable( void ) const;     // filename_inf.tab
    QString getfilename_vectable( void ) const;     // filename_vec.tab
    QString getfilename_gcf( void ) const;          // filename_gcf.tab (temporary file)
    QString getfilename_gcfresults( void ) const;   // filename_gcf.res (temporary file, only used in case of reduce)
    QString getreducefilename( void ) const;        // filename.red
    QString getmaplefilename( void ) const;         // filename.mpl
    QString getrunfilename( void ) const;           // filename.run

    bool load( void );
    bool save( void );
    void reset( void );
    bool checkevaluated( void );

    static bool FileExists( QString );

    void PrepareReduceParameters( QTextStream * );
    void PrepareReduceVectorField( QTextStream * );
    void PrepareMapleParameters( QTextStream * );
    void PrepareMapleVectorField( QTextStream * );
    QString booleanString( int value ) const;
    QString ConvertMapleUserParameterLabels( QString );
    QString ConvertReduceUserParameterLabels( QString );

    void PrepareFile( QTextStream * ); // used by Prepare()

    void Prepare( void );
    void Evaluate( void );

    bool PrepareGcf( struct term2 * f, double, double, int, int );
    bool PrepareGcf_LyapunovCyl( double, double, int, int );
    bool PrepareGcf_LyapunovR2( int, int );
    bool EvaluateGcf( void );
    void CreateProcessWindow( void );

public slots:
    void finishEvaluation( int );
    void catchProcessError( QProcess::ProcessError );
    void ReadProcessStdout( void );
    void OnTerminateButton( void );
    void OnClearButton( void );
    void finishGcfEvaluation( void );
};

extern QInputVF * ThisVF;


#endif /* FILE_VF_H */
