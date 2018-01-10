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

#include "print_bitmap.h"

#include "custom.h"
#include "main.h"
#include "plot_tools.h"
#include "print_points.h"

#include <QBrush>
#include <QPainter>

static bool s_P4PrintBlackWhite = true;

static QPainter *s_P4PrintPainter = nullptr;

static int s_P4PrintLineWidth = 0;
static int s_P4PrintSymbolWidth = 0;
static int s_LastP4PrintX0 = 0;
static int s_LastP4PrintY0 = 0;
static int s_LastP4Printcolor = 0;

// ---------------------------------------------------------------------------------------
int printColorTable(int color)
{
    int colorTable[NUMXFIGCOLORS] = {
        WHITE, // black --> white when printing
        BLUE,   GREEN,  CYAN,   RED,   MAGENTA,
        BLACK, // yellow --> black when printing
        BLACK, // white --> black when printing
        BLUE1,  BLUE2,  BLUE3,  BLUE4, GREEN1,  GREEN2,   GREEN3,   CYAN1,
        CYAN2,  CYAN3,  RED1,   RED2,  RED3,    MAGENTA1, MAGENTA2, MAGENTA3,
        BROWN1, BROWN2, BROWN3, PINK1, PINK2,   PINK3,    PINK4,    GOLD};
    int colorTableReverse[NUMXFIGCOLORS] = {
        BLACK,  BLUE,   GREEN,  CYAN,  RED,    MAGENTA,  YELLOW,   WHITE,
        BLUE1,  BLUE2,  BLUE3,  BLUE4, GREEN1, GREEN2,   GREEN3,   CYAN1,
        CYAN2,  CYAN3,  RED1,   RED2,  RED3,   MAGENTA1, MAGENTA2, MAGENTA3,
        BROWN1, BROWN2, BROWN3, PINK1, PINK2,  PINK3,    PINK4,    GOLD};

    if (bgColours::PRINT_WHITE_BG)
        return colorTable[color];
    else
        return colorTableReverse[color];
}

static void P4Print_comment(QString s)
{
    UNUSED(s);
    // do nothing
}

static void P4Print_print_saddle(double _x, double _y)
{
    int x;
    int y;
    int color;
    x = (int)_x;
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print box:
    s_P4PrintPainter->drawRect(x - s_P4PrintSymbolWidth / 2,
                               y - s_P4PrintSymbolWidth / 2,
                               s_P4PrintSymbolWidth, s_P4PrintSymbolWidth);
}

static void P4Print_print_stablenode(double _x, double _y)
{
    int x;
    int y;
    int color;

    x = (int)_x;
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_S);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print box:
    s_P4PrintPainter->drawRect(x - s_P4PrintSymbolWidth / 2,
                               y - s_P4PrintSymbolWidth / 2,
                               s_P4PrintSymbolWidth, s_P4PrintSymbolWidth);
}

static void P4Print_print_unstablenode(double _x, double _y)
{
    int x;
    int y;
    int color;
    x = (int)_x;
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_U);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print box:
    s_P4PrintPainter->drawRect(x - s_P4PrintSymbolWidth / 2,
                               y - s_P4PrintSymbolWidth / 2,
                               s_P4PrintSymbolWidth, s_P4PrintSymbolWidth);
}

static void P4Print_print_stableweakfocus(double _x, double _y)
{
    int x;
    int y;
    int color;

    x = (int)_x;
    y = (int)_y;
    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS_S);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);

    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_unstableweakfocus(double _x, double _y)
{
    int x;
    int y;
    int color;

    x = (int)_x;
    y = (int)_y;
    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS_U);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);

    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_weakfocus(double _x, double _y)
{
    int x;
    int y;
    int color;

    x = (int)_x;
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);

    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_center(double _x, double _y)
{
    int x;
    int y;
    int color;

    x = (int)_x;
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CCENTER);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);

    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_stablestrongfocus(double _x, double _y)
{
    int x;
    int y;
    int color;

    x = (int)_x;
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSTRONG_FOCUS_S);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);

    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_unstablestrongfocus(double _x, double _y)
{
    int x, y;
    int color;

    x = (int)_x; // seems to be necessary
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSTRONG_FOCUS_U);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:

    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);

    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_sesaddle(double _x, double _y)
{
    int x;
    int y;
    int color;

    x = (int)_x;
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:

    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_sesaddlenode(double _x, double _y)
{
    int x, y;
    int color;

    x = (int)_x; // seems to be necessary
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE_NODE);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));
    // print triangle:

    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_sestablenode(double _x, double _y)
{
    int x, y;
    int color;

    x = (int)_x; // seems to be necessary
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_S);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:

    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_seunstablenode(double _x, double _y)
{
    int x, y;
    int color;

    x = (int)_x; // seems to be necessary
    y = (int)_y;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_U);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:

    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_degen(double _x, double _y)
{
    int i, x, y;
    int color;

    i = s_P4PrintLineWidth * 26 / 20;
    //  if( i > s_P4PrintSymbolWidth/2 )
    //      i = s_P4PrintSymbolWidth/4;

    x = (int)_x;
    y = (int)_y;

    // print cross:

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CDEGEN);

    QPen p = QPen(QXFIGCOLOR(color), i);
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->drawLine(
        x - s_P4PrintSymbolWidth / 2, y - s_P4PrintSymbolWidth / 2,
        x + s_P4PrintSymbolWidth / 2, y + s_P4PrintSymbolWidth / 2);
    s_P4PrintPainter->drawLine(
        x + s_P4PrintSymbolWidth / 2, y - s_P4PrintSymbolWidth / 2,
        x - s_P4PrintSymbolWidth / 2, y + s_P4PrintSymbolWidth / 2);
}

static void P4Print_print_elips(double x0, double y0, double a, double b,
                                int color, bool dotted,
                                struct P4POLYLINES *ellipse)
{
    int color2;
    if (s_P4PrintBlackWhite)
        color2 = printColorTable(bgColours::CFOREGROUND);
    else
        color2 = printColorTable(color);

    QPen p = QPen(QXFIGCOLOR(color2), s_P4PrintLineWidth);
    p.setCapStyle(Qt::RoundCap);
    s_P4PrintPainter->setPen(p);

    // we do not use the (x0,y0,a,b,dotted) parameters.
    // Instead, we use the "precompiled" ellipse parameter.  Here, a list of
    // lines is computed
    // that approximates the ellipse.

    UNUSED(x0);
    UNUSED(y0);
    UNUSED(a);
    UNUSED(b);
    UNUSED(dotted);
    while (ellipse != nullptr) {
        s_P4PrintPainter->drawLine((int)(ellipse->x1), (int)(ellipse->y1),
                                   (int)(ellipse->x2), (int)(ellipse->y2));
        ellipse = ellipse->next;
    }
}

static void P4Print_print_line(double _x0, double _y0, double _x1, double _y1,
                               int color)
{
    int x0, y0, x1, y1;

    color = printColorTable(color);
    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);

    x0 = (int)_x0;
    x1 = (int)_x1;
    y0 = (int)_y0;
    y1 = (int)_y1;

    if (x0 == x1 && y0 == y1)
        return;

    QPen p = QPen(QXFIGCOLOR(color), s_P4PrintLineWidth);
    p.setCapStyle(Qt::RoundCap);

    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->drawLine(x0, y0, x1, y1);
}

static void P4Print_print_point(double _x0, double _y0, int color)
{
    int x0;
    int y0;

    if (s_P4PrintBlackWhite)
        color = bgColours::CFOREGROUND;
    color = printColorTable(color);

    x0 = (int)_x0;
    y0 = (int)_y0;

    if (x0 == s_LastP4PrintX0 && y0 == s_LastP4PrintY0 &&
        color == s_LastP4Printcolor)
        return;

    s_LastP4PrintX0 = x0;
    s_LastP4PrintY0 = y0;
    s_LastP4Printcolor = color;

    QPen p = QPen(QXFIGCOLOR(color), s_P4PrintLineWidth);
    p.setCapStyle(Qt::RoundCap);

    s_P4PrintPainter->setPen(p);
    if (s_P4PrintLineWidth > 1) {
        s_P4PrintPainter->drawLine(x0, y0, x0, y0);
    } else {
        s_P4PrintPainter->drawPoint(x0, y0);
    }
}

// ---------------------------------------------------------------------------------------

void prepareP4Printing(int w, int h, bool isblackwhite, QPainter *p4paint,
                       int linewidth, int symbolwidth)
{
    QString s;

    s_P4PrintBlackWhite = isblackwhite;
    s_P4PrintPainter = p4paint;
    s_P4PrintLineWidth = linewidth;
    s_P4PrintSymbolWidth = symbolwidth;

    plot_l = spherePrintLine;
    plot_p = spherePrintPoint;

    print_saddle = P4Print_print_saddle;
    print_stablenode = P4Print_print_stablenode;
    print_unstablenode = P4Print_print_unstablenode;
    print_stableweakfocus = P4Print_print_stableweakfocus;
    print_unstableweakfocus = P4Print_print_unstableweakfocus;
    print_weakfocus = P4Print_print_weakfocus;
    print_stablestrongfocus = P4Print_print_stablestrongfocus;
    print_unstablestrongfocus = P4Print_print_unstablestrongfocus;
    print_sesaddle = P4Print_print_sesaddle;
    print_sesaddlenode = P4Print_print_sesaddlenode;
    print_sestablenode = P4Print_print_sestablenode;
    print_seunstablenode = P4Print_print_seunstablenode;
    print_degen = P4Print_print_degen;
    print_center = P4Print_print_center;
    print_elips = P4Print_print_elips;
    print_point = P4Print_print_point;
    print_line = P4Print_print_line;
    print_comment = P4Print_comment;

    s_LastP4Printcolor = -1;

    p4paint->fillRect(
        0, 0, w, h,
        QBrush(QXFIGCOLOR(printColorTable(bgColours::CBACKGROUND))));
}

void finishP4Printing(void)
{
    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;
    s_P4PrintPainter = nullptr;
}
