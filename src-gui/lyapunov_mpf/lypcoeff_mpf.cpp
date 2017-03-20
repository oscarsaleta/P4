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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void DEBUG_MONOME(poly *f)
{
    mpfr_t absre, absim;
    mpfr_init(absre);
    mpfr_init(absim);
    mpfr_abs(absre, f->re, MPFR_RNDN);
    mpfr_abs(absim, f->im, MPFR_RNDN);

    if (f->degz + f->degzb != 0)
        if (!mpfr_negligible(absre) || !mpfr_negligible(absim))
            printf("(");
    if (mpfr_negligible(absre)) {
        if (mpfr_negligible(absim)) {
            printf("0.0");
        } else {
            printf("%gi", mpfr_get_d(f->im, MPFR_RNDN));
        }
    } else {
        printf("%g", mpfr_get_d(f->re, MPFR_RNDN));

        if (!mpfr_negligible(absim))
            printf("%+gi", mpfr_get_d(f->im, MPFR_RNDN));
    }
    if (f->degz + f->degzb != 0)
        if (!mpfr_negligible(absre) || !mpfr_negligible(absim))
            printf(")");
    if (f->degz > 0)
        if (f->degz == 1)
            printf("z");
        else
            printf("z^%d", f->degz);
    if (f->degzb > 0)
        if (f->degzb == 1)
            printf("w");
        else
            printf("w^%d", f->degzb);
}

void DEBUG_POLY(poly *f)
{
    if (f->next_poly == nullptr)
        printf("0");
    else {
        for (f = f->next_poly; f != nullptr; f = f->next_poly) {
            DEBUG_MONOME(f);
            if (f->next_poly != nullptr)
                printf(" + ");
        }
    }
}

// --------------------------------------------------------------------------
//                      FIND_POLY
// --------------------------------------------------------------------------

poly *find_poly(hom_poly *f, int i)
{
    poly *g = nullptr;

    while ((f = f->next_hom_poly) != nullptr)
        if (i > f->total_degree)
            break;
        else if (f->total_degree == i) {
            g = f->p;
            break;
        }
    return (g);
}

// --------------------------------------------------------------------------
//                      DIFF
// --------------------------------------------------------------------------

void diff(poly *f)
{
    poly *w = f->next_poly;
    mpfr_t accu;

    mpfr_init(accu);
    while (w != nullptr) {
        if (w->degz) {
            mpfr_mul_ui(accu, w->re, w->degz, MPFR_RNDN);
            mpfr_set(w->re, accu, MPFR_RNDN);
            mpfr_mul_ui(accu, w->im, w->degz, MPFR_RNDN);
            mpfr_set(w->im, accu, MPFR_RNDN);
            w->degz--;
            f = w;
            w = w->next_poly;
        } else {
            delete_coeff(f);
            f = w;
            w = w->next_poly;
        }
    }
    mpfr_clear(accu);
}

// --------------------------------------------------------------------------
//                      G
// --------------------------------------------------------------------------

void G(poly *f)
{
    int deg;
    mpfr_t a, b;
    mpfr_init(a);
    mpfr_init(b);
    poly *w = f->next_poly;

    if (w)
        deg = w->degz + w->degzb;
    while (w != nullptr) {
        if (deg - 2 * w->degzb) {
            if (deg - 2 * w->degzb > 0) {
                mpfr_set_si(a, 2, MPFR_RNDN);
                mpfr_div_ui(b, a, deg - 2 * w->degzb, MPFR_RNDN);
            } else {
                mpfr_set_si(a, -2, MPFR_RNDN);
                mpfr_div_ui(b, a, 2 * w->degzb - deg, MPFR_RNDN);
            }
            mpfr_mul(a, w->re, b, MPFR_RNDN);
            mpfr_set(w->re, a, MPFR_RNDN);
            mpfr_mul(a, w->im, b, MPFR_RNDN);
            mpfr_set(w->im, a, MPFR_RNDN);
            f = w;
            w = w->next_poly;
        } else {
            delete_coeff(f);
            f = w;
            w = w->next_poly;
        }
    }
    mpfr_clear(a);
    mpfr_clear(b);
}

// --------------------------------------------------------------------------
//                      IMGZ
// --------------------------------------------------------------------------

void Imgz(poly *f, poly *g)
{
    poly *h;
    prod_poly(f, g);
    diff(f);
    G(f);
    h = conj_poly(f);
    sub_poly(f, h);
    delete_poly(h);
    multc_poly(f, zero, minusonehalf);
}

// --------------------------------------------------------------------------
//                      REGZ
// --------------------------------------------------------------------------

void Regz(poly *f, poly *g)
{
    poly *h;

    prod_poly(f, g);
    multc_poly(f, zero, minusone);
    diff(f);
    G(f);
    h = conj_poly(f);
    add_poly(f, h);
    delete_poly(h);
    multc_poly(f, onehalf, zero);
}

// --------------------------------------------------------------------------
//                      LL
// --------------------------------------------------------------------------

void LL(poly *f, poly *g, int i, mpfr_t *v)
{
    mpfr_set_si(v[0], 0, MPFR_RNDN);
    mpfr_set_si(v[1], 0, MPFR_RNDN);
    prod_poly(f, g);
    diff(f);
    while ((f = f->next_poly) != nullptr) {
        if (i > f->degz)
            break;
        if (i == f->degz && i > f->degzb)
            break;
        if (i == f->degz && i == f->degzb) {
            mpfr_set(v[0], f->re, MPFR_RNDN);
            mpfr_set(v[1], f->im, MPFR_RNDN);
            break;
        }
    }
}

// --------------------------------------------------------------------------
//                      PART_LYAPUNOV_COEFF
// --------------------------------------------------------------------------

void part_lyapunov_coeff(char *s, int k, mpfr_t *returnvalue)
{
    poly *R[DIM1], *f = nullptr;
    int i, t = 0, ok = 1, j = 0;
    char a[DIM1];
    mpfr_t v[2], w;

    mpfr_init(v[0]);
    mpfr_init(v[1]);
    mpfr_init(w);

    for (i = 0; s[i]; i++) {
        if (s[i] == ',') {
            R[t] =
                find_poly(vec_field, atoi(a) + 1); /* find the homogeneus part
                                                      of degree a+1 */
            if (R[t] == nullptr) {
                ok = 0;
                break;
            }
            t++;
            j = 0;
        } else {
            a[j] = s[i];
            j++;
        }
    }
    if (ok) {
        R[t] = find_poly(vec_field, atoi(a) + 1);
        if (R[t] == nullptr)
            ok = 0;
    }
    if (ok) {
        //        printf( "k=%d\n", k);
        /*
        for( i = 0; i <= t; i++ )
        {
            printf( "R[%d] = ",i);
            DEBUG_POLY(R[i]);
            printf( "\n");
        }
*/ f = (poly *)malloc(sizeof(poly));
        f->next_poly = nullptr;
        ins_poly(f, 0, 0, minusone, zero); /* f=-1 */
        //        printf( "f = ",i);        DEBUG_POLY(f);        printf( "\n");
        for (i = 0; i < t; i++) {
            if (i % 2)
                Regz(f, R[i]);
            else
                Imgz(f, R[i]);
            //            printf( "f = ",i);        DEBUG_POLY(f);
            //            printf( "\n");
        }
        if (t % 2) {
            multc_poly(f, zero, minusone);
            LL(f, R[t], (k - 1) / 2, v);
            //            printf( "f = ",i);        DEBUG_POLY(f);
            //            printf( "\n");
            mpfr_set(w, v[1], MPFR_RNDN);
        } else {
            LL(f, R[t], (k - 1) / 2, v);
            //            printf( "f = ",i);        DEBUG_POLY(f);
            //            printf( "\n");
            mpfr_set(w, v[0], MPFR_RNDN);
        }
        delete_poly(f);
    } else
        mpfr_set_si(w, 0, MPFR_RNDN);
    //    printf( "w = %f\n", mpfr_get_d(w));
    mpfr_set(*returnvalue, w, MPFR_RNDN);

    mpfr_clear(v[0]);
    mpfr_clear(v[1]);
    mpfr_clear(w);
}
