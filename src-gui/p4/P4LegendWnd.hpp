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

#include <QWidget>

class QPaintEvent;

class P4LegendWnd : public QWidget
{
    Q_OBJECT

  public:
    P4LegendWnd(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *);
    void calculateGeometry();
    void loadState();

  private:
    int orbitwidth_;
    int sepwidth_;

    int hmargin1_;
    int hmargin2_;
    int hmargin3_;
    int hmargin4_;
    int hmargin5_;
    int hmargin6_;
    int interline_;
    int xheight_;

    int vmargin1_;
    int vmargin2_;
    int vmargin25_;
    int vmargin3_;
    int vmargin4_;
    int vmargin5_;
    int vmargin6_;
    int vmargin7_;
    int vmargin8_;
};
