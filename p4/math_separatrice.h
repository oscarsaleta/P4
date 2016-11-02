#ifndef MATH_SEPARATRICE_H
#define MATH_SEPARATRICE_H

#include "file_tab.h"
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

void plot_all_sep( QWinSphere * spherewnd );
void draw_sep( QWinSphere * spherewnd, orbits_points *sep );
void draw_selected_sep( QWinSphere * spherewnd, orbits_points *sep,int color);

int findSepColor2(term2 *f,int type,double y[2]);
int findSepColor3(term3 *f,int type,double y[2]);

void integrate_poincare_sep( double p0, double p1, double p2, double * pcoord,
                            double * hhi, int * type, int * color, int * dashes, int * dir,
                            double h_min, double h_max);
void integrate_lyapunov_sep( double p0, double p1, double p2, double * pcoord,
                            double * hhi, int * type, int * color, int * dashes,
                            int * dir, double h_min, double h_max);

int change_type(int type);

orbits_points * plot_separatrice(QWinSphere * spherewnd, double x0, double y0,double a11, double a12,
                                        double a21, double a22, double epsilon, sep * sep1,
                                        orbits_points ** orbit, short int chart);

void make_transformations(transformations * trans, double x0, double y0, double * point);


#endif // MATH_SEPARATRICE_H
