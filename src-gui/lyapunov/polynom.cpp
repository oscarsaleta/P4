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
//                      INS_HOM_POLY
// --------------------------------------------------------------------------

void ins_hom_poly( hom_poly volatile *f, int degz, int degzb, double re, double im )
{
	int total_degree = degz + degzb, ok = 1;
	hom_poly *w = f->next_hom_poly;

    while ( w != nullptr && ok )
	{
		if ( total_degree == w->total_degree )
		{
			ok = 0;
			ins_poly( w->p, degz, degzb, re, im );
			break;
		}
		if ( total_degree > w->total_degree )
			break;
		f = w;
		w = w->next_hom_poly;
	}
	if ( ok )
	{
        f->next_hom_poly = new hom_poly;
		f = f->next_hom_poly;
		f->total_degree = total_degree;
        f->p = new poly;
        f->p->next_poly = nullptr;
		f->next_hom_poly = w;
		ins_poly( f->p, degz, degzb, re, im );
	}
}

// --------------------------------------------------------------------------
//                      INS_POLY
// --------------------------------------------------------------------------

void ins_poly( poly volatile *f, int degz, int degzb, double re, double im )
{
	poly *w = f->next_poly;

    while ( w != nullptr )
	{
		if ( degz > w->degz )
			break;
		if ( degz == w->degz && degzb > w->degzb )
			break;
		f = w;
		w = w->next_poly;
	}
    f->next_poly = new poly;
	f = f->next_poly;
	f->degz = degz;
	f->degzb = degzb;
	f->re = re;
	f->im = im;
	f->next_poly = w;
}

// --------------------------------------------------------------------------
//                      COPY_POLY
// --------------------------------------------------------------------------

poly volatile *copy_poly( poly volatile *f )
{
    if (f == nullptr)
        return f;
    poly volatile *current = f;

    poly *copy = new poly;
    copy->degz = current->degz;
    copy->degzb = current->degzb;
    copy->re = current->re;
    copy->im = current->im;
    copy->next_poly = nullptr;

    poly *const head = copy;

    current = current->next_poly;

    while (current != nullptr) {
        copy = copy->next_poly = new poly;
        copy->degz = current->degz;
        copy->degzb = current->degzb;
        copy->re = current->re;
        copy->im = current->im;
        copy->next_poly = nullptr;
        current = current->next_poly;
    }
    return head;
}

// --------------------------------------------------------------------------
//                      SEARCH_POLY
// --------------------------------------------------------------------------

poly volatile *search_poly( poly volatile *f, int degz, int degzb, int *present )
{
    poly volatile *w;

	w = f;
	*present = 0;
    while ( ( f = f->next_poly ) != nullptr )
	{
		if ( degz > f->degz )
			break;
		if ( degz == f->degz && degzb == f->degzb )
		{
			*present = 1;
			break;
		}
		if ( degz == f->degz && degzb > f->degzb )
			break;
		w = f;
	}
	return ( w );
}

// --------------------------------------------------------------------------
//                      INS_COEFF
// --------------------------------------------------------------------------

void ins_coeff( poly volatile *f, int degz, int degzb, double re, double im )
{

	poly *w;

    w = new poly;
	w->degz = degz;
	w->degzb = degzb;
	w->re = re;
	w->im = im;
	w->next_poly = f->next_poly;
	f->next_poly = w;
}

// --------------------------------------------------------------------------
//                      DELETE_COEFF
// --------------------------------------------------------------------------

void delete_coeff( poly volatile *f )
{
	poly *w;

	w = f->next_poly->next_poly;
    //free( f->next_poly );
    delete f->next_poly;
	f->next_poly = w;
}

// --------------------------------------------------------------------------
//                      ADD_POLY
// --------------------------------------------------------------------------

void add_poly( poly volatile *f, poly volatile *g )
{
	int present;
    poly volatile *w1, *w2;

    while ( ( g = g->next_poly ) != nullptr )
	{
		w1 = search_poly( f, g->degz, g->degzb, &present );
		if ( !present )
			ins_coeff( w1, g->degz, g->degzb, g->re, g->im );
		else
		{
			w2 = w1->next_poly;
			w2->re += g->re;
			w2->im += g->im;
			if ( !( w2->re || w2->im ) )
			{
				w1->next_poly = w2->next_poly;
                delete w2;
                w2 = nullptr;
			}
		}
	}
}

// --------------------------------------------------------------------------
//                      SUB_POLY
// --------------------------------------------------------------------------

void sub_poly( poly volatile *f, poly volatile *g )
{
	int present;
    poly volatile *w1, *w2;

    while ( ( g = g->next_poly ) != nullptr )
	{
		w1 = search_poly( f, g->degz, g->degzb, &present );
		if ( !present )
			ins_coeff( w1, g->degz, g->degzb, -g->re, -g->im );
		else
		{
			w2 = w1->next_poly;
			w2->re -= g->re;
			w2->im -= g->im;
			if ( !( w2->re || w2->im ) )
			{
				w1->next_poly = w2->next_poly;
                delete w2;
                w2 = nullptr;
			}
		}
	}
}

// --------------------------------------------------------------------------
//                      TERM_PROD
// --------------------------------------------------------------------------

void term_prod( poly volatile *f, int degz, int degzb, double re, double im )
{
	double r, i;

    while ( ( f = f->next_poly ) != nullptr )

	{
		f->degz += degz;
		f->degzb += degzb;
		r = f->re * re - f->im * im;
		i = f->re * im + f->im * re;
		f->re = r;
		f->im = i;
	}
}

// --------------------------------------------------------------------------
//                      PROD_POLY
// --------------------------------------------------------------------------

void prod_poly( poly volatile *f, poly volatile *g )
{
    poly volatile *w1, *w2;

    if ( ( f->next_poly != nullptr ) && ( g->next_poly != nullptr ) )
	{
		w2 = copy_poly( f );
		g = g->next_poly;
		term_prod( f, g->degz, g->degzb, g->re, g->im );
        while ( ( g = g->next_poly ) != nullptr )
		{
			w1 = copy_poly( w2 );
			term_prod( w1, g->degz, g->degzb, g->re, g->im );
			add_poly( f, w1 );
            delete_poly( &w1 );
		}
        delete_poly( &w2 );
	}
	else
	{
        delete_poly( &f );
		/* we must return the zero poly and not a empty list !! */
        f = new poly;
        f->degz = 0;
        f->degzb = 0;
        f->re = 0;
        f->im = 0;
        f->next_poly = nullptr;
	}
}

// --------------------------------------------------------------------------
//                      MULTC_POLY
// --------------------------------------------------------------------------

void multc_poly( poly volatile *f, double re, double im )
{
	double r, i;
    poly volatile *w = f;

    while ( ( w = w->next_poly ) != nullptr )
	{
        r = w->re * re - w->im * im;
        i = w->re * im + w->im * re;
        w->re = r;
        w->im = i;
	}
}

// --------------------------------------------------------------------------
//                      DELETE_POLY
// --------------------------------------------------------------------------

void delete_poly( poly volatile **f )
{
    poly volatile *w, *tmp;
    w = *f;
    while (w != nullptr) {
        tmp = w;
        w = w->next_poly;
        delete tmp;
        tmp = nullptr;
    }
    *f = nullptr;
    /*if ( f != nullptr )
		do
		{
			w = f;
			f = f->next_poly;
			free( w );
		}
        while ( f != nullptr );*/
}

// --------------------------------------------------------------------------
//                      CONJ_POLY
// --------------------------------------------------------------------------

poly volatile *conj_poly( poly volatile * f )
{
    poly volatile *g;

    g = new poly;//( poly * ) malloc( sizeof( poly ) );
    g->next_poly = nullptr;
    while ( ( f = f->next_poly ) != nullptr )
		ins_poly( g, f->degzb, f->degz, f->re, -f->im );
	return ( g );
}

// --------------------------------------------------------------------------
//                      PRINT_POLY
// --------------------------------------------------------------------------

void print_poly( poly volatile * f )
{
	if( f )
	while( ( f = f->next_poly ) )
		printf( "%f  %f  %d  %d\n", f->re, f->im, f->degz, f->degzb );
	printf( "\n\n" );
}

// --------------------------------------------------------------------------
//                      READ_POLY
// --------------------------------------------------------------------------

void read_poly( poly volatile *f )
{
	int degz, degzb;
	double re, im;

	for ( ;; )
	{
		scanf( "%lf", &re );
		scanf( "%lf", &im );
		scanf( "%d", &degz );
		scanf( "%d", &degzb );
		if ( degz < 0 )
			break;
		ins_poly( f, degz, degzb, re, im );
	}
}

/*
void main()
{
 
  poly *f,*g,*h;
  
  f=(poly *) malloc(sizeof(poly));
  f->next_poly=nullptr;
  printf("read f\n");
  read_poly(f);
  print_poly(f); 
  g=conj_poly(f);
  print_poly(g); 
  delete_poly(g);
  g=(poly *) malloc(sizeof(poly));
  printf("read g\n");
  read_poly(g);
  print_poly(g);
  h=copy_poly(g);
  print_poly(h);
  add_poly(h,f);
  print_poly(h);
  h=copy_poly(g);
  sub_poly(h,f);
  print_poly(g);
  h=copy_poly(g);
  prod_poly(h,f);
  print_poly(h);
}
 
*/
