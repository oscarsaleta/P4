/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
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

#ifndef WIN_PARAMS_H
#define WIN_PARAMS_H

#include <QWidget>

class QFindDlg;

class QBoxLayout;
class QRadioButton;
class QSpinBox;
class QLineEdit;
class QLabel;

class QParamsDlg : public QWidget
{
    Q_OBJECT

  public:
    QParamsDlg(QFindDlg *startwindow);
    ~QParamsDlg();
    void getDataFromDlg(void);
    void updateDlgData(void);

  private:
    QFindDlg *parent_;
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

  public slots:
    void btn_num_toggled(bool);
    void btn_alg_toggled(bool);
    void btn_sepyes_toggled(bool);
    void btn_sepno_toggled(bool);
    void onLevelChange(int);
};

#endif /* WIN_PARAMS_H */
