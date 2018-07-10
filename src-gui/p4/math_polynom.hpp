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

namespace P4Polynom
{
struct term1;
struct term2;
struct term3;
} // namespace P4Polynom

double eval_term1(const P4Polynom::term1 *, const double);
double eval_term1(const std::vector<P4Polynom::term1> &p, const double t);
double eval_term2(const P4Polynom::term2 *, const double *);
double eval_term2(const std::vector<P4Polynom::term2> &f, const double *value);
double eval_term3(const P4Polynom::term3 *, const double *);
double eval_term3(const std::vector<P4Polynom::term3> &F, const double *value);

const char *dumpPoly1(P4Polynom::term1 *f, const char *x);
const char *dumpPoly2(P4Polynom::term2 *f, const char *x, const char *y);
const char *dumpPoly3(P4Polynom::term3 *f, const char *x, const char *y,
                      const char *z);

char *printterm2(char *buf, const P4Polynom::term2 *f, bool isfirst,
                 const char *x, const char *y);
char *printterm3(char *buf, const P4Polynom::term3 *f, bool isfirst,
                 const char *r, const char *Co, const char *Si);

bool readTerm1(FILE *fp, P4Polynom::term1 *p, int N);
bool readTerm1(FILE *fp, std::vector<P4Polynom::term1> &p, int N);
bool readTerm2(FILE *fp, P4Polynom::term2 *p, int N);
bool readTerm2(FILE *fp, std::vector<P4Polynom::term2> &p, int N);
bool readTerm3(FILE *fp, P4Polynom::term3 *p, int N);
bool readTerm3(FILE *fp, std::vector<P4Polynom::term3> &p, int N);
