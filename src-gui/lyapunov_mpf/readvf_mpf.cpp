#include <stdlib.h>
#include <stdio.h>
#include "lyapunov_mpf.h"

// --------------------------------------------------------------------------
//                      SETMPFPRECISION
// --------------------------------------------------------------------------

void SetMPFPrecision( int digits )
{
    char buf[100];
    int k;
    mpfr_t accu1;
    mpfr_t accu2;
    mpfr_t accu3;

    sprintf( buf, "1e-%d", digits );
    printf( "Requesting %d digits of precision ...\n",digits );

    for( k = 10; k <= 10000; k++ )
    {
        mpfr_set_default_prec(k);
        mpfr_init_set_si(accu1,1,MPFR_RNDN);
        mpfr_init_set_str(accu2,buf,10,MPFR_RNDN);
        mpfr_init(accu3);
        mpfr_add( accu3, accu1, accu2,MPFR_RNDN );
        if( mpfr_cmp( accu3, accu1 ) != 0 )
        {
            mpfr_clear(accu3);
            mpfr_clear(accu2);
            mpfr_clear(accu1);

            break;  // we can distinguish 1 from 1+1e-x where x = digits
        }
        mpfr_clear(accu3);
        mpfr_clear(accu2);
        mpfr_clear(accu1);
    }
    if( k >= 10000 )
    {
        printf( "Requested precision >= 10000 --> error !\n");
        exit(1);
    }
    mpfr_init(accu1);
    printf( "Working with >= %d bits (=%d bits) precision.\n",k, mpfr_get_prec(accu1));
    mpfr_clear(accu1);
    return;
}

// --------------------------------------------------------------------------
//                      READ_TABLE
// --------------------------------------------------------------------------

void read_table( const char * file )
{
	FILE * fp;
    char re_string[10000];
    char im_string[10000];
	int l, t, degx, degy;
    char prec[10000];
    int prec0;
    mpfr_t re, im;
	
	vec_field = (struct hom_poly *)malloc( sizeof( struct hom_poly ) );
	vec_field->next_hom_poly = NULL;

	fp = fopen( file, "r" );
	if( fp == NULL )
	{
		perror(file);
		exit(-5);
	}

    fscanf( fp, "%d", &prec0 );
    prec0 = (prec0-2)/2;
    SetMPFPrecision(prec0);
    mpfr_set_default_prec(200);
    mpfr_init_set_si(precision,0,MPFR_RNDN);       // precision=0
    mpfr_init(re);
    mpfr_init(im);

    fscanf( fp, "%d %s", &weakness_level, prec );
    mpfr_set_str( precision, prec, 10,MPFR_RNDN );

    fscanf( fp, "%i\n\n", &l );
    for( t = 1; t <= l; t++ )
	{
        fscanf( fp, "%i\n\n%i\n\n%s\n\n%s\n\n", &degx, &degy, re_string, im_string );
        mpfr_set_str(re, re_string, 10,MPFR_RNDN);
        mpfr_set_str(im, im_string, 10,MPFR_RNDN);
        ins_hom_poly( vec_field, degx, degy, re, im );
	}
    mpfr_clear(re);
    mpfr_clear(im);
	fclose( fp );
}
