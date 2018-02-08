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

#pragma once

#include <memory>

class QPainter;

void win_plot_saddle(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_stablenode(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_unstablenode(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_weakfocus(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_stableweakfocus(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_unstableweakfocus(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_center(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_stablestrongfocus(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_unstablestrongfocus(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_sesaddlenode(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_sestablenode(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_seunstablenode(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_sesaddle(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_degen(std::unique_ptr<QPainter> p, int x, int y);

void win_plot_virtualsaddle(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualstablenode(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualunstablenode(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualweakfocus(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualunstableweakfocus(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualcenter(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualstablestrongfocus(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualunstablestrongfocus(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualsesaddlenode(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualsestablenode(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualseunstablenode(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualsesaddle(std::unique_ptr<QPainter> p, int x, int y);
void win_plot_virtualdegen(std::unique_ptr<QPainter> p, int x, int y);

void win_plot_coinciding(std::unique_ptr<QPainter> p, int x, int y);
