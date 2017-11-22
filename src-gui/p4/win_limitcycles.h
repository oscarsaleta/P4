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

#ifndef WIN_LIMITCYCLES_H
#define WIN_LIMITCYCLES_H

#include <QWidget>

class QPlotWnd;
class QWinSphere;

class QPushButton;
class QLineEdit;
class QSpinBox;
class QBoxLayout;

bool stop_search_limit(void);
void write_to_limit_window(double x, double y);

class QLimitCyclesDlg : public QWidget
{
    Q_OBJECT

  public:
    QLimitCyclesDlg(QPlotWnd *, QWinSphere *);
    void reset(void);
    void setSection(double, double, double, double);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

  private:
    QWinSphere *mainSphere_;
    QPlotWnd *plotwnd_;

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

    double selected_x0_;
    double selected_y0_;
    double selected_x1_;
    double selected_y1_;
    double selected_grid_;
    int selected_numpoints_;

  public slots:
    void onbtn_start(void);
    void onbtn_cancel(void);
    void onbtn_delall(void);
    void onbtn_dellast(void);
};

#endif
