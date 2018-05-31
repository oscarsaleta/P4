/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere.
 *  URL: http://github.com/oscarsaleta/P4
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

#include "main.hpp"

#include <QDebug>
#include <QMessageBox>
#include <QPixmap>
#include <QPrinter>

#include "P4Application.hpp"
#include "P4FindDlg.hpp"
#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4SettingsDlg.hpp"
#include "P4StartDlg.hpp"
#include "p4settings.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#include "../version.h"
#endif

/*
    ----------------------------------------------------------------------------
    TO DO LIST:
    -----------

    -   poincare-lyapunov printing: improve dotted line when printing
    -   system printer support

    ----------------------------------------------------------------------------
*/

QString gP4version;
QString gP4versionDate;
QString gP4platform;

// in the find menu (this is not saved in the .inp file)
bool gActionOnlyPrepareFile = false;
// in the find menu (this is not saved in the .inp file)
bool gActionSaveAll = DEFAULTSAVEALL;

QPixmap *gP4smallIcon;
QPrinter *gP4printer;

QString gCmdLineFilename;
bool gCmdLineAutoEvaluate;
bool gCmdLineAutoPlot;
bool gCmdLineAutoExit;

P4ParentStudy gVFResults;
P4InputVF *gThisVF;
P4StartDlg *gP4startDlg;
P4Application *gP4app;

// -----------------------------------------------------------------------
//          Functions for handling command line options
// -----------------------------------------------------------------------
void handleCommandLineOption(char *arg)
{
    while (*arg != 0) {
        if (*arg == 'e' || *arg == 'E') // auto-evaluate
        {
            gCmdLineAutoEvaluate = true;
            arg++;
            continue;
        }

        if (*arg == 'p' || *arg == 'P') // auto-plot
        {
            gCmdLineAutoPlot = true;
            arg++;
            continue;
        }

        if (*arg == 'x' || *arg == 'X') // auto-evaluate + exit
        {
            gCmdLineAutoEvaluate = true;
            gCmdLineAutoExit = true;
            arg++;
            continue;
        }

        // unknown option: skip

        break;
    }
}

void handleCommandLineArgument(char *arg)
{
    if (*arg == '-') {
        handleCommandLineOption(arg + 1);
        return;
    }

    if (gCmdLineFilename.length() == 0) {
        gCmdLineFilename = arg;
        return;
    }

    // a second filename given??? --> ignore

    return;
}

// -----------------------------------------------------------------------
//          Main function
// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
    qSetMessagePattern("%{file}(%{line}): %{message}");

    int returnvalue;
    int v, i;

    gP4platform = "";
    gP4version = VERSION; // initialize version string
    gP4versionDate = VERSIONDATE;

    gCmdLineFilename = "";
    gCmdLineAutoEvaluate = false;
    gCmdLineAutoPlot = false;
    gCmdLineAutoExit = false;

    for (i = 1; i < argc; i++)
        handleCommandLineArgument(argv[i]);
    if (gCmdLineAutoEvaluate && gCmdLineFilename.length() == 0)
        gCmdLineFilename = DEFAULTFILENAME;

    gP4app = new P4Application{argc, argv};
    gP4app->setOrganizationName("P4");
    gP4app->setOrganizationDomain("gsd.uab.cat");
    gP4app->setApplicationName("P4");

    gP4app->addLibraryPath(gP4app->applicationDirPath());

    gP4app->setQuitOnLastWindowClosed(false);
    v = readP4Settings();

    gP4printer = new QPrinter{QPrinter::PrinterResolution};
    gP4smallIcon = new QPixmap{};
    if (gP4smallIcon->load(getP4BinPath() + "/p4smallicon.ico") == false) {
        delete gP4smallIcon;
        gP4smallIcon = nullptr;
    }

    //  gP4app->setStyle( new QCDEStyle() );

    gThisVF = new P4InputVF{};

    gP4startDlg = new P4StartDlg{gCmdLineFilename};
    if (!gCmdLineAutoExit)
        gP4startDlg->show();

    gP4app->setQuitOnLastWindowClosed(true);

    if (gCmdLineFilename.length() != 0) {
        if (!(gThisVF->load())) {
            gCmdLineAutoEvaluate = false;
            gCmdLineAutoPlot = false;
            if (gCmdLineAutoExit) {
                delete gP4printer;
                gP4printer = nullptr;
                delete gP4app;
                gP4app = nullptr;
                return (-1);
            }
        }
        gP4startDlg->getFindWindowPtr()->onBtnLoad();
    }

    if (!v) {
        QMessageBox::information(gP4startDlg, "P4",
                                 "Welcome to P4.\n\n"
                                 "Since P4 did not find program settings, it "
                                 "assumes that this is the first time\n"
                                 "you start the program.  So Please check "
                                 "whether the settings are correct.\n\n"
                                 "In particular, check if P4 has found the "
                                 "Command-Line Version of MAPLE.\n");

        auto psettings =
            new P4SettingsDlg{nullptr, static_cast<Qt::WindowFlags>(0)};
        psettings->exec();
    }

    returnvalue = gP4app->exec();

    saveP4Settings();

    if (gP4smallIcon != nullptr) {
        delete gP4smallIcon;
        gP4smallIcon = nullptr;
    }
    if (gP4printer != nullptr) {
        delete gP4printer;
        gP4printer = nullptr;
    }
    if (gP4startDlg != nullptr) {
        delete gP4startDlg;
        gP4startDlg = nullptr;
    }
    if (gP4app != nullptr) {
        delete gP4app;
        gP4app = nullptr;
    }
    if (gThisVF != nullptr) {
        delete gThisVF;
        gThisVF = nullptr;
    }

    return returnvalue;
}

// -----------------------------------------------------------------------
//                          SETP4WINDOWTITLE
// -----------------------------------------------------------------------

void setP4WindowTitle(QWidget *win, const QString &title)
{
    win->setWindowTitle(title);
}
