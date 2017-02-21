/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
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
 
#include "lyapunov.h"

#include <stdlib.h>
#include <stdio.h>

// --------------------------------------------------------------------------
//                      CHECK_SUM
// --------------------------------------------------------------------------
//
// check if the table exist

void check_sum( char * s, int k )
{
	FILE * fp;

	if( env_windows )
	{
        if( win_sumtablepath == nullptr )
			sprintf( s, "sum%d.tab", k );
		else
			sprintf( s, "%ssum%d.tab", win_sumtablepath, k );
	}
	else
	{
        if( getenv( "P4_DIR" )== nullptr ){
			sprintf( s, "sum%d.tab", k );
        }
		else
            sprintf( s, "%s/sumtables/sum%d.tab", getenv( "P4_DIR" ), k );
	}
	
	fp = fopen( s, "r" );
    if ( fp == nullptr )
	{
		printf( "table sum%d.tab does not exist\n", k );
		printf( "creating the table\n" );
		printf( "please wait \n" );
		create_sum( k );
	}
	else
	{
		printf( "table sum%d.tab exist\n", k );
		fclose( fp );
	}
}
