/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4
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

#ifndef MAIN_H
#define MAIN_H


#include <QPixmap>
#include <QPrinter>
#include <QString>


#define UNUSED(x) (void)(x)   //  to avoid warnings of unused variables in case we cannot avoid it

struct P4POLYLINES
{
    double x1;
    double y1;
    double x2;
    double y2;
    P4POLYLINES * next;
};

extern bool lcWindowIsUp;	// true when Limitcycles window is shown.
                            // in that case, left mouse button action is redefined
                            // to set set points for LC window instead of for orbits window

extern QString p4Version;
extern QString p4VersionDate;
extern QString p4Platform;

extern QPixmap * p4smallicon;
extern QPrinter * p4printer;

extern bool action_OnlyPrepareFile;
extern bool action_SaveAll;

extern QString cmdLine_Filename;
extern bool cmdLine_AutoEvaluate;
extern bool cmdLine_AutoPlot;
extern bool cmdLine_AutoExit;

void setP4WindowTitle( QWidget * win, QString title );

void handleCommandLineOption( char * arg );
void handleCommandLineArgument( char * arg );


#endif // MAIN_H
