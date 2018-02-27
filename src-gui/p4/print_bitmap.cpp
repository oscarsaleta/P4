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

#include "print_bitmap.hpp"

#include "custom.hpp"
#include "main.hpp"
#include "plot_tools.hpp"
#include "print_points.hpp"

#include <QBrush>
#include <QPainter>

static bool sP4PrintBlackWhite = true;

static std::unique_ptr<QPainter> sP4PrintPainter{};

static int sP4PrintLineWidth = 0;
static int sP4PrintSymbolWidth = 0;
static int sLastP4PrintX0 = 0;
static int sLastP4PrintY0 = 0;
static int sLastP4PrintColor = 0;

// ----------------------------------------------------------------------------
int printColorTable(int color)
{
    int colorTable[NUMXFIGCOLORS] = {
        WHITE,  // black --> white when printing
        BLUE,   GREEN,  CYAN,   RED,   MAGENTA,
        BLACK,  // yellow --> black when printing
        BLACK,  // white --> black when printing
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

static void p4Print_comment(QString s)
{
    // do nothing
}

static void p4Print_print_saddle(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print box:
    sP4PrintPainter->drawRect(x - sP4PrintSymbolWidth / 2,
                              y - sP4PrintSymbolWidth / 2, sP4PrintSymbolWidth,
                              sP4PrintSymbolWidth);
}

static void p4Print_print_virtualsaddle(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print box:
    sP4PrintPainter->drawRect(x - sP4PrintSymbolWidth / 2,
                              y - sP4PrintSymbolWidth / 2, sP4PrintSymbolWidth,
                              sP4PrintSymbolWidth);
}

static void p4Print_print_stablenode(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_S);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print box:
    sP4PrintPainter->drawRect(x - sP4PrintSymbolWidth / 2,
                              y - sP4PrintSymbolWidth / 2, sP4PrintSymbolWidth,
                              sP4PrintSymbolWidth);
}

static void p4Print_print_virtualstablenode(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_S);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print box:
    sP4PrintPainter->drawRect(x - sP4PrintSymbolWidth / 2,
                              y - sP4PrintSymbolWidth / 2, sP4PrintSymbolWidth,
                              sP4PrintSymbolWidth);
}

static void p4Print_print_unstablenode(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_U);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print box:
    sP4PrintPainter->drawRect(x - sP4PrintSymbolWidth / 2,
                              y - sP4PrintSymbolWidth / 2, sP4PrintSymbolWidth,
                              sP4PrintSymbolWidth);
}

static void p4Print_print_virtualunstablenode(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_U);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print box:
    sP4PrintPainter->drawRect(x - sP4PrintSymbolWidth / 2,
                              y - sP4PrintSymbolWidth / 2, sP4PrintSymbolWidth,
                              sP4PrintSymbolWidth);
}

static void p4Print_print_stableweakfocus(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS_S);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);

    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_virtualstableweakfocus(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS_S);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_unstableweakfocus(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS_U);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_virtualunstableweakfocus(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS_U);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_weakfocus(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_virtualweakfocus(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_center(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CCENTER);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_virtualcenter(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CCENTER);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_stablestrongfocus(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSTRONG_FOCUS_S);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_virtualstablestrongfocus(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSTRONG_FOCUS_S);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_unstablestrongfocus(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSTRONG_FOCUS_U);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_virtualunstablestrongfocus(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSTRONG_FOCUS_U);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - sP4PrintSymbolWidth * 13 / 20,
                  x + sP4PrintSymbolWidth * 13 / 20, y, x,
                  y + sP4PrintSymbolWidth * 13 / 20,
                  x - sP4PrintSymbolWidth * 13 / 20, y);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_sesaddle(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(
        3, x - sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20,
        x + sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20, x,
        y - sP4PrintSymbolWidth * 12 / 20);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_virtualsesaddle(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(
        3, x - sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20,
        x + sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20, x,
        y - sP4PrintSymbolWidth * 12 / 20);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_sesaddlenode(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE_NODE);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(
        3, x - sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20,
        x + sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20, x,
        y - sP4PrintSymbolWidth * 12 / 20);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_virtualsesaddlenode(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE_NODE);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(
        3, x - sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20,
        x + sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20, x,
        y - sP4PrintSymbolWidth * 12 / 20);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_sestablenode(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_S);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(
        3, x - sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20,
        x + sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20, x,
        y - sP4PrintSymbolWidth * 12 / 20);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_virtualsestablenode(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_S);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(
        3, x - sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20,
        x + sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20, x,
        y - sP4PrintSymbolWidth * 12 / 20);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_seunstablenode(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_U);

    sP4PrintPainter->setPen(QXFIGCOLOR(color));
    sP4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(
        3, x - sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20,
        x + sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20, x,
        y - sP4PrintSymbolWidth * 12 / 20);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_virtualseunstablenode(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_U);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);
    sP4PrintPainter->setBrush(Qt::NoBrush);

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(
        3, x - sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20,
        x + sP4PrintSymbolWidth * 12 / 20, y + sP4PrintSymbolWidth * 12 / 20, x,
        y - sP4PrintSymbolWidth * 12 / 20);
    sP4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void p4Print_print_degen(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CDEGEN);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);

    // print cross:
    sP4PrintPainter->drawLine(
        x - sP4PrintSymbolWidth / 2, y - sP4PrintSymbolWidth / 2,
        x + sP4PrintSymbolWidth / 2, y + sP4PrintSymbolWidth / 2);
    sP4PrintPainter->drawLine(
        x + sP4PrintSymbolWidth / 2, y - sP4PrintSymbolWidth / 2,
        x - sP4PrintSymbolWidth / 2, y + sP4PrintSymbolWidth / 2);
}

static void p4Print_print_virtualdegen(double _x, double _y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CDEGEN);

    QPen p{QXFIGCOLOR(color), (int)(sP4PrintLineWidth * 26 / 20)};
    sP4PrintPainter->setPen(p);

    // print cross:
    sP4PrintPainter->drawLine(
        x - sP4PrintSymbolWidth / 2, y - sP4PrintSymbolWidth / 2,
        x + sP4PrintSymbolWidth / 2, y + sP4PrintSymbolWidth / 2);
    sP4PrintPainter->drawLine(
        x + sP4PrintSymbolWidth / 2, y - sP4PrintSymbolWidth / 2,
        x - sP4PrintSymbolWidth / 2, y + sP4PrintSymbolWidth / 2);
}

static void p4Print_print_coinciding(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CDEGEN);

    QPen p{QXFIGCOLOR(color), (int)P4PrintLineWidth * 26 / 20};
    sP4PrintPainter->setPen(p);

    // print double cross:
    sP4PrintPainter->drawLine(
        x - sP4PrintSymbolWidth / 2, y - sP4PrintSymbolWidth / 2,
        x + sP4PrintSymbolWidth / 2, y + sP4PrintSymbolWidth / 2);
    sP4PrintPainter->drawLine(
        x + sP4PrintSymbolWidth / 2, y - sP4PrintSymbolWidth / 2,
        x - sP4PrintSymbolWidth / 2, y + sP4PrintSymbolWidth / 2);
    sP4PrintPainter->drawLine(x, y - (sP4PrintSymbolWidth * 3) / 4, x,
                              y + (sP4PrintSymbolWidth * 3) / 4);
    sP4PrintPainter->drawLine(x + (sP4PrintSymbolWidth * 3) / 4, y,
                              x - (sP4PrintSymbolWidth * 3) / 4, y);
}

static void p4Print_print_elips(double x0, double y0, double a, double b,
                                int color, bool dotted,
                                const std::vector<P4POLYLINES> &ellipse)
{
    color = printColorTable(color);
    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);

    QPen p{QXFIGCOLOR(color), sP4PrintLineWidth};
    p.setCapStyle(Qt::RoundCap);
    sP4PrintPainter->setPen(p);

    // we do not use the (x0,y0,a,b,dotted) parameters. Instead, we use the
    // "precompiled" ellipse parameter.  Here, a list of lines is computed that
    // approximates the ellipse.

    for (auto const &it : ellipse) {
        sP4PrintPainter->drawLine((int)(it.x1), (int)(it.y1), (int)(it.x2),
                                  (int)(it.y2));
    }
}

static void p4Print_print_line(double _x0, double _y0, double _x1, double _y1,
                               int color)
{
    color = printColorTable(color);
    if (sP4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);

    int x0{(int)_x0};
    int x1{(int)_x1};
    int y0{(int)_y0};
    int y1{(int)_y1};

    if (x0 == x1 && y0 == y1)
        return;

    QPen p{QXFIGCOLOR(color), sP4PrintLineWidth};
    p.setCapStyle(Qt::RoundCap);

    sP4PrintPainter->setPen(p);
    sP4PrintPainter->drawLine(x0, y0, x1, y1);
}

static void p4Print_print_point(double _x0, double _y0, int color)
{
    if (sP4PrintBlackWhite)
        color = bgColours::CFOREGROUND;
    color = printColorTable(color);

    int x0{(int)_x0};
    int y0{(int)_y0};

    if (x0 == sLastP4PrintX0 && y0 == sLastP4PrintY0 &&
        color == sLastP4PrintColor)
        return;

    sLastP4PrintX0 = x0;
    sLastP4PrintY0 = y0;
    sLastP4PrintColor = color;

    QPen p{QXFIGCOLOR(color), sP4PrintLineWidth};
    p.setCapStyle(Qt::RoundCap);

    sP4PrintPainter->setPen(p);
    if (sP4PrintLineWidth > 1) {
        sP4PrintPainter->drawLine(x0, y0, x0, y0);
    } else {
        sP4PrintPainter->drawPoint(x0, y0);
    }
}

// ---------------------------------------------------------------------------------------

void prepareP4Printing(int w, int h, bool isblackwhite,
                       std::unique_ptr<QPainter> p4paint, int linewidth,
                       int symbolwidth)
{
    QString s;

    sP4PrintBlackWhite = isblackwhite;
    sP4PrintPainter = p4paint;
    sP4PrintLineWidth = linewidth;
    sP4PrintSymbolWidth = symbolwidth;

    plot_l = spherePrintLine;
    plot_p = spherePrintPoint;

    print_saddle = p4Print_print_saddle;
    print_virtualsaddle = p4Print_print_virtualsaddle;

    print_stablenode = p4Print_print_stablenode;
    print_virtualstablenode = p4Print_print_virtualstablenode;

    print_unstablenode = p4Print_print_unstablenode;
    print_virtualunstablenode = p4Print_print_virtualunstablenode;

    print_stableweakfocus = p4Print_print_stableweakfocus;
    print_virtualstableweakfocus = p4Print_print_virtualstableweakfocus;

    print_unstableweakfocus = p4Print_print_unstableweakfocus;
    print_virtualunstableweakfocus = p4Print_print_virtualunstableweakfocus;

    print_weakfocus = p4Print_print_weakfocus;
    print_virtualweakfocus = p4Print_print_virtualweakfocus;

    print_stablestrongfocus = p4Print_print_stablestrongfocus;
    print_virtualstablestrongfocus = p4Print_print_virtualstablestrongfocus;

    print_unstablestrongfocus = p4Print_print_unstablestrongfocus;
    print_virtualunstablestrongfocus = p4Print_print_virtualunstablestrongfocus;

    print_sesaddle = p4Print_print_sesaddle;
    print_virtualsesaddle = p4Print_print_virtualsesaddle;

    print_sesaddlenode = p4Print_print_sesaddlenode;
    print_virtualsesaddlenode = p4Print_print_virtualsesaddlenode;

    print_sestablenode = p4Print_print_sestablenode;
    print_virtualsestablenode = p4Print_print_virtualsestablenode;

    print_seunstablenode = p4Print_print_seunstablenode;
    print_virtualseunstablenode = p4Print_print_virtualseunstablenode;

    print_degen = p4Print_print_degen;
    print_virtualdegen = p4Print_print_virtualdegen;

    print_center = p4Print_print_center;
    print_virtualcenter = p4Print_print_virtualcenter;

    print_coinciding = p4Print_print_coinciding;
    print_elips = p4Print_print_elips;
    print_point = p4Print_print_point;
    print_line = p4Print_print_line;
    print_comment = p4Print_comment;

    sLastP4PrintColor = -1;

    p4paint->fillRect(
        0, 0, w, h,
        QBrush(QXFIGCOLOR(printColorTable(bgColours::CBACKGROUND))));
}

void finishP4Printing(void)
{
    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;

    sP4PrintPainter.reset();
}
