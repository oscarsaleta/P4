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
#include <QPushButton>
#include <QRadioButton>

#include <memory>

#include "P4ParamsDlg.hpp"
#include "P4VFSelectDlg.hpp"
#include "P4VectorFieldDlg.hpp"

class P4StartDlg;

class P4FindDlg : public QWidget
{
    Q_OBJECT

  public:
    P4FindDlg(P4StartDlg *startwindow);

    void getDataFromDlg();

    P4VFSelectDlg *getVfSelectWindowPtr() const;
    P4StartDlg *getParentPtr() const;

  private:
    P4StartDlg *parent_;

    std::unique_ptr<QBoxLayout> mainLayout_;
    std::unique_ptr<QBoxLayout> parLayout_;
    std::unique_ptr<QBoxLayout> superLayout_;

    std::unique_ptr<QRadioButton> btn_actionrun_;
    std::unique_ptr<QRadioButton> btn_actionprep_;
    std::unique_ptr<QRadioButton> btn_all_;
    std::unique_ptr<QRadioButton> btn_fin_;
    std::unique_ptr<QRadioButton> btn_inf_;
    std::unique_ptr<QRadioButton> btn_one_;
    std::unique_ptr<QRadioButton> btn_yes_;
    std::unique_ptr<QRadioButton> btn_no_;
    std::unique_ptr<QPushButton> btn_load_;
    std::unique_ptr<QPushButton> btn_save_;
    std::unique_ptr<QPushButton> btn_eval_;

    std::unique_ptr<P4VectorFieldDlg> vfWindow_;
    std::unique_ptr<P4ParamsDlg> paramsWindow_;
    std::unique_ptr<P4VFSelectDlg> vfSelectWindow_;

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
