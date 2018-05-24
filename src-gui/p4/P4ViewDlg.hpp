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
class QCheckBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QString;

class P4ViewDlg : public QWidget
{
    Q_OBJECT

  public:
    P4ViewDlg(bool virtualchk = false, QWidget *parent = nullptr);
    ~P4ViewDlg();
    bool getDataFromDlg();
    void updateDlgData();

  private:
    bool changed_;
    bool haveVirtualCheckBox_;

    QBoxLayout *mainLayout_;

    QLineEdit *edt_projection_;
    QLineEdit *edt_x0_;
    QLineEdit *edt_x1_;
    QLineEdit *edt_y0_;
    QLineEdit *edt_y1_;
    QCheckBox *chk_plotvirtuals_;
    QPushButton *btn_square_;

    QRadioButton *btn_sphere_;
    QRadioButton *btn_plane_;
    QRadioButton *btn_U1_;
    QRadioButton *btn_V1_;
    QRadioButton *btn_U2_;
    QRadioButton *btn_V2_;

    bool readFloatField(QLineEdit *, double &, double, double, double);

  public slots:
    void markBad(QLineEdit *edt);
    void onFieldChange();
    void btn_sphere_toggled();
    void btn_plane_toggled();
    void btn_U1_toggled();
    void btn_U2_toggled();
    void btn_V1_toggled();
    void btn_V2_toggled();
    void btn_square_clicked();
    void plotvirtuals_stateChanged(int);
};
