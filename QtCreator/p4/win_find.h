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

#ifndef WIN_FIND_H
#define WIN_FIND_H


#include "win_main.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>


/* Forward-declaration to solve cross-include problems */
class QParamsDlg;       // in win_params.h
class QVectorFieldDlg;  // in win_vf.h


class QFindDlg : public QWidget
{
    Q_OBJECT

public:
    QFindDlg( QStartDlg * startwindow );
    ~QFindDlg();
    void GetDataFromDlg( void );
    void ExclusiveToggle( bool, QRadioButton *, ... );

private:
    QStartDlg * parent;

    QBoxLayout * mainLayout;
    QBoxLayout * superLayout;

    QRadioButton * btn_maple;
    QRadioButton * btn_reduce;
    QRadioButton * btn_actionrun;
    QRadioButton * btn_actionprep;
    QRadioButton * btn_all;
    QRadioButton * btn_fin;
    QRadioButton * btn_inf;
    QRadioButton * btn_one;
    QRadioButton * btn_yes;
    QRadioButton * btn_no;
    QPushButton * btn_params;
    QPushButton * btn_vf;
    QPushButton * btn_load;
    QPushButton * btn_save;
    QPushButton * btn_eval;

    QVectorFieldDlg * Vf_Window;
    QParamsDlg * Params_Window;

public slots:
    void btn_maple_toggled( bool );
    void btn_reduce_toggled( bool );
    void btn_actionrun_toggled( bool );
    void btn_actionprep_toggled( bool );
    void btn_all_toggled( bool );
    void btn_fin_toggled( bool );
    void btn_inf_toggled( bool );
    void btn_one_toggled( bool );
    void btn_yes_toggled( bool );
    void btn_no_toggled( bool );
    void UpdateDlgData( void );

    void Signal_Evaluating( void );
    void Signal_Evaluated( void );

    void OnBtnParams( void );
    void OnBtnVf( void );
    void OnBtnLoad( void );
    void OnBtnSave( void );
    void OnBtnEval( void );
};


#endif /* WIN_FIND_H */
