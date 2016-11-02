#ifndef PLOT_POINTS_H
#define PLOT_POINTS_H


#include <QPainter>


void win_plot_saddle( QPainter * p, int x, int y );
void win_plot_stablenode( QPainter * p, int x, int y );
void win_plot_unstablenode( QPainter * p, int x, int y );
void win_plot_weakfocus( QPainter * p, int x, int y );
void win_plot_stableweakfocus( QPainter * p, int x, int y );
void win_plot_unstableweakfocus( QPainter * p, int x, int y );
void win_plot_center( QPainter * p, int x, int y );
void win_plot_stablestrongfocus( QPainter * p, int x, int y );
void win_plot_unstablestrongfocus( QPainter * p, int x, int y );
void win_plot_sesaddlenode( QPainter * p, int x, int y );
void win_plot_sestablenode( QPainter * p, int x, int y );
void win_plot_seunstablenode( QPainter * p, int x, int y );
void win_plot_sesaddle( QPainter * p, int x, int y );
void win_plot_degen( QPainter * p, int x, int y );


#endif // PLOT_POINTS_H
