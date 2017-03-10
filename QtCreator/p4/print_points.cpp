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

#include "print_points.h"

void (*print_saddle)(double, double) = nullptr;
void (*print_stablenode)(double, double) = nullptr;
void (*print_unstablenode)(double, double) = nullptr;
void (*print_stableweakfocus)(double, double) = nullptr;
void (*print_unstableweakfocus)(double, double) = nullptr;
void (*print_weakfocus)(double, double) = nullptr;
void (*print_stablestrongfocus)(double, double) = nullptr;
void (*print_unstablestrongfocus)(double, double) = nullptr;
void (*print_sesaddle)(double, double) = nullptr;
void (*print_sesaddlenode)(double, double) = nullptr;
void (*print_sestablenode)(double, double) = nullptr;
void (*print_seunstablenode)(double, double) = nullptr;
void (*print_degen)(double, double) = nullptr;
void (*print_center)(double, double) = nullptr;
void (*print_elips)(double, double, double, double, int, bool,
                    struct P4POLYLINES *) = nullptr;
void (*print_line)(double, double, double, double, int) = nullptr;
void (*print_point)(double, double, int) = nullptr;
void (*print_comment)(QString) = nullptr;
