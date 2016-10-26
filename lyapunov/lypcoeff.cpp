#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lyapunov.h"

void DEBUG_MONOME( poly volatile * f )
{
    if( f->degz + f->degzb != 0 )
        if( fabs( f->re) > 1e-10 && fabs(f->im ) > 1e-10 )
            fprintf(stderr, "(" );
    if( fabs( f->re ) <= 1e-10 )
    {
        if( fabs( f->im ) <= 1e-10 )
        {
            fprintf(stderr, "0.0" );
        }
        else
        {
            fprintf(stderr, "%gi", f->im );
        }
    }
    else
    {
        fprintf(stderr, "%g", f->re );

        if( fabs( f->im ) > 1e-10 )
            fprintf(stderr, "%+gi", f->im );
    }
    if( f->degz + f->degzb != 0 )
        if( fabs( f->re) > 1e-10 && fabs(f->im ) > 1e-10 )
            fprintf(stderr, ")" );
    if( f->degz > 0 ) {
        if( f->degz == 1 )
            fprintf(stderr, "z" );
        else
            fprintf(stderr, "z^%d", f->degz );
    }
    if( f->degzb > 0 ) {
        if( f->degzb == 1 )
            fprintf(stderr, "w" );
        else
            fprintf(stderr, "w^%d", f->degzb );
    }
}

void DEBUG_POLY( poly volatile * f )
{
    if( f->next_poly == nullptr )
        fprintf(stderr, "0" );
    else
    {
        for( f = f->next_poly; f != nullptr; f = f->next_poly )
        {
            DEBUG_MONOME( f );
            if( f->next_poly != nullptr )
                fprintf(stderr, " + " );
        }
    }
}

// --------------------------------------------------------------------------
//                      FIND_POLY
// --------------------------------------------------------------------------

poly volatile * find_poly( hom_poly volatile * f, int i )
{
    poly *g = nullptr;

    while ( ( f = f->next_hom_poly ) != nullptr )
		if ( i > f->total_degree )
			break;
		else if ( f->total_degree == i )
		{
			g = f->p;
			break;
		}
	return ( g );
}

// --------------------------------------------------------------------------
//                      DIFF
// --------------------------------------------------------------------------

void diff( poly volatile * f )
{
    poly *w = f->next_poly;

    while ( w != nullptr )
	{
		if ( w->degz )
		{
			w->re *= w->degz;
			w->im *= w->degz;
			w->degz--;
			f = w;
			w = w->next_poly;
		}
		else
		{
			w = w->next_poly;
			delete_coeff( f );
		}
	}
}

// --------------------------------------------------------------------------
//                      G
// --------------------------------------------------------------------------

void G( poly volatile * f )
{
	int deg;
	double a;
	poly *w = f->next_poly;

	if ( w )
		deg = w->degz + w->degzb;
    while ( w != nullptr )
	{
		if ( deg - 2 * w->degzb )
		{
			a = 2.0 / ( deg - 2 * w->degzb );
			w->re *= a;
			w->im *= a;
			f = w;
			w = w->next_poly;
		}
		else
		{
			w = w->next_poly;
			delete_coeff( f );
		}
	}
}

// --------------------------------------------------------------------------
//                      IMGZ
// --------------------------------------------------------------------------

void Imgz( poly volatile * f, poly volatile * g )
{
    poly volatile * h;

    prod_poly( f, g );
    diff( f );
    G( f );
    h = conj_poly( f );
    sub_poly( f, h );
    delete_poly( &h );
    multc_poly( f, 0.0, -0.5 );
}

// --------------------------------------------------------------------------
//                      REGZ
// --------------------------------------------------------------------------

void Regz( poly volatile * f, poly volatile * g )
{
    poly volatile * h;

	prod_poly( f, g );
	multc_poly( f, 0.0, -1.0 );
	diff( f );
	G( f );
	h = conj_poly( f );
	add_poly( f, h );
    delete_poly( &h );
	multc_poly( f, 0.5, 0.0 );
}

// --------------------------------------------------------------------------
//                      LL
// --------------------------------------------------------------------------

void LL( poly volatile * f, poly volatile * g, int i, double * v )
{
	v[ 0 ] = 0.0;
	v[ 1 ] = 0.0;
	prod_poly( f, g );
	diff( f );
    while ( ( f = f->next_poly ) != nullptr )
	{
		if ( i > f->degz )
			break;
		if ( i == f->degz && i > f->degzb )
			break;
		if ( i == f->degz && i == f->degzb )
		{
			v[ 0 ] = f->re;
			v[ 1 ] = f->im;
			break;
		}
	}
}

// --------------------------------------------------------------------------
//                      PART_LYAPUNOV_COEFF
// --------------------------------------------------------------------------

double part_lyapunov_coeff( char * s, int k )
{
    poly volatile * R[ DIM1 ];
    poly volatile *f = nullptr;
	int i, t = 0, ok = 1, j = 0;
	char a[ DIM1 ];
	double v[ 2 ], w;

    fprintf(stderr,"k=%d\n",k);
    for ( i = 0;s[ i ];i++ )
	{
        if ( s[ i ] == ',' )
		{
			R[ t ] = find_poly( vec_field, atoi( a ) + 1 );  /* find the homogeneus part
			                                                    of degree a+1 */
            if ( R[ t ] == nullptr )
			{
				ok = 0;
				break;
			}
			t++;
			j = 0;
		}
		else
		{
            a[ j ] = s[ i ];
			j++;
		}
	}
	if ( ok )
	{
		R[ t ] = find_poly( vec_field, atoi( a ) + 1 );
        if ( R[ t ] == nullptr )
			ok = 0;
	}
	if ( ok )
	{
        fprintf(stderr, "k=%d\n", k);

        for( i = 0; i <= t; i++ )
        {
            fprintf(stderr, "R[%d] = ",i);
            DEBUG_POLY(R[i]);
            fprintf(stderr, "\n");
        }

        f = ( poly * ) malloc( sizeof( poly ) );
        f->next_poly = nullptr;
        //fprintf(stderr,"degz:%d,degzb:%d, re:%g, im:%g, next:%d\n",f->degz,f->degzb,f->re,f->im,f->next_poly);
		ins_poly( f, 0, 0, -1.0, 0.0 ); /* f=-1 */
        //fprintf(stderr,"degz:%d,degzb:%d, re:%g, im:%g, next:%d\n",f->degz,f->degzb,f->re,f->im,f->next_poly);
        fprintf(stderr, "f = ");        DEBUG_POLY(f);        fprintf(stderr, "\n");
        for ( i = 0;i < t;i++ )
        {
            fprintf(stderr, "t=%d\n",t);
            fprintf(stderr, "f%d = ",i);        DEBUG_POLY(f);        fprintf(stderr, "\n");
            if ( i % 2 ){
				Regz( f, R[ i ] );
                //fprintf(stderr,"degz:%d,degzb:%d, re:%g, im:%g, next:%d\n",f->degz,f->degzb,f->re,f->im,f->next_poly);
            }
            else {
				Imgz( f, R[ i ] );
                //fprintf(stderr,"degz:%d,degzb:%d, re:%g, im:%g, next:%d\n",f->degz,f->degzb,f->re,f->im,f->next_poly);
            }
            fprintf(stderr, "f%d = ",i);        DEBUG_POLY(f);        fprintf(stderr, "\n");
        }
		if ( t % 2 )
		{
			multc_poly( f, 0.0, -1.0 );
            //fprintf(stderr,"degz:%d,degzb:%d, re:%g, im:%g, next:%d\n",f->degz,f->degzb,f->re,f->im,f->next_poly);
			LL( f, R[ t ], ( k - 1 ) / 2, v );
            //fprintf(stderr,"degz:%d,degzb:%d, re:%g, im:%g, next:%d\n",f->degz,f->degzb,f->re,f->im,f->next_poly);
            fprintf(stderr, "f%d = ",i);        DEBUG_POLY(f);        fprintf(stderr, "\n");
			w = v[ 1 ];
		}
		else
		{
			LL( f, R[ t ], ( k - 1 ) / 2, v );
            //fprintf(stderr,"degz:%d,degzb:%d, re:%g, im:%g, next:%d\n",f->degz,f->degzb,f->re,f->im,f->next_poly);
            fprintf(stderr, "f%d = ",i);        DEBUG_POLY(f);        fprintf(stderr, "\n");
            w = v[ 0 ];
		}
        fprintf(stderr, "f%d = ",i);        DEBUG_POLY(f);        fprintf(stderr, "\n");
        //fprintf(stderr,"degz:%d,degzb:%d, re:%g, im:%g, next:%d\n",f->degz,f->degzb,f->re,f->im,f->next_poly);
        delete_poly( &f );
	}
	else
		w = 0.0;
    printf( "w = %f\n", w);
	return w;
}
