/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4
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

#ifndef WIN_ORBITS_H
#define WIN_ORBITS_H


#include "win_plot.h"
#include "win_sphere.h"

#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>


class QOrbitsDlg : public QWidget
{
    Q_OBJECT

public:
    QOrbitsDlg( QPlotWnd *, QWinSphere * );
    void Reset( void );

private:
    QWinSphere * mainSphere;
    QPlotWnd * plotwnd;

    QPushButton * btn_forwards;
    QPushButton * btn_continue;
    QPushButton * btn_backwards;
    QPushButton * btn_delall;
    QPushButton * btn_dellast;
    QPushButton * btn_select;

    QLineEdit * edt_x0;
    QLineEdit * edt_y0;

    QBoxLayout * mainLayout;

    double selected_x0;
    double selected_y0;

    bool orbit_started;
    bool orbit_selected;

public slots:
    void OrbitEvent( int );
    void onbtn_select( void );
    void onbtn_backwards( void );
    void onbtn_continue( void );
    void onbtn_forwards( void );
    void onbtn_delall( void );
    void onbtn_dellast( void );

    void setInitialPoint( double, double );
};


#endif /* WIN_ORBITS_H */
