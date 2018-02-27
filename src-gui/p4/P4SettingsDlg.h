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

#include <QDialog>

#include <memory>

class QBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QString;
class QWidget;

class P4SettingsDlg : public QDialog
{
    Q_OBJECT

  public:
    P4SettingsDlg(QWidget *, Qt::WindowFlags);
    void browseForExistingPathOrFile(QLineEdit *, QString, bool);

  private:
    std::unique_ptr<QLabel> lbl_base_;
    std::unique_ptr<QLineEdit> edt_base_;
    std::unique_ptr<QPushButton> btn_base_;

    std::unique_ptr<QLabel> lbl_sum_;
    std::unique_ptr<QLineEdit> edt_sum_;
    std::unique_ptr<QPushButton> btn_sum_;

    std::unique_ptr<QLabel> lbl_temp_;
    std::unique_ptr<QLineEdit> edt_temp_;
    std::unique_ptr<QPushButton> btn_temp_;

    std::unique_ptr<QLabel> lbl_maple_;
    std::unique_ptr<QLineEdit> edt_maple_;
    std::unique_ptr<QPushButton> btn_maple_;

    std::unique_ptr<QLabel> lbl_bgcolor_;
    std::unique_ptr<QRadioButton> btn_bgblack_;
    std::unique_ptr<QRadioButton> btn_bgwhite_;

    std::unique_ptr<QPushButton> btn_ok_;
    std::unique_ptr<QPushButton> btn_reset_;
    std::unique_ptr<QPushButton> btn_cancel_;

    std::unique_ptr<QBoxLayout> mainLayout_;

  public slots:
    void onOk();
    void onCancel();
    void onReset();
    void onBrowseBase();
    void onBrowseSum();
    void onBrowseTemp();
    void onBrowseMaple();
    // void OnBrowseReduce();
};
