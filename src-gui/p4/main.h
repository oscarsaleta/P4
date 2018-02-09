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
    P4POLYLINES() {}
    P4POLYLINES(double a1, double b1, double a2, double b2)
        : x1{a1}, y1{b1}, x2{a2}, y2{b2}
    {
    }
};

// (g_VFResults.p_,g_VFResults.q_) are lyapunov weights
extern P4ParentStudy g_VFResults;

// true when Limitcycles window is shown.
// in that case, left mouse button action is redefined
// to set set points for LC window instead of for orbits window
extern bool g_LCWindowIsUp;

extern QString g_p4Version;
extern QString g_p4VersionDate;
extern QString g_p4Platform;

extern std::unique_ptr<QPixmap> g_p4smallicon;
extern std::unique_ptr<QPrinter> g_p4printer;

extern bool g_action_OnlyPrepareFile;
extern bool g_action_SaveAll;

extern QString g_cmdLine_Filename;
extern bool g_cmdLine_AutoEvaluate;
extern bool g_cmdLine_AutoPlot;
extern bool g_cmdLine_AutoExit;

void setP4WindowTitle(QWidget *win, QString title);

void handleCommandLineOption(char *arg);
void handleCommandLineArgument(char *arg);
