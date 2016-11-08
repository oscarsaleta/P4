#ifndef WIN_ZOOM_H
#define WIN_ZOOM_H


#include "win_plot.h"
#include "win_sphere.h"

#include <QAction>
#include <QBoxLayout>
#include <QEvent>
#include <QHideEvent>
#include <QMainWindow>


class QZoomWnd : public QMainWindow
{
    Q_OBJECT

public:
    QZoomWnd( QPlotWnd *, int, double x1, double y1, double x2, double y2 );
    ~QZoomWnd();

    int zoomid;

private:
    QPlotWnd * parent;
    QBoxLayout * mainLayout;
    QAction * ActClose;
    QAction * ActRefresh;
    QAction * ActPrint;

    QWinSphere * sphere;                // main sphere

public slots:
    void Signal_Evaluating( void );
    void Signal_Evaluated( void );
    void Signal_Changed( void );

    void OnBtnClose( void );
    void OnBtnRefresh( void );
    void OnBtnPrint( void );
    bool close( void );

    void configure( void );
    void customEvent( QEvent * e );
    void hideEvent( QHideEvent * h );
    void AdjustHeight( void );
};


#endif /* WIN_ZOOM_H */
