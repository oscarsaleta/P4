/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
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

#include "p4settings.h"

#include "file_paths.h"
#include "P4InputVF.h"
#include "main.h"

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

static QString s_Settings_mathmanipulator;
static QString s_Settings_p4path;
static QString s_Settings_sumtablepath;
static QString s_Settings_temppath;
static QString s_Settings_mapleexe;
// static QString s_Settings_reduceexe;
static bool s_Settings_changed;

QString getP4Path(void) { return s_Settings_p4path; }
QString getP4TempPath(void) { return s_Settings_temppath; }
QString getP4SumTablePath(void) { return s_Settings_sumtablepath; }
QString getMapleExe(void) { return s_Settings_mapleexe; }

void setMathManipulator(QString s)
{
    if (s_Settings_mathmanipulator != s) {
        s_Settings_mathmanipulator = s;
        s_Settings_changed = true;
    }
}

void setP4Path(QString s)
{
    if (s_Settings_p4path != s) {
        s_Settings_p4path = s;
        s_Settings_changed = true;
    }
}

void setP4TempPath(QString s)
{
    if (s_Settings_temppath != s) {
        s_Settings_temppath = s;
        s_Settings_changed = true;
    }
}

void setP4SumTablePath(QString s)
{
    if (s_Settings_sumtablepath != s) {
        s_Settings_sumtablepath = s;
        s_Settings_changed = true;
    }
}

void setMapleExe(QString s)
{
    if (s_Settings_mapleexe != s) {
        s_Settings_mapleexe = s;
        s_Settings_changed = true;
    }
}

QString getP4MaplePath(void)
{
    QString f, g;
    f = getP4Path();
    if (f.isNull() == false) {
        g = f;
        g += QDir::separator();
        g += "bin";
    }
    return g;
}

QString getMathManipulator(void) { return s_Settings_mathmanipulator; }

int getMathPackage(void) { return PACKAGE_MAPLE; }

QString getP4HelpPath(void)
{
    QString f, g;
    f = getP4Path();
    if (f.isNull() == false) {
        g = f;
        g += QDir::separator();
        g += "help";
    }
    return g;
}

QString getP4BinPath(void)
{
    QString f, g;
    f = getP4Path();
    if (f.isNull() == false) {
        g = f;
        g += QDir::separator();
        g += "bin";
    }
    return g;
}

// ------------------------------------------------------------------------------------------

bool readP4Settings(void)
{
    std::unique_ptr<QSettings> p4settings{new QSettings()};
    bool _ok{false};

    s_Settings_changed = false;

    if (p4settings->value("/Version").toString() == g_p4Version) {
        _ok = true;
        s_Settings_p4path = p4settings->value("/InstallPath").toString();
        s_Settings_sumtablepath = p4settings->value("/SumtablePath").toString();
        s_Settings_temppath = p4settings->value("/TempPath").toString();
        s_Settings_mapleexe = p4settings->value("/MapleExe").toString();
        s_Settings_mathmanipulator = "Maple";
        if (s_Settings_p4path == "" || (s_Settings_mapleexe == "")) {
            _ok = false;
        }
    }

    p4settings.reset();

    if (!_ok) {
        s_Settings_p4path = getDefaultP4Path();
        s_Settings_sumtablepath = getDefaultP4SumTablePath();
        s_Settings_temppath = getDefaultP4TempPath();
        s_Settings_mapleexe = getDefaultMapleInstallation();
        s_Settings_mathmanipulator = getDefaultMathManipulator();
        s_Settings_changed = true;
        return false;
    }

    return true;
}

void saveP4Settings(void)
{
    if (s_Settings_changed == false)
        return;

    std::unique_ptr<QSettings> p4settings{new QSettings()};

    p4settings->setValue("/Version", g_p4Version);
    p4settings->setValue("/BuildDate", g_p4VersionDate);
    p4settings->setValue("/InstallPath", getP4Path());
    p4settings->setValue("/SumtablePath", getP4SumTablePath());
    p4settings->setValue("/TempPath", getP4TempPath());
    p4settings->setValue("/MapleExe", getMapleExe());
#ifndef Q_OS_WIN
    p4settings->setValue("/Math", getMathManipulator());
#endif

    p4settings.reset();
}
