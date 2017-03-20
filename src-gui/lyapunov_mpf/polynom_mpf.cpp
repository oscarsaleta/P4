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
 
#include "lyapunov_mpf.h"

#include <stdio.h>
#include <stdlib.h>

// --------------------------------------------------------------------------
//                      INS_HOM_POLY
// --------------------------------------------------------------------------

void ins_hom_poly(hom_poly *f, int degz, int degzb, mpfr_t re, mpfr_t im)
{
    int total_degree = degz + degzb, ok = 1;
    hom_poly *w = f->next_hom_poly;

    while (w != nullptr && ok) {
        if (total_degree == w->total_degree) {
            ok = 0;
            ins_poly(w->p, degz, degzb, re, im);
            break;
        }
        if (total_degree > w->total_degree)
            break;
        f = w;
        w = w->next_hom_poly;
    }
    if (ok) {
        f->next_hom_poly = new hom_poly;
        f = f->next_hom_poly;
        f->total_degree = total_degree;
        f->p = new poly;
        f->p->next_poly = nullptr;
        f->next_hom_poly = w;
        ins_poly(f->p, degz, degzb, re, im);
    }
}

// --------------------------------------------------------------------------
//                      INS_POLY
// --------------------------------------------------------------------------

void ins_poly(poly *f, int degz, int degzb, mpfr_t re, mpfr_t im)
{
    poly *w = f->next_poly;

    while (w != nullptr) {
        if (degz > w->degz)
            break;
        if (degz == w->degz && degzb > w->degzb)
            break;
        f = w;
        w = w->next_poly;
    }
    f->next_poly = new poly;
    f = f->next_poly;
    f->degz = degz;
    f->degzb = degzb;
    mpfr_init_set(f->re, re, MPFR_RNDN);
    mpfr_init_set(f->im, im, MPFR_RNDN);
    f->next_poly = w;
}

// --------------------------------------------------------------------------
//                      COPY_POLY
// --------------------------------------------------------------------------

poly *copy_poly(poly *f)
{
    poly *g, *h;

    g = new poly;
    g->next_poly = nullptr;
    h = g;
    while ((f = f->next_poly) != nullptr) {
        g->next_poly = new poly;
        g = g->next_poly;
        g->degz = f->degz;
        g->degzb = f->degzb;
        mpfr_init_set(g->re, f->re, MPFR_RNDN);
        mpfr_init_set(g->im, f->im, MPFR_RNDN);
        g->next_poly = nullptr;
    }
    return (h);
}

// --------------------------------------------------------------------------
//                      SEARCH_POLY
// --------------------------------------------------------------------------

poly *search_poly(poly *f, int degz, int degzb, int *present)
{
    poly *w;

    w = f;
    *present = 0;
    while ((f = f->next_poly) != nullptr) {
        if (degz > f->degz)
            break;
        if (degz == f->degz && degzb == f->degzb) {
            *present = 1;
            break;
        }
        if (degz == f->degz && degzb > f->degzb)
            break;
        w = f;
    }
    return (w);
}

// --------------------------------------------------------------------------
//                      INS_COEFF
// --------------------------------------------------------------------------

void ins_coeff(poly *f, int degz, int degzb, mpfr_t re, mpfr_t im)
{

    poly *w;

    w = new poly;
    w->degz = degz;
    w->degzb = degzb;
    mpfr_init_set(w->re, re, MPFR_RNDN);
    mpfr_init_set(w->im, im, MPFR_RNDN);
    w->next_poly = f->next_poly;
    f->next_poly = w;
}

// --------------------------------------------------------------------------
//                      DELETE_COEFF
// --------------------------------------------------------------------------

void delete_coeff(poly *f)
{
    poly *w;

    w = f->next_poly->next_poly;
    mpfr_clear(f->next_poly->re);
    mpfr_clear(f->next_poly->im);
    delete f->next_poly;
    f->next_poly = w;
}

// --------------------------------------------------------------------------
//                      ADD_POLY
// --------------------------------------------------------------------------

void add_poly(poly *f, poly *g)
{
    int present;
    poly *w1, *w2;
    mpfr_t accu;
    mpfr_init(accu);

    while ((g = g->next_poly) != nullptr) {
        w1 = search_poly(f, g->degz, g->degzb, &present);
        if (!present)
            ins_coeff(w1, g->degz, g->degzb, g->re, g->im);
        else {
            w2 = w1->next_poly;
            mpfr_add(accu, w2->re, g->re, MPFR_RNDN);
            mpfr_set(w2->re, accu, MPFR_RNDN);
            mpfr_add(accu, w2->im, g->im, MPFR_RNDN);
            mpfr_set(w2->im, accu, MPFR_RNDN);
            if (mpfr_negligible(w2->re) && mpfr_negligible(w2->im)) {
                w1->next_poly = w2->next_poly;
                mpfr_clear(w2->re);
                mpfr_clear(w2->im);
                delete w2;
                w2 = nullptr;
            }
        }
    }
    mpfr_clear(accu);
}

// --------------------------------------------------------------------------
//                      SUB_POLY
// --------------------------------------------------------------------------

void sub_poly(poly *f, poly *g)
{
    int present;
    mpfr_t accu_re, accu_im;

    mpfr_init(accu_re);
    mpfr_init(accu_im);
    poly *w1, *w2;

    while ((g = g->next_poly) != nullptr) {
        w1 = search_poly(f, g->degz, g->degzb, &present);
        if (!present) {
            mpfr_ui_sub(accu_re, 0, g->re, MPFR_RNDN);
            mpfr_ui_sub(accu_im, 0, g->im, MPFR_RNDN);
            ins_coeff(w1, g->degz, g->degzb, accu_re, accu_im);
        } else {
            w2 = w1->next_poly;
            mpfr_sub(accu_im, w2->im, g->im, MPFR_RNDN);
            mpfr_set(w2->im, accu_im, MPFR_RNDN);
            mpfr_sub(accu_re, w2->re, g->re, MPFR_RNDN);
            mpfr_set(w2->re, accu_re, MPFR_RNDN);
            if (mpfr_negligible(w2->re) && mpfr_negligible(w2->im)) {
                w1->next_poly = w2->next_poly;
                mpfr_clear(w2->re);
                mpfr_clear(w2->im);
                delete w2;
                w2 = nullptr;
            }
        }
    }
    mpfr_clear(accu_re);
    mpfr_clear(accu_im);
}

// --------------------------------------------------------------------------
//                      TERM_PROD
// --------------------------------------------------------------------------

void term_prod(poly *f, int degz, int degzb, mpfr_t re, mpfr_t im)
{
    mpfr_t r, i, accu, accu2;
    mpfr_init(r);
    mpfr_init(i);
    mpfr_init(accu);
    mpfr_init(accu2);

    while ((f = f->next_poly) != nullptr) {
        f->degz += degz;
        f->degzb += degzb;
        // r = f->re * re - f->im * im;
        mpfr_mul(r, f->re, re, MPFR_RNDN);
        mpfr_mul(accu, f->im, im, MPFR_RNDN);
        mpfr_sub(accu2, r, accu, MPFR_RNDN);
        mpfr_set(r, accu2, MPFR_RNDN);

        // i = f->re * im + f->im * re;
        mpfr_mul(accu2, f->re, im, MPFR_RNDN);
        mpfr_mul(accu, f->im, re, MPFR_RNDN);
        mpfr_add(i, accu2, accu, MPFR_RNDN);

        mpfr_set(f->re, r, MPFR_RNDN);
        mpfr_set(f->im, i, MPFR_RNDN);
    }

    mpfr_clear(accu2);
    mpfr_clear(accu);
    mpfr_clear(i);
    mpfr_clear(r);
}

// --------------------------------------------------------------------------
//                      PROD_POLY
// --------------------------------------------------------------------------

void prod_poly(poly *f, poly *g)
{
    poly *w1, *w2;

    if ((f->next_poly != nullptr) && (g->next_poly != nullptr)) {
        w2 = copy_poly(f);
        g = g->next_poly;
        term_prod(f, g->degz, g->degzb, g->re, g->im);
        while ((g = g->next_poly) != nullptr) {
            w1 = copy_poly(w2);
            term_prod(w1, g->degz, g->degzb, g->re, g->im);
            add_poly(f, w1);
            delete_poly(w1);
        }
        delete_poly(w2);
    } else {
        delete_poly(f);
        /* we must return the zero poly and not a empty list !! */
        f = new poly;
        f->next_poly = nullptr;
    }
}

// --------------------------------------------------------------------------
//                      MULTC_POLY
// --------------------------------------------------------------------------

void multc_poly(poly *f, mpfr_t re, mpfr_t im)
{
    mpfr_t r, i, accu, accu2;
    mpfr_init(r);
    mpfr_init(i);
    mpfr_init(accu);
    mpfr_init(accu2);

    while ((f = f->next_poly) != nullptr) {
        // r = f->re * re - f->im * im;
        mpfr_mul(r, f->re, re, MPFR_RNDN);
        mpfr_mul(accu, f->im, im, MPFR_RNDN);
        mpfr_sub(accu2, r, accu, MPFR_RNDN);
        mpfr_set(r, accu2, MPFR_RNDN);

        // i = f->re * im + f->im * re;
        mpfr_mul(accu2, f->re, im, MPFR_RNDN);
        mpfr_mul(accu, f->im, re, MPFR_RNDN);
        mpfr_add(i, accu2, accu, MPFR_RNDN);

        mpfr_set(f->re, r, MPFR_RNDN);
        mpfr_set(f->im, i, MPFR_RNDN);
    }
    mpfr_clear(accu2);
    mpfr_clear(accu);
    mpfr_clear(i);
    mpfr_clear(r);
}

// --------------------------------------------------------------------------
//                      DELETE_POLY
// --------------------------------------------------------------------------

void delete_poly(poly *f)
{
    poly *w;

    w = f;
    f = f->next_poly;
    delete w;
    w = nullptr;

    if (f != nullptr)
        do {
            w = f;
            f = f->next_poly;
            mpfr_clear(w->re);
            mpfr_clear(w->im);
            delete w;
            w = nullptr;
        } while (f != nullptr);
}

// --------------------------------------------------------------------------
//                      CONJ_POLY
// --------------------------------------------------------------------------

poly *conj_poly(poly *f)
{
    poly *g;
    mpfr_t minusim;

    mpfr_init(minusim);

    g = new poly;
    g->next_poly = nullptr;
    while ((f = f->next_poly) != nullptr) {
        mpfr_ui_sub(minusim, 0, f->im, MPFR_RNDN);
        ins_poly(g, f->degzb, f->degz, f->re, minusim);
    }

    mpfr_clear(minusim);
    return (g);
}

// --------------------------------------------------------------------------
//                      PRINT_POLY
// --------------------------------------------------------------------------

void print_poly(poly *f)
{
    double re, im;
    if (f)
        while ((f = f->next_poly)) {
            re = mpfr_get_d(f->re, MPFR_RNDN);
            im = mpfr_get_d(f->im, MPFR_RNDN);
            printf("%f  %f  %d  %d\n", re, im, f->degz, f->degzb);
        }
    printf("\n\n");
}

// --------------------------------------------------------------------------
//                      READ_POLY
// --------------------------------------------------------------------------

void read_poly(poly *f)
{
    int degz, degzb;
    mpfr_t re, im;
    char re_str[10000], im_str[10000];

    mpfr_init(re);
    mpfr_init(im);

    for (;;) {
        scanf("%s", re_str);
        scanf("%s", im_str);
        scanf("%d", &degz);
        scanf("%d", &degzb);
        if (degz < 0)
            break;
        mpfr_set_str(re, re_str, 10, MPFR_RNDN);
        mpfr_set_str(im, im_str, 10, MPFR_RNDN);
        ins_poly(f, degz, degzb, re, im);
    }

    mpfr_clear(im);
    mpfr_clear(re);
}

/*
void main()
{

  poly *f,*g,*h;

  f=new poly;
  f->next_poly=nullptr;
  printf("read f\n");
  read_poly(f);
  print_poly(f);
  g=conj_poly(f);
  print_poly(g);
  delete_poly(g);
  g=new poly;
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
