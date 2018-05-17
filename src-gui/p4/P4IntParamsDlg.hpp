/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
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

#pragma once

#include <QWidget>

class QBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;

class P4IntParamsDlg : public QWidget
{
    Q_OBJECT

  public:
    P4IntParamsDlg();
    ~P4IntParamsDlg();

    void getDataFromDlg();
    void updateDlgData();
    void setCurrentStep(double curstep);

  private:
    bool changed_;

    QBoxLayout *mainLayout_;

    QPushButton *btn_reset_;
    QRadioButton *btn_org_;
    QRadioButton *btn_red_;
    QRadioButton *btn_dots_;
    QRadioButton *btn_dashes_;
    QLineEdit *edt_minstep_;
    QLineEdit *edt_branchminstep_;
    QLineEdit *edt_maxstep_;
    QLineEdit *edt_stepsize_;
    QLabel *lbl_curstep_;
    QLineEdit *edt_tolerance_;

    QLabel *lbl_minstep_;
    QLabel *lbl_maxstep_;
    QLabel *lbl_branchminstep_;
    QLabel *lbl_stepsize_;
    QLabel *lbl0_curstep_;
    QLabel *lbl_tolerance_;

    QSpinBox *spin_numpoints_;

    bool readFloatField(QLineEdit *, double &, double, double, double);

  public slots:
    void markBad(QLineEdit *edt);
    void on_btn_reset();
};
