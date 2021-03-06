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

#include <vector>

class QBoxLayout;
class QButtonGroup;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QString;

class P4FindDlg;

class P4ParamsDlg : public QWidget
{
    Q_OBJECT

  public:
    P4ParamsDlg(P4FindDlg *startwindow);
    ~P4ParamsDlg();

    void getDataFromDlg();
    void updateDlgData();

  private:
    P4FindDlg *parent_;

    QBoxLayout *mainLayout_;

    QRadioButton *btn_sepyes_;
    QRadioButton *btn_sepno_;
    QRadioButton *btn_num_;
    QRadioButton *btn_alg_;
    QSpinBox *spin_precis_;
    QSpinBox *spin_precis0_;
    QSpinBox *spin_level_;
    QSpinBox *spin_numlevel_;
    QSpinBox *spin_maxlevel_;
    QSpinBox *spin_weakness_;
    QSpinBox *spin_p_;
    QSpinBox *spin_q_;
    QLineEdit *edt_epsilon_;
    QLineEdit *edt_x0_;
    QLineEdit *edt_y0_;

    QLabel *lbl_x0_;
    QLabel *lbl_y0_;
    QLabel *lbl_p_;
    QLabel *lbl_q_;

    QButtonGroup *btngrp_numeric_;
    QButtonGroup *btngrp_testsep_;

    // FIXME raw pointers
    void setSpinBoxCommonValue(QSpinBox *, const std::vector<int> &val,
                               int minval);
    bool getSpinBoxCommonValue(const QSpinBox *, std::vector<int> &val);
    void setLineEditCommonValue(QLineEdit *le, const std::vector<QString> &val);
    bool getLineEditCommonValue(const QLineEdit *le,
                                std::vector<QString> &newval);

  public slots:
    void btn_num_toggled(bool);
    void btn_alg_toggled(bool);
    void btn_sepyes_toggled(bool);
    void btn_sepno_toggled(bool);
    //    void onLevelChange(int);
};
