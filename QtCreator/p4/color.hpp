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

#include <QColor>

//
// XFIG 3.1 DEFAULT COLOR TABLE
//

namespace P4Colours
{
enum {
    black = 0,   // pure black
    blue = 1,    // pure blue
    green = 2,   // pure green
    cyan = 3,    // pure cyan
    red = 4,     // pure red
    magenta = 5, // pure magenta
    yellow = 6,  // pure yellow
    white = 7,   // pure white
    blue1 = 8,   // 4 shades of blue (dark to light)
    blue2 = 9,
    blue3 = 10,
    blue4 = 11,
    green1 = 12, // 3 shades of green (dark to light)
    green2 = 13,
    green3 = 14,
    cyan1 = 15, // 3 shades of cyan (dark to light)
    cyan2 = 16,
    cyan3 = 17,
    red1 = 18, // 3 shades of red (dark to light)
    red2 = 19,
    red3 = 20,
    magenta1 = 21, // 3 shades of magenta (dark to light)
    magenta2 = 22,
    magenta3 = 23,
    brown1 = 24, // 3 shades of brown (dark to light)
    brown2 = 25,
    brown3 = 26,
    pink1 = 27, // 3 shades of pink (dark to light)
    pink2 = 28,
    pink3 = 29,
    pink4 = 30,
    gold = 31, // gold
    darkgray = 32
};

const int numXfigColors = 32;

struct P4RGBITEM {
    int r;
    int g;
    int b;
};

P4RGBITEM gXFigToRGB[numXfigColors] = {
    // 8 pure colours:
    {0, 0, 0},       // BLACK
    {0, 0, 255},     // BLUE
    {0, 255, 0},     // GREEN
    {0, 255, 255},   // CYAN
    {255, 0, 0},     // RED
    {255, 0, 255},   // MAGENTA
    {255, 255, 0},   // YELLOW
    {255, 255, 255}, // WHITE

    // shaded colours (ordered from dark to light):
    {0, 0, 143},     // BLUE1
    {0, 0, 176},     // BLUE2
    {0, 0, 209},     // BLUE3
    {135, 207, 255}, // BLUE4
    {0, 143, 0},     // GREEN1
    {0, 176, 0},     // GREEN2
    {0, 209, 0},     // GREEN3
    {0, 143, 143},   // CYAN1
    {0, 176, 176},   // CYAN2
    {0, 209, 209},   // CYAN3
    {143, 0, 0},     // RED1
    {176, 0, 0},     // RED2
    {209, 0, 0},     // RED3
    {143, 0, 143},   // MAGENTA1
    {176, 0, 176},   // MAGENTA2
    {209, 32, 209},  // MAGENTA3
    {128, 48, 0},    // BROWN1
    {161, 64, 0},    // BROWN2
    {191, 97, 0},    // BROWN3
    {255, 128, 128}, // PINK1
    {255, 161, 161}, // PINK2
    {255, 191, 191}, // PINK3
    {255, 224, 224}, // PINK4
    {255, 214, 0}    // GOLD
};

QColor p4XfigColour(int x)
{
    return QColor{gXFigToRGB[x].r, gXFigToRGB[x].g, gXFigToRGB[x].b};
}
} // namespace P4Colours
