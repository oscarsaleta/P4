#include <qprogressdialog.h>
#include <qnamespace.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <math.h>
#include "custom.h"
#include "file_tab.h"
#include "main.h"
#include "math_p4.h"
#include "p4application.h"
#include "win_limitcycles.h"
#include "win_plot.h"
#include "win_sphere.h"

bool lcWindowIsUp = false;      // see definition in main.h

extern void setP4WindowTitle( QWidget *, QString );
extern QP4Application * p4app;
extern void searchLimitCycle( QWinSphere *, double, double, double, double, double );
extern void deleteLastLimitCycle( QWinSphere * spherewnd );

static QProgressDialog * LCprogressDlg = nullptr;
static int LCprogressCount=1;
static int LCmaxProgressCount=0;

QLimitCyclesDlg::QLimitCyclesDlg( QPlotWnd * plt, QWinSphere * sp )
    : QWidget(nullptr,Qt::Tool | Qt::WindowStaysOnTopHint)
{
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    mainSphere = sp;
    plotwnd = plt;

    edt_x0 = new QLineEdit( "", this );
    QLabel * lbl1 = new QLabel( "x0 = ", this );
    edt_y0 = new QLineEdit( "", this );
    QLabel * lbl2 = new QLabel( "y0 = ", this );
    edt_x1 = new QLineEdit( "", this );
    QLabel * lbl3 = new QLabel( "x1 = ", this );
    edt_y1 = new QLineEdit( "", this );
    QLabel * lbl4 = new QLabel( "y1 = ", this );
    edt_grid = new QLineEdit( "", this );
    QLabel * lbl5 = new QLabel( "Grid: ", this );

    spin_numpoints = new QSpinBox(this);
    spin_numpoints->setMinimum(MIN_LCPOINTS);
    spin_numpoints->setMaximum(MAX_LCPOINTS);
    QLabel * lbl6 = new QLabel( "# Points: ", this );

    btn_start = new QPushButton( "&Start", this );
    btn_cancel = new QPushButton( "&Reset setpoints", this );
    btn_dellast = new QPushButton( "&Delete Last LC", this );
    btn_delall = new QPushButton( "Delete &All LC", this );

#ifdef TOOLTIPS
    edt_x0->setToolTip( "Start point of the transverse section.\n"
                            "You can also click on the plot window to fill this field." );
    edt_y0->setToolTip( "Start point of the transverse section.\n"
                            "You can also click on the plot window to fill this field." );
    edt_x1->setToolTip( "End point of the transverse section.\n"
                            "You can also click on the plot window to fill this field." );
    edt_y1->setToolTip( "End point of the transverse section.\n"
                            "You can also click on the plot window to fill this field." );
    spin_numpoints->setToolTip( "Number of points to integrate for each orbit" );
    edt_grid->setToolTip( "Diameter of intervals dividing the transverse section" );
    btn_start->setToolTip( "Start integrating from the transverse section,\n"
                            "looking for the presence of a limit cycle" );
    btn_dellast->setToolTip( "Delete last limit cycle" );
    btn_delall->setToolTip( "Delete all limit cycles" );
    btn_cancel->setToolTip( "Reset set points" );
#endif

    // layout

    mainLayout = new QBoxLayout( QBoxLayout::TopToBottom, this );

    QGridLayout * lay00 = new QGridLayout();
    lay00->addWidget( lbl1, 0, 0 );
    lay00->addWidget( edt_x0, 0, 1 );
    lay00->addWidget( lbl2, 0, 2 );
    lay00->addWidget( edt_y0, 0, 3 );
    lay00->addWidget( lbl3, 1, 0 );
    lay00->addWidget( edt_x1, 1, 1 );
    lay00->addWidget( lbl4, 1, 2 );
    lay00->addWidget( edt_y1, 1, 3 );

    QHBoxLayout * layout1 = new QHBoxLayout();
    layout1->addWidget( lbl5 );
    layout1->addWidget( edt_grid );

    QHBoxLayout * layout2 = new QHBoxLayout();
    layout2->addWidget( btn_start );
    layout2->addWidget( btn_cancel );

    QHBoxLayout * layout3 = new QHBoxLayout();
    layout3->addWidget( lbl6 );
    layout3->addWidget( spin_numpoints );

    QHBoxLayout * layout4 = new QHBoxLayout();
    layout4->addWidget( btn_dellast );
    layout4->addWidget( btn_delall );

    mainLayout->addLayout( lay00 );
    mainLayout->addLayout( layout1 );
    mainLayout->addLayout( layout3 );
    mainLayout->addLayout( layout2 );
    mainLayout->addLayout( layout4 );

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    // connections

    QObject::connect( btn_start,        SIGNAL(clicked()), this, SLOT(onbtn_start()) );
    QObject::connect( btn_cancel,       SIGNAL(clicked()), this, SLOT(onbtn_cancel()) );
    QObject::connect( btn_delall,       SIGNAL(clicked()), this, SLOT(onbtn_delall()) );
    QObject::connect( btn_dellast,      SIGNAL(clicked()), this, SLOT(onbtn_dellast()) );

    // finishing

    selected_x0 = 0;
    selected_y0 = 0;
    selected_x1 = 0;
    selected_y1 = 0;
    selected_grid = DEFAULT_LCGRID;
    selected_numpoints = DEFAULT_LCPOINTS;

    spin_numpoints->setValue( selected_numpoints );
    QString buf;
    buf.sprintf( "%g", (float)selected_grid );
    edt_grid->setText( buf );

    if( VFResults.first_lim_cycle == nullptr )
    {
        btn_delall->setEnabled(false);
        btn_dellast->setEnabled(false);
    }

    setP4WindowTitle( this, "Limit Cycles" );
}

void QLimitCyclesDlg::setSection( double x0, double y0, double x1, double y1 )
{
    QString buf;
    QString bufx;
    QString bufy;
    bufx.sprintf( "%g", (float)x0 );
    bufy.sprintf( "%g", (float)y0 );
    edt_x0->setText( bufx );
    edt_y0->setText( bufy );

    bufx.sprintf( "%g", (float)x1 );
    bufy.sprintf( "%g", (float)y1 );
    edt_x1->setText( bufx );
    edt_y1->setText( bufy );
}

void QLimitCyclesDlg::onbtn_start( void )
{
    double d;
    QString bufx;
    QString bufy;
    QString buf;
    bool empty;

    plotwnd->getDlgData();
    
    bufx = edt_x0->text();
    bufy = edt_y0->text();

    empty=false;

    if( bufx.length() == 0 || bufy.length() == 0 )
    {
        empty=true;
    }

    selected_x0 = bufx.toDouble();
    selected_y0 = bufy.toDouble();

    bufx = edt_x1->text();
    bufy = edt_y1->text();

    if( bufx.length() == 0 || bufy.length() == 0 || empty )
    {
        QMessageBox::critical( this, "P4",
            "Please enter setpoint coordinates for the transverse section.\n"
            "You can also click on the poincare sphere with the left mouse button\n"
            "while the limit cycle window is opened.\n" );
        return;
    }

    selected_x1 = bufx.toDouble();
    selected_y1 = bufy.toDouble();

    buf = edt_grid->text();
    selected_grid = buf.toDouble();

    selected_numpoints = spin_numpoints->value();

    d = (selected_x0 - selected_x1)*(selected_x0 - selected_x1);
    d += (selected_y0 - selected_y1)*(selected_y0 - selected_y1);
    d = sqrt(d);
    if( selected_grid > d || selected_grid < MIN_LCGRID || selected_grid > MAX_LCGRID )
    {
        QMessageBox::critical( this, "P4",
            "Grid size is either too big or too small for this transverse section." );
        return;
    }

    d /= selected_grid;
    if( d < MIN_LCORBITS || d > MAX_LCORBITS )
    {
        QMessageBox::critical( this, "P4",
            "Grid size is either too big or too small for this transverse section." );
        return;
    }

    // SEARCH FOR LIMIT CYCLES:

    LCmaxProgressCount = (int)(d+0.5);
    LCprogressDlg = new QProgressDialog( "Searching for limit cycles...", "Stop search",
                                            0, LCmaxProgressCount, this, 0 );
    LCprogressDlg->setAutoReset(false);
    LCprogressDlg->setAutoClose(false);
    LCprogressDlg->setMinimumDuration(0);
    LCprogressDlg->setValue(LCprogressCount=0);
    setP4WindowTitle( LCprogressDlg, "Searching for limit cycles..." );

    searchLimitCycle( mainSphere, selected_x0, selected_y0, selected_x1, selected_y1, selected_grid );

    // update buttons

    if( VFResults.first_lim_cycle == nullptr )
    {
        btn_delall->setEnabled(false);
        btn_dellast->setEnabled(false);
    }
    else
    {
        btn_delall->setEnabled(true);
        btn_dellast->setEnabled(true);
    }

    p4app->processEvents();
    delete LCprogressDlg;
    LCprogressDlg = nullptr;
}

void QLimitCyclesDlg::onbtn_cancel( void )
{
    edt_x0->setText("");
    edt_y0->setText("");
    edt_x1->setText("");
    edt_y1->setText("");
}

void QLimitCyclesDlg::Reset( void )
{
    // finishing

    selected_x0 = 0;
    selected_y0 = 0;
    selected_x1 = 0;
    selected_y1 = 0;
    selected_grid = DEFAULT_LCGRID;
    selected_numpoints = DEFAULT_LCPOINTS;

    edt_x0->setText("");
    edt_y0->setText("");
    edt_x1->setText("");
    edt_y1->setText("");
    
    QString buf;
    buf.sprintf( "%g", (float)selected_grid );
    edt_grid->setText( buf );
    spin_numpoints->setValue( selected_numpoints );

    if( VFResults.first_lim_cycle == nullptr )
    {
        btn_delall->setEnabled(false);
        btn_dellast->setEnabled(false);
    }
    else
    {
        btn_delall->setEnabled(true);
        btn_dellast->setEnabled(true);
    }
}

void QLimitCyclesDlg::showEvent( QShowEvent * se )
{
    QWidget::showEvent(se);
    lcWindowIsUp = true;
}

void QLimitCyclesDlg::hideEvent( QHideEvent * he )
{
    QWidget::hideEvent(he);
    lcWindowIsUp = false;
}


void QLimitCyclesDlg::onbtn_delall( void )
{
    plotwnd->getDlgData();

    btn_delall->setEnabled(false);
    btn_dellast->setEnabled(false);

    VFResults.deleteOrbit( VFResults.first_lim_cycle );
    VFResults.first_lim_cycle = nullptr;
    VFResults.current_lim_cycle = nullptr;

    mainSphere->refresh();
}

void QLimitCyclesDlg::onbtn_dellast( void )
{
    plotwnd->getDlgData();

    mainSphere->prepareDrawing();
    deleteLastLimitCycle( mainSphere );
    mainSphere->finishDrawing();

    if( VFResults.first_lim_cycle == nullptr )
    {
        btn_delall->setEnabled(false);
        btn_dellast->setEnabled(false);
    }
}

bool stop_search_limit( void )
{
    p4app->processEvents();
    if( LCprogressDlg->wasCanceled() )
        return true;

    return false;
}

void write_to_limit_window( double x, double y )
{
    UNUSED(x);
    UNUSED(y);
    LCprogressCount++;

    if( !(LCprogressDlg->wasCanceled()) )
        LCprogressDlg->setValue( LCprogressCount );
}
