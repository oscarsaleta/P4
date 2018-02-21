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

#include "print_xfig.h"

#include "custom.h"
#include "file_tab.h"
#include "P4InputVF.h"
#include "main.h"
#include "plot_tools.h"
#include "print_bitmap.h"
#include "print_points.h"

#include <QFile>
#include <QTextStream>

static std::unique_ptr<QFile> s_XFigFile();
static QTextStream s_XFigStream;

static bool s_XFigBlackWhitePrint = true;
static int s_XFigSymbolWidth = 0;
static int s_XFigLineWidth = 0;      // linewidth at 80 DPI (for xfig only)
static int s_XFigRealLineWidth = 0;  // linewidth at chosen resolution
static int s_XFigResolution = 0;
static int s_XFigW = 0;
static int s_XFigH = 0;

static int s_LastXfigX0 = 0;
static int s_LastXfigY0 = 0;
static int s_LastXfigcolor = -1;

static bool s_xfig_line_busy = false;
static int s_xfig_line_lastx = 0;
static int s_xfig_line_lasty = 0;
static int s_xfig_line_color = 0;
static int s_xfig_line_numpoints = 0;
// NOTE: check if this works
static std::unique_ptr<int[]> s_xfig_line_points;

static void xfig_line_start(int x0, int y0, int x1, int y1, int color);
static void xfig_line_continue(int x1, int y1);
static void xfig_line_finish(void);

// ---------------------------------------------------------------------------------------

static void xfig_print_comment(QString s)
{
    if (s_xfig_line_busy)
        xfig_line_finish();

    if (s_XFigFile) {
        s_XFigStream << "# " << s << "\n";
    }
}

static void xfig_print_box(int x, int y, int color)
{
    if (!s_XFigFile)
        return;

    if (s_xfig_line_busy)
        xfig_line_finish();

    x *= 1200;
    x /= s_XFigResolution;
    y *= 1200;
    y /= s_XFigResolution;

    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

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
    s_XFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d %d %d\n", x + s_XFigSymbolWidth / 2,
              y - s_XFigSymbolWidth / 2, x + s_XFigSymbolWidth / 2,
              y + s_XFigSymbolWidth / 2, x - s_XFigSymbolWidth / 2,
              y + s_XFigSymbolWidth / 2, x - s_XFigSymbolWidth / 2,
              y - s_XFigSymbolWidth / 2, x + s_XFigSymbolWidth / 2,
              y - s_XFigSymbolWidth / 2);
    s_XFigStream << s;
}

static void xfig_print_box2(int x, int y, int color)
{
    if (!s_XFigFile)
        return;

    if (s_xfig_line_busy)
        xfig_line_finish();

    x *= 1200;
    x /= s_XFigResolution;
    y *= 1200;
    y /= s_XFigResolution;

    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

    QString s;
    s.sprintf("2 2 0 1 %d %d 0 0 -1 0.0 0 0 0 0 0 5\n", color, -1);
    s_XFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d %d %d\n", x + XFigSymbolWidth / 2,
              y - XFigSymbolWidth / 2, x + XFigSymbolWidth / 2,
              y + XFigSymbolWidth / 2, x - XFigSymbolWidth / 2,
              y + XFigSymbolWidth / 2, x - XFigSymbolWidth / 2,
              y - XFigSymbolWidth / 2, x + XFigSymbolWidth / 2,
              y - XFigSymbolWidth / 2);
    s_XFigStream << s;
}

static void xfig_print_diamond(int x, int y, int color)
{
    if (!s_XFigFile)
        return;

    if (s_xfig_line_busy)
        xfig_line_finish();

    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

    x *= 1200;
    x /= s_XFigResolution;
    y *= 1200;
    y /= s_XFigResolution;

    // structure info: see xfig_print_box
    // sub-type = 3 (polygon)

    QString s;
    s.sprintf("2 3 0 1 %d %d -1 0 46 0.0 0 0 0 0 0 5\n", color, color);
    s_XFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d %d %d\n", x,
              y - s_XFigSymbolWidth * 13 / 20, x + s_XFigSymbolWidth * 13 / 20,
              y, x, y + s_XFigSymbolWidth * 13 / 20,
              x - s_XFigSymbolWidth * 13 / 20, y, x,
              y - s_XFigSymbolWidth * 13 / 20);
    s_XFigStream << s;
}

static void xfig_print_diamond2(int x, int y, int color)
{
    if (!s_XFigFile)
        return;

    if (s_xfig_line_busy)
        xfig_line_finish();

    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

    x *= 1200;
    x /= s_XFigResolution;
    y *= 1200;
    y /= s_XFigResolution;

    // structure info: see xfig_print_box
    // sub-type = 3 (polygon)

    QString s;
    s.sprintf("2 3 0 1 %d %d -1 0 -1 0.0 0 0 0 0 0 5\n", color, -1);
    s_XFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d %d %d\n", x,
              y - s_XFigSymbolWidth * 13 / 20, x + s_XFigSymbolWidth * 13 / 20,
              y, x, y + s_XFigSymbolWidth * 13 / 20,
              x - s_XFigSymbolWidth * 13 / 20, y, x,
              y - s_XFigSymbolWidth * 13 / 20);
    s_XFigStream << s;
}

static void xfig_print_triangle(int x, int y, int color)
{
    if (!s_XFigFile)
        return;

    if (s_xfig_line_busy)
        xfig_line_finish();

    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

    x *= 1200;
    x /= s_XFigResolution;
    y *= 1200;
    y /= s_XFigResolution;

    // structure info: see xfig_print_box (except that here: npoints = 4 instead
    // of 5)
    // sub-type = 3 (polygon)

    QString s;
    // TODO check this -1 (is 0 in p5)
    s.sprintf("2 3 0 1 %d %d -1 0 46 0.0 0 0 0 0 0 4\n", color, color);
    s_XFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d\n", x + s_XFigSymbolWidth * 12 / 20,
              y + s_XFigSymbolWidth * 12 / 20, x,
              y - s_XFigSymbolWidth * 12 / 20, x - s_XFigSymbolWidth * 12 / 20,
              y + s_XFigSymbolWidth * 12 / 20, x + s_XFigSymbolWidth * 12 / 20,
              y + s_XFigSymbolWidth * 12 / 20);
    s_XFigStream << s;
}

static void xfig_print_triangle2(int x, int y, int color)
{
    if (!s_XFigFile)
        return;

    if (s_xfig_line_busy)
        xfig_line_finish();

    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

    x *= 1200;
    x /= s_XFigResolution;
    y *= 1200;
    y /= s_XFigResolution;

    // structure info: see xfig_print_box (except that here: npoints = 4 instead
    // of 5)
    // sub-type = 3 (polygon)

    QString s;
    s.sprintf("2 3 0 1 %d %d -1 0 -1 0.0 0 0 0 0 0 4\n", color, -1);
    s_XFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d\n", x + s_XFigSymbolWidth * 12 / 20,
              y + s_XFigSymbolWidth * 12 / 20, x,
              y - s_XFigSymbolWidth * 12 / 20, x - s_XFigSymbolWidth * 12 / 20,
              y + s_XFigSymbolWidth * 12 / 20, x + s_XFigSymbolWidth * 12 / 20,
              y + s_XFigSymbolWidth * 12 / 20);
    s_XFigStream << s;
}

static void xfig_print_cross(int x, int y, int color)
{
    if (!s_XFigFile)
        return;

    if (s_xfig_line_busy)
        xfig_line_finish();

    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

    x *= 1200;
    x /= s_XFigResolution;
    y *= 1200;
    y /= s_XFigResolution;

    // cross is made with linewidth * 2.6

    int lw{s_XFigLineWidth};
    lw *= 26;
    lw += 10;
    lw /= 20;

    QString s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x - s_XFigSymbolWidth / 2, y - s_XFigSymbolWidth / 2,
              x + s_XFigSymbolWidth / 2, y + s_XFigSymbolWidth / 2);
    s_XFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x + s_XFigSymbolWidth / 2, y - s_XFigSymbolWidth / 2,
              x - s_XFigSymbolWidth / 2, y + s_XFigSymbolWidth / 2);
    s_XFigStream << s;
}

static void xfig_print_cross2(int x, int y, int color)
{
    if (!s_XFigFile)
        return;

    if (s_xfig_line_busy)
        xfig_line_finish();

    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

    x *= 1200;
    x /= s_XFigResolution;
    y *= 1200;
    y /= s_XFigResolution;

    // cross is made with linewidth * 2.6

    int lw{s_XFigLineWidth};
    lw *= 26;
    lw += 10;
    lw /= 20;

    QString s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x - s_XFigSymbolWidth / 3, y - s_XFigSymbolWidth / 3,
              x + s_XFigSymbolWidth / 3, y + s_XFigSymbolWidth / 3);
    s_XFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x + s_XFigSymbolWidth / 3, y - s_XFigSymbolWidth / 3,
              x - s_XFigSymbolWidth / 3, y + s_XFigSymbolWidth / 3);
    s_XFigStream << s;
}

void xfig_print_doublecross(int x, int y, int color)
{
    if (!s_XFigFile)
        return;

    if (s_xfig_line_busy)
        xfig_line_finish();

    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

    x *= 1200;
    x /= s_XFigResolution;
    y *= 1200;
    y /= s_XFigResolution;

    // cross is made with linewidth * 2.6

    int lw{s_XFigLineWidth};
    lw *= 13;
    lw += 10;
    lw /= 20;

    QString s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x - s_XFigSymbolWidth / 2, y - s_XFigSymbolWidth / 2,
              x + s_XFigSymbolWidth / 2, y + s_XFigSymbolWidth / 2);
    s_XFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x + s_XFigSymbolWidth / 2, y - s_XFigSymbolWidth / 2,
              x - s_XFigSymbolWidth / 2, y + s_XFigSymbolWidth / 2);
    s_XFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x, y - (s_XFigSymbolWidth * 3) / 4, x,
              y + (s_XFigSymbolWidth * 3) / 4);
    s_XFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x + (s_XFigSymbolWidth * 3) / 4, y,
              x - (s_XFigSymbolWidth * 3) / 4, y);
    s_XFigStream << s;
}

static void xfig_print_line(double _x0, double _y0, double _x1, double _y1,
                            int color)
{
    if (!s_XFigFile)
        return;

    _x0 *= 1200;
    _x0 /= s_XFigResolution;
    _y0 *= 1200;
    _y0 /= s_XFigResolution;

    _x1 *= 1200;
    _x1 /= s_XFigResolution;
    _y1 *= 1200;
    _y1 /= s_XFigResolution;

    color = printColorTable(color);
    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

    int x0{(int)_x0};
    int y0{(int)_y0};
    int x1{(int)_x1};
    int y1{(int)_y1};
    if (x0 == x1 && y0 == y1)
        return;

    if (s_xfig_line_busy && s_xfig_line_lastx == x0 &&
        s_xfig_line_lasty == y0 && s_xfig_line_color == color) {
        xfig_line_continue(x1, y1);
    } else {
        if (s_xfig_line_busy)
            xfig_line_finish();

        xfig_line_start(x0, y0, x1, y1, color);
    }
}

static void xfig_print_elips(double x0, double y0, double a, double b,
                             int color, bool dotted,
                             const std::vector<P4POLYLINES> &ellipse)
{
    if (!s_XFigFile)
        return;

    if (s_xfig_line_busy)
        xfig_line_finish();

    x0 *= 1200;
    x0 /= s_XFigResolution;
    y0 *= 1200;
    y0 /= s_XFigResolution;

    a *= 1200;
    a /= s_XFigResolution;
    b *= 1200;
    b /= s_XFigResolution;

    if (x0 - a >= 0 && x0 + a < s_XFigW && y0 - b >= 0 && y0 + b < s_XFigH) {
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
            linestyle = 1;  // dashed
            // style_val = on/off length (in 1/80 of inch) = 8*linestyle
            styleval = s_XFigLineWidth * 4;
        } else {
            linestyle = 0;
            styleval = 0.0;
        }

        QString s;
        s.sprintf("1 1 %d %d %d %d 50 0 -1 %g 1 0.0 %d %d %d %d %d %d %d %d\n",
                  linestyle, s_XFigLineWidth / 2, printColorTable(color),
                  printColorTable(color), (float)styleval, (int)x0, (int)y0,
                  (int)a, (int)b, (int)x0, (int)y0, ((int)x0) + ((int)a),
                  (int)y0);
        s_XFigStream << s;
    } else {
        // ellipse is only partially visible, so emulate with polygon.
        for (auto const &it : ellipse) {
            xfig_print_line(it.x1, it.y1, it.x2, it.y2, color);
        }
    }
}

static void xfig_line_start(int x0, int y0, int x1, int y1, int color)
{
    s_xfig_line_busy = true;
    s_xfig_line_points[0] = x0;
    s_xfig_line_points[1] = y0;
    s_xfig_line_points[2] = x1;
    s_xfig_line_points[3] = y1;
    s_xfig_line_numpoints = 2;
    s_xfig_line_lastx = x1;
    s_xfig_line_lasty = y1;
    s_xfig_line_color = color;
}

static void xfig_line_continue(int x1, int y1)
{
    if (s_xfig_line_numpoints < XFIG_LINE_MAXPOINTS) {
        s_xfig_line_points[2 * s_xfig_line_numpoints] = s_xfig_line_lastx = x1;
        s_xfig_line_points[2 * s_xfig_line_numpoints + 1] = s_xfig_line_lasty =
            y1;
        s_xfig_line_numpoints++;
    } else {
        int x0{s_xfig_line_lastx};
        int y0{s_xfig_line_lasty};
        int color{s_xfig_line_color};

        xfig_line_finish();
        xfig_line_start(x0, y0, x1, y1, color);
    }
}

static void xfig_line_finish(void)
{
    if (!s_xfig_line_busy)
        return;

    s_xfig_line_busy = false;

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

    if (s_XFigFile) {
        QString s;
        s.sprintf("2 1 0 %d %d 7 50 0 -1 0.0 0 1 -1 0 0 %d\n   ",
                  s_XFigLineWidth / 2, s_xfig_line_color,
                  s_xfig_line_numpoints);
        s_XFigStream << s;
        for (int i = 0; i < s_xfig_line_numpoints; i += 8) {
            k = 2 * (s_xfig_line_numpoints - i);
            if (k >= 16)
                k = 16;

            for (int j = 0; j < k; j++) {
                s.sprintf(" %d", s_xfig_line_points[2 * i + j]);
                s_XFigStream << s;
            }

            if (i + 8 < s_xfig_line_numpoints)
                s_XFigStream << "\n   ";
            else
                s_XFigStream << "\n";
        }
    }
}

static void xfig_print_point(double _x0, double _y0, int color)
{
    if (s_xfig_line_busy)
        xfig_line_finish();

    _x0 *= 1200;
    _x0 /= s_XFigResolution;
    _y0 *= 1200;
    _y0 /= s_XFigResolution;

    if (!s_XFigFile)
        return;

    color = printColorTable(color);

    if (s_XFigBlackWhitePrint)
        color = printColorTable(bgColours::CFOREGROUND);

    int x0{(int)_x0};
    int y0{(int)_y0};

    if (x0 == s_LastXfigX0 && y0 == s_LastXfigY0 && color == s_LastXfigcolor)
        return;

    s_LastXfigX0 = x0;
    s_LastXfigY0 = y0;
    s_LastXfigcolor = color;

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
              color, color, x0, y0, s_XFigRealLineWidth / 2,
              s_XFigRealLineWidth / 2, x0, y0, x0 + s_XFigRealLineWidth, y0);
    s_XFigStream << s;
}

static void xfig_print_saddle(double x, double y)
{
    xfig_print_box((int)x, (int)y, printColorTable(CSADDLE));
}

static void xfig_print_stablenode(double x, double y)
{
    xfig_print_box((int)x, (int)y, printColorTable(CNODE_S));
}
static void xfig_print_unstablenode(double x, double y)
{
    xfig_print_box((int)x, (int)y, printColorTable(CNODE_U));
}

static void xfig_print_stableweakfocus(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable(CWEAK_FOCUS_S));
}

static void xfig_print_unstableweakfocus(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable(CWEAK_FOCUS_U));
}

static void xfig_print_weakfocus(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable(CWEAK_FOCUS));
}

static void xfig_print_center(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable(CCENTER));
}

static void xfig_print_stablestrongfocus(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable(CSTRONG_FOCUS_S));
}

static void xfig_print_unstablestrongfocus(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable(CSTRONG_FOCUS_U));
}

static void xfig_print_sesaddle(double x, double y)
{
    xfig_print_triangle((int)x, (int)y, printColorTable(CSADDLE));
}

static void xfig_print_sesaddlenode(double x, double y)
{
    xfig_print_triangle((int)x, (int)y, printColorTable(CSADDLE_NODE));
}

static void xfig_print_sestablenode(double x, double y)
{
    xfig_print_triangle((int)x, (int)y, printColorTable(CNODE_S));
}

static void xfig_print_seunstablenode(double x, double y)
{
    xfig_print_triangle((int)x, (int)y, printColorTable(CNODE_U));
}

static void xfig_print_degen(double x, double y)
{
    xfig_print_cross((int)x, (int)y, printColorTable(CDEGEN));
}

static void xfig_print_virtualsaddle(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_box2((int)x, (int)y, printColorTable(CSADDLE));
}

static void xfig_print_virtualstablenode(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_box2((int)x, (int)y, printColorTable(CNODE_S));
}
static void xfig_print_virtualunstablenode(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_box2((int)x, (int)y, printColorTable(CNODE_U));
}

static void xfig_print_virtualstableweakfocus(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_diamond2((int)x, (int)y, printColorTable(CWEAK_FOCUS_S));
}

static void xfig_print_virtualunstableweakfocus(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_diamond2((int)x, (int)y, printColorTable(CWEAK_FOCUS_U));
}

static void xfig_print_virtualweakfocus(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_diamond2((int)x, (int)y, printColorTable(CWEAK_FOCUS));
}

static void xfig_print_virtualcenter(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_diamond2((int)x, (int)y, printColorTable(CCENTER));
}

static void xfig_print_virtualstablestrongfocus(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_diamond2((int)x, (int)y, printColorTable(CSTRONG_FOCUS_S));
}

static void xfig_print_virtualunstablestrongfocus(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_diamond2((int)x, (int)y, printColorTable(CSTRONG_FOCUS_U));
}

static void xfig_print_virtualsesaddle(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_triangle2((int)x, (int)y, printColorTable(CSADDLE));
}

static void xfig_print_virtualsesaddlenode(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_triangle2((int)x, (int)y, printColorTable(CSADDLE_NODE));
}

static void xfig_print_virtualsestablenode(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_triangle2((int)x, (int)y, printColorTable(CNODE_S));
}

static void xfig_print_virtualseunstablenode(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_triangle2((int)x, (int)y, printColorTable(CNODE_U));
}

static void xfig_print_virtualdegen(double x, double y)
{
    if (g_VFResults.plotVirtualSingularities_)
        xfig_print_cross2((int)x, (int)y, printColorTable(CDEGEN));
}

void xfig_print_coinciding(double x, double y)
{
    xfig_print_doublecross((int)x, (int)y, printColorTable(CDEGEN);
}

// ---------------------------------------------------------------------------------------

void prepareXFigPrinting(int w, int h, bool iszoom, bool isblackwhite,
                         int resolution, int linewidth, int symbolwidth)
{
    QString s;

    s_XFigBlackWhitePrint = isblackwhite;
    s_XFigRealLineWidth = linewidth;
    s_XFigLineWidth = linewidth;
    s_XFigSymbolWidth = symbolwidth;
    s_XFigResolution = resolution;
    // since XFIG always reads the file at 1200 DPI, we rescale points in this
    // module. by multiplying x and y coordinates with (1200 / resolution), just
    // before coordinates are printed.

    s_XFigSymbolWidth *= 1200;
    s_XFigSymbolWidth /= resolution;
    s_XFigLineWidth *= 1200;
    s_XFigLineWidth /= resolution;
    s_XFigRealLineWidth *= 1200;
    s_XFigRealLineWidth /= resolution;

    // make sure it is even, and nonzero:
    s_XFigRealLineWidth /= 2;
    if (s_XFigRealLineWidth == 0)
        s_XFigRealLineWidth = 1;
    s_XFigRealLineWidth *= 2;

    // XFIG Line width is always at 80 DPI, regardless of resolution
    s_XFigLineWidth *= 80;
    s_XFigLineWidth =
        (s_XFigLineWidth + (resolution / 2)) / resolution;  // divide + round up

    // make sure it is even, and nonzero
    s_XFigLineWidth /= 2;
    if (s_XFigLineWidth == 0)
        s_XFigLineWidth = 1;  // minimal line width = 1
    s_XFigLineWidth *= 2;

    s = g_ThisVF->getbarefilename() + ".fig";

    s_XFigFile.reset(new QFile(s));
    if (s_XFigFile->open(QIODevice::WriteOnly)) {
        s_XFigStream.setDevice(s_XFigFile);
    } else {
        s_XFigFile.reset();
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

    s_LastXfigcolor = -1;

    s_xfig_line_busy = false;
    s_xfig_line_points =
        std::make_unique<int[]>(new int[XFIG_LINE_MAXPOINTS * 2]);

    s_XFigW = w * 1200;
    s_XFigW /= resolution;
    s_XFigH = h * 1200;
    s_XFigH /= resolution;

    if (s_XFigFile) {
        s_XFigStream
            << "#FIG 3.1\n"  // XFIG version 3.1 file format
               "Portrait\n"  // portrait orientation
               "Center\n"    // center on page
               "Inches\n"    // use inches (XFIG makes rounding errors with
                             // centimeters)
               "1200 2\n";   // unused in XFIG???
        if (!bgColours::PRINT_WHITE_BG) {
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
            s.sprintf(
                "2 2 0 1 0 7 999 -1 0 0.0000000 0 0 0 0 0 5\n"
                "    %d %d %d %d %d %d %d %d %d %d\n",
                0, 0, s_XFigW, 0, s_XFigW, s_XFigH, 0, s_XFigH, 0, 0);
            s_XFigStream << s;
        }
        if (g_VFResults.typeofview_ == TYPEOFVIEW_PLANE || iszoom) {
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
            s.sprintf(
                "2 2 0 1 0 7 998 0 -1 0.0000000 0 0 0 0 0 5\n"
                "    %d %d %d %d %d %d %d %d %d %d\n",
                0, 0, s_XFigW, 0, s_XFigW, s_XFigH, 0, s_XFigH, 0, 0);
            s_XFigStream << s;
        }
    }
}

void finishXFigPrinting(void)
{
    if (s_XFigFile) {
        s_XFigStream.flush();
        s_XFigStream.setDevice(nullptr);
        s_XFigFile->close();
        s_XFigFile.reset();
    }

    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;

    if (s_xfig_line_points != nullptr) {
        delete[] s_xfig_line_points;
        s_xfig_line_points = nullptr;
    }
}
