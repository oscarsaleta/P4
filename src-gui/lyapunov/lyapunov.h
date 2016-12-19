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
#ifndef LYAPUNOV_H
#define LYAPUNOV_H

// definitions

#define DIM     100
#define DIM1    (DIM+1)
#define DIM2    (2*DIM)
#define PI       3.1415926535897932384626433832

// structures

struct poly
{
	int degz, degzb;
	double re, im;
	poly * next_poly;
};

struct hom_poly
{
	int total_degree;
	poly * p;
	hom_poly * next_hom_poly;
};

// variables

extern bool env_maple;
extern bool env_reduce;
extern bool env_windows;
extern int weakness_level;
extern double precision;
extern char * win_sumtablepath;
extern hom_poly volatile * vec_field;

// prototypes

void create_sum( int );
void check_sum( char *, int );
void read_table( const char * );
void ins_hom_poly( hom_poly volatile *, int, int, double, double );
void ins_poly( poly volatile *, int, int, double, double );
poly volatile * copy_poly( poly volatile * );
poly volatile * search_poly( poly volatile *, int, int, int * );
void ins_coeff( poly volatile *, int, int, double, double );
void delete_coeff( poly volatile * );
void add_poly( poly volatile *, poly volatile * );
void sub_poly( poly volatile *, poly volatile * );
void term_prod( poly volatile *, int, int, double, double );
void prod_poly( poly volatile *, poly volatile * );
void multc_poly( poly volatile *, double, double );
void delete_poly(poly volatile ** );
poly volatile * conj_poly( poly volatile * );
void print_poly( poly volatile * );
void read_poly( poly volatile * );
poly volatile * find_poly(hom_poly volatile *, int );
void diff( poly volatile * );
void G( poly volatile * );
void Imgz( poly volatile *, poly volatile * );
void Regz( poly volatile *, poly volatile * );
void LL( poly volatile *, poly volatile *, int, double * );
double part_lyapunov_coeff( char *, int );

#endif // LYAPUNOV_H
