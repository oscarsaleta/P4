#define UNUSED(x) (void)(x)   //  to avoid warnings of unused variables in case we cannot avoid it

struct P4POLYLINES
{
	double x1;
	double y1;
	double x2;
	double y2;
	struct P4POLYLINES * next;
};

extern bool lcWindowIsUp;	// true when Limitcycles window is shown.
							// in that case, left mouse button action is redefined
							// to set set points for LC window instead of for orbits window

extern QString P4Version;
extern QString P4VersionDate;
extern QString P4Platform;

class QFindDlg;
class QStartDlg;
class QVectorFieldDlg;
class QParamsDlg;
class QPlotWnd;
class QLegendWnd;
class QIntParamsDlg;
class QOrbitsDlg;
class QSepDlg;
class QPrintDlg;
class QViewDlg;
class QSettingsDlg;
class QPixmap;

extern QPixmap * p4smallicon;
