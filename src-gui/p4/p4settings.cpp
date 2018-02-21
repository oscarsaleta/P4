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

static QString sSettingsMathManipulator;
static QString sSettingsP4Path;
static QString sSettingsSumtablePath;
static QString sSettingsTempPath;
static QString sSettingsMapleExe;
// static QString sSettingsReduceExe;
static bool sSettingsChanged;

QString getP4Path(void) { return sSettingsP4Path; }
QString getP4TempPath(void) { return sSettingsTempPath; }
QString getP4SumTablePath(void) { return sSettingsSumtablePath; }
QString getMapleExe(void) { return sSettingsMapleExe; }

void setMathManipulator(QString s)
{
    if (sSettingsMathManipulator != s) {
        sSettingsMathManipulator = s;
        sSettingsChanged = true;
    }
}

void setP4Path(QString s)
{
    if (sSettingsP4Path != s) {
        sSettingsP4Path = s;
        sSettingsChanged = true;
    }
}

void setP4TempPath(QString s)
{
    if (sSettingsTempPath != s) {
        sSettingsTempPath = s;
        sSettingsChanged = true;
    }
}

void setP4SumTablePath(QString s)
{
    if (sSettingsSumtablePath != s) {
        sSettingsSumtablePath = s;
        sSettingsChanged = true;
    }
}

void setMapleExe(QString s)
{
    if (sSettingsMapleExe != s) {
        sSettingsMapleExe = s;
        sSettingsChanged = true;
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

QString getMathManipulator(void) { return sSettingsMathManipulator; }

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

    sSettingsChanged = false;

    if (p4settings->value("/Version").toString() == gP4version) {
        _ok = true;
        sSettingsP4Path = p4settings->value("/InstallPath").toString();
        sSettingsSumtablePath = p4settings->value("/SumtablePath").toString();
        sSettingsTempPath = p4settings->value("/TempPath").toString();
        sSettingsMapleExe = p4settings->value("/MapleExe").toString();
        sSettingsMathManipulator = "Maple";
        if (sSettingsP4Path == "" || (sSettingsMapleExe == "")) {
            _ok = false;
        }
    }

    p4settings.reset();

    if (!_ok) {
        sSettingsP4Path = getDefaultP4Path();
        sSettingsSumtablePath = getDefaultP4SumTablePath();
        sSettingsTempPath = getDefaultP4TempPath();
        sSettingsMapleExe = getDefaultMapleInstallation();
        sSettingsMathManipulator = getDefaultMathManipulator();
        sSettingsChanged = true;
        return false;
    }

    return true;
}

void saveP4Settings(void)
{
    if (sSettingsChanged == false)
        return;

    std::unique_ptr<QSettings> p4settings{new QSettings()};

    p4settings->setValue("/Version", gP4version);
    p4settings->setValue("/BuildDate", gP4versionDate);
    p4settings->setValue("/InstallPath", getP4Path());
    p4settings->setValue("/SumtablePath", getP4SumTablePath());
    p4settings->setValue("/TempPath", getP4TempPath());
    p4settings->setValue("/MapleExe", getMapleExe());
#ifndef Q_OS_WIN
    p4settings->setValue("/Math", getMathManipulator());
#endif

    p4settings.reset();
}
