#ifndef MAIN_H
#define MAIN_H


#include <QPixmap>
#include <QPrinter>
#include <QString>

//#include "file_vf.h"
//#include "file_tab.h"
//#include "p4application.h"
//#include "win_main.h"


#define UNUSED(x) (void)(x)   //  to avoid warnings of unused variables in case we cannot avoid it

struct P4POLYLINES
{
    double x1;
    double y1;
    double x2;
    double y2;
    P4POLYLINES * next;
};

extern bool lcWindowIsUp;	// true when Limitcycles window is shown.
                            // in that case, left mouse button action is redefined
                            // to set set points for LC window instead of for orbits window

extern QString p4Version;
extern QString p4VersionDate;
extern QString p4Platform;

extern QPixmap * p4smallicon;
extern QPrinter * p4printer;

extern bool action_OnlyPrepareFile;
extern bool action_SaveAll;

extern QString cmdLine_Filename;
extern bool cmdLine_AutoEvaluate;
extern bool cmdLine_AutoPlot;
extern bool cmdLine_AutoExit;

void setP4WindowTitle( QWidget * win, QString title );

void handleCommandLineOption( char * arg );
void handleCommandLineArgument( char * arg );


#endif // MAIN_H
