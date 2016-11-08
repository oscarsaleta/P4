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

#ifndef WIN_VIEW_H
#define WIN_VIEW_H


#include "win_find.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QString>
#include <QWidget>


class QViewDlg : public QWidget
{
    Q_OBJECT

public:
    QViewDlg(QWidget *);
    ~QViewDlg();
    bool GetDataFromDlg( void );
    void UpdateDlgData( void );
    void ExclusiveToggle( bool, QRadioButton *, ... );

private:
    bool changed;
    QFindDlg * parent;
    QBoxLayout * mainLayout;

    QRadioButton * btn_sphere;
    QRadioButton * btn_plane;
    QRadioButton * btn_U1;
    QRadioButton * btn_V1;
    QRadioButton * btn_U2;
    QRadioButton * btn_V2;
    QLineEdit * edt_projection;
    QLineEdit * edt_x0;
    QLineEdit * edt_x1;
    QLineEdit * edt_y0;
    QLineEdit * edt_y1;
    QPushButton * btn_square;

    QLabel * lbl_projection;
    QLabel * lbl_x0;
    QLabel * lbl_x1;
    QLabel * lbl_y0;
    QLabel * lbl_y1;

    QSpinBox * spin_numpoints;
    bool ReadFloatField( QLineEdit *, double *, double, double, double );

public slots:
    void MarkBad( QLineEdit * edt );
    void OnFieldChange( const QString & dummy );
    void btn_sphere_toggled( bool );
    void btn_plane_toggled( bool );
    void btn_square_clicked( void );
    void btn_U1_toggled( bool );
    void btn_U2_toggled( bool );
    void btn_V1_toggled( bool );
    void btn_V2_toggled( bool );
};


#endif /* WIN_VIEW_H */
