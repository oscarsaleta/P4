#ifndef WIN_LIMITCYCLES_H
#define WIN_LIMITCYCLES_H


#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

#include "win_plot.h"
#include "win_sphere.h"


class QLimitCyclesDlg : public QWidget
{
    Q_OBJECT

public:
    QLimitCyclesDlg( QPlotWnd *, QWinSphere * );
    void Reset( void );
    void setSection( double, double, double, double );
    void showEvent( QShowEvent * );
    void hideEvent( QHideEvent * );

private:
    QWinSphere * mainSphere;
    QPlotWnd * plotwnd;

    QPushButton * btn_start;
    QPushButton * btn_cancel;
    QPushButton * btn_delall;
    QPushButton * btn_dellast;

    QLineEdit * edt_x0;
    QLineEdit * edt_y0;
    QLineEdit * edt_x1;
    QLineEdit * edt_y1;
    QLineEdit * edt_grid;

    QSpinBox * spin_numpoints;

    QBoxLayout * mainLayout;

    double selected_x0;
    double selected_y0;
    double selected_x1;
    double selected_y1;
    double selected_grid;
    int selected_numpoints;

public slots:
    void onbtn_start( void );
    void onbtn_cancel( void );
    void onbtn_delall( void );
    void onbtn_dellast( void );
};


#endif
