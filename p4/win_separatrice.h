#ifndef WIN_SEPARATRICES_H
#define WIN_SEPARATRICES_H


#include "win_sphere.h"
#include "win_plot.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>


extern QString CurrentSingularityInfo[];
extern double CurrentSeparatriceEpsilon;

class QSepDlg : public QWidget
{
    Q_OBJECT

public:
    QSepDlg( QPlotWnd *, QWinSphere * );
    void Reset( void );

private:
    QWinSphere * mainSphere;
    QPlotWnd * plotwnd;

    QPushButton * btn_selectnext;
    QPushButton * btn_intnext;
    QPushButton * btn_start;
    QPushButton * btn_cont;
    QLineEdit * edt_epsilon;
    QLabel * lbl_info[4];

    QBoxLayout * mainLayout;

    bool started;
    bool selected;

public slots:
    void onbtn_selectnext( void );
    void onbtn_intnext( void );
    void onbtn_cont( void );
    void onbtn_start( void );

    void setInitialPoint( void );
    void SepEvent( int );
    void onepsilon_enter( void );
    void MarkBad( QLineEdit * );
};


#endif /* WIN_SEPARATRICES_H */
