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

#ifndef WIN_SEPARATRICES_H
#define WIN_SEPARATRICES_H

#include <QWidget>

class QWinSphere;
class QPlotWnd;

class QPushButton;
class QLineEdit;
class QLabel;
class QBoxLayout;

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

    QPushButton *btn_selectnext_;
    QPushButton *btn_intnext_;
    QPushButton *btn_start_;
    QPushButton *btn_cont_;
    QLineEdit *edt_epsilon_;
    QLabel *lbl_info_[4];

    QBoxLayout *mainLayout_;

    bool started_;
    bool selected_;

  public slots:
    void onbtn_selectnext(void);
    void onbtn_intnext(void);
    void onbtn_cont(void);
    void onbtn_start(void);

    void setInitialPoint(void);
    void sepEvent(int);
    void onepsilon_enter(void);
    void markBad(QLineEdit *);
};

#endif /* WIN_SEPARATRICES_H */
