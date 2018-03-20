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

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

#include <memory>

#include "P4WinInputSphere.hpp"

class QListWidgetItem;

class P4FindDlg;
class P4ViewDlg;

class P4SeparatingCurvesDlg : public QWidget
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

    std::unique_ptr<QBoxLayout> mainLayout_;

    std::unique_ptr<QLabel> lbl_status_;
    std::unique_ptr<QLabel> lbl_vf_or_curves_;
    std::unique_ptr<QLabel> lbl_info_;
    std::unique_ptr<QLabel> lbl_numpoints_;

    std::unique_ptr<P4ViewDlg> viewParamsWindow_;

    std::unique_ptr<QPushButton> btn_add_;
    std::unique_ptr<QPushButton> btn_del_;
    std::unique_ptr<QPushButton> btn_edit_;
    std::unique_ptr<QPushButton> btn_refresh_;
    std::unique_ptr<QPushButton> btn_eval_;
    std::unique_ptr<QPushButton> btn_mark_;
    std::unique_ptr<QPushButton> btn_unmark_;
    std::unique_ptr<QPushButton> btn_resetmarks_;
    std::unique_ptr<QPushButton> btn_view_;
    std::unique_ptr<QPushButton> btn_zoomout_;

    std::unique_ptr<QListWidget> lst_curves_;
    std::unique_ptr<QListWidget> lst_vfs_;

    std::unique_ptr<P4WinInputSphere> isphere_;

    std::unique_ptr<QLineEdit> edt_numpoints_;

    bool markingvf_;
    bool marking_;

  public slots:
    void onBtnZoomOut();
    void onBtnAdd();
    void onBtnDel();
    void onBtnEdit();
    void onNumpointsEditingFinished();

    void onBtnMarkToggled(bool);
    void onBtnUnmarkToggled(bool);
    void onBtnResetMarks();
    void onBtnView();
    void onBtnEval();
    void onBtnRefresh();

    void onCurvesClicked(QListWidgetItem *);
    void onVfsClicked(QListWidgetItem *);
    void onVfsItemActivated(QListWidgetItem *);
    void onCurvesItemActivated(QListWidgetItem *);
    void onCurveChanged(QListWidgetItem *);
    void onCurvesSelectionChanged();
    void onVfsSelectionChanged();
};
