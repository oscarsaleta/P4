#include <qwidget.h>

class QVFParams;
class QFindDlg;
class QBoxLayout;
class QLineEdit;
class QSpinBox;
class QScrollBar;
class QHBoxLayout;
class QLabel;

#ifndef MAXNUMPARAMSSHOWN
#include "custom.h"
#endif

class QVectorFieldDlg : public QWidget
{
    Q_OBJECT

public:
    QVectorFieldDlg( QFindDlg * startwindow );
    void GetDataFromDlg( void );
    void UpdateDlgData( void );
    ~QVectorFieldDlg();

private:
    QBoxLayout * mainLayout;
    QFindDlg * parent;
    QLineEdit * edt_xprime;
    QLineEdit * edt_yprime;
    QLineEdit * edt_gcf;
    QSpinBox * spin_numparams;
    QHBoxLayout * paramLayout;
    QScrollBar * sb_params;
    QVFParams * params;

public slots:
    void numParamsChanged( int );
};

class QVFParams : public QWidget
{
    Q_OBJECT

public:
    QVFParams( QVectorFieldDlg * parent, QScrollBar * sb );
    ~QVFParams();
    bool GetDataFromDlg( void );
    bool UpdateDlgData( void );

private:
    bool datainvalid;
    int currentnumparams;
    int currentshownparams;
    int currentpageindex;

    QScrollBar * sb_params;
    QBoxLayout * mainLayout;
    QBoxLayout * superLayout;
    QHBoxLayout * Param_Lines[MAXNUMPARAMSSHOWN];
    QLineEdit * Param_Names[MAXNUMPARAMSSHOWN];
    QLabel * Param_Labels[MAXNUMPARAMSSHOWN];
    QLineEdit * Param_Values[MAXNUMPARAMSSHOWN];

    QString S_Labels[MAXNUMPARAMS];
    QString S_Values[MAXNUMPARAMS];

protected:
    bool focusNextPrevChild ( bool next );

public slots:
    void paramsSliderChanged( int );
};

