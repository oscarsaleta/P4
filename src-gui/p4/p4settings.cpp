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

#include "p4settings.hpp"

#include <QDir>
#include <QSettings>

#include "P4InputVF.hpp"
#include "file_paths.hpp"
#include "main.hpp"

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

QString getP4Path() { return sSettingsP4Path; }
QString getP4TempPath() { return sSettingsTempPath; }
QString getP4SumTablePath() { return sSettingsSumtablePath; }
QString getMapleExe() { return sSettingsMapleExe; }

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

QString getP4MaplePath()
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

QString getMathManipulator() { return sSettingsMathManipulator; }

QString getP4HelpPath()
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

QString getP4BinPath()
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

bool readP4Settings()
{
    auto p4settings = new QSettings{};
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

    delete p4settings;
    p4settings = nullptr;

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

void saveP4Settings()
{
    if (sSettingsChanged == false)
        return;

    auto p4settings = new QSettings{};
    p4settings->setValue("/Version", gP4version);
    p4settings->setValue("/BuildDate", gP4versionDate);
    p4settings->setValue("/InstallPath", getP4Path());
    p4settings->setValue("/SumtablePath", getP4SumTablePath());
    p4settings->setValue("/TempPath", getP4TempPath());
    p4settings->setValue("/MapleExe", getMapleExe());
#ifndef Q_OS_WIN
    p4settings->setValue("/Math", getMathManipulator());
#endif

    delete p4settings;
    p4settings = nullptr;
}
