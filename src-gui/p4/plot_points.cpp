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

#include "plot_points.h"

#include "custom.h"

#include <QPainter>

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------
//
// This uses the pixel-coordinate system, i.e. any transformations should
// be done beforehand.

void win_plot_saddle(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CSADDLE));
    p->setBrush(QXFIGCOLOR(CSADDLE));
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_virtualsaddle(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(CSADDLE));
    p->setBrush(Qt::NoBrush);
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_stablenode(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CNODE_S));
    p->setBrush(QXFIGCOLOR(CNODE_S));
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_virtualstablenode(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(CNODE_S));
    p->setBrush(Qt::NoBrush);
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_unstablenode(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CNODE_U));
    p->setBrush(QXFIGCOLOR(CNODE_U));
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_virtualunstablenode(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(CNODE_U));
    p->setBrush(Qt::NoBrush);
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_weakfocus(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CWEAK_FOCUS));
    p->setBrush(QXFIGCOLOR(CWEAK_FOCUS));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualweakfocus(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(CWEAK_FOCUS));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_stableweakfocus(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CWEAK_FOCUS_S));
    p->setBrush(QXFIGCOLOR(CWEAK_FOCUS_S));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualunstableweakfocus(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(CWEAK_FOCUS_U));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_unstableweakfocus(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CWEAK_FOCUS_U));
    p->setBrush(QXFIGCOLOR(CWEAK_FOCUS_U));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_center(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CCENTER));
    p->setBrush(QXFIGCOLOR(CCENTER));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualcenter(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(CCENTER));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_stablestrongfocus(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CSTRONG_FOCUS_S));
    p->setBrush(QXFIGCOLOR(CSTRONG_FOCUS_S));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualstablestrongfocus(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(CSTRONG_FOCUS_S));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_unstablestrongfocus(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CSTRONG_FOCUS_U));
    p->setBrush(QXFIGCOLOR(CSTRONG_FOCUS_U));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualunstablestrongfocus(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(CSTRONG_FOCUS_U));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_sesaddlenode(std::unique_ptr<QPainter> p, int x, int y)
{
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(CSADDLE_NODE));
    p->setBrush(QXFIGCOLOR(CSADDLE_NODE));
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_virtualsesaddlenode(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(CSADDLE_NODE));
    p->setBrush(Qt::NoBrush);
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_sestablenode(std::unique_ptr<QPainter> p, int x, int y)
{
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(CNODE_S));
    p->setBrush(QXFIGCOLOR(CNODE_S));
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_virtualsestablenode(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(CNODE_S));
    p->setBrush(Qt::NoBrush);
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_seunstablenode(std::unique_ptr<QPainter> p, int x, int y)
{
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(CNODE_U));
    p->setBrush(QXFIGCOLOR(CNODE_U));
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_virtualseunstablenode(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(CNODE_U));
    p->setBrush(Qt::NoBrush);
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_sesaddle(std::unique_ptr<QPainter> p, int x, int y)
{
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(CSADDLE));
    p->setBrush(QXFIGCOLOR(CSADDLE));
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_virtualsesaddle(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(CSADDLE));
    p->setBrush(Qt::NoBrush);
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_degen(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CDEGEN));
    p->drawLine(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2,
                y + SYMBOLHEIGHT / 2);
    p->drawLine(x + SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2,
                y + SYMBOLHEIGHT / 2);
}

void win_plot_virtualdegen(std::unique_ptr<QPainter> p, int x, int y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(CDEGEN));
    p->drawLine(x - SYMBOLWIDTH / 3, y - SYMBOLHEIGHT / 3, x + SYMBOLWIDTH / 3,
                y + SYMBOLHEIGHT / 3);
    p->drawLine(x + SYMBOLWIDTH / 3, y - SYMBOLHEIGHT / 3, x - SYMBOLWIDTH / 3,
                y + SYMBOLHEIGHT / 3);
}

void win_plot_coinciding(std::unique_ptr<QPainter> p, int x, int y)
{
    p->setPen(QXFIGCOLOR(CDEGEN));
    p->drawLine(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2,
                y + SYMBOLHEIGHT / 2);
    p->drawLine(x + SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2,
                y + SYMBOLHEIGHT / 2);
    p->setPen(QXFIGCOLOR(CDEGEN));
    p->drawLine(x, y - (SYMBOLHEIGHT * 3) / 4, x, y + (SYMBOLHEIGHT * 3) / 4);
    p->drawLine(x + (SYMBOLWIDTH * 3) / 4, y, x - (SYMBOLWIDTH * 3) / 4, y);
}
