#ifndef PRINT_XFIG_H
#define PRINT_XFIG_H


#define XFIG_LINE_MAXPOINTS     2000        // group line pieces together in polylines
                                            //  of at most this number of points.

void prepareXFigPrinting( int w, int h, bool iszoom, bool isblackwhite, int resolution,
                            int linewidth, int symbolwidth );
void finishXFigPrinting( void );


#endif // PRINT_XFIG_H
