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

#ifndef WIN_SEPARATRICES_H
#define WIN_SEPARATRICES_H

#include "win_plot.h"
#include "win_sphere.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

extern QString g_CurrentSingularityInfo[];
extern double g_CurrentSeparatriceEpsilon;

class QSepDlg : public QWidget
{
    Q_OBJECT

  public:
    QSepDlg(QPlotWnd *, QWinSphere *);
    void reset(void);

  private:
    QWinSphere *mainSphere_;
    QPlotWnd *plotwnd_;

    QPushButton *btn_selectnext;
    QPushButton *btn_intnext;
    QPushButton *btn_start;
    QPushButton *btn_cont;
    QLineEdit *edt_epsilon;
    QLabel *lbl_info[4];

    QBoxLayout *mainLayout_;

    bool started;
    bool selected;

  public slots:
    void onbtn_selectnext(void);
    void onbtn_intnext(void);
    void onbtn_cont(void);
    void onbtn_start(void);

    void setInitialPoint(void);
    void SepEvent(int);
    void onepsilon_enter(void);
    void markBad(QLineEdit *);
};

#endif /* WIN_SEPARATRICES_H */
