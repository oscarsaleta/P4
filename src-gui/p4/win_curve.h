/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
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
    void Reset(void);
    void finishCurveEvaluation(void);

  private:
    QWinSphere *mainSphere;
    QPlotWnd *plotwnd;

    QPushButton *btn_evaluate;
    QPushButton *btn_plot;
    QPushButton *btn_delete;

    QRadioButton *btn_dots;
    QRadioButton *btn_dashes;
    QLineEdit *edt_curve;
    QLineEdit *edt_points;
    QLineEdit *edt_memory;

    QBoxLayout *mainLayout;

    int evaluating_points;
    int evaluating_memory;

  public slots:
    void onbtn_evaluate(void);
    void onbtn_plot(void);
    void onbtn_delete(void);
};

#endif /* WIN_CURVE_H */
