#ifndef MATH_SEPARATRICE_H
#define MATH_SEPARATRICE_H


#include "win_sphere.h"


extern void (* change_epsilon)( QWinSphere *, double );
extern void (*start_plot_sep)( QWinSphere * );
extern void (*cont_plot_sep)( QWinSphere * );
extern void (*plot_next_sep)( QWinSphere * );
extern void (*select_next_sep)( QWinSphere * );

void start_plot_saddle_sep( QWinSphere * );
void cont_plot_saddle_sep( QWinSphere * );
void plot_next_saddle_sep( QWinSphere * );
void select_next_saddle_sep( QWinSphere * );
void change_epsilon_saddle( QWinSphere *, double );

void start_plot_se_sep( QWinSphere * );
void cont_plot_se_sep( QWinSphere * );
void plot_next_se_sep( QWinSphere * );
void select_next_se_sep( QWinSphere * );
void change_epsilon_se( QWinSphere *, double );

void start_plot_de_sep( QWinSphere * );
void cont_plot_de_sep( QWinSphere * );
void plot_next_de_sep( QWinSphere * );
void select_next_de_sep( QWinSphere * );
void change_epsilon_de( QWinSphere *, double );


#endif // MATH_SEPARATRICE_H
