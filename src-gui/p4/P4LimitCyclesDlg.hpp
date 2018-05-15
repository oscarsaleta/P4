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

#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>

#include <memory>

#include "custom.hpp"

class P4PlotWnd;
class P4WinSphere;

bool stop_search_limit();
void write_to_limit_window(double x, double y);

class P4LimitCyclesDlg : public QWidget
{
    Q_OBJECT

  public:
    P4LimitCyclesDlg(P4PlotWnd *, P4WinSphere *);
    void reset();
    void setSection(double, double, double, double);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

  private:
    P4PlotWnd *plotwnd_;
    P4WinSphere *mainSphere_;

    std::unique_ptr<QPushButton> btn_start_;
    std::unique_ptr<QPushButton> btn_cancel_;
    std::unique_ptr<QPushButton> btn_delall_;
    std::unique_ptr<QPushButton> btn_dellast_;

    std::unique_ptr<QLineEdit> edt_x0_;
    std::unique_ptr<QLineEdit> edt_y0_;
    std::unique_ptr<QLineEdit> edt_x1_;
    std::unique_ptr<QLineEdit> edt_y1_;
    std::unique_ptr<QLineEdit> edt_grid_;

    std::unique_ptr<QLabel> lbl1_;
    std::unique_ptr<QLabel> lbl2_;
    std::unique_ptr<QLabel> lbl3_;
    std::unique_ptr<QLabel> lbl4_;
    std::unique_ptr<QLabel> lbl5_;
    std::unique_ptr<QLabel> lbl6_;

    std::unique_ptr<QGridLayout> lay00_;
    std::unique_ptr<QHBoxLayout> layout1_;
    std::unique_ptr<QHBoxLayout> layout2_;
    std::unique_ptr<QHBoxLayout> layout3_;
    std::unique_ptr<QHBoxLayout> layout4_;

    std::unique_ptr<QSpinBox> spin_numpoints_;

    std::unique_ptr<QBoxLayout> mainLayout_;

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
