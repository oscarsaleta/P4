/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "win_separatrice.h"

#include "custom.h"
#include "math_separatrice.h"


QString CurrentSingularityInfo[4] = { "", "", "", "" };
double CurrentSeparatriceEpsilon = 0;

QSepDlg::QSepDlg( QPlotWnd * plt, QWinSphere * sp )
    : QWidget(nullptr,Qt::Tool | Qt::WindowStaysOnTopHint)
{
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    mainSphere = sp;
    plotwnd = plt;

    btn_start = new QPushButton( "&Start integrating sep", this );
    btn_cont = new QPushButton( "&Cont integrating sep", this );
    btn_intnext = new QPushButton( "&Integrate next sep", this );
    btn_selectnext = new QPushButton( "Select &Next sep", this );

    edt_epsilon = new QLineEdit( "", this );
    QLabel * lbl1 = new QLabel( "&Epsilon = ", this ); lbl1->setBuddy( edt_epsilon );

    lbl_info[0] = new QLabel( "", this );
    lbl_info[1] = new QLabel( "", this );
    lbl_info[2] = new QLabel( "", this );
    lbl_info[3] = new QLabel( "", this );

    // layout

    mainLayout = new QBoxLayout( QBoxLayout::TopToBottom, this );

    mainLayout->addWidget( lbl_info[0] );
    mainLayout->addWidget( lbl_info[1] );
    mainLayout->addWidget( lbl_info[2] );
    mainLayout->addWidget( lbl_info[3] );

    QHBoxLayout * layout3 = new QHBoxLayout();
    layout3->addWidget( lbl1 );
    layout3->addWidget( edt_epsilon );

    QHBoxLayout * layout4 = new QHBoxLayout();
    layout4->addWidget( btn_start );
    layout4->addWidget( btn_cont );

    QHBoxLayout * layout5 = new QHBoxLayout();
    layout5->addWidget( btn_intnext );
    layout5->addWidget( btn_selectnext );

    mainLayout->addLayout( layout3 );
    mainLayout->addLayout( layout4 );
    mainLayout->addLayout( layout5 );
 
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
   setLayout(mainLayout);

#ifdef TOOLTIPS
    edt_epsilon->setToolTip( "Epsilon value for this separatrix\n"
                    "Confirm any change by pressing ENTER" );
    btn_start->setToolTip( "Start integrating this separatrix" );
    btn_cont->setToolTip( "Continue integrating this separatrix" );
    btn_intnext->setToolTip( "Select next separatrix, and start integrating"  );
    btn_selectnext->setToolTip( "Select next separatrix of this singular point" );
#endif

    // connections

    QObject::connect( btn_selectnext,   SIGNAL(clicked()), this, SLOT(onbtn_selectnext()) );
    QObject::connect( btn_intnext,      SIGNAL(clicked()), this, SLOT(onbtn_intnext()) );
    QObject::connect( btn_start,        SIGNAL(clicked()), this, SLOT(onbtn_start()) );
    QObject::connect( btn_cont,         SIGNAL(clicked()), this, SLOT(onbtn_cont()) );
    QObject::connect( edt_epsilon,      SIGNAL(returnPressed()), this, SLOT(onepsilon_enter()) );

    // finishing

    Reset();

    setP4WindowTitle( this, "Separatrices" );
}

void QSepDlg::setInitialPoint( void )
{
    QString buf;

    plotwnd->getDlgData();

    selected = true;
    started = false;
    btn_start->setEnabled(true);
    btn_cont->setEnabled(false);
    btn_selectnext->setEnabled(true);
    btn_intnext->setEnabled(true);

    lbl_info[0]->setText( CurrentSingularityInfo[0] );
    lbl_info[1]->setText( CurrentSingularityInfo[1] );
    lbl_info[2]->setText( CurrentSingularityInfo[2] );
    lbl_info[3]->setText( CurrentSingularityInfo[3] );
    
    buf.sprintf( "%g", (float)CurrentSeparatriceEpsilon );

    edt_epsilon->setText( buf );
    show();
//  raise();
}

void QSepDlg::onbtn_selectnext( void )
{
    if( !selected )
        return;

    plotwnd->getDlgData();

    started = false;

    btn_start->setEnabled(true);
    btn_cont->setEnabled(false);
    btn_selectnext->setEnabled(true);
    btn_intnext->setEnabled(true);

    mainSphere->prepareDrawing();
    (*select_next_sep)(mainSphere);
    mainSphere->finishDrawing();
}

void QSepDlg::onbtn_intnext( void )
{
    if( !selected )
        return;

    plotwnd->getDlgData();

    started = true;

    btn_start->setEnabled(false);
    btn_cont->setEnabled(true);
    btn_selectnext->setEnabled(true);
    btn_intnext->setEnabled(true);

    mainSphere->prepareDrawing();
    (*plot_next_sep)(mainSphere);
    mainSphere->finishDrawing();
}

void QSepDlg::onbtn_start( void )
{
    if( !selected || started )
        return;

    plotwnd->getDlgData();

    started = true;
    btn_start->setEnabled(false);
    btn_cont->setEnabled(true);
    btn_selectnext->setEnabled(true);
    btn_intnext->setEnabled(true);

    mainSphere->prepareDrawing();
    (*start_plot_sep)(mainSphere);
    mainSphere->finishDrawing();
}

void QSepDlg::onbtn_cont( void )
{
    if( !selected || !started )
        return;

    plotwnd->getDlgData();

    mainSphere->prepareDrawing();
    (*cont_plot_sep)(mainSphere);
    mainSphere->finishDrawing();
}

void QSepDlg::Reset( void )
{
    lbl_info[0]->setText( "no point selected." );
    lbl_info[1]->setText( "" );
    lbl_info[2]->setText( "Use Shift+Left Button" );
    lbl_info[3]->setText( "in the plot window to select." );

    started = false;
    selected = false;

    btn_start->setEnabled(false);
    btn_cont->setEnabled(false);
    btn_selectnext->setEnabled(false);
    btn_intnext->setEnabled(false);
}

void QSepDlg::SepEvent( int i )
{
    switch( i )
    {
    case -1:    setInitialPoint();  break;
    case 0:     onbtn_cont();       break;
    case 1:     onbtn_start();      break;
    case 2:     onbtn_intnext();    break;
    case 3:     onbtn_selectnext(); break;
    }
}

void QSepDlg::MarkBad( QLineEdit * edt )
{
    QString t;
    int i;

    t = edt->text();
    while( (i = t.indexOf( '?' )) >= 0 )
        t.remove(i,1);
    t = t.trimmed();
    t.append( " ???" );

    edt->setText(t);
}

void QSepDlg::onepsilon_enter( void )
{
    // called when user presses ENTER after changing the epsilon value

    QString s;
    double eps;
    bool ok;

    s = edt_epsilon->text();
    eps = s.toDouble( &ok );
    if( !ok || eps <= 0 )
    {
        MarkBad(edt_epsilon);
        return;
    }
    
    CurrentSeparatriceEpsilon = eps;
    s.sprintf( "%g", (float)eps );
    edt_epsilon->setText(s);

    // pass on to math routines

    if( !selected )
        return;

    started = false;

    btn_start->setEnabled(true);
    btn_cont->setEnabled(false);
    btn_selectnext->setEnabled(true);
    btn_intnext->setEnabled(true);

    mainSphere->prepareDrawing();
    (*change_epsilon)(mainSphere,eps);
    mainSphere->finishDrawing();
}
