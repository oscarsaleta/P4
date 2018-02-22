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

class P4PlotWnd;
class P4WinSphere;

class QPushButton;
class QRadioButton;
class QLineEdit;
class QBoxLayout;

class P4IsoclinesDlg : public QWidget
{
    Q_OBJECT

   public:
    P4IsoclinesDlg(std::shared_ptr<P4PlotWnd> , std::shared_ptr<P4WinSphere> );
    void reset();
    void finishIsoclinesEvaluation();

   private:
    std::shared_ptr<P4WinSphere> mainSphere_;
    std::shared_ptr<P4PlotWnd> plotwnd_;

    std::unique_ptr<QPushButton> btnEvaluate_;
    std::unique_ptr<QPushButton> btnPlot_;
    std::unique_ptr<QPushButton> btnDelAll_;
    std::unique_ptr<QPushButton> btnDelLast_;

    std::unique_ptr<QRadioButton> btn_dots_;
    std::unique_ptr<QRadioButton> btn_dashes_;
    std::unique_ptr<QLineEdit> edt_value_;
    std::unique_ptr<QLineEdit> edt_points_;
    std::unique_ptr<QLineEdit> edt_precis_;
    std::unique_ptr<QLineEdit> edt_memory_;

    std::unique_ptr<QBoxLayout> mainLayout_;

    int evaluating_points_;
    int evaluating_memory_;
    int evaluating_precision_;

    void setValue(double v);

   public slots:
    void onBtnEvaluate();
    void onBtnPlot();
    void onBtnDelAll();
    void onBtnDelLast();
};

#endif /* WIN_ISOCLINES_H */
