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

#include "color.hpp"

namespace c = P4Colours;
namespace cs = P4ColourSettings;

const int c::numXfigColors = 32;

c::P4RGBITEM c::gXFigToRGB[c::numXfigColors] = {
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

QColor c::p4XfigColour(int x)
{
    return QColor{c::gXFigToRGB[x].r, c::gXFigToRGB[x].g, c::gXFigToRGB[x].b};
}

int cs::colour_foreground{c::white};
int cs::colour_background{c::black};
int cs::colour_orbit{c::yellow};
bool cs::print_white_bg{true};
const int cs::colour_saddle_node{c::magenta};
const int cs::colour_saddle{c::green2};
const int cs::colour_node_stable{c::blue};
const int cs::colour_node_unstable{c::red};
const int &cs::colour_weak_focus{cs::colour_foreground};
const int cs::colour_weak_focus_stable{c::blue2};
const int cs::colour_weak_focus_unstable{c::red2};
const int cs::colour_strong_focus_stable{c::blue};
const int cs::colour_strong_focus_unstable{c::red};
const int cs::colour_center{c::green2};
const int &cs::colour_degen{cs::colour_foreground};
const int &cs::colour_line_at_infinity{cs::colour_foreground};
const int cs::colour_separatrice_stable{c::blue};
const int cs::colour_separatrice_unstable{c::red};
const int cs::colour_separatrice_center_stable{c::blue2};
const int cs::colour_separatrice_center_unstable{c::red2};
const int cs::colour_selected_separatrice{c::gold};
const int cs::colour_limit_cycle{c::magenta2};
const int cs::colour_curve_singularities{c::green};
const int cs::colour_arbitrary_curve{c::cyan};
const int cs::colour_isoclines{c::pink1};
const int cs::colour_separating_curve{c::brown3};
const int cs::colour_shaded_curve{c::brown1};
