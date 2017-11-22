/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere.
 *  URL: http://github.com/oscarsaleta/P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
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

#include "main.h"

#include "file_vf.h"
#include "p4application.h"
#include "p4settings.h"
#include "win_find.h"
#include "win_main.h"
#include "win_settings.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#include "../version.h"
#endif

#include <QMessageBox>
#include <QPixmap>
#include <QPrinter>

/*
    ----------------------------------------------------------------------------
    TO DO LIST:
    -----------

    -   poincare-lyapunov printing: improve dotted line when printing
    -   system printer support

    -----------------------------------------------------------------------------
*/

QString g_p4Version;
QString g_p4VersionDate;
QString g_p4Platform;

bool g_action_OnlyPrepareFile =
    false; // in the find menu (this is not saved in the .inp file)
bool g_action_SaveAll =
    DEFAULTSAVEALL; // in the find menu (this is not saved in the .inp file)

QPixmap *g_p4smallicon = nullptr;
QPrinter *g_p4printer = nullptr;

QString g_cmdLine_Filename;
bool g_cmdLine_AutoEvaluate;
bool g_cmdLine_AutoPlot;
bool g_cmdLine_AutoExit;

void handleCommandLineOption(char *arg)
{
    while (*arg != 0) {
        if (*arg == 'e' || *arg == 'E') // auto-evaluate
        {
            g_cmdLine_AutoEvaluate = true;
            arg++;
            continue;
        }

        if (*arg == 'p' || *arg == 'P') // auto-plot
        {
            g_cmdLine_AutoPlot = true;
            arg++;
            continue;
        }

        if (*arg == 'x' || *arg == 'X') // auto-evaluate + exit
        {
            g_cmdLine_AutoEvaluate = true;
            g_cmdLine_AutoExit = true;
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

    if (g_cmdLine_Filename.length() == 0) {
        g_cmdLine_Filename = arg;
        return;
    }

    // a second filename given??? --> ignore

    return;
}

int main(int argc, char *argv[])
{
    int returnvalue;
    int v, i;

    g_p4Platform = "";
    g_p4Version = VERSION; // initialize version string
    g_p4VersionDate = VERSIONDATE;

    g_cmdLine_Filename = "";
    g_cmdLine_AutoEvaluate = false;
    g_cmdLine_AutoPlot = false;
    g_cmdLine_AutoExit = false;

    for (i = 1; i < argc; i++)
        handleCommandLineArgument(argv[i]);
    if (g_cmdLine_AutoEvaluate && g_cmdLine_Filename.length() == 0)
        g_cmdLine_Filename = DEFAULTFILENAME;

    g_p4app = new QP4Application(argc, argv);
    g_p4app->setOrganizationName("P4");
    g_p4app->setOrganizationDomain("gsd.uab.cat");
    g_p4app->setApplicationName("P4");

    g_p4app->addLibraryPath(g_p4app->applicationDirPath());

    g_p4app->setQuitOnLastWindowClosed(false);
    v = readP4Settings();

    g_p4printer = new QPrinter(QPrinter::PrinterResolution);
    g_p4smallicon = new QPixmap();
    if (g_p4smallicon->load(getP4BinPath() + "/p4smallicon.ico") == false) {
        delete g_p4smallicon;
        g_p4smallicon = nullptr;
    }

    //  g_p4app->setStyle( new QCDEStyle() );

    g_ThisVF = new QInputVF();

    g_p4stardlg = new QStartDlg(g_cmdLine_Filename);
    if (!g_cmdLine_AutoExit)
        g_p4stardlg->show();

    g_p4app->setQuitOnLastWindowClosed(true);

    if (g_cmdLine_Filename.length() != 0) {
        if (!(g_ThisVF->load())) {
            g_cmdLine_AutoEvaluate = false;
            g_cmdLine_AutoPlot = false;
            if (g_cmdLine_AutoExit) {
                delete g_p4printer;
                g_p4printer = nullptr;
                delete g_p4app;
                g_p4app = nullptr;
                return (-1);
            }
        }
        g_p4stardlg->findWindow_->onBtnLoad();
    }

    if (!v) {
        QMessageBox::information(g_p4stardlg, "P4",
                                 "Welcome to P4.\n\n"
                                 "Since P4 did not find program settings, it "
                                 "assumes that this is the first time\n"
                                 "you start the program.  So Please check "
                                 "whether the settings are correct.\n\n"
                                 "In particular, check if P4 has found the "
                                 "Command-Line Version of MAPLE.\n");

        QSettingsDlg *psettings;
        psettings = new QSettingsDlg(nullptr, 0);
        psettings->exec();
        delete psettings;
        psettings = nullptr;
    }

    returnvalue = g_p4app->exec();

    delete g_p4printer;
    g_p4printer = nullptr;

    saveP4Settings();

    delete g_p4app;
    g_p4app = nullptr;
    return returnvalue;
}

// -----------------------------------------------------------------------
//                          SETP4WINDOWTITLE
// -----------------------------------------------------------------------

void setP4WindowTitle(QWidget *win, QString title)
{
    win->setWindowTitle(title);
}
