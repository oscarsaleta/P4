#ifndef WIN_VF_H
#define WIN_VF_H


#include "win_find.h"
#include "custom.h"

#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include <QSpinBox>
#include <QWidget>


class QVFParams; // declare them first because both classes defined in
                 // this file need each other*/

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


#endif /* WIN_VF_H */
