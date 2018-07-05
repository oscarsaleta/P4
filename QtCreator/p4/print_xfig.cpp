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

#include "print_xfig.hpp"

#include <QFile>
#include <QTextStream>

#include <cmath>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "custom.hpp"
#include "main.hpp"
#include "plot_tools.hpp"
#include "print_bitmap.hpp"
#include "print_points.hpp"
#include "structures.hpp"

static QFile *sXFigFile{nullptr};
static QTextStream sXFigStream;

static bool sXFigBlackWhitePrint = true;
static int sXFigSymbolWidth = 0;
static int sXFigLineWidth = 0;     // linewidth at 80 DPI (for xfig only)
static int sXFigRealLineWidth = 0; // linewidth at chosen resolution
static int sXFigResolution = 0;
static int sXFigW = 0;
static int sXFigH = 0;

static int sLastXFigX0 = 0;
static int sLastXFigY0 = 0;
static int sLastXFigColor = -1;

static bool sXFigLineBusy = false;
static int sXFigLineLastX = 0;
static int sXFigLineLastY = 0;
static int sXFigLineColor = 0;
static int sXFigLineNumPoints = 0;
// NOTE: check if this works
static int *sXFigLinePoints{nullptr};

static void xfig_line_start(int x0, int y0, int x1, int y1, int color);
static void xfig_line_continue(int x1, int y1);
static void xfig_line_finish(void);

// -----------------------------------------------------------------------------

static void xfig_print_comment(QString s)
{
    if (sXFigLineBusy)
        xfig_line_finish();

    if (sXFigFile) {
        sXFigStream << "# " << s << "\n";
    }
}

static void xfig_print_box(int x, int y, int color)
{
    if (!sXFigFile)
        return;

    if (sXFigLineBusy)
        xfig_line_finish();

    x *= 1200;
    x /= sXFigResolution;
    y *= 1200;
    y /= sXFigResolution;

    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    /*
        object type     2   (=polyline)
        subtype         2   (=box)
        linestyle       0   (=solid)
        thickness       1   (in 1/80 of an inch)
        pencolor        color
        fillcolor       color
        depth           0  (precedence over other objects !!!)
        penstyle        0   (unused in XFIG 3.1)
        areafill        46  (45 degree cross hatch)
        styleval        0.0 (only relevant for dashed lines)
        joinstyle       0   (=miter)
        capstyle        0   (=butt)
        radius          0   (no rounded box)
        forwardarrow    0   (no arrow)
        backwardarrow   0   (no arrow)
        npoints         5   (=5 points)
    */

    QString s;
    s.sprintf("2 2 0 1 %d %d 0 0 46 0.0 0 0 0 0 0 5\n", color, color);
    sXFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d %d %d\n", x + sXFigSymbolWidth / 2,
              y - sXFigSymbolWidth / 2, x + sXFigSymbolWidth / 2,
              y + sXFigSymbolWidth / 2, x - sXFigSymbolWidth / 2,
              y + sXFigSymbolWidth / 2, x - sXFigSymbolWidth / 2,
              y - sXFigSymbolWidth / 2, x + sXFigSymbolWidth / 2,
              y - sXFigSymbolWidth / 2);
    sXFigStream << s;
}

static void xfig_print_box2(int x, int y, int color)
{
    if (!sXFigFile)
        return;

    if (sXFigLineBusy)
        xfig_line_finish();

    x *= 1200;
    x /= sXFigResolution;
    y *= 1200;
    y /= sXFigResolution;

    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    QString s;
    s.sprintf("2 2 0 1 %d %d 0 0 -1 0.0 0 0 0 0 0 5\n", color, -1);
    sXFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d %d %d\n", x + sXFigSymbolWidth / 2,
              y - sXFigSymbolWidth / 2, x + sXFigSymbolWidth / 2,
              y + sXFigSymbolWidth / 2, x - sXFigSymbolWidth / 2,
              y + sXFigSymbolWidth / 2, x - sXFigSymbolWidth / 2,
              y - sXFigSymbolWidth / 2, x + sXFigSymbolWidth / 2,
              y - sXFigSymbolWidth / 2);
    sXFigStream << s;
}

static void xfig_print_diamond(int x, int y, int color)
{
    if (!sXFigFile)
        return;

    if (sXFigLineBusy)
        xfig_line_finish();

    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    x *= 1200;
    x /= sXFigResolution;
    y *= 1200;
    y /= sXFigResolution;

    // structure info: see xfig_print_box
    // sub-type = 3 (polygon)

    QString s;
    s.sprintf("2 3 0 1 %d %d -1 0 46 0.0 0 0 0 0 0 5\n", color, color);
    sXFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d %d %d\n", x,
              y - sXFigSymbolWidth * 13 / 20, x + sXFigSymbolWidth * 13 / 20, y,
              x, y + sXFigSymbolWidth * 13 / 20, x - sXFigSymbolWidth * 13 / 20,
              y, x, y - sXFigSymbolWidth * 13 / 20);
    sXFigStream << s;
}

static void xfig_print_diamond2(int x, int y, int color)
{
    if (!sXFigFile)
        return;

    if (sXFigLineBusy)
        xfig_line_finish();

    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    x *= 1200;
    x /= sXFigResolution;
    y *= 1200;
    y /= sXFigResolution;

    // structure info: see xfig_print_box
    // sub-type = 3 (polygon)

    QString s;
    s.sprintf("2 3 0 1 %d %d -1 0 -1 0.0 0 0 0 0 0 5\n", color, -1);
    sXFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d %d %d\n", x,
              y - sXFigSymbolWidth * 13 / 20, x + sXFigSymbolWidth * 13 / 20, y,
              x, y + sXFigSymbolWidth * 13 / 20, x - sXFigSymbolWidth * 13 / 20,
              y, x, y - sXFigSymbolWidth * 13 / 20);
    sXFigStream << s;
}

static void xfig_print_triangle(int x, int y, int color)
{
    if (!sXFigFile)
        return;

    if (sXFigLineBusy)
        xfig_line_finish();

    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    x *= 1200;
    x /= sXFigResolution;
    y *= 1200;
    y /= sXFigResolution;

    // structure info: see xfig_print_box (except that here: npoints = 4 instead
    // of 5)
    // sub-type = 3 (polygon)

    QString s;
    // TODO check this -1 (is 0 in p5)
    s.sprintf("2 3 0 1 %d %d -1 0 46 0.0 0 0 0 0 0 4\n", color, color);
    sXFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d\n", x + sXFigSymbolWidth * 12 / 20,
              y + sXFigSymbolWidth * 12 / 20, x, y - sXFigSymbolWidth * 12 / 20,
              x - sXFigSymbolWidth * 12 / 20, y + sXFigSymbolWidth * 12 / 20,
              x + sXFigSymbolWidth * 12 / 20, y + sXFigSymbolWidth * 12 / 20);
    sXFigStream << s;
}

static void xfig_print_triangle2(int x, int y, int color)
{
    if (!sXFigFile)
        return;

    if (sXFigLineBusy)
        xfig_line_finish();

    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    x *= 1200;
    x /= sXFigResolution;
    y *= 1200;
    y /= sXFigResolution;

    // structure info: see xfig_print_box (except that here: npoints = 4 instead
    // of 5)
    // sub-type = 3 (polygon)

    QString s;
    s.sprintf("2 3 0 1 %d %d -1 0 -1 0.0 0 0 0 0 0 4\n", color, -1);
    sXFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d\n", x + sXFigSymbolWidth * 12 / 20,
              y + sXFigSymbolWidth * 12 / 20, x, y - sXFigSymbolWidth * 12 / 20,
              x - sXFigSymbolWidth * 12 / 20, y + sXFigSymbolWidth * 12 / 20,
              x + sXFigSymbolWidth * 12 / 20, y + sXFigSymbolWidth * 12 / 20);
    sXFigStream << s;
}

static void xfig_print_cross(int x, int y, int color)
{
    if (!sXFigFile)
        return;

    if (sXFigLineBusy)
        xfig_line_finish();

    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    x *= 1200;
    x /= sXFigResolution;
    y *= 1200;
    y /= sXFigResolution;

    // cross is made with linewidth * 2.6

    int lw{sXFigLineWidth};
    lw *= 26;
    lw += 10;
    lw /= 20;

    QString s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x - sXFigSymbolWidth / 2, y - sXFigSymbolWidth / 2,
              x + sXFigSymbolWidth / 2, y + sXFigSymbolWidth / 2);
    sXFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x + sXFigSymbolWidth / 2, y - sXFigSymbolWidth / 2,
              x - sXFigSymbolWidth / 2, y + sXFigSymbolWidth / 2);
    sXFigStream << s;
}

static void xfig_print_cross2(int x, int y, int color)
{
    if (!sXFigFile)
        return;

    if (sXFigLineBusy)
        xfig_line_finish();

    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    x *= 1200;
    x /= sXFigResolution;
    y *= 1200;
    y /= sXFigResolution;

    // cross is made with linewidth * 2.6

    int lw{sXFigLineWidth};
    lw *= 26;
    lw += 10;
    lw /= 20;

    QString s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x - sXFigSymbolWidth / 3, y - sXFigSymbolWidth / 3,
              x + sXFigSymbolWidth / 3, y + sXFigSymbolWidth / 3);
    sXFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x + sXFigSymbolWidth / 3, y - sXFigSymbolWidth / 3,
              x - sXFigSymbolWidth / 3, y + sXFigSymbolWidth / 3);
    sXFigStream << s;
}

void xfig_print_doublecross(int x, int y, int color)
{
    if (!sXFigFile)
        return;

    if (sXFigLineBusy)
        xfig_line_finish();

    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    x *= 1200;
    x /= sXFigResolution;
    y *= 1200;
    y /= sXFigResolution;

    // cross is made with linewidth * 2.6

    int lw{sXFigLineWidth};
    lw *= 13;
    lw += 10;
    lw /= 20;

    QString s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x - sXFigSymbolWidth / 2, y - sXFigSymbolWidth / 2,
              x + sXFigSymbolWidth / 2, y + sXFigSymbolWidth / 2);
    sXFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x + sXFigSymbolWidth / 2, y - sXFigSymbolWidth / 2,
              x - sXFigSymbolWidth / 2, y + sXFigSymbolWidth / 2);
    sXFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x, y - (sXFigSymbolWidth * 3) / 4, x,
              y + (sXFigSymbolWidth * 3) / 4);
    sXFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x + (sXFigSymbolWidth * 3) / 4, y,
              x - (sXFigSymbolWidth * 3) / 4, y);
    sXFigStream << s;
}

static void xfig_print_line(double _x0, double _y0, double _x1, double _y1,
                            int color)
{
    if (!sXFigFile)
        return;

    _x0 *= 1200;
    _x0 /= sXFigResolution;
    _y0 *= 1200;
    _y0 /= sXFigResolution;

    _x1 *= 1200;
    _x1 /= sXFigResolution;
    _y1 *= 1200;
    _y1 /= sXFigResolution;

    color = printColorTable(color);
    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    int x0{static_cast<int>(std::floor(_x0))};
    int y0{static_cast<int>(std::floor(_y0))};
    int x1{static_cast<int>(std::floor(_x1))};
    int y1{static_cast<int>(std::floor(_y1))};
    if (x0 == x1 && y0 == y1)
        return;

    if (sXFigLineBusy && sXFigLineLastX == x0 && sXFigLineLastY == y0 &&
        sXFigLineColor == color) {
        xfig_line_continue(x1, y1);
    } else {
        if (sXFigLineBusy)
            xfig_line_finish();

        xfig_line_start(x0, y0, x1, y1, color);
    }
}

static void xfig_print_elips(double x0, double y0, double a, double b,
                             int color, bool dotted,
                             const std::vector<P4POLYLINES> &ellipse)
{
    if (!sXFigFile)
        return;

    if (sXFigLineBusy)
        xfig_line_finish();

    x0 *= 1200;
    x0 /= sXFigResolution;
    y0 *= 1200;
    y0 /= sXFigResolution;

    a *= 1200;
    a /= sXFigResolution;
    b *= 1200;
    b /= sXFigResolution;

    if (x0 - a >= 0 && x0 + a < sXFigW && y0 - b >= 0 && y0 + b < sXFigH) {
        // full ellipse is visible

        /*
            object type     1   (=ellipse)
            subtype         3   (=circle defined by radius)
            linestyle       0 or 1  (=solid or dashed)
            thickness       ... (in 1/80 of an inch)
            pencolor        color
            fillcolor       color
            depth           50   (no precedence over other objects)
            penstyle        0   (unused in XFIG 3.1)
            areafill        -1  (45 degree cross hatch)  USE 20 INSTEAD (FULL
           SATURATION OF THE COLOR)
            styleval        0.0 (only relevant for dashed lines)
            direction       1   (=counterclockwise, must be 1)
            angle           0.0 (=angle of the x-axis)
            center_x        x0
            center_y        y0
            radius_x        linewidth (in Fig Units!!!)
            radius_y        linewidth (in Fig Units!!!)
            start_x         x0
            start_y         y0
            end_x           x0 + linewidth
            end_y           y0
        */

        double styleval;
        int linestyle;

        if (dotted) {
            linestyle = 1; // dashed
            // style_val = on/off length (in 1/80 of inch) = 8*linestyle
            styleval = sXFigLineWidth * 4;
        } else {
            linestyle = 0;
            styleval = 0.0;
        }

        QString s;
        s.sprintf("1 1 %d %d %d %d 50 0 -1 %g 1 0.0 %d %d %d %d %d %d %d %d\n",
                  linestyle, sXFigLineWidth / 2, printColorTable(color),
                  printColorTable(color), (float)styleval, (int)x0, (int)y0,
                  (int)a, (int)b, (int)x0, (int)y0, ((int)x0) + ((int)a),
                  (int)y0);
        sXFigStream << s;
    } else {
        // ellipse is only partially visible, so emulate with polygon.
        for (auto const &it : ellipse) {
            xfig_print_line(it.x1, it.y1, it.x2, it.y2, color);
        }
    }
}

static void xfig_line_start(int x0, int y0, int x1, int y1, int color)
{
    sXFigLineBusy = true;
    sXFigLinePoints[0] = x0;
    sXFigLinePoints[1] = y0;
    sXFigLinePoints[2] = x1;
    sXFigLinePoints[3] = y1;
    sXFigLineNumPoints = 2;
    sXFigLineLastX = x1;
    sXFigLineLastY = y1;
    sXFigLineColor = color;
}

static void xfig_line_continue(int x1, int y1)
{
    if (sXFigLineNumPoints < XFIG_LINE_MAXPOINTS) {
        sXFigLinePoints[2 * sXFigLineNumPoints] = sXFigLineLastX = x1;
        sXFigLinePoints[2 * sXFigLineNumPoints + 1] = sXFigLineLastY = y1;
        sXFigLineNumPoints++;
    } else {
        int x0{sXFigLineLastX};
        int y0{sXFigLineLastY};
        int color{sXFigLineColor};

        xfig_line_finish();
        xfig_line_start(x0, y0, x1, y1, color);
    }
}

static void xfig_line_finish(void)
{
    if (!sXFigLineBusy)
        return;

    sXFigLineBusy = false;

    /*
        object type     2   (=polyline)
        subtype         1   (=polyline)
        linestyle       0   (=solid)
        thickness       thickness   (in 1/80 of an inch)
        pencolor        color
        fillcolor       7   (=white)
        depth           50   (no precedence over other objects)
        penstyle        0   (unused in XFIG 3.1)
        areafill        -1  (no fill)
        styleval        0.0 (only relevant for dashed lines)
        joinstyle       0   (=miter)
        capstyle        1   (=round cap)
        radius          -1  (only relevant for arced boxes, irrelevant here)
        forwardarrow    0   (no arrow)
        backwardarrow   0   (no arrow)
        npoints         2   (=2 points)
    */

    int k;

    if (sXFigFile) {
        QString s;
        s.sprintf("2 1 0 %d %d 7 50 0 -1 0.0 0 1 -1 0 0 %d\n   ",
                  sXFigLineWidth / 2, sXFigLineColor, sXFigLineNumPoints);
        sXFigStream << s;
        for (int i = 0; i < sXFigLineNumPoints; i += 8) {
            k = 2 * (sXFigLineNumPoints - i);
            if (k >= 16)
                k = 16;

            for (int j = 0; j < k; j++) {
                s.sprintf(" %d", sXFigLinePoints[2 * i + j]);
                sXFigStream << s;
            }

            if (i + 8 < sXFigLineNumPoints)
                sXFigStream << "\n   ";
            else
                sXFigStream << "\n";
        }
    }
}

static void xfig_print_point(double _x0, double _y0, int color)
{
    if (sXFigLineBusy)
        xfig_line_finish();

    _x0 *= 1200;
    _x0 /= sXFigResolution;
    _y0 *= 1200;
    _y0 /= sXFigResolution;

    if (!sXFigFile)
        return;

    color = printColorTable(color);

    if (sXFigBlackWhitePrint)
        color = printColorTable(P4ColourSettings::colour_foreground);

    int x0{(int)_x0};
    int y0{(int)_y0};

    if (x0 == sLastXFigX0 && y0 == sLastXFigY0 && color == sLastXFigColor)
        return;

    sLastXFigX0 = x0;
    sLastXFigY0 = y0;
    sLastXFigColor = color;

    /*
        object type     1   (=ellipse)
        subtype         3   (=circle defined by radius)
        linestyle       0   (=solid)
        thickness       1   (in 1/80 of an inch)
        pencolor        color
        fillcolor       color
        depth           50  (no precedence over other objects)
        penstyle        0   (unused in XFIG 3.1)
        areafill        46  (45 degree cross hatch)  USE 20 INSTEAD (FULL
       SATURATION OF THE COLOR)
        styleval        0.0 (only relevant for dashed lines)
        direction       1   (=counterclockwise, must be 1)
        angle           0.0 (=angle of the x-axis)
        center_x        x0
        center_y        y0
        radius_x        linewidth (in Fig Units!!!)
        radius_y        linewidth (in Fig Units!!!)
        start_x         x0
        start_y         y0
        end_x           x0 + linewidth
        end_y           y0
    */

    QString s;
    s.sprintf("1 3 0 1 %d %d 50 0 46 0.0 1 0.0 %d %d %d %d %d %d %d %d\n",
              color, color, x0, y0, sXFigRealLineWidth / 2,
              sXFigRealLineWidth / 2, x0, y0, x0 + sXFigRealLineWidth, y0);
    sXFigStream << s;
}

static void xfig_print_saddle(double x, double y)
{
    xfig_print_box(static_cast<int>(std::floor(x)),
                   static_cast<int>(std::floor(y)), printColorTable(P4ColourSettings::colour_saddle));
}

static void xfig_print_stablenode(double x, double y)
{
    xfig_print_box(static_cast<int>(std::floor(x)),
                   static_cast<int>(std::floor(y)), printColorTable(P4ColourSettings::colour_node_stable));
}
static void xfig_print_unstablenode(double x, double y)
{
    xfig_print_box(static_cast<int>(std::floor(x)),
                   static_cast<int>(std::floor(y)), printColorTable(P4ColourSettings::colour_node_unstable));
}

static void xfig_print_stableweakfocus(double x, double y)
{
    xfig_print_diamond(static_cast<int>(std::floor(x)),
                       static_cast<int>(std::floor(y)),
                       printColorTable(P4ColourSettings::colour_weak_focus_stable));
}

static void xfig_print_unstableweakfocus(double x, double y)
{
    xfig_print_diamond(static_cast<int>(std::floor(x)),
                       static_cast<int>(std::floor(y)),
                       printColorTable(P4ColourSettings::colour_weak_focus_unstable));
}

static void xfig_print_weakfocus(double x, double y)
{
    xfig_print_diamond(static_cast<int>(std::floor(x)),
                       static_cast<int>(std::floor(y)),
                       printColorTable(P4ColourSettings::colour_weak_focus));
}

static void xfig_print_center(double x, double y)
{
    xfig_print_diamond(static_cast<int>(std::floor(x)),
                       static_cast<int>(std::floor(y)),
                       printColorTable(P4ColourSettings::colour_center));
}

static void xfig_print_stablestrongfocus(double x, double y)
{
    xfig_print_diamond(static_cast<int>(std::floor(x)),
                       static_cast<int>(std::floor(y)),
                       printColorTable(P4ColourSettings::colour_strong_focus_stable));
}

static void xfig_print_unstablestrongfocus(double x, double y)
{
    xfig_print_diamond(static_cast<int>(std::floor(x)),
                       static_cast<int>(std::floor(y)),
                       printColorTable(P4ColourSettings::colour_strong_focus_unstable));
}

static void xfig_print_sesaddle(double x, double y)
{
    xfig_print_triangle(static_cast<int>(std::floor(x)),
                        static_cast<int>(std::floor(y)),
                        printColorTable(P4ColourSettings::colour_saddle));
}

static void xfig_print_sesaddlenode(double x, double y)
{
    xfig_print_triangle(static_cast<int>(std::floor(x)),
                        static_cast<int>(std::floor(y)),
                        printColorTable(colour_saddle_node));
}

static void xfig_print_sestablenode(double x, double y)
{
    xfig_print_triangle(static_cast<int>(std::floor(x)),
                        static_cast<int>(std::floor(y)),
                        printColorTable(P4ColourSettings::colour_node_stable));
}

static void xfig_print_seunstablenode(double x, double y)
{
    xfig_print_triangle(static_cast<int>(std::floor(x)),
                        static_cast<int>(std::floor(y)),
                        printColorTable(P4ColourSettings::colour_node_unstable));
}

static void xfig_print_degen(double x, double y)
{
    xfig_print_cross(static_cast<int>(std::floor(x)),
                     static_cast<int>(std::floor(y)), printColorTable(P4ColourSettings::colour_degen));
}

static void xfig_print_virtualsaddle(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_box2(static_cast<int>(std::floor(x)),
                        static_cast<int>(std::floor(y)),
                        printColorTable(P4ColourSettings::colour_saddle));
}

static void xfig_print_virtualstablenode(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_box2(static_cast<int>(std::floor(x)),
                        static_cast<int>(std::floor(y)),
                        printColorTable(P4ColourSettings::colour_node_stable));
}
static void xfig_print_virtualunstablenode(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_box2(static_cast<int>(std::floor(x)),
                        static_cast<int>(std::floor(y)),
                        printColorTable(P4ColourSettings::colour_node_unstable));
}

static void xfig_print_virtualstableweakfocus(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_diamond2(static_cast<int>(std::floor(x)),
                            static_cast<int>(std::floor(y)),
                            printColorTable(P4ColourSettings::colour_weak_focus_stable));
}

static void xfig_print_virtualunstableweakfocus(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_diamond2(static_cast<int>(std::floor(x)),
                            static_cast<int>(std::floor(y)),
                            printColorTable(P4ColourSettings::colour_weak_focus_unstable));
}

static void xfig_print_virtualweakfocus(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_diamond2(static_cast<int>(std::floor(x)),
                            static_cast<int>(std::floor(y)),
                            printColorTable(P4ColourSettings::colour_weak_focus));
}

static void xfig_print_virtualcenter(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_diamond2(static_cast<int>(std::floor(x)),
                            static_cast<int>(std::floor(y)),
                            printColorTable(P4ColourSettings::colour_center));
}

static void xfig_print_virtualstablestrongfocus(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_diamond2(static_cast<int>(std::floor(x)),
                            static_cast<int>(std::floor(y)),
                            printColorTable(P4ColourSettings::colour_strong_focus_stable));
}

static void xfig_print_virtualunstablestrongfocus(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_diamond2(static_cast<int>(std::floor(x)),
                            static_cast<int>(std::floor(y)),
                            printColorTable(P4ColourSettings::colour_strong_focus_unstable));
}

static void xfig_print_virtualsesaddle(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_triangle2(static_cast<int>(std::floor(x)),
                             static_cast<int>(std::floor(y)),
                             printColorTable(P4ColourSettings::colour_saddle));
}

static void xfig_print_virtualsesaddlenode(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_triangle2(static_cast<int>(std::floor(x)),
                             static_cast<int>(std::floor(y)),
                             printColorTable(colour_saddle_node));
}

static void xfig_print_virtualsestablenode(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_triangle2(static_cast<int>(std::floor(x)),
                             static_cast<int>(std::floor(y)),
                             printColorTable(P4ColourSettings::colour_node_stable));
}

static void xfig_print_virtualseunstablenode(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_triangle2(static_cast<int>(std::floor(x)),
                             static_cast<int>(std::floor(y)),
                             printColorTable(P4ColourSettings::colour_node_unstable));
}

static void xfig_print_virtualdegen(double x, double y)
{
    if (gVFResults.plotVirtualSingularities_)
        xfig_print_cross2(static_cast<int>(std::floor(x)),
                          static_cast<int>(std::floor(y)),
                          printColorTable(P4ColourSettings::colour_degen));
}

void xfig_print_coinciding(double x, double y)
{
    xfig_print_doublecross(static_cast<int>(std::floor(x)),
                           static_cast<int>(std::floor(y)),
                           printColorTable(P4ColourSettings::colour_degen));
}

// -----------------------------------------------------------------------------

void prepareXFigPrinting(int w, int h, bool iszoom, bool isblackwhite,
                         int resolution, int linewidth, int symbolwidth)
{
    QString s;

    sXFigBlackWhitePrint = isblackwhite;
    sXFigRealLineWidth = linewidth;
    sXFigLineWidth = linewidth;
    sXFigSymbolWidth = symbolwidth;
    sXFigResolution = resolution;
    // since XFIG always reads the file at 1200 DPI, we rescale points in this
    // module. by multiplying x and y coordinates with (1200 / resolution), just
    // before coordinates are printed.

    sXFigSymbolWidth *= 1200;
    sXFigSymbolWidth /= resolution;
    sXFigLineWidth *= 1200;
    sXFigLineWidth /= resolution;
    sXFigRealLineWidth *= 1200;
    sXFigRealLineWidth /= resolution;

    // make sure it is even, and nonzero:
    sXFigRealLineWidth /= 2;
    if (sXFigRealLineWidth == 0)
        sXFigRealLineWidth = 1;
    sXFigRealLineWidth *= 2;

    // XFIG Line width is always at 80 DPI, regardless of resolution
    sXFigLineWidth *= 80;
    sXFigLineWidth =
        (sXFigLineWidth + (resolution / 2)) / resolution; // divide + round up

    // make sure it is even, and nonzero
    sXFigLineWidth /= 2;
    if (sXFigLineWidth == 0)
        sXFigLineWidth = 1; // minimal line width = 1
    sXFigLineWidth *= 2;

    s = gThisVF->getbarefilename() + ".fig";

    sXFigFile = new QFile{s};
    if (sXFigFile->open(QIODevice::WriteOnly)) {
        sXFigStream.setDevice(sXFigFile);
    } else {
        delete sXFigFile;
        sXFigFile = nullptr;
    }

    plot_l = spherePrintLine;
    plot_p = spherePrintPoint;

    print_saddle = xfig_print_saddle;
    print_stablenode = xfig_print_stablenode;
    print_unstablenode = xfig_print_unstablenode;
    print_stableweakfocus = xfig_print_stableweakfocus;
    print_unstableweakfocus = xfig_print_unstableweakfocus;
    print_weakfocus = xfig_print_weakfocus;
    print_stablestrongfocus = xfig_print_stablestrongfocus;
    print_unstablestrongfocus = xfig_print_unstablestrongfocus;
    print_sesaddle = xfig_print_sesaddle;
    print_sesaddlenode = xfig_print_sesaddlenode;
    print_sestablenode = xfig_print_sestablenode;
    print_seunstablenode = xfig_print_seunstablenode;
    print_degen = xfig_print_degen;
    print_center = xfig_print_center;
    print_elips = xfig_print_elips;
    print_point = xfig_print_point;
    print_line = xfig_print_line;
    print_comment = xfig_print_comment;
    print_virtualsaddle = xfig_print_virtualsaddle;
    print_virtualstablenode = xfig_print_virtualstablenode;
    print_virtualunstablenode = xfig_print_virtualunstablenode;
    print_virtualstableweakfocus = xfig_print_virtualstableweakfocus;
    print_virtualunstableweakfocus = xfig_print_virtualunstableweakfocus;
    print_virtualweakfocus = xfig_print_virtualweakfocus;
    print_virtualstablestrongfocus = xfig_print_virtualstablestrongfocus;
    print_virtualunstablestrongfocus = xfig_print_virtualunstablestrongfocus;
    print_virtualsesaddle = xfig_print_virtualsesaddle;
    print_virtualsesaddlenode = xfig_print_virtualsesaddlenode;
    print_virtualsestablenode = xfig_print_virtualsestablenode;
    print_virtualseunstablenode = xfig_print_virtualseunstablenode;
    print_virtualdegen = xfig_print_virtualdegen;
    print_virtualcenter = xfig_print_virtualcenter;
    print_coinciding = xfig_print_coinciding;

    sLastXFigColor = -1;

    sXFigLineBusy = false;
    // max points *2 for x and y coords
    sXFigLinePoints = new int[XFIG_LINE_MAXPOINTS * 2];

    sXFigW = w * 1200;
    sXFigW /= resolution;
    sXFigH = h * 1200;
    sXFigH /= resolution;

    if (sXFigFile) {
        sXFigStream << "#FIG 3.1\n" // XFIG version 3.1 file format
                       "Portrait\n" // portrait orientation
                       "Center\n"   // center on page
                       "Inches\n" // use inches (XFIG makes rounding errors with
                                  // centimeters)
                       "1200 2\n"; // unused in XFIG???
        if (!P4ColourSettings::print_white_bg) {
            /*
                object type     2   (=polyline)
                subtype         2   (=box)
                linestyle       0   (=solid)
                thickness       1   (in 1/80 of an inch)
                pencolor        0   (=black)
                fillcolor       7   (=white)
                depth           999 (below other objects)
                penstyle        -1  (unused in XFIG 3.1)
                areafill        0   (pencolor fill)
                styleval        0.0 (only relevant for dashed lines)
                joinstyle       0   (=miter)
                capstyle        0   (=butt)
                radius          0   (no rounded box)
                forwardarrow    0   (no arrow)
                backwardarrow   0   (no arrow)
                npoints         5   (=5 points)
            */
            QString s;
            s.sprintf("2 2 0 1 0 7 999 -1 0 0.0000000 0 0 0 0 0 5\n"
                      "    %d %d %d %d %d %d %d %d %d %d\n",
                      0, 0, sXFigW, 0, sXFigW, sXFigH, 0, sXFigH, 0, 0);
            sXFigStream << s;
        }
        if (gVFResults.typeofview_ == TYPEOFVIEW_PLANE || iszoom) {
            /*
                object type     2   (=polyline)
                subtype         2   (=box)
                linestyle       0   (=solid)
                thickness       1   (in 1/80 of an inch)
                pencolor        0   (=black)
                fillcolor       7   (=white)
                depth           998 (below all objects except background)
                penstyle        0   (unused in XFIG 3.1)
                areafill        -1  (no fill)
                styleval        0.0 (only relevant for dashed lines)
                joinstyle       0   (=miter)
                capstyle        0   (=butt)
                radius          0   (no rounded box)
                forwardarrow    0   (no arrow)
                backwardarrow   0   (no arrow)
                npoints         5   (=5 points)
            */
            QString s;
            s.sprintf("2 2 0 1 0 7 998 0 -1 0.0000000 0 0 0 0 0 5\n"
                      "    %d %d %d %d %d %d %d %d %d %d\n",
                      0, 0, sXFigW, 0, sXFigW, sXFigH, 0, sXFigH, 0, 0);
            sXFigStream << s;
        }
    }
}

void finishXFigPrinting(void)
{
    if (sXFigFile) {
        sXFigStream.flush();
        sXFigStream.setDevice(nullptr);
        sXFigFile->close();
        delete sXFigFile;
        sXFigFile = nullptr;
    }

    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;

    if (sXFigLinePoints != nullptr) {
        delete[] sXFigLinePoints;
        sXFigLinePoints = nullptr;
    }
}
