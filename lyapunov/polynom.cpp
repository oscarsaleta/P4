#include <stdlib.h>
#include <stdio.h>
#include "lyapunov.h"

// --------------------------------------------------------------------------
//                      INS_HOM_POLY
// --------------------------------------------------------------------------

void ins_hom_poly( struct hom_poly *f, int degz, int degzb, double re, double im )
{
	int total_degree = degz + degzb, ok = 1;
	struct hom_poly *w = f->next_hom_poly;

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

void ins_poly( struct poly *f, int degz, int degzb, double re, double im )
{
	struct poly *w = f->next_poly;

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

struct poly *copy_poly( struct poly *f )
{
    if (f == nullptr)
        return f;
    poly *current = f;

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

struct poly *search_poly( struct poly *f, int degz, int degzb, int *present )
{
	struct poly *w;

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

void ins_coeff( struct poly *f, int degz, int degzb, double re, double im )
{

	struct poly *w;

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

void delete_coeff( struct poly *f )
{
	struct poly *w;

	w = f->next_poly->next_poly;
	free( f->next_poly );
	f->next_poly = w;
}

// --------------------------------------------------------------------------
//                      ADD_POLY
// --------------------------------------------------------------------------

void add_poly( struct poly *f, struct poly *g )
{
	int present;
	struct poly *w1, *w2;

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
			}
		}
	}
}

// --------------------------------------------------------------------------
//                      SUB_POLY
// --------------------------------------------------------------------------

void sub_poly( struct poly *f, struct poly *g )
{
	int present;
	struct poly *w1, *w2;

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
			}
		}
	}
}

// --------------------------------------------------------------------------
//                      TERM_PROD
// --------------------------------------------------------------------------

void term_prod( struct poly *f, int degz, int degzb, double re, double im )
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

void prod_poly( struct poly *f, struct poly *g )
{
	struct poly *w1, *w2;

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

void multc_poly( struct poly *f, double re, double im )
{
	double r, i;
    struct poly *w = f;

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

void delete_poly( struct poly **f )
{
    struct poly *w, *tmp;
    w = *f;
    while (w != nullptr) {
        tmp = w;
        w = w->next_poly;
        delete tmp;
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

struct poly *conj_poly( struct poly * f )
{
	struct poly *g;

    g = new poly;//( struct poly * ) malloc( sizeof( struct poly ) );
    g->next_poly = nullptr;
    while ( ( f = f->next_poly ) != nullptr )
		ins_poly( g, f->degzb, f->degz, f->re, -f->im );
	return ( g );
}

// --------------------------------------------------------------------------
//                      PRINT_POLY
// --------------------------------------------------------------------------

void print_poly( struct poly * f )
{
	if( f )
	while( ( f = f->next_poly ) )
		printf( "%f  %f  %d  %d\n", f->re, f->im, f->degz, f->degzb );
	printf( "\n\n" );
}

// --------------------------------------------------------------------------
//                      READ_POLY
// --------------------------------------------------------------------------

void read_poly( struct poly *f )
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
 
  struct poly *f,*g,*h;
  
  f=(struct poly *) malloc(sizeof(struct poly));
  f->next_poly=nullptr;
  printf("read f\n");
  read_poly(f);
  print_poly(f); 
  g=conj_poly(f);
  print_poly(g); 
  delete_poly(g);
  g=(struct poly *) malloc(sizeof(struct poly));
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
