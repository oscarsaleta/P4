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

#include <stdlib.h>
#include <stdio.h>
#include "lyapunov.h"

// --------------------------------------------------------------------------
//                      READ_TABLE
// --------------------------------------------------------------------------

void read_table( const char * file )
{
	FILE * fp;
	int l, t, degx, degy;
	double re, im;
    int dummy;
	
	vec_field = (hom_poly *)malloc( sizeof( hom_poly ) );
    vec_field->next_hom_poly = nullptr;

	fp = fopen( file, "r" );
    if( fp == nullptr )
	{
		perror(file);
		exit(-5);
	}

    fscanf(fp, "%d", &dummy ); // dummy should be 0, it is only nonzero when the mpf version of this program is called.

	fscanf( fp, "%d %lf", &weakness_level, &precision );
	fscanf( fp, "%i\n\n", &l );
	for( t = 1; t <= l; t++ )
	{
		fscanf( fp, "%i\n\n%i\n\n%lf\n\n%lf\n\n", &degx, &degy, &re, &im );
		ins_hom_poly( vec_field, degx, degy, re, im );
	}
	fclose( fp );
}
