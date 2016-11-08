#ifndef PRINT_POINTS_H
#define PRINT_POINTS_H


#include "main.h"

#include <QString>



extern void (* print_saddle)( double, double );
extern void (* print_stablenode)( double, double );
extern void (* print_unstablenode)( double, double );
extern void (* print_stableweakfocus)( double, double );
extern void (* print_unstableweakfocus)( double, double );
extern void (* print_weakfocus)( double, double );
extern void (* print_stablestrongfocus)( double, double );
extern void (* print_unstablestrongfocus)( double, double );
extern void (* print_sesaddle)( double, double );
extern void (* print_sesaddlenode)( double, double );
extern void (* print_sestablenode)( double, double );
extern void (* print_seunstablenode)( double, double );
extern void (* print_degen)( double, double );
extern void (* print_center)( double, double );
extern void (* print_elips)( double, double, double, double, int, bool, P4POLYLINES * );
extern void (* print_line)( double, double, double, double, int );
extern void (* print_point)( double, double, int );
extern void (* print_comment)( QString );


#endif // PRINT_POINTS_H
