#include <qdir.h>
#include <qsettings.h>
#include "win_p4.h"
#include "file_vf.h"

extern QString GetDefaultP4Path( void );
extern QString GetDefaultP4MaplePath( void );
extern QString GetDefaultP4BinPath( void );
extern QString GetDefaultP4Path( void );
extern QString GetDefaultP4ReducePath( void );
extern QString GetDefaultP4TempPath( void );
extern QString GetDefaultP4HelpPath( void );
extern QString GetDefaultP4SumTablePath( void );
extern QString GetDefaultMapleInstallation( void );
extern QString GetDefaultReduceInstallation( void );
extern QString GetDefaultMathManipulator( void );

/*
    P4 Base Path
            --> /bin        for bin files
            --> /help       for help files
            --> /bin        for maple files
            --> /reduce     for reduce files
    Sumtable path
    Temporary path
    Maple Exe
    Reduce Exe


*/

static QString Settings_mathmanipulator;
static QString Settings_p4path;
static QString Settings_sumtablepath;
static QString Settings_temppath;
static QString Settings_mapleexe;
static QString Settings_reduceexe;
static bool Settings_changed;

QString GetP4Path( void ) { return Settings_p4path; }
QString GetP4TempPath( void ) { return Settings_temppath; }
QString GetP4SumTablePath( void ) { return Settings_sumtablepath; }
QString GetMapleExe( void ) { return Settings_mapleexe; }
QString GetReduceExe( void ) { return Settings_reduceexe; }

void SetMathManipulator( QString s )
{
     if( Settings_mathmanipulator != s )
     {
         Settings_mathmanipulator = s;
         Settings_changed = true;
     }
}

void SetP4Path( QString s )
{
    if( Settings_p4path != s )
    {
        Settings_p4path = s;
        Settings_changed = true;
    }
}

void SetP4TempPath( QString s )
{
    if( Settings_temppath != s )
    {
        Settings_temppath = s;
        Settings_changed = true;
    }
}

void SetP4SumTablePath( QString s )
{
    if( Settings_sumtablepath != s )
    {
        Settings_sumtablepath = s;
        Settings_changed = true;
    }
}

void SetMapleExe( QString s )
{
    if( Settings_mapleexe != s )
    {
        Settings_mapleexe = s;
        Settings_changed = true;
    }
}

void SetReduceExe( QString s )
{
    if( Settings_reduceexe != s )
    {
        Settings_reduceexe = s;
        Settings_changed = true;
    }
}

extern QString GetP4MaplePath( void )
{
    QString f, g;
    f = GetP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "bin";
    }
    return g;
}

QString GetMathManipulator( void ) { return Settings_mathmanipulator; }

int GetMathPackage(void)
{
     QString s;
     s = GetMathManipulator().simplified().toLower();
     if( s == "maple" )
         return PACKAGE_MAPLE;
     else
         return PACKAGE_REDUCE;
}

void SetMathPackage( int pck )
{
    if( GetMathPackage() != pck )
    {
        if( pck == PACKAGE_MAPLE )
            SetMathManipulator("Maple");
        else
            SetMathManipulator("Reduce");
    }
}

extern QString GetP4HelpPath( void )
{
    QString f, g;
    f = GetP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "help";
    }
    return g;
}

extern QString GetP4BinPath( void )
{
    QString f, g;
    f = GetP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "bin";
    }
    return g;
}

extern QString GetP4ReducePath( void )
{
    QString f, g;

    f = GetP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "reduce";
    }

    return g;
}

// ------------------------------------------------------------------------------------------

bool ReadP4Settings( void )
{
    QSettings * p4settings;
    bool _ok;

    p4settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope, "P4", "P4" );

    Settings_changed = false;

    _ok = false;

    if( p4settings->value( "/Version" ).toString() == P4Version )
    {
        _ok = true;
        Settings_p4path       = p4settings->value( "/InstallPath" ).toString();
        Settings_sumtablepath = p4settings->value( "/SumtablePath" ).toString();
        Settings_temppath     = p4settings->value( "/TempPath" ).toString();
        Settings_mapleexe     = p4settings->value( "/MapleExe" ).toString();
#ifndef Q_OS_WIN
        Settings_reduceexe    = p4settings->value( "/ReduceExe" ).toString();
        Settings_mathmanipulator = p4settings->value( "/Math" ).toString();
#else
        Settings_mathmanipulator = "Maple";
#endif
       if( Settings_p4path == "" || (Settings_mapleexe == "" 
#ifndef Q_OS_WIN
               && Settings_reduceexe == ""
#endif
                  ))
        {
            _ok = false;
        }
    }

    delete p4settings;
    p4settings = NULL;

    if( !_ok )
    {
        Settings_p4path = GetDefaultP4Path();
        Settings_sumtablepath = GetDefaultP4SumTablePath();
        Settings_temppath = GetDefaultP4TempPath();
        Settings_mapleexe = GetDefaultMapleInstallation();
        Settings_reduceexe = GetDefaultReduceInstallation();
        Settings_mathmanipulator = GetDefaultMathManipulator();
        Settings_changed = true;
        return false;
    }

    return true;
}

void SaveP4Settings( void )
{
    QSettings * p4settings;

    if( Settings_changed == false )
        return;

    p4settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope, "P4", "P4" );

    p4settings->setValue( "/Version", P4Version );
    p4settings->setValue( "/BuildDate", P4VersionDate );
    p4settings->setValue( "/InstallPath", GetP4Path() );
    p4settings->setValue( "/SumtablePath", GetP4SumTablePath() );
    p4settings->setValue( "/TempPath", GetP4TempPath() );
    p4settings->setValue( "/MapleExe", GetMapleExe() );
#ifndef Q_OS_WIN
    p4settings->setValue( "/Math", GetMathManipulator() );
    p4settings->setValue( "/ReduceExe", GetReduceExe() );
#endif

    delete p4settings;
    p4settings = NULL;
}
