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
#include "lyapunov.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --------------------------------------------------------------------------
//                      CREATE_SUM
// --------------------------------------------------------------------------

void create_sum(int k)
{
    int i;
    FILE *fp1;
    FILE *fp2;
    char file_name[220];
    char s[DIM2];
    char file_name2[220];
    char s1[220];
    char s2[10];

    if (env_windows) {
        if (win_sumtablepath == nullptr)
            sprintf(s2, "sum%d.tab", k);
        else
            sprintf(s2, "%ssum%d.tab", win_sumtablepath, k);
    } else {
        sprintf(s2, "sum%d.tab", k);
        if (getenv("P4_DIR") == nullptr)
            *file_name2 = 0;
        else
            sprintf(file_name2, "%s/sumtables/sum%d.tab", getenv("P4_DIR"), k);
    }

    fp1 = fopen(s2, "w");
    if (fp1 == nullptr) {
        printf("Cannot create sumtables.  Please check installation.\n");
        perror(s2);
        exit(-6);
    }
    for (i = 1; i < k; i++) {
        // check if table (k-i) exist.  If not create the table
        check_sum(file_name, k - i);
        fp2 = fopen(file_name, "r");
        if (fp2 == nullptr) {
            perror(file_name);
            exit(-2);
        }
        do {
            fscanf(fp2, "%s", s);
            if (!feof(fp2))
                fprintf(fp1, "%d,%s\n", i, s);
        } while (!feof(fp2));
        fclose(fp2);
    }
    fprintf(fp1, "%d\n", k);
    fclose(fp1);

    if (!env_windows) {
        if (*file_name2 != 0) {
            sprintf(s1, "mv %s %s", s2, file_name2);
            // sprintf( s1, "cp %s %s", s2, file_name2 );
            system(s1);
            sprintf(s1, "chmod a+rw %s", file_name2);
            system(s1);
        }
    }
}
