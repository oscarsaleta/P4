/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
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

#pragma once

#include <stdio.h>
#include <vector>

namespace p4polynom
{
struct term1;
struct term2;
struct term3;
}

double eval_term1(const std::vector<p4polynom::term1> &, const double);
double eval_term2(const std::vector<p4polynom::term2> &, const double *);
double eval_term3(const std::vector<p4polynom::term3> &, const double *);

const char *dumpPoly1(const std::vector<p4polynom::term1> &f, const char *x);
const char *dumpPoly2(const std::vector<p4polynom::term2> &f, const char *x,
                      const char *y);
const char *dumpPoly3(const std::vector<p4polynom::term3> &f, const char *x,
                      const char *y, const char *z);

char *printterm2(char *buf, const p4polynom::term2 &f, bool isfirst,
                 const char *x, const char *y);
char *printterm3(char *buf, const p4polynom::term3 &f, bool isfirst,
                 const char *r, const char *Co, const char *Si);

bool readTerm1(FILE *fp, std::vector<p4polynom::term1> p, int N);
bool readTerm2(FILE *fp, std::vector<p4polynom::term2> p, int N);
bool readTerm3(FILE *fp, std::vector<p4polynom::term3> p, int N);
