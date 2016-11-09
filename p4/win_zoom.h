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

#ifndef WIN_ZOOM_H
#define WIN_ZOOM_H


#include "win_plot.h"
#include "win_sphere.h"

#include <QAction>
#include <QBoxLayout>
#include <QEvent>
#include <QHideEvent>
#include <QMainWindow>


class QZoomWnd : public QMainWindow
{
    Q_OBJECT

public:
    QZoomWnd( QPlotWnd *, int, double x1, double y1, double x2, double y2 );
    ~QZoomWnd();

    int zoomid;

private:
    QPlotWnd * parent;
    QBoxLayout * mainLayout;
    QAction * ActClose;
    QAction * ActRefresh;
    QAction * ActPrint;

    QWinSphere * sphere;                // main sphere

public slots:
    void Signal_Evaluating( void );
    void Signal_Evaluated( void );
    void Signal_Changed( void );

    void OnBtnClose( void );
    void OnBtnRefresh( void );
    void OnBtnPrint( void );
    bool close( void );

    void configure( void );
    void customEvent( QEvent * e );
    void hideEvent( QHideEvent * h );
    void AdjustHeight( void );
};


#endif /* WIN_ZOOM_H */
