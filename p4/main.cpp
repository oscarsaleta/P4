#include "main.h"

#include <QMessageBox>

#include "file_vf.h"
#include "p4application.h"
#include "p4settings.h"
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

QString p4Version;
QString p4VersionDate;
QString p4Platform;

bool action_OnlyPrepareFile = false;    // in the find menu (this is not saved in the .inp file)
bool action_SaveAll = DEFAULTSAVEALL;   // in the find menu (this is not saved in the .inp file)


QPixmap * p4smallicon = nullptr;
QPrinter * p4printer = nullptr;

QString cmdLine_Filename;
bool cmdLine_AutoEvaluate;
bool cmdLine_AutoPlot;
bool cmdLine_AutoExit;

void handleCommandLineOption( char * arg )
{
    while( *arg != 0 )
    {
        if( *arg == 'e' || *arg == 'E' )    // auto-evaluate
        {
            cmdLine_AutoEvaluate = true;
            arg++;
            continue;
        }
        
        if( *arg == 'p' || *arg == 'P' )    // auto-plot
        {
            cmdLine_AutoPlot = true;
            arg++;
            continue;
        }

        if( *arg == 'x' || *arg == 'X' )    // auto-evaluate + exit
        {
            cmdLine_AutoEvaluate = true;
            cmdLine_AutoExit = true;
            arg++;
            continue;
        }
        
        // unknown option: skip

        break;
    }
}

void handleCommandLineArgument( char * arg )
{
    if( *arg == '-' )
    {
        handleCommandLineOption( arg+1 );
        return;
    }
        
    if( cmdLine_Filename.length() == 0 )
    {
        cmdLine_Filename = arg;
        return;
    }
    
    // a second filename given??? --> ignore
    
    return;
}


int main( int argc, char *argv[] )
{
    int returnvalue;
    int v, i;

    p4Platform = "";
    p4Version = VERSION;        // initialize version string
    p4VersionDate = VERSIONDATE;

    cmdLine_Filename = "";
    cmdLine_AutoEvaluate = false;
    cmdLine_AutoPlot = false;
    cmdLine_AutoExit = false;
    
    for( i = 1; i < argc; i++ )
         handleCommandLineArgument( argv[i] );
    if( cmdLine_AutoEvaluate && cmdLine_Filename.length() == 0 )
        cmdLine_Filename = DEFAULTFILENAME;

    p4app = new QP4Application( argc, argv );

    p4app->addLibraryPath( p4app->applicationDirPath() );

    p4app->setQuitOnLastWindowClosed(false);
    v=readP4Settings();

    p4printer = new QPrinter( QPrinter::PrinterResolution );
    p4smallicon = new QPixmap();
    if( p4smallicon->load( getP4BinPath() + "/p4smallicon.png" ) == false )
    {
        delete p4smallicon;
        p4smallicon = nullptr;
    }

//  p4app->setStyle( new QCDEStyle() );

    ThisVF = new QInputVF();

    p4startdlg = new QStartDlg( cmdLine_Filename );
    if( !cmdLine_AutoExit )
        p4startdlg->show();

    p4app->setQuitOnLastWindowClosed(true);

    if( cmdLine_Filename.length() != 0 )
    {
        if( !(ThisVF->load()) )
        {
            cmdLine_AutoEvaluate = false;
            cmdLine_AutoPlot = false;
            if( cmdLine_AutoExit )
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

    saveP4Settings();

    delete p4app;
    p4app = nullptr;
    return returnvalue;
}

// -----------------------------------------------------------------------
//                          SETP4WINDOWTITLE
// -----------------------------------------------------------------------

void setP4WindowTitle( QWidget * win, QString title )
{
    win->setWindowTitle(title);
}
