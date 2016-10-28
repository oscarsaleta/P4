#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include "custom.h"
#include "file_tab.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_orbits.h"
#include "win_plot.h"
#include "win_sphere.h"
#include "p4application.h"
#include "math_orbits.h"

extern bool StartOrbit( QWinSphere *, double, double, bool );
extern void DeleteLastOrbit( QWinSphere * );
extern void SetP4WindowTitle( QWidget *, QString );

QOrbitsDlg::QOrbitsDlg( QPlotWnd * plt, QWinSphere * sp )
    : QWidget(nullptr,Qt::Tool | Qt::WindowStaysOnTopHint)
{
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    mainSphere = sp;
    plotwnd = plt;

    edt_x0 = new QLineEdit( "", this );
    QLabel * lbl1 = new QLabel( "&x0 = ", this ); lbl1->setBuddy( edt_x0 );
    edt_y0 = new QLineEdit( "", this );
    QLabel * lbl2 = new QLabel( "&y0 = ", this ); lbl2->setBuddy( edt_y0 );

    btn_select = new QPushButton( "&Select", this );

    btn_forwards = new QPushButton( "&Forwards", this );
    btn_continue = new QPushButton( "&Continue", this );
    btn_backwards = new QPushButton( "&Backwards", this );
    btn_dellast = new QPushButton( "&Delete Last Orbit", this );
    btn_delall = new QPushButton( "Delete &All Orbits", this );

#ifdef TOOLTIPS
    edt_x0->setToolTip( "Start point of orbit.\n"
                            "You can also click on the plot window to fill this field." );
    edt_y0->setToolTip( "Start point of orbit.\n"
                            "You can also click on the plot window to fill this field." );
    btn_select->setToolTip( "Validate your choice of (x0,y0).\n"
                            "When using the mouse, this is not necessary." );
    btn_forwards->setToolTip( "Start integrating the orbit in forward time" );
    btn_backwards->setToolTip( "Start integrating the orbit in backward time" );
    btn_continue->setToolTip( "Continue integrating the orbit in the chosen time direction" );
    btn_dellast->setToolTip( "Delete last orbit drawn" );
    btn_delall->setToolTip( "Delete all orbits (separatrices remain)" );
#endif

    // layout

    mainLayout = new QBoxLayout( QBoxLayout::TopToBottom, this );

    QGridLayout * lay00 = new QGridLayout();
    lay00->addWidget( lbl1, 0, 0 );
    lay00->addWidget( edt_x0, 0, 1 );
    lay00->addWidget( lbl2, 1, 0 );
    lay00->addWidget( edt_y0, 1, 1 );
    lay00->addWidget( btn_select, 0, 2, 2, 1 );

    QHBoxLayout * layout1 = new QHBoxLayout();
    layout1->addWidget( btn_forwards );
    layout1->addWidget( btn_continue );
    layout1->addWidget( btn_backwards );
    layout1->addStretch(0);

    QHBoxLayout * layout2 = new QHBoxLayout();
    layout2->addWidget( btn_dellast );
    layout2->addWidget( btn_delall );
    layout2->addStretch(0);

    mainLayout->addLayout( lay00 );
    mainLayout->addLayout( layout1 );
    mainLayout->addLayout( layout2 );

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    // connections

    QObject::connect( btn_select,       SIGNAL(clicked()), this, SLOT(onbtn_select()) );
    QObject::connect( btn_forwards,     SIGNAL(clicked()), this, SLOT(onbtn_forwards()) );
    QObject::connect( btn_backwards,    SIGNAL(clicked()), this, SLOT(onbtn_backwards()) );
    QObject::connect( btn_continue,     SIGNAL(clicked()), this, SLOT(onbtn_continue()) );
    QObject::connect( btn_delall,       SIGNAL(clicked()), this, SLOT(onbtn_delall()) );
    QObject::connect( btn_dellast,      SIGNAL(clicked()), this, SLOT(onbtn_dellast()) );

    // finishing

    selected_x0 = 0;
    selected_y0 = 0;

    btn_forwards->setEnabled(false);
    btn_backwards->setEnabled(false);
    btn_continue->setEnabled(false);

    if( VFResults.first_orbit == nullptr )
    {
        btn_delall->setEnabled(false);
        btn_dellast->setEnabled(false);
    }
    orbit_started = false;
    orbit_selected = false;

    SetP4WindowTitle( this, "Plot Orbits" );
}

void QOrbitsDlg::setInitialPoint( double x, double y )
{
    QString bufx;
    QString bufy;

    plotwnd->getDlgData();

    selected_x0 = x;
    selected_y0 = y;

    bufx.sprintf( "%g", (float)x );
    bufy.sprintf( "%g", (float)y );

    edt_x0->setText( bufx );
    edt_y0->setText( bufy );
    orbit_started = false;
    orbit_selected = true;
    btn_forwards->setEnabled(true);
    btn_backwards->setEnabled(true);
    btn_continue->setEnabled(false);
}

void QOrbitsDlg::onbtn_select( void )
{
    plotwnd->getDlgData();

    QString bufx;
    QString bufy;

    bufx = edt_x0->text();
    bufy = edt_y0->text();

    selected_x0 = bufx.toDouble();
    selected_y0 = bufy.toDouble();
    orbit_started = false;
    orbit_selected = true;

    btn_forwards->setEnabled(true);
    btn_backwards->setEnabled(true);
    btn_continue->setEnabled(false);
}

void QOrbitsDlg::onbtn_backwards( void )
{
    plotwnd->getDlgData();

    if( !orbit_started )
    {
        if( !orbit_selected )
            return;

        mainSphere->prepareDrawing();
        orbit_started = StartOrbit( mainSphere, selected_x0, selected_y0, true );
        mainSphere->finishDrawing();

        if( orbit_started )
        {
            btn_delall->setEnabled(true);
            btn_dellast->setEnabled(true);
        }
    }

    if( orbit_started )
    {
        mainSphere->prepareDrawing();
        IntegrateOrbit( mainSphere, -1 );
        mainSphere->finishDrawing();

        btn_backwards->setEnabled(false);
        btn_continue->setEnabled(true);
    }
}

void QOrbitsDlg::onbtn_continue( void )
{
    plotwnd->getDlgData();

    if( orbit_started )
    {
        mainSphere->prepareDrawing();
        IntegrateOrbit( mainSphere, 0 );
        mainSphere->finishDrawing();
    }
}

void QOrbitsDlg::onbtn_forwards( void )
{
    plotwnd->getDlgData();

    if( !orbit_started )
    {
        if( !orbit_selected )
            return;

        mainSphere->prepareDrawing();
        orbit_started = StartOrbit( mainSphere, selected_x0, selected_y0, true );
        mainSphere->finishDrawing();

        if( orbit_started )
        {
            btn_delall->setEnabled(true);
            btn_dellast->setEnabled(true);
        }
    }

    if( orbit_started )
    {
        mainSphere->prepareDrawing();
        IntegrateOrbit( mainSphere, 1 );
        mainSphere->finishDrawing();

        btn_forwards->setEnabled(false);
        btn_continue->setEnabled(true);
    }
}

void QOrbitsDlg::onbtn_delall( void )
{
    plotwnd->getDlgData();

    btn_forwards->setEnabled(false);
    btn_backwards->setEnabled(false);
    btn_continue->setEnabled(false);
    btn_delall->setEnabled(false);
    btn_dellast->setEnabled(false);

    VFResults.DeleteOrbit( VFResults.first_orbit );
    VFResults.first_orbit = nullptr;
    VFResults.current_orbit = nullptr;

    mainSphere->refresh();
}

void QOrbitsDlg::onbtn_dellast( void )
{
    plotwnd->getDlgData();

    mainSphere->prepareDrawing();
    DeleteLastOrbit( mainSphere );
    mainSphere->finishDrawing();

    orbit_started=false;
    orbit_selected=false;
    btn_forwards->setEnabled(false);
    btn_backwards->setEnabled(false);
    btn_continue->setEnabled(false);

    if( VFResults.first_orbit == nullptr )
    {
        btn_delall->setEnabled(false);
        btn_dellast->setEnabled(false);
    }
}

void QOrbitsDlg::OrbitEvent( int i )
{
    switch( i )
    {
    case -1:    onbtn_backwards();  break;
    case 1:     onbtn_forwards();   break;
    case 0:     onbtn_continue();   break;
    case 2:     onbtn_dellast();    break;
    case 3:     onbtn_delall();     break;
    }
}

void QOrbitsDlg::Reset( void )
{
    // finishing

    selected_x0 = 0;
    selected_y0 = 0;

    btn_forwards->setEnabled(false);
    btn_backwards->setEnabled(false);
    btn_continue->setEnabled(false);

    if( VFResults.first_orbit == nullptr )
    {
        btn_delall->setEnabled(false);
        btn_dellast->setEnabled(false);
    }
    orbit_started = false;
    orbit_selected = false;
}
