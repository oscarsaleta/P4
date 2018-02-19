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

class QWinSphere;
class P4PlotWnd;

class QPushButton;
class QLineEdit;
class QLabel;
class QBoxLayout;

extern QString g_CurrentSingularityInfo[];
extern double g_CurrentSeparatriceEpsilon;

class P4SepDlg : public QWidget
{
    Q_OBJECT

   public:
    P4SepDlg(P4PlotWnd *, QWinSphere *);
    void reset(void);

   private:
    std::shared_ptr<QWinSphere> mainSphere_;
    std::shared_ptr<P4PlotWnd> plotwnd_;

    std::unique_ptr<QPushButton> btn_selectnext_;
    std::unique_ptr<QPushButton> btn_intnext_;
    std::unique_ptr<QPushButton> btn_start_;
    std::unique_ptr<QPushButton> btn_cont_;
    std::unique_ptr<QLineEdit> edt_epsilon_;
    std::unique_ptr<QLabel> lbl_info_[4];

    std::unique_ptr<QBoxLayout> mainLayout_;

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
