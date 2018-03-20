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
#include <QRadioButton>

#include <memory>

class P4PlotWnd;
class P4WinSphere;

class P4ArbitraryCurveDlg : public QWidget
{
    Q_OBJECT

  public:
    P4ArbitraryCurveDlg(P4PlotWnd *, P4WinSphere *);
    void reset();
    void finishArbitraryCurveEvaluation();

  private:
    P4WinSphere *mainSphere_;
    P4PlotWnd *plotwnd_;

    std::unique_ptr<QPushButton> btnEvaluate_;
    std::unique_ptr<QPushButton> btnPlot_;
    std::unique_ptr<QPushButton> btnDelLast_;
    std::unique_ptr<QPushButton> btnDelAll_;

    std::unique_ptr<QRadioButton> btn_dots_;
    std::unique_ptr<QRadioButton> btn_dashes_;
    std::unique_ptr<QLineEdit> edt_curve_;
    std::unique_ptr<QLineEdit> edt_points_;
    std::unique_ptr<QLineEdit> edt_precis_;
    std::unique_ptr<QLineEdit> edt_memory_;

    std::unique_ptr<QBoxLayout> mainLayout_;

    int evaluating_points_{false};
    int evaluating_memory_{false};
    int evaluating_precision_{false};

  public slots:
    void onBtnEvaluate();
    void onBtnPlot();
    void onBtnDelAll();
    void onBtnDelLast();
};
