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

#pragma once

#include <memory>

#include <QString>

class P4ParentStudy;

class QPixmap;
class QPrinter;
class QWidget;

// to avoid warnings of unused variables in case we cannot avoid it
//#define UNUSED(x) (void)(x)

// FIXME fix usages
struct P4POLYLINES {
    double x1;
    double y1;
    double x2;
    double y2;
    P4POLYLINES(double a1 = 0, double b1 = 0, double a2 = 0, double b2 = 0)
        : x1{a1}, y1{b1}, x2{a2}, y2{b2}
    {
    }
};

// (gVFResults.p_,gVFResults.q_) are lyapunov weights
extern P4ParentStudy gVFResults;

// true when Limitcycles window is shown.
// in that case, left mouse button action is redefined
// to set set points for LC window instead of for orbits window
extern bool gLCWindowIsUp;

extern QString gP4version;
extern QString gP4versionDate;
extern QString gP4platform;

extern std::unique_ptr<QPixmap> gP4smallIcon;
extern std::unique_ptr<QPrinter> gP4printer;

extern bool gActionOnlyPrepareFile;
extern bool gActionSaveAll;

extern QString gCmdLineFilename;
extern bool gCmdLineAutoEvaluate;
extern bool gCmdLineAutoPlot;
extern bool gCmdLineAutoExit;

void setP4WindowTitle(QWidget *win, QString title);

void handleCommandLineOption(char *arg);
void handleCommandLineArgument(char *arg);
