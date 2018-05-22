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

#include <QString>

#include <vector>

#include "P4VFParams.hpp"
#include "custom.hpp"

class QBoxLayout;
class QLineEdit;
class QScrollBar;
class QSpinBox;

class P4FindDlg;

class P4VectorFieldDlg : public QWidget
{
    Q_OBJECT

  public:
    P4VectorFieldDlg(P4FindDlg *parent);
    ~P4VectorFieldDlg();

    void getDataFromDlg();
    void updateDlgData();
    bool getLineEditCommonValue(QLineEdit *, std::vector<QString> &);
    void setLineEditCommonValue(QLineEdit *, const std::vector<QString> &);

  private:
    P4FindDlg *parent_;
    P4VFParams *params_{nullptr};

    QBoxLayout *mainLayout_;

    QLineEdit *edt_xprime_;
    QLineEdit *edt_yprime_;
    QLineEdit *edt_gcf_;

    QSpinBox *spin_numparams_;
    QHBoxLayout *paramLayout_;
    QScrollBar *sb_params_{nullptr};

  public slots:
    void numParamsChanged(int);
    void editingFinished();
};
