/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4
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

#include "win_params.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "p4application.h"
#include "p4settings.h"

#include <QButtonGroup>


QParamsDlg::~QParamsDlg()
{
    GetDataFromDlg();
}

QParamsDlg::QParamsDlg( QFindDlg * finddlg )
#ifdef DOCK_PARAMSWINDOW
    : QWidget( finddlg )
#else
    : QWidget()
#endif
{
    parent = finddlg;
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    QLabel * p4title = new QLabel( "Find Singular Points Parameters", this );
    p4title->setFont( *(p4app->TitleFont) );

    QLabel * symlabel = new QLabel( "Calculations: ", this );
    symlabel->setFont( *(p4app->BoldFont) );
    btn_alg = new QRadioButton( "Algebraic", this );
    btn_num = new QRadioButton( "Numeric", this );

    QLabel * seplabel = new QLabel( "Test Separatrices:  ", this );
    seplabel->setFont( *(p4app->BoldFont) );
    btn_sepyes = new QRadioButton( "Yes", this );
    btn_sepno = new QRadioButton( "No", this );

    QLabel * lbl_precis = new QLabel( "Accurracy:", this );
    lbl_precis->setFont( *(p4app->BoldFont) );
    spin_precis = new QSpinBox(this);
    spin_precis->setMinimum(MINPRECISION);
    spin_precis->setMaximum(MAXPRECISION);

    QLabel * lbl_precis0 = new QLabel( "Precision:", this );
    lbl_precis0->setFont( *(p4app->BoldFont) );
    spin_precis0 = new QSpinBox(this);
    spin_precis0->setMinimum(MINPRECISION0);
    spin_precis0->setMaximum(MAXPRECISION0);

    QLabel * lbl_epsilon = new QLabel( "Epsilon:", this );
    lbl_epsilon->setFont( *(p4app->BoldFont) );
    edt_epsilon = new QLineEdit( ThisVF->epsilon, this );

    QLabel * lbl_level = new QLabel( "Level of Approximation:", this );
    lbl_level->setFont( *(p4app->BoldFont) );
    spin_level = new QSpinBox(this);
    spin_level->setMinimum(MINLEVEL);
    spin_level->setMaximum(MAXLEVEL);

    QLabel * lbl_numlevel = new QLabel( "Numeric level:", this );
    lbl_numlevel->setFont( *(p4app->BoldFont) );
    spin_numlevel = new QSpinBox(this);
    spin_numlevel->setMinimum(MINNUMLEVEL);
    spin_numlevel->setMaximum(MAXNUMLEVEL);

    QLabel * lbl_maxlevel = new QLabel( "Maximum level:", this );
    lbl_maxlevel->setFont( *(p4app->BoldFont) );
    spin_maxlevel = new QSpinBox(this);
    spin_maxlevel->setMinimum(MINMAXLEVEL);
    spin_maxlevel->setMaximum(MAXMAXLEVEL);

    QLabel * lbl_weakness = new QLabel( "Maximum level of weakness:", this );
    lbl_weakness->setFont( *(p4app->BoldFont) );
    spin_weakness = new QSpinBox(this);
    spin_weakness->setMinimum(MINWEAKNESS);
    spin_weakness->setMaximum(MAXWEAKNESS);

    lbl_p = new QLabel( "p:", this );
    lbl_p->setFont( *(p4app->BoldFont) );
    spin_p = new QSpinBox(this);
    spin_p->setMinimum(MINP);
    spin_p->setMaximum(MAXP);

    lbl_q = new QLabel( "q:", this );
    lbl_q->setFont( *(p4app->BoldFont) );
    spin_q = new QSpinBox(this);
    spin_q->setMinimum(MINP);
    spin_q->setMaximum(MAXP);

    lbl_x0 = new QLabel( "x0:", this );
    lbl_x0->setFont( *(p4app->BoldFont) );
    edt_x0 = new QLineEdit( "", this );

    lbl_y0 = new QLabel( "y0:", this );
    lbl_y0->setFont( *(p4app->BoldFont) );
    edt_y0 = new QLineEdit( "", this );

#ifdef TOOLTIPS
    btn_alg->setToolTip( "Let the symbolic manipulator work in algebraic mode.\n"
                            "The symbolic package will automatically switch over to\n"
                            "numeric calculations whenever a symbolic study is impossible." );
    btn_num->setToolTip( "Let the symbolic manipulator work in numeric mode" );
    btn_sepyes->setToolTip( "Enable numeric testing of the Taylor developments for the separatrices" );
    btn_sepno->setToolTip( "Disable separatrice testing" );
    spin_precis->setToolTip( "Number of digits required accurracy when checking for zero" );
    spin_precis0->setToolTip( "Number of digits of numeric precision of all calculations.  Put 0 for default double precision" );
    edt_epsilon->setToolTip( "Default epsilon for the separatrices.\n"
                    "This may be changed later when plotting individual separatrices" );
    spin_level->setToolTip( "Order of Taylor approximation\nThis order is increased as long as the separatrix test fails." );
    spin_numlevel->setToolTip( "Start working numerically when order of Taylor approximation reaches this level" );
    spin_maxlevel->setToolTip( "Maximum order of Taylor approximation.  Stop testing separatrices at this order." );
    spin_weakness->setToolTip( "Maximum order of Lyapunov constants to be calculated for weak foci" );
    spin_p->setToolTip( "Poincare-Lyapunov weights" );
    spin_q->setToolTip( "Poincare-Lyapunov weights" );
    edt_x0->setToolTip( "Coordinates of the singular point in case of study of ONE singularity" );
    edt_y0->setToolTip( "Coordinates of the singular point in case of study of ONE singularity" );
#endif

    // layout

    mainLayout = new QBoxLayout( QBoxLayout::TopToBottom, this );

    mainLayout->addWidget( p4title );

    QHBoxLayout * symLayout = new QHBoxLayout();
    symLayout->addWidget( symlabel );
    symLayout->addWidget( btn_alg );
    symLayout->addWidget( btn_num );
    mainLayout->addLayout( symLayout );

    QHBoxLayout * sepLayout = new QHBoxLayout();
    sepLayout->addWidget( seplabel );
    sepLayout->addWidget( btn_sepyes );
    sepLayout->addWidget( btn_sepno );
    mainLayout->addLayout( sepLayout );

    QHBoxLayout * layout1 = new QHBoxLayout();
    layout1->addWidget( lbl_precis );
    layout1->addWidget( spin_precis );
    layout1->addStretch(0);
    layout1->addWidget( lbl_precis0 );
    layout1->addWidget( spin_precis0 );

    QHBoxLayout * layout2 = new QHBoxLayout();
    layout2->addWidget( lbl_epsilon );
    layout2->addWidget( edt_epsilon );
    layout2->addStretch(0);

    QHBoxLayout * layout3 = new QHBoxLayout();
    layout3->addWidget( lbl_level );
    layout3->addWidget( spin_level );
    layout3->addStretch(0);

    QHBoxLayout * layout4 = new QHBoxLayout();
    layout4->addWidget( lbl_numlevel );
    layout4->addWidget( spin_numlevel );
    layout4->addStretch(0);

    QHBoxLayout * layout5 = new QHBoxLayout();
    layout5->addWidget( lbl_maxlevel );
    layout5->addWidget( spin_maxlevel );
    layout5->addStretch(0);

    QHBoxLayout * layout6 = new QHBoxLayout();
    layout6->addWidget( lbl_weakness );
    layout6->addWidget( spin_weakness );
    layout6->addStretch(0);

    QHBoxLayout * layout7 = new QHBoxLayout();
    layout7->addWidget( lbl_p );
    layout7->addWidget( spin_p );
    layout7->addWidget( lbl_q );
    layout7->addWidget( spin_q );
    layout7->addStretch(0);

    QHBoxLayout * layout8 = new QHBoxLayout();
    layout8->addWidget( lbl_x0 );
    layout8->addWidget( edt_x0 );
    layout8->addStretch(0);

    QHBoxLayout * layout9 = new QHBoxLayout();
    layout9->addWidget( lbl_y0 );
    layout9->addWidget( edt_y0 );
    layout9->addStretch(0);

    mainLayout->addLayout( layout1 );
    mainLayout->addLayout( layout2 );
    mainLayout->addLayout( layout3 );
    mainLayout->addLayout( layout4 );
    mainLayout->addLayout( layout5 );
    mainLayout->addLayout( layout6 );
    mainLayout->addLayout( layout7 );
    mainLayout->addLayout( layout8 );
    mainLayout->addLayout( layout9 );

    setLayout(mainLayout);

    QButtonGroup * btngrp1 = new QButtonGroup( this );
    btngrp1->addButton( btn_alg );
    btngrp1->addButton( btn_num );

    QButtonGroup * btngrp2 = new QButtonGroup( this );
    btngrp2->addButton( btn_sepyes );
    btngrp2->addButton( btn_sepno );

    // connections

    QObject::connect( btn_alg,  SIGNAL(toggled(bool)), this, SLOT(btn_alg_toggled(bool)) );
    QObject::connect( btn_num,  SIGNAL(toggled(bool)), this, SLOT(btn_num_toggled(bool)) );
    QObject::connect( spin_level, SIGNAL(valueChanged(int)), this, SLOT(OnLevelChange(int)) );
    QObject::connect( btn_sepno, SIGNAL(toggled(bool)), this, SLOT( btn_sepno_toggled(bool)) );
    QObject::connect( btn_sepyes, SIGNAL(toggled(bool)), this, SLOT( btn_sepyes_toggled(bool)) );

    // finishing

    UpdateDlgData();
#ifndef DOCK_PARAMSWINDOW
    SetP4WindowTitle( this, "Parameters" );
#endif
}

void QParamsDlg::OnLevelChange( int value )
{
    spin_maxlevel->setMinimum( value );
    spin_numlevel->setMinimum( value );

    if( spin_maxlevel->value() < value )
        spin_maxlevel->setValue(value);

    if( spin_numlevel->value() < value )
        spin_numlevel->setValue(value);
}

void QParamsDlg::btn_alg_toggled( bool on )
{
    if( on ) 
    {
        if( ThisVF->numeric != false )
        {
            ThisVF->numeric = false;
            if( ThisVF->changed == false )
            {
                ThisVF->changed = true;
                p4app->Signal_Changed();
            }
        }
    }
}

void QParamsDlg::btn_num_toggled( bool on )
{
    if( on ) 
    {
        if( ThisVF->numeric != true )
        {
            ThisVF->numeric = true;
            if( ThisVF->changed == false )
            {
                ThisVF->changed = true;
                p4app->Signal_Changed();
            }
        }
    }
}

void QParamsDlg::btn_sepyes_toggled( bool on )
{
    if( on )
    {
        if( ThisVF->testsep != true )
        {
            ThisVF->testsep = true;
            if( ThisVF->changed == false )
            {
                ThisVF->changed = true;
                p4app->Signal_Changed();
            }
        }
    }
}

void QParamsDlg::btn_sepno_toggled( bool on )
{
    if( on )
    {
        if( ThisVF->testsep != false )
        {
            ThisVF->testsep = false;
            if( ThisVF->changed == false )
            {
                ThisVF->changed = true;
                p4app->Signal_Changed();
            }
        }
    }
}

void QParamsDlg::ExclusiveToggle( bool on, QRadioButton * first, ... )
{
     first->setChecked(on);
}


void QParamsDlg::GetDataFromDlg( void )
{
    QString epsilon;
    QString x0;
    QString y0;

    if( spin_level->value() != ThisVF->taylorlevel ||
        spin_numlevel->value() != ThisVF->numericlevel ||
        spin_maxlevel->value() != ThisVF->maxlevel ||
        spin_weakness->value() != ThisVF->weakness ||
        spin_p->value() != ThisVF->p ||
        spin_q->value() != ThisVF->q ||
        spin_precis->value() != ThisVF->precision ||
        spin_precis0->value() != ThisVF->precision0 )
    {
        ThisVF->taylorlevel = spin_level->value();
        ThisVF->numericlevel = spin_numlevel->value();
        ThisVF->maxlevel = spin_maxlevel->value();
        ThisVF->weakness = spin_weakness->value();
        ThisVF->p = spin_p->value();
        ThisVF->q = spin_q->value();
        ThisVF->precision = spin_precis->value();
        ThisVF->precision0 = spin_precis0->value();

        if( ThisVF->changed == false )
        {
            ThisVF->changed = true;
            p4app->Signal_Changed();
        }
    }

    epsilon = edt_epsilon->text();
    x0 = edt_x0->text();
    y0 = edt_y0->text();

    epsilon = epsilon.trimmed();
    x0 = x0.trimmed();
    y0 = y0.trimmed();

    if( epsilon.compare( ThisVF->epsilon ) != 0 ||
        x0.compare( ThisVF->x0 ) != 0 ||
        y0.compare( ThisVF->y0 ) != 0 )
    {
        ThisVF->epsilon = epsilon;
        ThisVF->x0 = x0;
        ThisVF->y0 = y0;

        if( ThisVF->changed == false )
        {
            ThisVF->changed = true;
            p4app->Signal_Changed();
        }
    }
}

void QParamsDlg::UpdateDlgData( void )
{
    if( ThisVF->numeric )
        ExclusiveToggle( true, btn_num, btn_alg, nullptr );
    else
        ExclusiveToggle( true, btn_alg, btn_num, nullptr );

    if( ThisVF->testsep )
        ExclusiveToggle( true, btn_sepyes, btn_sepno, nullptr );
    else
        ExclusiveToggle( true, btn_sepno, btn_sepyes, nullptr );

    spin_level->setValue( ThisVF->taylorlevel );
    spin_numlevel->setValue( ThisVF->numericlevel );
    spin_maxlevel->setValue( ThisVF->maxlevel );
    spin_weakness->setValue( ThisVF->weakness );
    edt_epsilon->setText( ThisVF->epsilon );
    spin_precis->setValue( ThisVF->precision );
    spin_precis0->setValue( ThisVF->precision0 );

    if( ThisVF->typeofstudy == TYPEOFSTUDY_ONE )
    {
        edt_x0->setText( ThisVF->x0 );
        edt_y0->setText( ThisVF->y0 );
        spin_p->setValue( ThisVF->p );
        spin_q->setValue( ThisVF->q );

        spin_p->setEnabled(false);
        spin_q->setEnabled(false);
        edt_x0->setEnabled(true);
        edt_y0->setEnabled(true);
        
        lbl_x0->setFont( *(p4app->BoldFont) );
        lbl_y0->setFont( *(p4app->BoldFont) );
        lbl_p->setFont( *(p4app->StandardFont) );
        lbl_q->setFont( *(p4app->StandardFont) );
    }
    else
    {
        spin_p->setValue( ThisVF->p );
        spin_q->setValue( ThisVF->q );
        edt_x0->setText( ThisVF->x0 );
        edt_y0->setText( ThisVF->x0 );

        spin_p->setEnabled(true);
        spin_q->setEnabled(true);
        edt_x0->setEnabled(false);
        edt_y0->setEnabled(false);
        
        lbl_p->setFont( *(p4app->BoldFont) );
        lbl_q->setFont( *(p4app->BoldFont) );
        lbl_x0->setFont( *(p4app->StandardFont) );
        lbl_y0->setFont( *(p4app->StandardFont) );
    }
}
