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

#include <QMainWindow>

class QBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class P4FindDlg;
class P4ViewDlg;
class P4InputSphere;

class P4SeparatingCurvesDlg : public QMainWindow
{
    Q_OBJECT

  public:
    P4SeparatingCurvesDlg(P4FindDlg *startwindow);

    void updateDlgData();
    void signalSeparatingCurvesEvaluated();

    void makeZoom(double, double, double, double);
    void onMouseClickLeft(double, double, double);
    void onMouseClickRight(double, double, double);

  private:
    P4FindDlg *parent_;
    QWidget *mainWidget_;

    QBoxLayout *mainLayout_;

    QLabel *lbl_status_;
    QLabel *lbl_vf_or_curves_;
    QLabel *lbl_info_;

    P4ViewDlg *viewParamsWindow_;

    QPushButton *btn_add_;
    QPushButton *btn_del_;
    QPushButton *btn_edit_;
    QPushButton *btn_refresh_;
    QPushButton *btn_eval_;
    QPushButton *btn_mark_;
    QPushButton *btn_unmark_;
    QPushButton *btn_resetmarks_;
    QPushButton *btn_view_;
    QPushButton *btn_zoomout_;

    QListWidget *lst_curves_;
    QListWidget *lst_vfs_;

    P4InputSphere *isphere_;

    QLineEdit *edt_numpoints_;

    bool markingvf_;
    bool marking_;

  public slots:
    void onBtnAdd();
    void onBtnDel();
    void onBtnEdit();
    void onNumpointsEditingFinished();

    void onBtnMarkToggled(bool);
    void onBtnUnmarkToggled(bool);
    void onBtnResetMarks();
    void onBtnEval();
    void onBtnRefresh();

    void onCurveChanged(QListWidgetItem *);
    void onCurvesSelectionChanged();
    void onVfsSelectionChanged();
};
