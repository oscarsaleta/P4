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
class QRadioButton;
class QSpinBox;
class QLineEdit;
class QLabel;
class QButtonGroup;

class P4ParamsDlg : public QWidget
{
    Q_OBJECT

  public:
    P4ParamsDlg(std::shared_ptr<P4FindDlg> startwindow);
    ~P4ParamsDlg();
    void getDataFromDlg();
    void updateDlgData();

  private:
    std::shared_ptr<P4FindDlg> parent_;

    std::unique_ptr<QBoxLayout> mainLayout_;
    std::unique_ptr<QRadioButton> btn_sepyes_;
    std::unique_ptr<QRadioButton> btn_sepno_;
    std::unique_ptr<QRadioButton> btn_num_;
    std::unique_ptr<QRadioButton> btn_alg_;
    std::unique_ptr<QSpinBox> spin_precis_;
    std::unique_ptr<QSpinBox> spin_precis0_;
    std::unique_ptr<QSpinBox> spin_level_;
    std::unique_ptr<QSpinBox> spin_numlevel_;
    std::unique_ptr<QSpinBox> spin_maxlevel_;
    std::unique_ptr<QSpinBox> spin_weakness_;
    std::unique_ptr<QSpinBox> spin_p_;
    std::unique_ptr<QSpinBox> spin_q_;
    std::unique_ptr<QLineEdit> edt_epsilon_;
    std::unique_ptr<QLineEdit> edt_x0_;
    std::unique_ptr<QLineEdit> edt_y0_;

    std::unique_ptr<QLabel> lbl_x0_;
    std::unique_ptr<QLabel> lbl_y0_;
    std::unique_ptr<QLabel> lbl_p_;
    std::unique_ptr<QLabel> lbl_q_;

    std::unique_ptr<QButtonGroup> btngrp_numeric_;
    std::unique_ptr<QButtonGroup> btngrp_testsep_;

    void setSpinBoxCommonValue(std::unique_ptr<QSpinBox> &sb,
                               const std::vector<int> &val, int minval);
    void getSpinBoxCommonValue(std::unique_ptr<QSpinBox> &sb,
                               std::vector<int> &val);
    void setLineEditCommonValue(std::unique_ptr<QLineEdit> &le,
                                const std::vector<QString> &val);
    bool getLineEditCommonValue(std::unique_ptr<QLineEdit> &le,
                                std::vector<QString> &newval);

  public slots:
    void btn_num_toggled(bool);
    void btn_alg_toggled(bool);
    void btn_sepyes_toggled(bool);
    void btn_sepno_toggled(bool);
    void onLevelChange(int);
};
