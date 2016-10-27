#ifndef WIN_PLOT_H
#define WIN_PLOT_H


#include <QAction>
#include <QBoxLayout>
#include <QEvent>
#include <QHideEvent>
#include <QMainWindow>

#include "win_main.h"
#include "win_legend.h"
#include "win_sphere.h"
#include "win_view.h"


/* Forward-declarations to solve cross-include problems */
class QGcfDlg;          // in win_gcf.h
class QIntParamsDlg;    // in win_params.h
class QSepDlg;          // in win_separatrices.h
class QOrbitsDlg;       // in win_orbits.h
class QLimitCyclesDlg;  // in win_limitcycles.h
class QZoomWnd;         // in win_zoom.h

class QPlotWnd : public QMainWindow
{
    Q_OBJECT

public:
    QPlotWnd( QStartDlg * );
    ~QPlotWnd();

private:
    QStartDlg * parent;
    QBoxLayout * mainLayout;
    QAction * ActClose;
    QAction * ActRefresh;
    QAction * ActLegend;
    QAction * ActOrbits;
    QAction * ActIntParams;
    QAction * ActView;
    QAction * ActGCF;
    QAction * ActPlotSep;
    QAction * ActPlotAllSeps;
    QAction * ActLimitCycles;
    QAction * ActPrint;

    QLegendWnd * Legend_Window;
    QOrbitsDlg * Orbits_Window;
    QSepDlg * Sep_Window;
    QLimitCyclesDlg * LC_Window;
    QGcfDlg * GCF_Window;

    QWinSphere * sphere;                // main sphere

    int numZooms;
    int lastZoomIdentifier;
    QZoomWnd * * ZoomWindows;

public:
    QIntParamsDlg * IntParams_Window;
    QViewDlg * ViewParams_Window;

public slots:
    void Signal_Evaluating( void );
    void Signal_Evaluated( void );
    void Signal_Changed( void );

    void OnBtnClose( void );
    void OnBtnRefresh( void );
    void OnBtnLegend( void );
    void OnBtnOrbits( void );
    void OnBtnIntParams( void );
    void OnBtnView( void );
    void OnBtnGCF( void );
    void OnBtnPlotSep( void );
    void OnBtnPlotAllSeps( void );
    void OnBtnLimitCycles( void );
    void OnBtnPrint( void );
    bool close( void );

    void OpenZoomWindow( double, double, double, double );
    void CloseZoomWindow( int id );
    void configure( void );
    void customEvent( QEvent * e );
    void hideEvent ( QHideEvent * h );
    void getDlgData( void );
    void AdjustHeight( void );
};


#endif /* WIN_PLOT_H */
