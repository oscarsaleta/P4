#ifndef PRINT_POSTSCRIPT_H
#define PRINT_POSTSCRIPT_H


void preparePostscriptPrinting( int x0, int y0, int w, int h, bool iszoom, bool isblackwhite,
                                int resolution, int linewidth, int symbolwidth );
void finishPostscriptPrinting( void );


#endif // PRINT_POSTSCRIPT_H
