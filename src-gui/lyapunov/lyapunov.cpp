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


#include "lyapunov.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#include "../version.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/*
	LYAPUNOV	inputfile outputfile

			--> work in reduce/linux mode

			In linux mode, we use the env. variable P4_DIR
			to locate the sumtable files.  New sum tables
			are created in the current working directory temporarily,
			and are then copied with the correct permissions to the
			P4_DIR/sum_tables directory.

			The output file is in a syntax that can be understood by reduce.

	LYAPUNOV	inputfile outputfile MAPLE

			--> work in maple/linux mode

			Same as before for the sum tables, but the outputfile
			has the syntax that can be understood by maple.

	LYAPUNOV	inputfile outputfile MAPLE WINDOWS [path]

			--> work in maple/windows mode
				optionally use path to load&store sum tables.
				if not present, using current working directory.
*/

// initialise lyapunov.h global variables
bool env_maple = false;
bool env_reduce = false;
bool env_windows = false;
int weakness_level = 0;
double precision = 0.0;
char * win_sumtablepath = nullptr;
struct hom_poly volatile * vec_field = nullptr;

// --------------------------------------------------------------------------
//                      REMOVEQUOTES
// --------------------------------------------------------------------------

char * RemoveQuotes( char * x )
{
	while( *x == '\"' || *x == ' ' )
		x++;

	while( strlen(x) > 0 )
	{
		if( x[strlen(x)-1] == '\"' || x[strlen(x)-1] == ' ' )
			x[strlen(x)-1] = 0;
		else
			break;
	}

	return x;
}

// --------------------------------------------------------------------------
//                      ADDTRAILINGSLASH
// --------------------------------------------------------------------------

char * AddTrailingSlash( char * buf, char s )
{
	int i;
	i = strlen(buf);
	if(i>0)
	{
		if( buf[i-1] != '\\' && buf[i-1] != '/' )
		{
			buf[i] = s;
			buf[i+1] = 0;
		}
	}

	return buf;
}

// --------------------------------------------------------------------------
//                      MAIN
// --------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	double V;
	int k, ok = 0;
	char file_name[ 220 ], s[ DIM2 ];
	static char wstpbuf[256];
	FILE *fp, *fp2;

    env_reduce = true;
	env_maple = false;
	env_windows = false;
    win_sumtablepath = nullptr;

    if( argc <= 1 )
    {
        printf( "Lyapunov Version: %s Date: %s\n%s", VERSION, VERSIONDATE,
                "THIS FILE IS PART OF THE P4 DISTRIBUTION.\n"
                "It is called automatically from reduce/maple.  It calculates\n"
                "lyapunov coefficients for a weak focus/center.\n\n"
                "SYNTAX: lyapunov inputfile outputfile\n"
                "\t--> work in reduce/linux mode\n"
                "\tIn linux mode, we use the env. variable P4_DIR to locate\n"
                "\tthe sumtable files.  New sum tables are created in the current\n"
                "\tworking directory temporarily, and are then copied with the \n"
                "\tcorrect permissions to the P4_DIR/sum_tables directory.\n"
                "\tThe output file is in a syntax that can be understood by reduce.\n"
                "\n"
                "SYNTAX: lyapunov inputfile outputfile MAPLE\n"
                "\t--> work in maple/linux mode\n"
                "\tSame as before for the sum tables, but the outputfile\n"
                "\thas the syntax that can be understood by maple.\n"
                "\n"
                "SYNTAX: lyapunov inputfile outputfile MAPLE WINDOWS [path]\n"
                "\t--> work in maple/windows mode\n"
                "\toptionally use path to load&store sum tables.\n"
                "\tIf not present, using current working directory.\n" );
        exit(-3);
    }

    if( getenv( "P4_DIR" ) == nullptr )
	{
		printf( "Warning: P4_DIR environment variable is unset.\n" );
		printf( "Using current directory as place to store sumtables...\n");
	}

	if( argc >= 4 )
	{
		if( !strcmp( argv[3], "MAPLE" ) )
        {
            env_maple = true;
            env_reduce = false;
        }

		if( argc >= 5 )
		{
		    if( !strcmp( argv[4], "WINDOWS" ) )
			{
				env_windows = true;
				env_reduce = false;
				env_maple = true;
				if( argc >= 6 )
				{
					win_sumtablepath = AddTrailingSlash(RemoveQuotes(strcpy(wstpbuf,argv[5])),'\\');
				}
			}
		}
	}
	read_table( RemoveQuotes( argv[1]) );  // read in precision and vector field

	fp2 = fopen( RemoveQuotes(argv[2]), "w" );
    if( fp2 == nullptr )
		exit(-4);

	if( env_reduce )
	{
		fprintf( fp2, "off echo$\n" );
		fprintf( fp2, "openfile(result_file)$\n" );
		fprintf( fp2, "write$\n" );
	}

	for( k = 1; k <= weakness_level; k++ )
	{
		check_sum( file_name, 2*k );

		// check if table for the decomposition of 2*k exist, if not create it

		fp = fopen( file_name, "r" );
        if( fp == nullptr )
		{
			perror(file_name);
			exit(-3);
		}

		V = 0.0;  // V is the lyapunov coeff that we want to calculate
		while ( !feof( fp ) )
		{
            if (fscanf( fp, "%s", s )!=1) {
                fprintf(stderr,"Error reading Lyapunov coeff.\n");
                exit(1);
            }
			if ( !feof( fp ) )
            {
                V -= part_lyapunov_coeff( s, 2 * k + 1 );
            }
		}
		fclose( fp );
		// V*=2.0*PI/(k+1);

		printf( "k=%d V=%20.19f, precision=%20.19f\n", k, V, precision );
		
		if( env_reduce )
			fprintf( fp2, "write \"V(%d)=%e\"$\n", 2 * k + 1, V );

		if( env_maple )
			fprintf( fp2, "%d\n%e\n", 2*k+1, V );
		
		if( fabs(V) >= precision )
		{
			ok = 1;
			if( env_maple )
				fprintf( fp2, "-1\n" );
			break;
		}
	}

	// write the result to a file

	if( env_reduce )
	{
		fprintf( fp2, "out 't$\n" );
		if( ok )
			fprintf( fp2, "w:=%d$\n lyp:=%e$\n", k, V );
		else
			fprintf( fp2, "lyp:=0$\n" );
	}

	if( env_maple )
	{
		if( ok )
			fprintf( fp2, "1\n%d\n%e\n", k, V );
		else
			fprintf( fp2, "0\n0\n0\n" );
	}

	fclose( fp2 );
	return 0;
}
