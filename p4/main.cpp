//#include <qcdestyle.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qprinter.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qevent.h>
#include "custom.h"
#include "table.h"
#include "file_vf.h"
#include "win_p4.h"
#include "p4application.h"
#include "win_main.h"
#include "win_find.h"
#include "win_event.h"
#include "win_settings.h"

#include "../version.h"

/*
    ----------------------------------------------------------------------------
    TO DO LIST:
    -----------

    -   windows: create installer
    -   toolbars can be deleted by pressing right mouse button --> avoid this
    -   poincare-lyapunov printing: improve dotted line when printing
    -   system printer support

    -----------------------------------------------------------------------------
*/

QInputVF * ThisVF = nullptr;
QVFStudy VFResults;
QString P4Version;
QString P4VersionDate;
QString P4Platform;

bool action_OnlyPrepareFile = false;    // in the find menu (this is not saved in the .inp file)
bool action_SaveAll = DEFAULTSAVEALL;   // in the find menu (this is not saved in the .inp file)
QP4Application * p4app = nullptr;
QStartDlg * p4startdlg = nullptr;
QPixmap * p4smallicon = nullptr;

QPrinter * p4printer = nullptr;

extern QString GetP4BinPath();

extern bool ReadP4Settings( void );
extern void SaveP4Settings( void );

QString CmdLine_Filename;
bool CmdLine_AutoEvaluate;
bool CmdLine_AutoPlot;
bool CmdLine_AutoExit;

void HandleCommandLineOption( char * arg )
{
    while( *arg != 0 )
    {
        if( *arg == 'e' || *arg == 'E' )    // auto-evaluate
        {
            CmdLine_AutoEvaluate = true;
            arg++;
            continue;
        }
        
        if( *arg == 'p' || *arg == 'P' )    // auto-plot
        {
            CmdLine_AutoPlot = true;
            arg++;
            continue;
        }

        if( *arg == 'x' || *arg == 'X' )    // auto-evaluate + exit
        {
            CmdLine_AutoEvaluate = true;
            CmdLine_AutoExit = true;
            arg++;
            continue;
        }
        
        // unknown option: skip

        break;
    }
}

void HandleCommandLineArgument( char * arg )
{
    if( *arg == '-' )
    {
        HandleCommandLineOption( arg+1 );
        return;
    }
        
    if( CmdLine_Filename.length() == 0 )
    {
        CmdLine_Filename = arg;
        return;
    }
    
    // a second filename given??? --> ignore
    
    return;
}


int main( int argc, char *argv[] )
{
    int returnvalue;
    int v, i;

    P4Platform = "";
    P4Version = VERSION;        // initialize version string
    P4VersionDate = VERSIONDATE;

    CmdLine_Filename = "";
    CmdLine_AutoEvaluate = false;
    CmdLine_AutoPlot = false;
    CmdLine_AutoExit = false;
    
    for( i = 1; i < argc; i++ )
         HandleCommandLineArgument( argv[i] );
    if( CmdLine_AutoEvaluate && CmdLine_Filename.length() == 0 )
        CmdLine_Filename = DEFAULTFILENAME;

    p4app = new QP4Application( argc, argv );

    p4app->addLibraryPath( p4app->applicationDirPath() );

    p4app->setQuitOnLastWindowClosed(false);
    v=ReadP4Settings();

    p4printer = new QPrinter( QPrinter::PrinterResolution );
    p4smallicon = new QPixmap();
    if( p4smallicon->load( GetP4BinPath() + "/p4smallicon.png" ) == false )
    {
        delete p4smallicon;
        p4smallicon = nullptr;
    }

//  p4app->setStyle( new QCDEStyle() );

    ThisVF = new QInputVF();

    p4startdlg = new QStartDlg( CmdLine_Filename );
    if( !CmdLine_AutoExit )
        p4startdlg->show();

    p4app->setQuitOnLastWindowClosed(true);

    if( CmdLine_Filename.length() != 0 )
    {
        if( !(ThisVF->load()) )
        {
            CmdLine_AutoEvaluate = false;
            CmdLine_AutoPlot = false;
            if( CmdLine_AutoExit )
            {
                delete p4printer;
                p4printer = nullptr;
                delete p4app;
                p4app = nullptr;
                return (-1);
            }
        }
        p4startdlg->Find_Window->OnBtnLoad();
    }

    if( !v )
    {
        QMessageBox::information( p4startdlg, "P4",
            "Welcome to P4.\n\n"
            "Since P4 did not find program settings, it assumes that this is the first time\n"
            "you start the program.  So Please check whether the settings are correct.\n\n"
            "In particular, check if P4 has found the Command-Line Version of MAPLE.\n" );

        QSettingsDlg * psettings;
        psettings = new QSettingsDlg( nullptr, 0 );
        psettings->exec();
        delete psettings;
        psettings = nullptr;
    }

    returnvalue = p4app->exec();
    
    delete p4printer;
    p4printer = nullptr;

    SaveP4Settings();

    delete p4app;
    p4app = nullptr;
    return returnvalue;
}

// -----------------------------------------------------------------------
//                          SETP4WINDOWTITLE
// -----------------------------------------------------------------------

void SetP4WindowTitle( QWidget * win, QString title )
{
    win->setWindowTitle(title);
}

// -----------------------------------------------------------------------
//                          SIGNALS
// -----------------------------------------------------------------------
//
// Following signals may be called from another thread (see task.cpp)


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

    if( CmdLine_AutoExit )
    {
        CmdLine_AutoPlot = false;
        p4startdlg->OnQuit();
        return;
    }
    
    if( CmdLine_AutoPlot )
    {
        CmdLine_AutoPlot = false;
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

    if( CmdLine_AutoEvaluate )
    {
        CmdLine_AutoEvaluate = false;
        p4startdlg->Find_Window->OnBtnEval();
        return;
    }
    if( CmdLine_AutoPlot )
    {
        CmdLine_AutoPlot = false;
        p4startdlg->OnPlot();
        return;
    }
}

void QP4Application::Signal_Saved( void )
{
    QP4Event * e = new QP4Event( (QEvent::Type)TYPE_SIGNAL_SAVED, nullptr );
    p4app->postEvent( p4startdlg, e );
}

QP4Event::QP4Event( QEvent::Type t, void * data )
                    : QEvent(t)
{
    customData = data;
}

void * QP4Event::data() const { return customData; }

QP4Event::~QP4Event()
{
    // data is freeed by caller                     
}
                     
