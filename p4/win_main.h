/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WIN_MAIN_H
#define WIN_MAIN_H


#include <QBoxLayout>
#include <QCloseEvent>
#include <QLineEdit>
#include <QMenu>
#include <QPalette>
#include <QPushButton>
#include <QString>
#include <QWidget>


#define TYPE_SIGNAL_EVALUATING      (QEvent::User + 1)
#define TYPE_SIGNAL_EVALUATED       (QEvent::User + 2)
#define TYPE_SIGNAL_CHANGED         (QEvent::User + 3)
#define TYPE_SIGNAL_LOADED          (QEvent::User + 4)
#define TYPE_SIGNAL_SAVED           (QEvent::User + 5)

#define TYPE_SELECT_ORBIT           (QEvent::User + 6)
#define TYPE_CLOSE_PLOTWINDOW       (QEvent::User + 7)
#define TYPE_ORBIT_EVENT            (QEvent::User + 8)
#define TYPE_SEP_EVENT              (QEvent::User + 9)
#define TYPE_CLOSE_ZOOMWINDOW       (QEvent::User + 10)
#define TYPE_OPENZOOMWINDOW         (QEvent::User + 11)
#define TYPE_SELECT_LCSECTION       (QEvent::User + 12) 


/* Forward-declarations to solve cross-include problems */
class QPlotWnd; // in win_plot.h
class QFindDlg; // in win_find.h

struct DOUBLEPOINT
{
    double x;
    double y;
};


// class defined in win_main.cpp
class QStartDlg : public QWidget
{
    Q_OBJECT
        
public:
    QStartDlg( const QString & );
    void closeEvent( QCloseEvent* ce );

public slots:
    // following slots are called by QT when a button is pressed or a file name is changed:

    void OnQuit();
    void OnPlot();
    void OnViewFinite();
    void OnViewInfinite();
    void OnHelp();
    void OnFind();
    void OnBrowse();
    void OnAbout();
    void OnFilenameChange( const QString & );
    QWidget * Showtext( QWidget * win, QString caption, QString fname );

    // following slots are called by other P4 widgets when something happened:

public:
    void Signal_Evaluating( void );
    void Signal_Evaluated( void );
    void Signal_Changed( void );
    void Signal_Loaded( void );
    void Signal_Saved( void );

    void customEvent( QEvent * e );

private:
    QBoxLayout * mainLayout;
    QPushButton * btn_quit;
    QPushButton * btn_find;
    QPushButton * btn_view;
    QPushButton * btn_plot;
    QPushButton * btn_help;
    QPushButton * btn_about;
    QLineEdit * edt_name;
    QMenu * viewmenu;

    QWidget * Help_Window;
    QWidget * View_Infinite_Window;
    QWidget * View_Finite_Window;
public:
    QFindDlg * Find_Window;
    QPlotWnd * Plot_Window;
};

extern QStartDlg * p4startdlg;

#endif /* WIN_MAIN_H */
