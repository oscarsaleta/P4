#include <qwidget.h>

#define TYPE_SIGNAL_EVALUATING      (QEvent::User + 1)
#define TYPE_SIGNAL_EVALUATED       (QEvent::User + 2)
#define TYPE_SIGNAL_CHANGED         (QEvent::User + 3)
#define TYPE_SIGNAL_LOADED          (QEvent::User + 4)
#define TYPE_SIGNAL_SAVED           (QEvent::User + 5)

#define TYPE_SELECT_ORBIT           (QEvent::User + 6)
#define TYPE_CLOSE_PLOTWINDOW       (QEvent::User + 7)
#define TYPE_ORBIT_EVENT            (QEvent::User + 8)
#define TYPE_SEP_EVENT              (QEvent::User + 9)
#define TYPE_CLOSE_ZOOMWINDOW       (QEvent::User + 10)
#define TYPE_OPENZOOMWINDOW         (QEvent::User + 11)
#define TYPE_SELECT_LCSECTION       (QEvent::User + 12) 

struct DOUBLEPOINT
{
    double x;
    double y;
};

class QFindDlg;
class QAction;
class QPlotWnd;
class QBoxLayout;
class QPushButton;
class QLineEdit;
class QMenu;
class QP4Event;

class QStartDlg : public QWidget
{
    Q_OBJECT
        
public:
    QStartDlg( const QString & );
    void closeEvent( QCloseEvent* ce );

public slots:
    // following slots are called by QT when a button is pressed or a file name is changed:

    void OnQuit();
    void OnPlot();
    void OnViewFinite();
    void OnViewInfinite();
    void OnHelp();
    void OnFind();
    void OnBrowse();
    void OnAbout();
    void OnFilenameChange( const QString & );
    QWidget * Showtext( QWidget * win, QString caption, QString fname );

    // following slots are called by other P4 widgets when something happened:

public:
    void Signal_Evaluating( void );
    void Signal_Evaluated( void );
    void Signal_Changed( void );
    void Signal_Loaded( void );
    void Signal_Saved( void );

    void customEvent( QEvent * e );

private:
    QBoxLayout * mainLayout;
    QPushButton * btn_quit;
    QPushButton * btn_find;
    QPushButton * btn_view;
    QPushButton * btn_plot;
    QPushButton * btn_help;
    QPushButton * btn_about;
    QLineEdit * edt_name;
    QMenu * viewmenu;

    QWidget * Help_Window;
    QWidget * View_Infinite_Window;
    QWidget * View_Finite_Window;
public:
    QFindDlg * Find_Window;
    QPlotWnd * Plot_Window;
};
