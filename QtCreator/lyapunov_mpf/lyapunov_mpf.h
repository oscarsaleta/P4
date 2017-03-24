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

#include "mpfr.h"

// definitions

#define DIM 100
#define DIM1 (DIM + 1)
#define DIM2 (2 * DIM)
#define PI 3.1415926535897932384626433832

// structures

struct poly {
    int degz, degzb;
    mpfr_t re, im;
    poly *next_poly;

    poly() : next_poly(nullptr){};
};

struct hom_poly {
    int total_degree;
    struct poly *p;
    hom_poly *next_hom_poly;

    hom_poly() : next_hom_poly(nullptr){};
};

// variables

extern bool env_maple;
extern bool env_reduce;
extern bool env_windows;
extern int weakness_level;
extern mpfr_t precision;
extern char *win_sumtablepath;
extern hom_poly *vec_field;

// prototypes

void create_sum(int);
void check_sum(char *, int);
void read_table(const char *);
void ins_hom_poly(hom_poly *, int, int, mpfr_t, mpfr_t);
void ins_poly(poly *, int, int, mpfr_t, mpfr_t);
poly *copy_poly(poly *);
poly *search_poly(poly *, int, int, int *);
void ins_coeff(poly *, int, int, mpfr_t, mpfr_t);
void delete_coeff(poly *);
void add_poly(poly *, poly *);
void sub_poly(poly *, poly *);
void term_prod(poly *, int, int, mpfr_t, mpfr_t);
void prod_poly(poly *, poly *);
void multc_poly(poly *, mpfr_t, mpfr_t);
void delete_poly(poly *);
poly *conj_poly(poly *);
void print_poly(poly *);
void read_poly(poly *);
poly *find_poly(hom_poly *, int);
void diff(poly *);
void G(poly *);
void Imgz(poly *, poly *);
void Regz(poly *, poly *);
void LL(poly *, poly *, int, mpfr_t *);
void part_lyapunov_coeff(char *, int, mpfr_t *);

extern mpfr_t zero;
extern mpfr_t minusone;
extern mpfr_t onehalf;
extern mpfr_t minusonehalf;
extern mpfr_t one;

bool mpfr_negligible(mpfr_t);
bool mpfr_negligible_V(mpfr_t);
