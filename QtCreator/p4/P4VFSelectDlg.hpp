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
class QComboBox;
class QPushButton;

class P4FindDlg;
class P4SeparatingCurvesDlg;

class P4VFSelectDlg : public QWidget
{
    Q_OBJECT

  public:
    P4VFSelectDlg(P4FindDlg *startwindow);

    void updateDlgData();

    void closeConfigWindow();
    bool checkPlotWindowClosed();
    P4SeparatingCurvesDlg *getConfigWindowPtr() const;

  public slots:
    void onBtnAdd();
    void onBtnDel();
    void onBtnPrev();
    void onBtnNext();
    void onBtnP4Config();
    void onVfSelectionChanged(int);

  private:
    P4FindDlg *parent_;
    P4SeparatingCurvesDlg *win_curves_{nullptr};

    QBoxLayout *mainLayout_;

    QComboBox *cbb_vfselect_;
    QPushButton *btn_add_;
    QPushButton *btn_del_;
    QPushButton *btn_prev_;
    QPushButton *btn_next_;
    QPushButton *btn_p4config_;
};
