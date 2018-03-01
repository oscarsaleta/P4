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

class P4FindDlg;

class QBoxLayout;
class QPushButton;
class QRadioButton;
class QLineEdit;
class QLabel;
class QSpinBox;
class QString;

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

    std::unique_ptr<QBoxLayout> mainLayout_;

    std::unique_ptr<QPushButton> btn_reset_;
    std::unique_ptr<QRadioButton> btn_org_;
    std::unique_ptr<QRadioButton> btn_red_;
    std::unique_ptr<QRadioButton> btn_dots_;
    std::unique_ptr<QRadioButton> btn_dashes_;
    std::unique_ptr<QLineEdit> edt_minstep_;
    std::unique_ptr<QLineEdit> edt_branchminstep_;
    std::unique_ptr<QLineEdit> edt_maxstep_;
    std::unique_ptr<QLineEdit> edt_stepsize_;
    std::unique_ptr<QLabel> lbl_curstep_;
    std::unique_ptr<QLineEdit> edt_tolerance_;

    std::unique_ptr<QLabel> lbl_minstep_;
    std::unique_ptr<QLabel> lbl_maxstep_;
    std::unique_ptr<QLabel> lbl_branchminstep_;
    std::unique_ptr<QLabel> lbl_stepsize_;
    std::unique_ptr<QLabel> lbl0_curstep_;
    std::unique_ptr<QLabel> lbl_tolerance_;

    std::unique_ptr<QSpinBox> spin_numpoints_;

    bool readFloatField(QLineEdit *, double *, double, double, double);

  public slots:
    void markBad(QLineEdit *edt);
    void on_btn_reset();
};
