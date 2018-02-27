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

#include <memory>

class P4PlotWnd;
class P4WinSphere;

class QPushButton;
class QLineEdit;
class QSpinBox;
class QBoxLayout;

bool stop_search_limit(void);
void write_to_limit_window(double x, double y);

class P4LimitCyclesDlg : public QWidget
{
    Q_OBJECT

  public:
    P4LimitCyclesDlg(std::unique_ptr<P4PlotWnd>, std::unique_ptr<P4WinSphere>);
    void reset();
    void setSection(double, double, double, double);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

  private:
    std::unique_ptr<P4WinSphere> mainSphere_;
    std::unique_ptr<P4PlotWnd> plotwnd_;

    std::unique_ptr<QPushButton> btn_start_;
    std::unique_ptr<QPushButton> btn_cancel_;
    std::unique_ptr<QPushButton> btn_delall_;
    std::unique_ptr<QPushButton> btn_dellast_;

    std::unique_ptr<QLineEdit> edt_x0_;
    std::unique_ptr<QLineEdit> edt_y0_;
    std::unique_ptr<QLineEdit> edt_x1_;
    std::unique_ptr<QLineEdit> edt_y1_;
    std::unique_ptr<QLineEdit> edt_grid_;

    std::unique_ptr<QSpinBox> spin_numpoints_;

    std::unique_ptr<QBoxLayout> mainLayout_;

    double selected_x0_;
    double selected_y0_;
    double selected_x1_;
    double selected_y1_;
    double selected_grid_;
    int selected_numpoints_;

  public slots:
    void onbtn_start();
    void onbtn_cancel();
    void onbtn_delall();
    void onbtn_dellast();
};
