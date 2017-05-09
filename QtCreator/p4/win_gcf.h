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

#ifndef WIN_GCF_H
#define WIN_GCF_H

#include "win_plot.h"
#include "win_sphere.h"

#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>

class QGcfDlg : public QWidget
{
    Q_OBJECT

  public:
    QGcfDlg(QPlotWnd *, QWinSphere *);
    void Reset(void);
    void finishGcfEvaluation(void);

  private:
    QWinSphere *mainSphere;
    QPlotWnd *plotwnd;

    QPushButton *btn_evaluate;

    QRadioButton *btn_dots;
    QRadioButton *btn_dashes;
    QLineEdit *edt_points;
    QLineEdit *edt_precis;
    QLineEdit *edt_memory;

    QBoxLayout *mainLayout;

    void ExclusiveToggle(bool, QRadioButton *, QRadioButton *);

    int evaluating_points;
    int evaluating_memory;
    int evaluating_precision;

  public slots:
    void onbtn_evaluate(void);
    void btn_dots_toggled(bool);
    void btn_dashes_toggled(bool);
};

#endif /* WIN_GCF_H */
