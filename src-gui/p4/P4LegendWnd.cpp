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

#include "P4LegendWnd.h"

#include "custom.h"
#include "main.h"
#include "p4application.h"
#include "plot_points.h"

#include <QPainter>

// -----------------------------------------------------------------------
//                          Define Colors in RGB
// -----------------------------------------------------------------------

P4RGBITEM g_XFigToRGB[NUMXFIGCOLORS] = {
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

// -----------------------------------------------------------------------
//                              LEGEND
// -----------------------------------------------------------------------

P4LegendWnd::P4LegendWnd() : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    int w, h;

    // general initialization

    calculateGeometry();
    resize(w = hmargin6_ + 1, h = vmargin7_ + 1);
    setMinimumSize(w, h);
    setMaximumSize(w, h);
    setP4WindowTitle(this, "P4 Legend");
}

void P4LegendWnd::paintEvent(QPaintEvent *p)
{
    QPainter paint(this);
    paint.setFont(*(g_p4app->legendFont_));

    paint.setPen(QPen(QXFIGCOLOR(bgColours::CFOREGROUND)));
    paint.drawText(hmargin1_, vmargin1_, "Non-Degenerate:");
    paint.drawText(hmargin4_, vmargin1_, "Semi-hyperbolic:");
    paint.drawText(hmargin1_, vmargin3_, "Separatrices:");
    paint.drawText(hmargin4_, vmargin25_, "Orbits:");
    paint.drawText(hmargin4_, vmargin3_, "Curve of Singularities:");
    paint.drawText(hmargin4_, vmargin4_, "Limit Cycles:");
    paint.drawText(hmargin4_, vmargin4_ + 2 * interline_, "Arbitrary Curve:");

    paint.setPen(QPen(QXFIGCOLOR(CSADDLE)));
    paint.drawText(hmargin2_, vmargin2_, "Saddle");
    paint.setPen(QPen(QXFIGCOLOR(CNODE_S)));
    paint.drawText(hmargin2_, vmargin2_ + 1 * interline_, "Stable Node");
    paint.setPen(QPen(QXFIGCOLOR(CNODE_U)));
    paint.drawText(hmargin2_, vmargin2_ + 2 * interline_, "Unstable Node");
    paint.setPen(QPen(QXFIGCOLOR(CWEAK_FOCUS)));
    paint.drawText(hmargin2_, vmargin2_ + 3 * interline_,
                   "Weak Focus or center");
    paint.setPen(QPen(QXFIGCOLOR(CWEAK_FOCUS_S)));
    paint.drawText(hmargin2_, vmargin2_ + 4 * interline_, "Stable Weak Focus");
    paint.setPen(QPen(QXFIGCOLOR(CWEAK_FOCUS_U)));
    paint.drawText(hmargin2_, vmargin2_ + 5 * interline_,
                   "Unstable Weak Focus");
    paint.setPen(QPen(QXFIGCOLOR(CCENTER)));
    paint.drawText(hmargin2_, vmargin2_ + 6 * interline_, "Center");
    paint.setPen(QPen(QXFIGCOLOR(CSTRONG_FOCUS_S)));
    paint.drawText(hmargin2_, vmargin2_ + 7 * interline_,
                   "Stable Strong Focus");
    paint.setPen(QPen(QXFIGCOLOR(CSTRONG_FOCUS_U)));
    paint.drawText(hmargin2_, vmargin2_ + 8 * interline_,
                   "Unstable Strong Focus");

    win_plot_saddle(&paint, hmargin1_ + SYMBOLWIDTH / 2,
                    vmargin2_ - SYMBOLHEIGHT / 2);
    win_plot_stablenode(&paint, hmargin1_ + SYMBOLWIDTH / 2,
                        vmargin2_ - SYMBOLHEIGHT / 2 + 1 * interline_);
    win_plot_unstablenode(&paint, hmargin1_ + SYMBOLWIDTH / 2,
                          vmargin2_ - SYMBOLHEIGHT / 2 + 2 * interline_);
    win_plot_weakfocus(&paint, hmargin1_ + SYMBOLWIDTH / 2,
                       vmargin2_ - SYMBOLHEIGHT / 2 + 3 * interline_);
    win_plot_stableweakfocus(&paint, hmargin1_ + SYMBOLWIDTH / 2,
                             vmargin2_ - SYMBOLHEIGHT / 2 + 4 * interline_);
    win_plot_unstableweakfocus(&paint, hmargin1_ + SYMBOLWIDTH / 2,
                               vmargin2_ - SYMBOLHEIGHT / 2 + 5 * interline_);
    win_plot_center(&paint, hmargin1_ + SYMBOLWIDTH / 2,
                    vmargin2_ - SYMBOLHEIGHT / 2 + 6 * interline_);
    win_plot_stablestrongfocus(&paint, hmargin1_ + SYMBOLWIDTH / 2,
                               vmargin2_ - SYMBOLHEIGHT / 2 + 7 * interline_);
    win_plot_unstablestrongfocus(&paint, hmargin1_ + SYMBOLWIDTH / 2,
                                 vmargin2_ - SYMBOLHEIGHT / 2 + 8 * interline_);

    paint.setPen(QPen(QXFIGCOLOR(CSADDLE_NODE)));
    paint.drawText(hmargin5_, vmargin2_, "Saddle-Node");
    paint.setPen(QPen(QXFIGCOLOR(CNODE_S)));
    paint.drawText(hmargin5_, vmargin2_ + 1 * interline_, "Stable-Node");
    paint.setPen(QPen(QXFIGCOLOR(CNODE_U)));
    paint.drawText(hmargin5_, vmargin2_ + 2 * interline_, "Unstable-Node");
    paint.setPen(QPen(QXFIGCOLOR(CSADDLE)));
    paint.drawText(hmargin5_, vmargin2_ + 3 * interline_, "Saddle");
    paint.setPen(QPen(QXFIGCOLOR(CDEGEN)));
    paint.drawText(hmargin5_, vmargin2_ + 5 * interline_, "Non-elementary");

    win_plot_sesaddlenode(&paint, hmargin4_ + SYMBOLWIDTH / 2,
                          vmargin2_ - SYMBOLHEIGHT / 2);
    win_plot_sestablenode(&paint, hmargin4_ + SYMBOLWIDTH / 2,
                          vmargin2_ - SYMBOLHEIGHT / 2 + 1 * interline_);
    win_plot_seunstablenode(&paint, hmargin4_ + SYMBOLWIDTH / 2,
                            vmargin2_ - SYMBOLHEIGHT / 2 + 2 * interline_);
    win_plot_sesaddle(&paint, hmargin4_ + SYMBOLWIDTH / 2,
                      vmargin2_ - SYMBOLHEIGHT / 2 + 3 * interline_);
    win_plot_degen(&paint, hmargin4_ + SYMBOLWIDTH / 2,
                   vmargin2_ - SYMBOLHEIGHT / 2 + 5 * interline_);

    paint.setPen(QPen(QXFIGCOLOR(CSTABLE)));
    paint.drawText(hmargin3_, vmargin4_, "Stable");
    paint.drawLine(hmargin1_, vmargin4_ - xheight_, hmargin1_ + sepwidth_ - 1,
                   vmargin4_ - xheight_);
    paint.setPen(QPen(QXFIGCOLOR(CUNSTABLE)));
    paint.drawText(hmargin3_, vmargin4_ + 1 * interline_, "Unstable");
    paint.drawLine(hmargin1_, vmargin4_ + interline_ - xheight_,
                   hmargin1_ + sepwidth_ - 1,
                   vmargin4_ + interline_ - xheight_);
    paint.setPen(QPen(QXFIGCOLOR(CCENT_STABLE)));
    paint.drawText(hmargin3_, vmargin4_ + 2 * interline_, "Center-Stable");
    paint.drawLine(hmargin1_, vmargin4_ + 2 * interline_ - xheight_,
                   hmargin1_ + sepwidth_ - 1,
                   vmargin4_ + 2 * interline_ - xheight_);
    paint.setPen(QPen(QXFIGCOLOR(CCENT_UNSTABLE)));
    paint.drawText(hmargin3_, vmargin4_ + 3 * interline_, "Center-Unstable");
    paint.drawLine(hmargin1_, vmargin4_ + 3 * interline_ - xheight_,
                   hmargin1_ + sepwidth_ - 1,
                   vmargin4_ + 3 * interline_ - xheight_);

    paint.setPen(QPen(QXFIGCOLOR(bgColours::CORBIT)));
    paint.drawLine(hmargin4_, vmargin25_ + interline_ - xheight_,
                   hmargin4_ + orbitwidth_ - 1,
                   vmargin25_ + interline_ - xheight_);
    paint.setPen(QPen(QXFIGCOLOR(CSING)));
    paint.drawLine(hmargin4_, vmargin3_ + interline_ - xheight_,
                   hmargin4_ + orbitwidth_ - 1,
                   vmargin3_ + interline_ - xheight_);
    paint.setPen(QPen(QXFIGCOLOR(CLIMIT)));
    paint.drawLine(hmargin4_, vmargin4_ + interline_ - xheight_,
                   hmargin4_ + orbitwidth_ - 1,
                   vmargin4_ + interline_ - xheight_);
    paint.setPen(QPen(QXFIGCOLOR(CCURV)));
    paint.drawLine(hmargin4_, vmargin4_ + 3 * interline_ - xheight_,
                   hmargin4_ + orbitwidth_ - 1,
                   vmargin4_ + 3 * interline_ - xheight_);

    setP4WindowTitle(this, "P4 Legend");
}

/*
    Non-degenerate                  Semi-hyperbolic

    X Saddle                        X Saddle-Node
    X Stable Node                   X Stable-Node
    X Unstable Node                 X Unstable-Node
    X Weak Focus                    X Saddle
    X Stable Weak Focus
    X Unstable Weak Focus           X Non-elementary
    X Center
    X Stable Strong Focus
    X Unstable Strong Focus         Orbits:
                                    -----------------
    Separatrices:                   Line of Singularities:
                                    ------------------
    ------ Stable                   Limit Cycles:
    ------ Unstable                 -----------------
    ------ Center-Stable            Arbitrary Curve:
    ------ Center-unstable          -----------------

horizontal margins:
    1 2    3                        4 5                    6

vertical margins:
    1 non-deg / semi-hyp
    2 saddle / saddle-node
    25 unstable strong focus
    3 sep/orbits
    4 stable
    5 line of sing
    6 limit cycl
    7 bottom
*/

void P4LegendWnd::calculateGeometry(void)
{
    int e, me;

    QFontMetrics fm{*(g_p4app->legendFont_)};

    QPalette palette;
    palette.setColor(backgroundRole(), QXFIGCOLOR(bgColours::CBACKGROUND));
    setPalette(palette);

    hmargin1_ = fm.maxWidth();
    interline_ = fm.lineSpacing();

    if (interline_ < SYMBOLHEIGHT + 2)
        interline_ = SYMBOLHEIGHT + 2;

    hmargin2_ = hmargin1_ + SYMBOLWIDTH + fm.maxWidth();

    orbitwidth_ = fm.width("abcdefghk");
    sepwidth_ = fm.width("abc");

    hmargin3_ = hmargin1_ + sepwidth_ + fm.maxWidth();

    me = hmargin1_ + fm.width("Non-degenerate:");

    e = hmargin2_ + fm.width("Saddle");
    if (me < e)
        me = e;
    e = hmargin2_ + fm.width("Stable Node");
    if (me < e)
        me = e;
    e = hmargin2_ + fm.width("Unstable Node");
    if (me < e)
        me = e;
    e = hmargin2_ + fm.width("Weak Focus");
    if (me < e)
        me = e;
    e = hmargin2_ + fm.width("Stable Weak Focus");
    if (me < e)
        me = e;
    e = hmargin2_ + fm.width("Unstable Weak Focus");
    if (me < e)
        me = e;
    e = hmargin2_ + fm.width("Center");
    if (me < e)
        me = e;
    e = hmargin2_ + fm.width("Stable Strong Focus");
    if (me < e)
        me = e;
    e = hmargin2_ + fm.width("Unstable Strong Focus");
    if (me < e)
        me = e;

    e = hmargin1_ + fm.width("Separatrices:");
    if (me < e)
        me = e;
    e = hmargin3_ + fm.width("Stable");
    if (me < e)
        me = e;
    e = hmargin3_ + fm.width("Unstable");
    if (me < e)
        me = e;
    e = hmargin3_ + fm.width("Center-Stable");
    if (me < e)
        me = e;
    e = hmargin3_ + fm.width("Center-Unstable");
    if (me < e)
        me = e;

    hmargin4_ = me + fm.maxWidth() * 3; // 3 chars intercolumn space

    hmargin5_ = hmargin4_ + SYMBOLWIDTH + fm.maxWidth();

    me = hmargin4_ + fm.width("Semi-hyperbolic");
    e = hmargin5_ + fm.width("Saddle-Node");
    if (me < e)
        me = e;
    e = hmargin5_ + fm.width("Stable Node");
    if (me < e)
        me = e;
    e = hmargin5_ + fm.width("Unstable Node");
    if (me < e)
        me = e;
    e = hmargin5_ + fm.width("Saddle");
    if (me < e)
        me = e;

    e = hmargin5_ + fm.width("Non-elementary:");
    if (me < e)
        me = e;
    e = hmargin4_ + fm.width("Orbits:");
    if (me < e)
        me = e;
    e = hmargin4_ + fm.width("Curve of Singularities:");
    if (me < e)
        me = e;
    e = hmargin4_ + fm.width("Limit Cycles:");
    if (me < e)
        me = e;

    hmargin6_ = me + fm.maxWidth();

    vmargin1_ = fm.height() + fm.descent();
    vmargin2_ = vmargin1_ + 2 * interline_;
    vmargin25_ = vmargin2_ + 8 * interline_;
    vmargin3_ = vmargin2_ + 10 * interline_;
    vmargin4_ = vmargin3_ + 2 * interline_;

    vmargin5_ = vmargin3_ + (5 * interline_) / 2;
    vmargin6_ = vmargin5_ + (5 * interline_) / 2;

    me = vmargin6_ + 2 * interline_;
    e = vmargin4_ + 4 * interline_;
    if (me < e)
        me = e;

    vmargin7_ = e + interline_;

    xheight_ = fm.ascent() / 2;
}
