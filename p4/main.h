#ifndef MAIN_H
#define MAIN_H


#include <QPixmap>
#include <QPrinter>
#include <QString>

#include "file_vf.h"
#include "file_tab.h"
#include "p4application.h"
#include "win_main.h"


extern QInputVF * ThisVF;
extern QVFStudy VFResults;
extern QString P4Version;
extern QString P4VersionDate;
extern QString P4Platform;

extern bool action_OnlyPrepareFile;
extern bool action_SaveAll;
extern QP4Application * p4app;
extern QStartDlg * p4startdlg;
extern QPixmap * p4smallicon;

extern QPrinter * p4printer;


void SetP4WindowTitle( QWidget * win, QString title );
void HandleCommandLineArgument( char * arg );


#endif // MAIN_H
