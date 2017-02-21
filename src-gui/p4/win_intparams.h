/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  P4 is free software: you can redistribute it and/or modify
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

#ifndef WIN_INTPARAMS_H
#define WIN_INTPARAMS_H


#include "win_find.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QString>
#include <QWidget>


class QIntParamsDlg : public QWidget
{
    Q_OBJECT

public:
    QIntParamsDlg( void );
    ~QIntParamsDlg();
    void GetDataFromDlg( void );
    void UpdateDlgData( void );
    void ExclusiveToggle( bool, QRadioButton *, ... );
    void SetCurrentStep( double curstep );

private:
    bool changed;
    QFindDlg * parent;
    QBoxLayout * mainLayout;

    QPushButton * btn_reset;
    QRadioButton * btn_org;
    QRadioButton * btn_red;
    QRadioButton * btn_dots;
    QRadioButton * btn_dashes;
    QLineEdit * edt_minstep;
    QLineEdit * edt_maxstep;
    QLineEdit * edt_stepsize;
    QLabel * lbl_curstep;
    QLineEdit * edt_tolerance;

    QLabel * lbl_minstep;
    QLabel * lbl_maxstep;
    QLabel * lbl_stepsize;
    QLabel * lbl0_curstep;
    QLabel * lbl_tolerance;

    QSpinBox * spin_numpoints;
    bool readFloatField( QLineEdit *, double *, double, double, double );

public slots:
    void MarkBad( QLineEdit * edt );
    void OnFieldChange( const QString & dummy );
    void OnFieldChange( int dummy );
    void btn_org_toggled( bool );
    void on_btn_reset( void );
    void btn_red_toggled( bool );
    void btn_dots_toggled( bool );
    void btn_dashes_toggled( bool );
};


#endif /* WIN_INTPARAMS_H */
