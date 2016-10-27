#ifndef WIN_ORBITS_H
#define WIN_ORBITS_H


#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include "win_plot.h"
#include "win_sphere.h"


class QOrbitsDlg : public QWidget
{
    Q_OBJECT

public:
    QOrbitsDlg( QPlotWnd *, QWinSphere * );
    void Reset( void );

private:
    QWinSphere * mainSphere;
    QPlotWnd * plotwnd;

    QPushButton * btn_forwards;
    QPushButton * btn_continue;
    QPushButton * btn_backwards;
    QPushButton * btn_delall;
    QPushButton * btn_dellast;
    QPushButton * btn_select;

    QLineEdit * edt_x0;
    QLineEdit * edt_y0;

    QBoxLayout * mainLayout;

    double selected_x0;
    double selected_y0;

    bool orbit_started;
    bool orbit_selected;

public slots:
    void OrbitEvent( int );
    void onbtn_select( void );
    void onbtn_backwards( void );
    void onbtn_continue( void );
    void onbtn_forwards( void );
    void onbtn_delall( void );
    void onbtn_dellast( void );

    void setInitialPoint( double, double );
};


#endif /* WIN_ORBITS_H */
