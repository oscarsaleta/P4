#include <qwidget.h>

class QRadioButton;
class QWinSphere;
class QSpinBox;
class QPlotWnd;
class QPushButton;
class QLineEdit;
class QBoxLayout;

class QGcfDlg : public QWidget
{
    Q_OBJECT

public:
    QGcfDlg( QPlotWnd *, QWinSphere * );
    void Reset( void );
    void finishGcfEvaluation( void );

private:
    QWinSphere * mainSphere;
    QPlotWnd * plotwnd;

    QPushButton * btn_evaluate;

    QRadioButton * btn_dots;
    QRadioButton * btn_dashes;
    QLineEdit * edt_points;
    QLineEdit * edt_precis;
    QLineEdit * edt_memory;

    QBoxLayout * mainLayout;

    void ExclusiveToggle( bool, QRadioButton *, QRadioButton * );

    int evaluating_points;
    int evaluating_memory;
    int evaluating_precision;

public slots:
    void onbtn_evaluate( void );
    void btn_dots_toggled( bool );
    void btn_dashes_toggled( bool );
};
