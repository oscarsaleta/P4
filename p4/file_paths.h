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

#ifndef EXTERN_METHODS_H
#define EXTERN_METHODS_H


#include <QString>


// Methods implemented in file_paths.cpp

QString getDefaultP4Path( void );
QString getDefaultP4TempPath( void );
QString getDefaultP4SumTablePath( void );
QString getDefaultMathManipulator( void );
QString getDefaultP4HelpPath( void );
QString getDefaultP4ReducePath( void );
QString getDefaultP4MaplePath( void );
QString getDefaultP4BinPath( void );
QString getDefaultReduceInstallation( void );
QString getDefaultMapleInstallation( void );
void removeFile( QString fname );


#endif // EXTERN_METHODS_H
