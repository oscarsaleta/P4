#include "print_points.h"


void (* print_saddle)( double, double ) = nullptr;
void (* print_stablenode)( double, double ) = nullptr;
void (* print_unstablenode)( double, double ) = nullptr;
void (* print_stableweakfocus)( double, double ) = nullptr;
void (* print_unstableweakfocus)( double, double ) = nullptr;
void (* print_weakfocus)( double, double ) = nullptr;
void (* print_stablestrongfocus)( double, double ) = nullptr;
void (* print_unstablestrongfocus)( double, double ) = nullptr;
void (* print_sesaddle)( double, double ) = nullptr;
void (* print_sesaddlenode)( double, double ) = nullptr;
void (* print_sestablenode)( double, double ) = nullptr;
void (* print_seunstablenode)( double, double ) = nullptr;
void (* print_degen)( double, double ) = nullptr;
void (* print_center)( double, double ) = nullptr;
void (* print_elips)( double, double, double, double, int, bool, struct P4POLYLINES * ) = nullptr;
void (* print_line)( double, double, double, double, int ) = nullptr;
void (* print_point)( double, double, int ) = nullptr;
void (* print_comment)( QString ) = nullptr;

