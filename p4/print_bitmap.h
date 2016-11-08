#ifndef PRINT_BITMAP_H
#define PRINT_BITMAP_H


#include "color.h"

#include <QPainter>


extern int printColorTable[NUMXFIGCOLORS];

void prepareP4Printing( int w, int h, bool isblackwhite, QPainter * p4paint, int linewidth, int symbolwidth );
void finishP4Printing( void );


#endif // PRINT_BITMAP_H
