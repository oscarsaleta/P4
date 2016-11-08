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

/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "p4settings.h"

#include "file_vf.h"
#include "file_paths.h"

#include <QDir>
#include <QSettings>


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

QString getP4Path( void ) { return Settings_p4path; }
QString getP4TempPath( void ) { return Settings_temppath; }
QString getP4SumTablePath( void ) { return Settings_sumtablepath; }
QString getMapleExe( void ) { return Settings_mapleexe; }
QString getReduceExe( void ) { return Settings_reduceexe; }

void setMathManipulator( QString s )
{
     if( Settings_mathmanipulator != s )
     {
         Settings_mathmanipulator = s;
         Settings_changed = true;
     }
}

void setP4Path( QString s )
{
    if( Settings_p4path != s )
    {
        Settings_p4path = s;
        Settings_changed = true;
    }
}

void setP4TempPath( QString s )
{
    if( Settings_temppath != s )
    {
        Settings_temppath = s;
        Settings_changed = true;
    }
}

void setP4SumTablePath( QString s )
{
    if( Settings_sumtablepath != s )
    {
        Settings_sumtablepath = s;
        Settings_changed = true;
    }
}

void setMapleExe( QString s )
{
    if( Settings_mapleexe != s )
    {
        Settings_mapleexe = s;
        Settings_changed = true;
    }
}

void setReduceExe( QString s )
{
    if( Settings_reduceexe != s )
    {
        Settings_reduceexe = s;
        Settings_changed = true;
    }
}

extern QString getP4MaplePath( void )
{
    QString f, g;
    f = getP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "bin";
    }
    return g;
}

QString getMathManipulator( void ) { return Settings_mathmanipulator; }

int getMathPackage(void)
{
     QString s;
     s = getMathManipulator().simplified().toLower();
     if( s == "maple" )
         return PACKAGE_MAPLE;
     else
         return PACKAGE_REDUCE;
}

void setMathPackage( int pck )
{
    if( getMathPackage() != pck )
    {
        if( pck == PACKAGE_MAPLE )
            setMathManipulator("Maple");
        else
            setMathManipulator("Reduce");
    }
}

extern QString getP4HelpPath( void )
{
    QString f, g;
    f = getP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "help";
    }
    return g;
}

extern QString getP4BinPath( void )
{
    QString f, g;
    f = getP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "bin";
    }
    return g;
}

QString getP4ReducePath( void )
{
    QString f, g;

    f = getP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "reduce";
    }

    return g;
}

// ------------------------------------------------------------------------------------------

bool readP4Settings( void )
{
    QSettings * p4settings;
    bool _ok;

    p4settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope, "P4", "P4" );

    Settings_changed = false;

    _ok = false;

    if( p4settings->value( "/Version" ).toString() == p4Version )
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
    p4settings = nullptr;

    if( !_ok )
    {
        Settings_p4path = getDefaultP4Path();
        Settings_sumtablepath = getDefaultP4SumTablePath();
        Settings_temppath = getDefaultP4TempPath();
        Settings_mapleexe = getDefaultMapleInstallation();
        Settings_reduceexe = getDefaultReduceInstallation();
        Settings_mathmanipulator = getDefaultMathManipulator();
        Settings_changed = true;
        return false;
    }

    return true;
}

void saveP4Settings( void )
{
    QSettings * p4settings;

    if( Settings_changed == false )
        return;

    p4settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope, "P4", "P4" );

    p4settings->setValue( "/Version", p4Version );
    p4settings->setValue( "/BuildDate", p4VersionDate );
    p4settings->setValue( "/InstallPath", getP4Path() );
    p4settings->setValue( "/SumtablePath", getP4SumTablePath() );
    p4settings->setValue( "/TempPath", getP4TempPath() );
    p4settings->setValue( "/MapleExe", getMapleExe() );
#ifndef Q_OS_WIN
    p4settings->setValue( "/Math", getMathManipulator() );
    p4settings->setValue( "/ReduceExe", getReduceExe() );
#endif

    delete p4settings;
    p4settings = nullptr;
}
