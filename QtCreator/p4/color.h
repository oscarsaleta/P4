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

#ifndef COLOR_H
#define COLOR_H

#include <QColor>

//
// XFIG 3.1 DEFAULT COLOR TABLE
//

#define BLACK 0   // pure black
#define BLUE 1    // pure blue
#define GREEN 2   // pure green
#define CYAN 3    // pure cyan
#define RED 4     // pure red
#define MAGENTA 5 // pure magenta
#define YELLOW 6  // pure yellow
#define WHITE 7   // pure white
#define BLUE1 8   // 4 shades of blue (dark to light)
#define BLUE2 9
#define BLUE3 10
#define BLUE4 11
#define GREEN1 12 // 3 shades of green (dark to light)
#define GREEN2 13
#define GREEN3 14
#define CYAN1 15 // 3 shades of cyan (dark to light)
#define CYAN2 16
#define CYAN3 17
#define RED1 18 // 3 shades of red (dark to light)
#define RED2 19
#define RED3 20
#define MAGENTA1 21 // 3 shades of magenta (dark to light)
#define MAGENTA2 22
#define MAGENTA3 23
#define BROWN1 24 // 3 shades of brown (dark to light)
#define BROWN2 25
#define BROWN3 26
#define PINK1 27 // 3 shades of pink (dark to light)
#define PINK2 28
#define PINK3 29
#define PINK4 30
#define GOLD 31 // gold

#define NUMXFIGCOLORS 32
#define DARKGRAY NUMXFIGCOLORS

#define QXFIGCOLOR(x) QColor(XFigToRGB[x].r, XFigToRGB[x].g, XFigToRGB[x].b)

struct P4RGBITEM {
    int r;
    int g;
    int b;
};

extern struct P4RGBITEM XFigToRGB[NUMXFIGCOLORS]; // defined in win_legend.cpp

#endif /* COLOR_H */
