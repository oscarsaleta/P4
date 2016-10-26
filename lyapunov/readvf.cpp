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
