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

class QGridLayout;
class QBoxLayout;
class QHBoxLayout;
class QButtonGroup;
class QLabel;
class QRadioButton;
class QPushButton;

class P4ParamsDlg;
class P4SeparatingCurvesDlg;
class P4StartDlg;
class P4VFSelectDlg;
class P4VectorFieldDlg;

class P4FindDlg : public QWidget
{
    Q_OBJECT

  public:
    P4FindDlg(P4StartDlg *startwindow);

    void getDataFromDlg();

    P4VFSelectDlg *getVfSelectWindowPtr() const;
    P4StartDlg *getParentPtr() const;
    P4SeparatingCurvesDlg *getPiecewiseConfigWindowPtr() const;
    void createPiecewiseConfigWindow();
    void closePiecewiseConfigWindow();

  private:
    P4StartDlg *parent_;

    QGridLayout *superLayout_;
    QBoxLayout *mainLayout_;
    QBoxLayout *parLayout_;
    QBoxLayout *vfLayout_;
    QBoxLayout *vfSelectLayout_;

    QHBoxLayout *actLayout_;
    QHBoxLayout *singlineLayout_;
    QGridLayout *singLayout_;
    QHBoxLayout *layout0_;
    QGridLayout *layout1_;

    QButtonGroup *btngrp1_;
    QButtonGroup *btngrp2_;
    QButtonGroup *btngrp3_;

    QLabel *p4title_;
    QLabel *actlabel_;
    QLabel *singpoints_;
    QLabel *saveall_;

    QRadioButton *btn_actionrun_;
    QRadioButton *btn_actionprep_;
    QRadioButton *btn_all_;
    QRadioButton *btn_fin_;
    QRadioButton *btn_inf_;
    QRadioButton *btn_one_;
    QRadioButton *btn_yes_;
    QRadioButton *btn_no_;
    QPushButton *btn_load_;
    QPushButton *btn_save_;
    QPushButton *btn_eval_;

    P4VectorFieldDlg *vfWindow_ = nullptr;
    P4ParamsDlg *paramsWindow_ = nullptr;
    P4VFSelectDlg *vfSelectWindow_ = nullptr;
    P4SeparatingCurvesDlg *piecewiseConfigWindow_ = nullptr;

    // void saveSettings();
    // void readSettings();

  public slots:
    void updateDlgData();
    void signalEvaluating();
    void signalEvaluated();
    void signalSeparatingCurvesEvaluated();
    void onBtnLoad();
    void onBtnSave();
    void onBtnEval();
    void onSaveSignal();
};
