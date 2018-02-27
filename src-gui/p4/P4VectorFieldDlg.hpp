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

#include "custom.hpp"

#include <QString>
#include <QWidget>

#include <memory>

class P4FindDlg;
class P4VFParams;

class QBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QScrollBar;
class QSpinBox;

class P4VectorFieldDlg : public QWidget
{
    Q_OBJECT

  public:
    P4VectorFieldDlg(std::shared_ptr<P4FindDlg> startwindow);
    ~P4VectorFieldDlg();

    void getDataFromDlg();
    void updateDlgData();
    bool getLineEditCommonValue(QLineEdit *, const std::vector<QString> &);
    bool setLineEditCommonValue(QLineEdit *, const std::vector<QString> &);

  private:
    std::shared_ptr<P4FindDlg> parent_;
    std::unique_ptr<P4VFParams> params_;

    std::unique_ptr<QBoxLayout> mainLayout_;
    std::unique_ptr<QLineEdit> edt_xprime_;
    std::unique_ptr<QLineEdit> edt_yprime_;
    std::unique_ptr<QLineEdit> edt_gcf_;
    std::unique_ptr<QSpinBox> spin_numparams_;
    std::unique_ptr<QHBoxLayout> paramLayout_;
    std::shared_ptr<QScrollBar> sb_params_;

  public slots:
    void numParamsChanged(int);
    void editingFinished();
};
