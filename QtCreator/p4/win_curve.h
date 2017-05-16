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

#ifndef WIN_CURVE_H
#define WIN_CURVE_H

#include "win_plot.h"
#include "win_sphere.h"

#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>

class QCurveDlg : public QWidget
{
    Q_OBJECT

  public:
    QCurveDlg(QPlotWnd *, QWinSphere *);
    void reset(void);
    void finishCurveEvaluation(void);

  private:
    QWinSphere *mainSphere_;
    QPlotWnd *plotwnd_;

    QPushButton *btn_evaluate_;
    QPushButton *btn_plot_;
    QPushButton *btn_delete_;

    QRadioButton *btn_dots_;
    QRadioButton *btn_dashes_;
    QLineEdit *edt_curve_;
    QLineEdit *edt_points_;
    QLineEdit *edt_precis_;
    QLineEdit *edt_memory_;

    QBoxLayout *mainLayout_;

    int evaluating_points_;
    int evaluating_memory_;
    int evaluating_precision_;

  public slots:
    void onbtn_evaluate(void);
    void onbtn_plot(void);
    void onbtn_delete(void);
};

#endif /* WIN_CURVE_H */
