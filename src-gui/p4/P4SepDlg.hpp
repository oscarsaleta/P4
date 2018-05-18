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

class P4WinSphere;
class P4PlotWnd;

extern QString gCurrentSingularityInfo[];
extern double gCurrentSeparatriceEpsilon;

class P4SepDlg : public QWidget
{
    Q_OBJECT

  public:
    P4SepDlg(P4PlotWnd *, P4WinSphere *);
    void reset();

  private:
    P4PlotWnd *plotWnd_;
    P4WinSphere *mainSphere_;

    QPushButton *btn_selectnext_;
    QPushButton *btn_intnext_;
    QPushButton *btn_start_;
    QPushButton *btn_cont_;
    QLineEdit *edt_epsilon_;
    QLabel *lbl1_;
    QLabel *lbl_info_0_;
    QLabel *lbl_info_1_;
    QLabel *lbl_info_2_;
    QLabel *lbl_info_3_;

    QBoxLayout *mainLayout_;

    bool started_;
    bool selected_;

  public slots:
    void onbtn_selectnext();
    void onbtn_intnext();
    void onbtn_cont();
    void onbtn_start();

    void setInitialPoint();
    void sepEvent(int);
    void onepsilon_enter();
    void markBad(QLineEdit *);
};
