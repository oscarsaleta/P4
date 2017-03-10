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

#include "lyapunov.h"

#include <stdio.h>
#include <stdlib.h>

// --------------------------------------------------------------------------
//                      READ_TABLE
// --------------------------------------------------------------------------

void read_table(const char *file)
{
    FILE *fp;
    int l, t, degx, degy;
    double re, im;
    int dummy;

    vec_field = (hom_poly *)malloc(sizeof(hom_poly));
    vec_field->next_hom_poly = nullptr;

    fp = fopen(file, "r");
    if (fp == nullptr) {
        perror(file);
        exit(-5);
    }
    // dummy should be 0, it is only nonzero when the mpf version of this
    // program is called.
    if (fscanf(fp, "%d", &dummy) != 1 ||
        fscanf(fp, "%d %lf", &weakness_level, &precision) != 2 ||
        fscanf(fp, "%i\n\n", &l) != 1) {
        fprintf(stderr, "Error reading %s\n", file);
        exit(-5);
    }

    for (t = 1; t <= l; t++) {
        if (fscanf(fp, "%i\n\n%i\n\n%lf\n\n%lf\n\n", &degx, &degy, &re, &im) !=
            4) {
            fprintf(stderr, "Error reading %s\n", file);
            exit(-5);
        }
        ins_hom_poly(vec_field, degx, degy, re, im);
    }
    fclose(fp);
}
