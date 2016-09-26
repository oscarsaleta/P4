#include <qmainwindow.h>

class QIntParamsDlg;
class QWinSphere;
class QStartDlg;
class QLegendWnd;
class QOrbitsDlg;
class QSepDlg;
class QPlotWnd;
class QAction;
class QBoxLayout;

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
