#ifndef WIN_P4_H
#define WIN_P4_H


#include <QPixmap>
#include <QString>


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

extern QString P4Version;
extern QString P4VersionDate;
extern QString P4Platform;

extern QPixmap * p4smallicon;


#endif /* WIN_P4_H */
