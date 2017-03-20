#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lyapunov_mpf.h"

// --------------------------------------------------------------------------
//                      CREATE_SUM
// --------------------------------------------------------------------------

void create_sum( int k )
{
	int i;
	FILE * fp1;
	FILE * fp2;
	char file_name[220];
	char s[DIM2];
	char file_name2[220];
	char s1[220];
	char s2[10];

	if( env_windows )
	{
		if( win_sumtablepath == NULL )
			sprintf( s2, "sum%d.tab", k );
		else
			sprintf( s2, "%ssum%d.tab", win_sumtablepath, k );
	}
	else
	{
		sprintf( s2, "sum%d.tab", k );
		if( getenv( "P4_DIR" ) == NULL )
			*file_name2 = 0;
		else
			sprintf( file_name2, "%s/sum_tables/sum%d.tab", getenv( "P4_DIR" ), k );
	}

	fp1 = fopen( s2, "w" );
	if( fp1 == NULL )
	{
        	printf( "Cannot create sumtables.  Please check installation.\n" );
		perror(s2);
		exit(-6);
	}
	for( i = 1; i < k; i++ )
	{
		// check if table (k-i) exist.  If not create the table
		
		check_sum( file_name, k - i );
		fp2 = fopen( file_name, "r" );
		if( fp2 == NULL )
		{
			perror(file_name);
			exit(-2);
		}
		do
		{
			fscanf( fp2, "%s", &s );
			if( !feof( fp2 ) )
				fprintf( fp1, "%d,%s\n", i, s );
		}
		while ( !feof( fp2 ) );
		fclose( fp2 );
	}
	fprintf( fp1, "%d\n", k );
	fclose( fp1 );

	if( !env_windows )
	{
		if( *file_name2 != 0 )
		{
			sprintf( s1, "mv %s %s", s2, file_name2 );
			system( s1 );
			sprintf( s1, "chmod a+rw %s", file_name2 );
			system( s1 );
		}
	}
}
