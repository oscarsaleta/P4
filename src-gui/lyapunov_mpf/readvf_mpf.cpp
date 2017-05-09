/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
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
//                      SETMPFPRECISION
// --------------------------------------------------------------------------

void SetMPFPrecision(int digits)
{
    char buf[100];
    int k;
    mpfr_t accu1;
    mpfr_t accu2;
    mpfr_t accu3;

    sprintf(buf, "1e-%d", digits);
    printf("Requesting %d digits of precision ...\n", digits);

    for (k = 10; k <= 10000; k++) {
        mpfr_set_default_prec(k);
        mpfr_init_set_si(accu1, 1, MPFR_RNDN);
        mpfr_init_set_str(accu2, buf, 10, MPFR_RNDN);
        mpfr_init(accu3);
        mpfr_add(accu3, accu1, accu2, MPFR_RNDN);
        if (mpfr_cmp(accu3, accu1) != 0) {
            mpfr_clear(accu3);
            mpfr_clear(accu2);
            mpfr_clear(accu1);

            break; // we can distinguish 1 from 1+1e-x where x = digits
        }
        mpfr_clear(accu3);
        mpfr_clear(accu2);
        mpfr_clear(accu1);
    }
    if (k >= 10000) {
        printf("Requested precision >= 10000 --> error !\n");
        exit(1);
    }
    mpfr_init(accu1);
    printf("Working with >= %d bits (=%ld bits) precision.\n", k,
           mpfr_get_prec(accu1));
    mpfr_clear(accu1);
    return;
}

// --------------------------------------------------------------------------
//                      READ_TABLE
// --------------------------------------------------------------------------

void read_table(const char *file)
{
    FILE *fp;
    char re_string[10000];
    char im_string[10000];
    int l, t, degx, degy;
    char prec[10000];
    int prec0;
    mpfr_t re, im;

    vec_field = new hom_poly;
    vec_field->next_hom_poly = nullptr;

    fp = fopen(file, "r");
    if (fp == nullptr) {
        perror(file);
        exit(-5);
    }

    fscanf(fp, "%d", &prec0);
    prec0 = (prec0 - 2) / 2;
    SetMPFPrecision(prec0);
    mpfr_set_default_prec(200);
    mpfr_init_set_si(precision, 0, MPFR_RNDN); // precision=0
    mpfr_init(re);
    mpfr_init(im);

    fscanf(fp, "%d %s", &weakness_level, prec);
    mpfr_set_str(precision, prec, 10, MPFR_RNDN);

    fscanf(fp, "%i\n\n", &l);
    for (t = 1; t <= l; t++) {
        fscanf(fp, "%i\n\n%i\n\n%s\n\n%s\n\n", &degx, &degy, re_string,
               im_string);
        mpfr_set_str(re, re_string, 10, MPFR_RNDN);
        mpfr_set_str(im, im_string, 10, MPFR_RNDN);
        ins_hom_poly(vec_field, degx, degy, re, im);
    }
    mpfr_clear(re);
    mpfr_clear(im);
    fclose(fp);
}
