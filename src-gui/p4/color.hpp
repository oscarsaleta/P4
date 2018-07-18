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
    darkgray = 32,
    numXfigColours = 32
};

// extern const int numXfigColors;

struct P4RGBITEM {
    int r;
    int g;
    int b;
};

extern P4RGBITEM gXFigToRGB[];

QColor p4XfigColour(int x);
} // namespace P4Colours

// Color of singular points:
namespace P4ColourSettings
{
extern int colour_foreground; // foreground color
extern int colour_background; // background color
extern int colour_orbit;      // orbits (use GREEN1 when bg is white)

// IN PRINT_BITMAP.CPP: PrintColorTable shows how these colors are treated when
// printing.
// For example, when printing, BLACK can be reversed with WHITE, so that the
// BLACK background on screen is not printed black.
extern bool print_white_bg;

// FIXED COLOURS
extern const int colour_saddle_node;                 // saddle-node
extern const int colour_saddle;                      // saddle
extern const int colour_node_stable;                 // stable node
extern const int colour_node_unstable;               // unstable node
extern const int &colour_weak_focus;                 // weak focus generic
extern const int colour_weak_focus_stable;           // stable weak focus
extern const int colour_weak_focus_unstable;         // unstable weak focus
extern const int colour_strong_focus_stable;         // stable strong focus
extern const int colour_strong_focus_unstable;       // unstable strong focus
extern const int colour_center;                      // center
extern const int &colour_degen;                      // degenerate point
extern const int &colour_line_at_infinity;           // poincare sphere
extern const int colour_separatrice_stable;          // stable separatrice
extern const int colour_separatrice_unstable;        // unstable separatrice
extern const int colour_separatrice_center_stable;   // center-stable sep
extern const int colour_separatrice_center_unstable; // center-unstable sep
extern const int colour_selected_separatrice;        // selected separatrice
extern const int colour_limit_cycle;                 // limit cycle
extern const int colour_curve_singularities;         // curve of singularities
extern const int colour_arbitrary_curve;             // arbitrary curve
extern const int colour_isoclines;                   // colour of first isocline
extern const int colour_separating_curve;            // separating curve
extern const int colour_shaded_curve;                // shaded curve
} // namespace P4ColourSettings
