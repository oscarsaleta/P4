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

#include <vector>

class QString;
struct P4POLYLINES;

extern void (*print_saddle)(double, double);
extern void (*print_virtualsaddle)(double, double);
extern void (*print_stablenode)(double, double);
extern void (*print_virtualstablenode)(double, double);
extern void (*print_unstablenode)(double, double);
extern void (*print_virtualunstablenode)(double, double);
extern void (*print_stableweakfocus)(double, double);
extern void (*print_virtualstableweakfocus)(double, double);
extern void (*print_unstableweakfocus)(double, double);
extern void (*print_virtualunstableweakfocus)(double, double);
extern void (*print_weakfocus)(double, double);
extern void (*print_virtualweakfocus)(double, double);
extern void (*print_stablestrongfocus)(double, double);
extern void (*print_virtualstablestrongfocus)(double, double);
extern void (*print_unstablestrongfocus)(double, double);
extern void (*print_virtualunstablestrongfocus)(double, double);
extern void (*print_sesaddle)(double, double);
extern void (*print_virtualsesaddle)(double, double);
extern void (*print_sesaddlenode)(double, double);
extern void (*print_virtualsesaddlenode)(double, double);
extern void (*print_sestablenode)(double, double);
extern void (*print_virtualsestablenode)(double, double);
extern void (*print_seunstablenode)(double, double);
extern void (*print_virtualseunstablenode)(double, double);
extern void (*print_degen)(double, double);
extern void (*print_virtualdegen)(double, double);
extern void (*print_center)(double, double);
extern void (*print_virtualcenter)(double, double);

extern void (*print_coinciding)(double, double);

extern void (*print_elips)(double, double, double, double, int, bool,
                           const std::vector<P4POLYLINES> &);
extern void (*print_line)(double, double, double, double, int);
extern void (*print_point)(double, double, int);
extern void (*print_comment)(QString);
