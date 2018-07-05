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

#include "plot_points.hpp"

#include <QDebug>
#include <QPainter>

#include "P4ParentStudy.hpp"
#include "custom.hpp"

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------
//
// This uses the pixel-coordinate system, i.e. any transformations should
// be done beforehand.

void win_plot_saddle(QPainter *p, int x, int y)
{
    //    qDebug() << "called win plot saddle";
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_saddle));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_saddle));
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_virtualsaddle(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_saddle));
    p->setBrush(Qt::NoBrush);
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_stablenode(QPainter *p, int x, int y)
{
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_node_stable));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_node_stable));
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_virtualstablenode(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_node_stable));
    p->setBrush(Qt::NoBrush);
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_unstablenode(QPainter *p, int x, int y)
{
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_node_unstable));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_node_unstable));
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_virtualunstablenode(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_node_unstable));
    p->setBrush(Qt::NoBrush);
    p->drawRect(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, SYMBOLWIDTH,
                SYMBOLHEIGHT);
}

void win_plot_weakfocus(QPainter *p, int x, int y)
{
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_weak_focus));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_weak_focus));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualweakfocus(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_weak_focus));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_stableweakfocus(QPainter *p, int x, int y)
{
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_weak_focus_stable));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_weak_focus_stable));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualstableweakfocus(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_weak_focus_stable));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_unstableweakfocus(QPainter *p, int x, int y)
{
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_weak_focus_unstable));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_weak_focus_unstable));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualunstableweakfocus(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_weak_focus_unstable));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_center(QPainter *p, int x, int y)
{
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_center));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_center));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualcenter(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_center));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_stablestrongfocus(QPainter *p, int x, int y)
{
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_strong_focus_stable));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_strong_focus_stable));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualstablestrongfocus(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_strong_focus_stable));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_unstablestrongfocus(QPainter *p, int x, int y)
{
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_strong_focus_unstable));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_strong_focus_unstable));
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_virtualunstablestrongfocus(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_strong_focus_unstable));
    p->setBrush(Qt::NoBrush);
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2, y, x,
                  y + SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2, y);

    p->drawPolygon(qpa);
}

void win_plot_sesaddlenode(QPainter *p, int x, int y)
{
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(colour_saddle_node));
    p->setBrush(QXFIGCOLOR(colour_saddle_node));
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_virtualsesaddlenode(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(colour_saddle_node));
    p->setBrush(Qt::NoBrush);
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_sestablenode(QPainter *p, int x, int y)
{
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_node_stable));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_node_stable));
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_virtualsestablenode(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_node_stable));
    p->setBrush(Qt::NoBrush);
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_seunstablenode(QPainter *p, int x, int y)
{
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_node_unstable));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_node_unstable));
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_virtualseunstablenode(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_node_unstable));
    p->setBrush(Qt::NoBrush);
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_sesaddle(QPainter *p, int x, int y)
{
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_saddle));
    p->setBrush(QXFIGCOLOR(P4ColourSettings::colour_saddle));
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_virtualsesaddle(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    QPolygon qpa(3);
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_saddle));
    p->setBrush(Qt::NoBrush);
    qpa.setPoints(3, x - SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2,
                  x + SYMBOLWIDTH / 2, y + SYMBOLHEIGHT / 2, x,
                  y - SYMBOLHEIGHT / 2);

    p->drawPolygon(qpa);
}

void win_plot_degen(QPainter *p, int x, int y)
{
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_degen));
    p->drawLine(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2,
                y + SYMBOLHEIGHT / 2);
    p->drawLine(x + SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2,
                y + SYMBOLHEIGHT / 2);
}

void win_plot_virtualdegen(QPainter *p, int x, int y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_degen));
    p->drawLine(x - SYMBOLWIDTH / 3, y - SYMBOLHEIGHT / 3, x + SYMBOLWIDTH / 3,
                y + SYMBOLHEIGHT / 3);
    p->drawLine(x + SYMBOLWIDTH / 3, y - SYMBOLHEIGHT / 3, x - SYMBOLWIDTH / 3,
                y + SYMBOLHEIGHT / 3);
}

void win_plot_coinciding(QPainter *p, int x, int y)
{
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_degen));
    p->drawLine(x - SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, x + SYMBOLWIDTH / 2,
                y + SYMBOLHEIGHT / 2);
    p->drawLine(x + SYMBOLWIDTH / 2, y - SYMBOLHEIGHT / 2, x - SYMBOLWIDTH / 2,
                y + SYMBOLHEIGHT / 2);
    p->setPen(QXFIGCOLOR(P4ColourSettings::colour_degen));
    p->drawLine(x, y - (SYMBOLHEIGHT * 3) / 4, x, y + (SYMBOLHEIGHT * 3) / 4);
    p->drawLine(x + (SYMBOLWIDTH * 3) / 4, y, x - (SYMBOLWIDTH * 3) / 4, y);
}
