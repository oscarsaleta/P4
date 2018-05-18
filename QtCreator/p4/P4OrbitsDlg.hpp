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
class QLineEdit;
class QPushButton;

class P4WinSphere;
class P4PlotWnd;

class P4OrbitsDlg : public QWidget
{
    Q_OBJECT

  public:
    P4OrbitsDlg(P4PlotWnd *, P4WinSphere *);

    void reset();

  private:
    P4PlotWnd *plotWnd_;
    P4WinSphere *mainSphere_;

    QPushButton *btnForwards_;
    QPushButton *btnContinue_;
    QPushButton *btnBackwards_;
    QPushButton *btnDelAll_;
    QPushButton *btnDelLast_;
    QPushButton *btnSelect_;

    QLineEdit *edt_x0_;
    QLineEdit *edt_y0_;

    QBoxLayout *mainLayout_;

    double selected_x0_{0};
    double selected_y0_{0};

    bool orbitStarted_{false};
    bool orbitSelected_{false};

  public slots:
    void orbitEvent(int);
    void onBtnSelect();
    void onBtnBackwards();
    void onBtnContinue();
    void onBtnForwards();
    void onBtnDelAll();
    void onBtnDelLast();

    void setInitialPoint(double, double);
};
