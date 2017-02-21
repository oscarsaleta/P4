/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  P4 is free software: you can redistribute it and/or modify
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

#include "p4application.h"

#include "custom.h"
#include "file_vf.h"
#include "win_event.h"

QP4Application * p4app = nullptr;

QP4Application::QP4Application( int & argc, char ** argv )
        : QApplication(argc,argv)
{
    StandardFont = new QFont();
    StandardFont->setPointSize( StandardFont->pointSize() + FONTSIZE );
    setFont( *StandardFont );

    BoldFont = new QFont();
    BoldFont->setWeight( QFont::Bold );

    TitleFont = new QFont();
    TitleFont->setPointSize( TitleFont->pointSize() + TITLEFONTSIZE );
    TitleFont->setWeight( QFont::Bold );

    CourierFont = new QFont;
    CourierFont->setFamily("Courier");
    CourierFont->setFixedPitch(true);
    CourierFont->setPointSize( CourierFont->pointSize() + FONTSIZE );

    BoldCourierFont = new QFont;
    BoldCourierFont->setFamily("Courier");
    BoldCourierFont->setFixedPitch(true);
    BoldCourierFont->setWeight(QFont::Bold);
    BoldCourierFont->setPointSize( BoldCourierFont->pointSize() + FONTSIZE );

    LegendFont = new QFont;
    LegendFont->setFamily("Courier");
    LegendFont->setFixedPitch(true);
    LegendFont->setPointSize( LegendFont->pointSize() + LEGENDFONTSIZE );
}

// -----------------------------------------------------------------------
//                          SIGNALS
// -----------------------------------------------------------------------
//
// Following signals may be called from another thread

void QP4Application::Signal_Changed( void )
{
    ThisVF->evaluated = false;
    ThisVF->changed = true;

    QP4Event * e = new QP4Event( (QEvent::Type)TYPE_SIGNAL_CHANGED, nullptr );
    p4app->postEvent( p4startdlg, e );
}

void QP4Application::Signal_Evaluated( int exitCode )
{
    ThisVF->evaluated = true;
    ThisVF->evaluating = false;

    ThisVF->finishEvaluation(exitCode);

    QP4Event * e = new QP4Event( (QEvent::Type)TYPE_SIGNAL_EVALUATED, nullptr );
    p4app->postEvent( p4startdlg, e );

    if( cmdLine_AutoExit )
    {
        cmdLine_AutoPlot = false;
        p4startdlg->OnQuit();
        return;
    }

    if( cmdLine_AutoPlot )
    {
        cmdLine_AutoPlot = false;
        p4startdlg->OnPlot();
    }
}

void QP4Application::Signal_GcfEvaluated( int exitCode )
{
    ThisVF->evaluated = true;
    ThisVF->evaluating = false;

    ThisVF->finishEvaluation(exitCode);
}

void QP4Application::Signal_Evaluating( void )
{
    ThisVF->evaluating = true;
    ThisVF->evaluated = false;

    QP4Event * e = new QP4Event( (QEvent::Type)TYPE_SIGNAL_EVALUATING, nullptr );
    p4app->postEvent( p4startdlg, e );
}

void QP4Application::cathProcessError( QProcess::ProcessError qperr )
{
     ThisVF->catchProcessError( qperr );
}

void QP4Application::Signal_Loaded( void )
{
    QP4Event * e = new QP4Event( (QEvent::Type)TYPE_SIGNAL_LOADED, nullptr );
    p4app->postEvent( p4startdlg, e );

    if( cmdLine_AutoEvaluate )
    {
        cmdLine_AutoEvaluate = false;
        p4startdlg->Find_Window->OnBtnEval();
        return;
    }
    if( cmdLine_AutoPlot )
    {
        cmdLine_AutoPlot = false;
        p4startdlg->OnPlot();
        return;
    }
}

void QP4Application::Signal_Saved( void )
{
    QP4Event * e = new QP4Event( (QEvent::Type)TYPE_SIGNAL_SAVED, nullptr );
    p4app->postEvent( p4startdlg, e );
}
