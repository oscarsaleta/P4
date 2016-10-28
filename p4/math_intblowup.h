#ifndef MATH_INTBLOWUP_H
#define MATH_INTBLOWUP_H

#include "file_tab.h"
#include "win_sphere.h"

void eval_blow_vec_field( double * y, double * f);
orbits_points * integrate_blow_up( QWinSphere * spherewnd, double * pcoord2,
                    blow_up_points * de_sep, double step,int dir, int type,
                    orbits_points ** orbit, int chart );

#endif // MATH_INTBLOWUP_H
