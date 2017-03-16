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

#include "print_xfig.h"

#include "custom.h"
#include "file_vf.h"
#include "plot_tools.h"
#include "print_bitmap.h"
#include "print_points.h"

#include <QFile>
#include <QTextStream>

static QFile *XFigFile = nullptr;
static QTextStream XFigStream;

static bool XFigBlackWhitePrint = true;
static int XFigSymbolWidth = 0;
static int XFigLineWidth = 0;     // linewidth at 80 DPI (for xfig only)
static int XFigRealLineWidth = 0; // linewidth at chosen resolution
static int XFigResolution = 0;
static int XFigW = 0;
static int XFigH = 0;

static int LastXfigX0 = 0;
static int LastXfigY0 = 0;
static int LastXfigcolor = -1;

static bool xfig_line_busy = false;
static int xfig_line_lastx = 0;
static int xfig_line_lasty = 0;
static int xfig_line_color = 0;
static int xfig_line_numpoints = 0;
static int *xfig_line_points = nullptr;

static void xfig_line_start(int x0, int y0, int x1, int y1, int color);
static void xfig_line_continue(int x1, int y1);
static void xfig_line_finish(void);

// ---------------------------------------------------------------------------------------

static void xfig_print_comment(QString s)
{
    if (xfig_line_busy)
        xfig_line_finish();

    if (XFigFile != nullptr) {
        XFigStream << "# " << s << "\n";
    }
}

static void xfig_print_box(int x, int y, int color)
{
    if (XFigFile == nullptr)
        return;

    if (xfig_line_busy)
        xfig_line_finish();

    x *= 1200;
    x /= XFigResolution;
    y *= 1200;
    y /= XFigResolution;

    if (XFigBlackWhitePrint)
        color = printColorTable[CFOREGROUND];

    /*
        object type     2   (=polyline)
        subtype         2   (=box)
        linestyle       0   (=solid)
        thickness       1   (in 1/80 of an inch)
        pencolor        color
        fillcolor       color
        depth           -1  (precedence over other objects !!!)
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
    s.sprintf("2 2 0 1 %d %d -1 0 46 0.0 0 0 0 0 0 5\n", color, color);
    XFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d %d %d\n", x + XFigSymbolWidth / 2,
              y - XFigSymbolWidth / 2, x + XFigSymbolWidth / 2,
              y + XFigSymbolWidth / 2, x - XFigSymbolWidth / 2,
              y + XFigSymbolWidth / 2, x - XFigSymbolWidth / 2,
              y - XFigSymbolWidth / 2, x + XFigSymbolWidth / 2,
              y - XFigSymbolWidth / 2);
    XFigStream << s;
}

static void xfig_print_diamond(int x, int y, int color)
{
    if (XFigFile == nullptr)
        return;

    if (xfig_line_busy)
        xfig_line_finish();

    if (XFigBlackWhitePrint)
        color = printColorTable[CFOREGROUND];

    x *= 1200;
    x /= XFigResolution;
    y *= 1200;
    y /= XFigResolution;

    // structure info: see xfig_print_box
    // sub-type = 3 (polygon)

    QString s;
    s.sprintf("2 3 0 1 %d %d -1 0 46 0.0 0 0 0 0 0 5\n", color, color);
    XFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d %d %d\n", x,
              y - XFigSymbolWidth * 13 / 20, x + XFigSymbolWidth * 13 / 20, y,
              x, y + XFigSymbolWidth * 13 / 20, x - XFigSymbolWidth * 13 / 20,
              y, x, y - XFigSymbolWidth * 13 / 20);
    XFigStream << s;
}

static void xfig_print_triangle(int x, int y, int color)
{
    if (XFigFile == nullptr)
        return;

    if (xfig_line_busy)
        xfig_line_finish();

    if (XFigBlackWhitePrint)
        color = printColorTable[CFOREGROUND];

    x *= 1200;
    x /= XFigResolution;
    y *= 1200;
    y /= XFigResolution;

    // structure info: see xfig_print_box (except that here: npoints = 4 instead
    // of 5)
    // sub-type = 3 (polygon)

    QString s;
    s.sprintf("2 3 0 1 %d %d -1 0 46 0.0 0 0 0 0 0 4\n", color, color);
    XFigStream << s;
    s.sprintf("    %d %d %d %d %d %d %d %d\n", x + XFigSymbolWidth * 12 / 20,
              y + XFigSymbolWidth * 12 / 20, x, y - XFigSymbolWidth * 12 / 20,
              x - XFigSymbolWidth * 12 / 20, y + XFigSymbolWidth * 12 / 20,
              x + XFigSymbolWidth * 12 / 20, y + XFigSymbolWidth * 12 / 20);
    XFigStream << s;
}

static void xfig_print_cross(int x, int y, int color)
{
    int lw;

    if (XFigFile == nullptr)
        return;

    if (xfig_line_busy)
        xfig_line_finish();

    if (XFigBlackWhitePrint)
        color = printColorTable[CFOREGROUND];

    x *= 1200;
    x /= XFigResolution;
    y *= 1200;
    y /= XFigResolution;

    // cross is made with linewidth * 2.6

    lw = XFigLineWidth;
    lw *= 26;
    lw += 10;
    lw /= 20;

    QString s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x - XFigSymbolWidth / 2, y - XFigSymbolWidth / 2,
              x + XFigSymbolWidth / 2, y + XFigSymbolWidth / 2);
    XFigStream << s;
    s.sprintf("2 1 0 %d %d 7 0 0 -1 0.0 0 0 0 0 0 2\n    %d %d %d %d\n", lw,
              color, x + XFigSymbolWidth / 2, y - XFigSymbolWidth / 2,
              x - XFigSymbolWidth / 2, y + XFigSymbolWidth / 2);
    XFigStream << s;
}

static void xfig_print_line(double _x0, double _y0, double _x1, double _y1,
                            int color)
{
    int x0, y0, x1, y1;

    _x0 *= 1200;
    _x0 /= XFigResolution;
    _y0 *= 1200;
    _y0 /= XFigResolution;

    _x1 *= 1200;
    _x1 /= XFigResolution;
    _y1 *= 1200;
    _y1 /= XFigResolution;

    if (XFigFile == nullptr)
        return;

    color = printColorTable[color];

    if (XFigBlackWhitePrint)
        color = printColorTable[CFOREGROUND];

    x0 = (int)_x0;
    y0 = (int)_y0;
    x1 = (int)_x1;
    y1 = (int)_y1;
    if (x0 == x1 && y0 == y1)
        return;

    if (xfig_line_busy && xfig_line_lastx == x0 && xfig_line_lasty == y0 &&
        xfig_line_color == color) {
        xfig_line_continue(x1, y1);
    } else {
        if (xfig_line_busy)
            xfig_line_finish();

        xfig_line_start(x0, y0, x1, y1, color);
    }
}

static void xfig_print_elips(double x0, double y0, double a, double b,
                             int color, bool dotted,
                             struct P4POLYLINES *ellipse)
{
    if (XFigFile == nullptr)
        return;

    if (xfig_line_busy)
        xfig_line_finish();

    x0 *= 1200;
    x0 /= XFigResolution;
    y0 *= 1200;
    y0 /= XFigResolution;

    a *= 1200;
    a /= XFigResolution;
    b *= 1200;
    b /= XFigResolution;

    if (x0 - a >= 0 && x0 + a < XFigW && y0 - b >= 0 && y0 + b < XFigH) {
        // full ellipse is visible

        /*
            object type     1   (=ellipse)
            subtype         3   (=circle defined by radius)
            linestyle       0 or 1  (=solid or dashed)
            thickness       ... (in 1/80 of an inch)
            pencolor        color
            fillcolor       color
            depth           0   (no precedence over other objects)
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
            styleval = XFigLineWidth * 4;
        } else {
            linestyle = 0;
            styleval = 0.0;
        }

        QString s;
        s.sprintf("1 1 %d %d %d %d 0 0 -1 %g 1 0.0 %d %d %d %d %d %d %d %d\n",
                  linestyle, XFigLineWidth / 2, printColorTable[color],
                  printColorTable[color], (float)styleval, (int)x0, (int)y0,
                  (int)a, (int)b, (int)x0, (int)y0, ((int)x0) + ((int)a),
                  (int)y0);
        XFigStream << s;
    } else {
        // ellipse is only partially visible, so emulate with polygon.

        while (ellipse != nullptr) {
            xfig_print_line(ellipse->x1, ellipse->y1, ellipse->x2, ellipse->y2,
                            color);
            ellipse = ellipse->next;
        }
    }
}

static void xfig_line_start(int x0, int y0, int x1, int y1, int color)
{
    xfig_line_busy = true;
    xfig_line_points[0] = x0;
    xfig_line_points[1] = y0;
    xfig_line_points[2] = x1;
    xfig_line_points[3] = y1;
    xfig_line_numpoints = 2;
    xfig_line_lastx = x1;
    xfig_line_lasty = y1;
    xfig_line_color = color;
}

static void xfig_line_continue(int x1, int y1)
{
    if (xfig_line_numpoints < XFIG_LINE_MAXPOINTS) {
        xfig_line_points[2 * xfig_line_numpoints] = xfig_line_lastx = x1;
        xfig_line_points[2 * xfig_line_numpoints + 1] = xfig_line_lasty = y1;
        xfig_line_numpoints++;
    } else {
        int x0;
        int y0;
        int color;

        x0 = xfig_line_lastx;
        y0 = xfig_line_lasty;
        color = xfig_line_color;

        xfig_line_finish();
        xfig_line_start(x0, y0, x1, y1, color);
    }
}

static void xfig_line_finish(void)
{
    int i, j, k;

    if (!xfig_line_busy)
        return;

    xfig_line_busy = false;

    /*
        object type     2   (=polyline)
        subtype         1   (=polyline)
        linestyle       0   (=solid)
        thickness       thickness   (in 1/80 of an inch)
        pencolor        color
        fillcolor       7   (=white)
        depth           0   (no precedence over other objects)
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

    if (XFigFile != nullptr) {
        QString s;
        s.sprintf("2 1 0 %d %d  7 0 0 -1 0.0 0 1 -1 0 0 %d\n   ",
                  XFigLineWidth / 2, xfig_line_color, xfig_line_numpoints);
        XFigStream << s;
        for (i = 0; i < xfig_line_numpoints; i += 8) {
            k = 2 * (xfig_line_numpoints - i);
            if (k >= 16)
                k = 16;

            for (j = 0; j < k; j++) {
                s.sprintf(" %d", xfig_line_points[2 * i + j]);
                XFigStream << s;
            }

            if (i + 8 < xfig_line_numpoints)
                XFigStream << "\n   ";
            else
                XFigStream << "\n";
        }
    }
}

static void xfig_print_point(double _x0, double _y0, int color)
{
    int x0, y0;

    if (xfig_line_busy)
        xfig_line_finish();

    _x0 *= 1200;
    _x0 /= XFigResolution;
    _y0 *= 1200;
    _y0 /= XFigResolution;

    if (XFigFile == nullptr)
        return;

    color = printColorTable[color];

    if (XFigBlackWhitePrint)
        color = printColorTable[CFOREGROUND];

    x0 = (int)_x0;
    y0 = (int)_y0;

    if (x0 == LastXfigX0 && y0 == LastXfigY0 && color == LastXfigcolor)
        return;

    LastXfigX0 = x0;
    LastXfigY0 = y0;
    LastXfigcolor = color;

    /*
        object type     1   (=ellipse)
        subtype         3   (=circle defined by radius)
        linestyle       0   (=solid)
        thickness       1   (in 1/80 of an inch)
        pencolor        color
        fillcolor       color
        depth           0   (no precedence over other objects)
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
    s.sprintf("1 3 0 1 %d %d 0 0 46 0.0 1 0.0 %d %d %d %d %d %d %d %d\n", color,
              color, x0, y0, XFigRealLineWidth / 2, XFigRealLineWidth / 2, x0,
              y0, x0 + XFigRealLineWidth, y0);
    XFigStream << s;
}

static void xfig_print_saddle(double x, double y)
{
    xfig_print_box((int)x, (int)y, printColorTable[CSADDLE]);
}

static void xfig_print_stablenode(double x, double y)
{
    xfig_print_box((int)x, (int)y, printColorTable[CNODE_S]);
}
static void xfig_print_unstablenode(double x, double y)
{
    xfig_print_box((int)x, (int)y, printColorTable[CNODE_U]);
}

static void xfig_print_stableweakfocus(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable[CWEAK_FOCUS_S]);
}

static void xfig_print_unstableweakfocus(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable[CWEAK_FOCUS_U]);
}

static void xfig_print_weakfocus(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable[CWEAK_FOCUS]);
}

static void xfig_print_center(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable[CCENTER]);
}

static void xfig_print_stablestrongfocus(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable[CSTRONG_FOCUS_S]);
}

static void xfig_print_unstablestrongfocus(double x, double y)
{
    xfig_print_diamond((int)x, (int)y, printColorTable[CSTRONG_FOCUS_U]);
}

static void xfig_print_sesaddle(double x, double y)
{
    xfig_print_triangle((int)x, (int)y, printColorTable[CSADDLE]);
}

static void xfig_print_sesaddlenode(double x, double y)
{
    xfig_print_triangle((int)x, (int)y, printColorTable[CSADDLE_NODE]);
}

static void xfig_print_sestablenode(double x, double y)
{
    xfig_print_triangle((int)x, (int)y, printColorTable[CNODE_S]);
}

static void xfig_print_seunstablenode(double x, double y)
{
    xfig_print_triangle((int)x, (int)y, printColorTable[CNODE_U]);
}

static void xfig_print_degen(double x, double y)
{
    xfig_print_cross((int)x, (int)y, printColorTable[CDEGEN]);
}

// ---------------------------------------------------------------------------------------

void prepareXFigPrinting(int w, int h, bool iszoom, bool isblackwhite,
                         int resolution, int linewidth, int symbolwidth)
{
    QString s;

    XFigBlackWhitePrint = isblackwhite;
    XFigRealLineWidth = linewidth;
    XFigLineWidth = linewidth;
    XFigSymbolWidth = symbolwidth;
    XFigResolution = resolution;
    // since XFIG always reads the file at 1200 DPI, we rescale points in this
    // module.
    // by multiplying x and y coordinates with (1200 / resolution), just before
    // coordinates are printed.

    XFigSymbolWidth *= 1200;
    XFigSymbolWidth /= resolution;
    XFigLineWidth *= 1200;
    XFigLineWidth /= resolution;
    XFigRealLineWidth *= 1200;
    XFigRealLineWidth /= resolution;

    // make sure it is even, and nonzero:
    XFigRealLineWidth /= 2;
    if (XFigRealLineWidth == 0)
        XFigRealLineWidth = 1;
    XFigRealLineWidth *= 2;

    // XFIG Line width is always at 80 DPI, regardless of resolution
    XFigLineWidth *= 80;
    XFigLineWidth =
        (XFigLineWidth + (resolution / 2)) / resolution; // divide + round up

    // make sure it is even, and nonzero

    XFigLineWidth /= 2;
    if (XFigLineWidth == 0)
        XFigLineWidth = 1; // minimal line width = 1
    XFigLineWidth *= 2;

    s = ThisVF->getbarefilename() + ".fig";

    XFigFile = new QFile(s);
    if (XFigFile->open(QIODevice::WriteOnly)) {
        XFigStream.setDevice(XFigFile);
    } else {
        delete XFigFile;
        XFigFile = nullptr;
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

    LastXfigcolor = -1;

    xfig_line_busy = false;
    xfig_line_points = new int[XFIG_LINE_MAXPOINTS * 2];

    XFigW = w * 1200;
    XFigW /= resolution;
    XFigH = h * 1200;
    XFigH /= resolution;

    if (XFigFile != nullptr) {
        XFigStream << "#FIG 3.1\n" // XFIG version 3.1 file format
                      "Portrait\n" // portrait orientation
                      "Center\n"   // center on page
                      "Inches\n"  // use inches (XFIG makes rounding errors with
                                  // centimeters)
                      "1200 2\n"; // unused in XFIG???

        if (VFResults.typeofview == TYPEOFVIEW_PLANE || iszoom) {
            /*
                object type     2   (=polyline)
                subtype         2   (=box)
                linestyle       0   (=solid)
                thickness       1   (in 1/80 of an inch)
                pencolor        0   (=black)
                fillcolor       7   (=white)
                depth           0   (no precedence over other objects)
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
            s.sprintf("2 2 0 1 0 7 0 0 -1 0.0000000 0 0 0 0 0 5\n"
                      "    %d %d %d %d %d %d %d %d %d %d\n",
                      0, 0, XFigW, 0, XFigW, XFigH, 0, XFigH, 0, 0);
            XFigStream << s;
        }
    }
}

void finishXFigPrinting(void)
{
    if (XFigFile != nullptr) {
        XFigStream.flush();
        XFigStream.setDevice(nullptr);
        XFigFile->close();
        delete XFigFile;
        XFigFile = nullptr;
    }

    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;

    if (xfig_line_points != nullptr) {
        delete[] xfig_line_points; // free( xfig_line_points );
        xfig_line_points = nullptr;
    }
}
