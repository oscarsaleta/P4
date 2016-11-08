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

#ifndef WIN_LIMITCYCLES_H
#define WIN_LIMITCYCLES_H


#include "win_plot.h"
#include "win_sphere.h"

#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>


bool stop_search_limit( void );
void write_to_limit_window( double x, double y );

class QLimitCyclesDlg : public QWidget
{
    Q_OBJECT

public:
    QLimitCyclesDlg( QPlotWnd *, QWinSphere * );
    void Reset( void );
    void setSection( double, double, double, double );
    void showEvent( QShowEvent * );
    void hideEvent( QHideEvent * );

private:
    QWinSphere * mainSphere;
    QPlotWnd * plotwnd;

    QPushButton * btn_start;
    QPushButton * btn_cancel;
    QPushButton * btn_delall;
    QPushButton * btn_dellast;

    QLineEdit * edt_x0;
    QLineEdit * edt_y0;
    QLineEdit * edt_x1;
    QLineEdit * edt_y1;
    QLineEdit * edt_grid;

    QSpinBox * spin_numpoints;

    QBoxLayout * mainLayout;

    double selected_x0;
    double selected_y0;
    double selected_x1;
    double selected_y1;
    double selected_grid;
    int selected_numpoints;

public slots:
    void onbtn_start( void );
    void onbtn_cancel( void );
    void onbtn_delall( void );
    void onbtn_dellast( void );
};


#endif
