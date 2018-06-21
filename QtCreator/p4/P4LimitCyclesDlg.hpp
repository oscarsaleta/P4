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

#include "custom.hpp"

class QBoxLayout;
class QLineEdit;
class QPushButton;
class QSpinBox;

class P4PlotWnd;
class P4Sphere;

bool stop_search_limit();
void write_to_limit_window();

class P4LimitCyclesDlg : public QWidget
{
    Q_OBJECT

  public:
    P4LimitCyclesDlg(P4PlotWnd *, P4Sphere *);
    void reset();
    void setSection(double, double, double, double);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

  private:
    P4PlotWnd *plotwnd_;
    P4Sphere *mainSphere_;

    QPushButton *btn_start_;
    QPushButton *btn_cancel_;
    QPushButton *btn_delall_;
    QPushButton *btn_dellast_;

    QLineEdit *edt_x0_;
    QLineEdit *edt_y0_;
    QLineEdit *edt_x1_;
    QLineEdit *edt_y1_;
    QLineEdit *edt_grid_;

    QSpinBox *spin_numpoints_;

    QBoxLayout *mainLayout_;

    double selected_x0_{0};
    double selected_y0_{0};
    double selected_x1_{0};
    double selected_y1_{0};
    double selected_grid_{DEFAULT_LCGRID};
    int selected_numpoints_{DEFAULT_LCPOINTS};

  public slots:
    void onbtn_start();
    void onbtn_cancel();
    void onbtn_delall();
    void onbtn_dellast();
};
