#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qprogressdialog.h>
#include <qnamespace.h>
#include <qlabel.h>
#include <qlayout.h>
#include <math.h>
#include "custom.h"
#include "table.h"
#include "file_vf.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_gcf.h"
#include "win_plot.h"
#include "p4application.h"

extern void SetP4WindowTitle( QWidget *, QString );
extern bool EvalGcfStart( QWinSphere * sp, int dashes, int points, int precis );
extern bool EvalGcfContinue( int, int );
extern bool EvalGcfFinish( void );

QGcfDlg::QGcfDlg( QPlotWnd * plt, QWinSphere * sp )
    : QWidget(nullptr,Qt::Tool | Qt::WindowStaysOnTopHint)
{
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    mainSphere = sp;
    plotwnd = plt;

    btn_dots = new QRadioButton( "Dots", this );
    btn_dashes = new QRadioButton( "Dashes", this );

    QLabel * lbl1 = new QLabel( "Appearance: ", this );

    edt_points = new QLineEdit( "", this );
    QLabel * lbl2 = new QLabel( "#Points: ", this );

    edt_precis = new QLineEdit( "", this );
    QLabel * lbl3 = new QLabel( "Precision: ", this );

    edt_memory = new QLineEdit( "", this );
    QLabel * lbl4 = new QLabel( "Max. Memory: ", this );

    btn_evaluate = new QPushButton( "&Evaluate", this );

#ifdef TOOLTIPS
    btn_dots->setToolTip( "Plot individual points of the curve of singularities" );
    btn_dashes->setToolTip( "Connect points of the curve of singularities with small line segments" );
    edt_points->setToolTip( "Number of points" );
    edt_precis->setToolTip( "Required precision" );
    edt_memory->setToolTip( "Maximum amount of memory (in kilobytes) spent on plotting GCF" );
    btn_evaluate->setToolTip( "Start evaluation (using symbolic manipulator)" );
#endif

    // layout

    mainLayout = new QBoxLayout( QBoxLayout::TopToBottom, this );

    QHBoxLayout * layout1 = new QHBoxLayout();
    layout1->addWidget( lbl1 );
    layout1->addWidget( btn_dots );
    layout1->addWidget( btn_dashes );

    QGridLayout * lay00 = new QGridLayout();
    lay00->addWidget( lbl2, 0, 0 );
    lay00->addWidget( edt_points, 0, 1 );
    lay00->addWidget( lbl3, 1, 0 );
    lay00->addWidget( edt_precis, 1, 1 );
    lay00->addWidget( lbl4, 2, 0 );
    lay00->addWidget( edt_memory, 2, 1 );

    QHBoxLayout * layout2 = new QHBoxLayout();
    layout2->addStretch(0);
    layout2->addWidget( btn_evaluate );
    layout2->addStretch(0);

    mainLayout->addLayout( layout1 );
    mainLayout->addLayout( lay00 );
    mainLayout->addLayout( layout2 );

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    // connections

    QObject::connect( btn_evaluate,     SIGNAL(clicked()), this, SLOT(onbtn_evaluate()) );
    QObject::connect( btn_dots,         SIGNAL(toggled(bool)), this, SLOT(btn_dots_toggled(bool)) );
    QObject::connect( btn_dashes,       SIGNAL(toggled(bool)), this, SLOT(btn_dashes_toggled(bool)) );

    // finishing

    SetP4WindowTitle( this, "GCF Plot" );
}

void QGcfDlg::Reset( void )
{
    QString buf;

    buf.sprintf( "%d", DEFAULT_GCFPOINTS );
    edt_points->setText( buf );

    buf.sprintf( "%d", DEFAULT_GCFPRECIS );
    edt_precis->setText( buf );

    buf.sprintf( "%d", DEFAULT_GCFMEMORY );
    edt_memory->setText( buf );

    if( VFResults.config_dashes )
        ExclusiveToggle( true, btn_dashes, btn_dots );
    else
        ExclusiveToggle( true, btn_dots, btn_dashes );
}

void QGcfDlg::btn_dots_toggled( bool on )
{
    ExclusiveToggle( on, btn_dots, btn_dashes );
}

void QGcfDlg::btn_dashes_toggled( bool on )
{
    ExclusiveToggle( on, btn_dashes, btn_dots );
}

void QGcfDlg::ExclusiveToggle( bool on, QRadioButton * first, QRadioButton * second )
{
    if( on )
    {
        if( first->isChecked() == false )
            first->toggle();

        if( second->isChecked() == true )
            second->toggle();
    }
    else
    {
        if( second->isChecked() == false )
            first->toggle();
    }
}

void QGcfDlg::onbtn_evaluate( void )
{
    bool dashes, result;
    int points, precis, memory;

    bool ok;
    QString buf;

    dashes = btn_dashes->isChecked();
    
    ok = true;
    
    buf = edt_points->text();
    points = buf.toInt();

    if( points < MIN_GCFPOINTS || points > MAX_GCFPOINTS )
    {
        buf += " ???";
        edt_points->setText(buf);
        ok = false;
    }

    buf = edt_precis->text();
    precis = buf.toInt();
    if( precis < MIN_GCFPRECIS || precis > MAX_GCFPRECIS )
    {
        buf += " ???";
        edt_precis->setText(buf);
        ok = false;
    }

    buf = edt_memory->text();
    memory = buf.toInt();
    if( memory < MIN_GCFMEMORY || memory > MAX_GCFMEMORY )
    {
        buf += " ???";
        edt_memory->setText(buf);
        ok = false;
    }

    if( !ok )
    {
        QMessageBox::information( this, "P4",
            "One of the fields has a value that is out of bounds.\n"
            "Please correct before continuing.\n" );

        return;
    }

    // Evaluate GCF with given parameters {dashes, points, precis, memory}.

    evaluating_points = points;
    evaluating_memory = memory;
    evaluating_precision = precis;

    btn_evaluate->setEnabled(false);

    ThisVF->gcfDlg = this;
    result = EvalGcfStart( mainSphere, dashes, points, precis );
    if( !result )
    {
        btn_evaluate->setEnabled(true);
        QMessageBox::critical( this, "P4",
            "An error occured while plotting the GCF.\nThe singular locus may not be visible, or may "
            "be partially visible." );
    }
}

void QGcfDlg::finishGcfEvaluation( void )
{
    bool result;

    if( btn_evaluate->isEnabled() == true )
        return; // not busy??

    result = EvalGcfContinue( evaluating_points, evaluating_precision );
 
    if( result )
    {
        btn_evaluate->setEnabled(true);
        result = EvalGcfFinish();       // return false in case an error occured
        if( !result )
        {
            QMessageBox::critical( this, "P4",
                "An error occured while plotting the GCF.\nThe singular locus may not be visible, or may "
                "be partially visible." );
        }
    }
}
