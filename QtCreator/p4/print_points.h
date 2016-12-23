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

#ifndef PRINT_POINTS_H
#define PRINT_POINTS_H


#include "main.h"

#include <QString>



extern void (* print_saddle)( double, double );
extern void (* print_stablenode)( double, double );
extern void (* print_unstablenode)( double, double );
extern void (* print_stableweakfocus)( double, double );
extern void (* print_unstableweakfocus)( double, double );
extern void (* print_weakfocus)( double, double );
extern void (* print_stablestrongfocus)( double, double );
extern void (* print_unstablestrongfocus)( double, double );
extern void (* print_sesaddle)( double, double );
extern void (* print_sesaddlenode)( double, double );
extern void (* print_sestablenode)( double, double );
extern void (* print_seunstablenode)( double, double );
extern void (* print_degen)( double, double );
extern void (* print_center)( double, double );
extern void (* print_elips)( double, double, double, double, int, bool, P4POLYLINES * );
extern void (* print_line)( double, double, double, double, int );
extern void (* print_point)( double, double, int );
extern void (* print_comment)( QString );


#endif // PRINT_POINTS_H
