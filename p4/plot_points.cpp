/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
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

/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "plot_points.h"

#include "custom.h"

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------
//
// This uses the pixel-coordinate system, i.e. any transformations should
// be done beforehand.

void win_plot_saddle( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSADDLE) );
    p->setBrush( QXFIGCOLOR(CSADDLE) );
    p->drawRect( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, SYMBOLWIDTH, SYMBOLHEIGHT );
}

void win_plot_stablenode( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CNODE_S) );
    p->setBrush( QXFIGCOLOR(CNODE_S) );
    p->drawRect( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, SYMBOLWIDTH, SYMBOLHEIGHT );
}

void win_plot_unstablenode( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CNODE_U) );
    p->setBrush( QXFIGCOLOR(CNODE_U) );
    p->drawRect( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, SYMBOLWIDTH, SYMBOLHEIGHT );
}

void win_plot_weakfocus( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CWEAK_FOCUS) );
    p->setBrush( QXFIGCOLOR(CWEAK_FOCUS) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_stableweakfocus( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CWEAK_FOCUS_S) );
    p->setBrush( QXFIGCOLOR(CWEAK_FOCUS_S) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_unstableweakfocus( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CWEAK_FOCUS_U) );
    p->setBrush( QXFIGCOLOR(CWEAK_FOCUS_U) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_center( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CCENTER) );
    p->setBrush( QXFIGCOLOR(CCENTER) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_stablestrongfocus( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSTRONG_FOCUS_S) );
    p->setBrush( QXFIGCOLOR(CSTRONG_FOCUS_S) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_unstablestrongfocus( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSTRONG_FOCUS_U) );
    p->setBrush( QXFIGCOLOR(CSTRONG_FOCUS_U) );
    QPolygon qpa(4);
    qpa.setPoints(4, x,y-SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y,
        x,y+SYMBOLHEIGHT/2,x-SYMBOLWIDTH/2,y);

    p->drawPolygon( qpa );
}

void win_plot_sesaddlenode( QPainter * p, int x, int y )
{
    QPolygon qpa(3);
    p->setPen( QXFIGCOLOR(CSADDLE_NODE) );
    p->setBrush( QXFIGCOLOR(CSADDLE_NODE) );
    qpa.setPoints(3, x-SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,
        x,y-SYMBOLHEIGHT/2);

    p->drawPolygon( qpa );
}

void win_plot_sestablenode( QPainter * p, int x, int y )
{
    QPolygon qpa(3);
    p->setPen( QXFIGCOLOR(CNODE_S) );
    p->setBrush( QXFIGCOLOR(CNODE_S) );
    qpa.setPoints(3, x-SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,
        x,y-SYMBOLHEIGHT/2);

    p->drawPolygon( qpa );
}

void win_plot_seunstablenode( QPainter * p, int x, int y )
{
    QPolygon qpa(3);
    p->setPen( QXFIGCOLOR(CNODE_U) );
    p->setBrush( QXFIGCOLOR(CNODE_U) );
    qpa.setPoints(3, x-SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,
        x,y-SYMBOLHEIGHT/2);

    p->drawPolygon( qpa );
}

void win_plot_sesaddle( QPainter * p, int x, int y )
{
    QPolygon qpa(3);
    p->setPen( QXFIGCOLOR(CSADDLE) );
    p->setBrush( QXFIGCOLOR(CSADDLE) );
    qpa.setPoints(3, x-SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,x+SYMBOLWIDTH/2,y+SYMBOLHEIGHT/2,
        x,y-SYMBOLHEIGHT/2);

    p->drawPolygon( qpa );
}

void win_plot_degen( QPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CDEGEN) );
    p->drawLine( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, x+SYMBOLWIDTH/2, y+SYMBOLHEIGHT/2 );
    p->drawLine( x+SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, x-SYMBOLWIDTH/2, y+SYMBOLHEIGHT/2 );
}
