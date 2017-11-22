/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
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

#ifndef PLOT_POINTS_H
#define PLOT_POINTS_H

class QPainter;

void win_plot_saddle(QPainter *p, int x, int y);
void win_plot_stablenode(QPainter *p, int x, int y);
void win_plot_unstablenode(QPainter *p, int x, int y);
void win_plot_weakfocus(QPainter *p, int x, int y);
void win_plot_stableweakfocus(QPainter *p, int x, int y);
void win_plot_unstableweakfocus(QPainter *p, int x, int y);
void win_plot_center(QPainter *p, int x, int y);
void win_plot_stablestrongfocus(QPainter *p, int x, int y);
void win_plot_unstablestrongfocus(QPainter *p, int x, int y);
void win_plot_sesaddlenode(QPainter *p, int x, int y);
void win_plot_sestablenode(QPainter *p, int x, int y);
void win_plot_seunstablenode(QPainter *p, int x, int y);
void win_plot_sesaddle(QPainter *p, int x, int y);
void win_plot_degen(QPainter *p, int x, int y);

#endif // PLOT_POINTS_H
