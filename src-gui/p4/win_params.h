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

#ifndef WIN_PARAMS_H
#define WIN_PARAMS_H


#include "win_find.h"

#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QWidget>


class QParamsDlg : public QWidget
{
    Q_OBJECT

public:
    QParamsDlg( QFindDlg * startwindow );
    ~QParamsDlg();
    void GetDataFromDlg( void );
    void UpdateDlgData( void );
    void ExclusiveToggle( bool, QRadioButton *, ... );

private:
    QFindDlg * parent;
    QBoxLayout * mainLayout;

    QRadioButton * btn_sepyes;
    QRadioButton * btn_sepno;
    QRadioButton * btn_num;
    QRadioButton * btn_alg;
    QSpinBox * spin_precis;
    QSpinBox * spin_precis0;
    QSpinBox * spin_level;
    QSpinBox * spin_numlevel;
    QSpinBox * spin_maxlevel;
    QSpinBox * spin_weakness;
    QSpinBox * spin_p;
    QSpinBox * spin_q;
    QLineEdit * edt_epsilon;
    QLineEdit * edt_x0;
    QLineEdit * edt_y0;

    QLabel * lbl_x0;
    QLabel * lbl_y0;
    QLabel * lbl_p;
    QLabel * lbl_q;

public slots:
    void btn_num_toggled( bool );
    void btn_alg_toggled( bool );
    void btn_sepyes_toggled( bool );
    void btn_sepno_toggled( bool );
    void OnLevelChange( int );
};


#endif /* WIN_PARAMS_H */
