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

class QBoxLayout;
class QButtonGroup;
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
    void browseForExistingPathOrFile(QLineEdit *, const QString &, bool);

  private:
    QLineEdit *edt_base_;
    QPushButton *btn_base_;

    QLineEdit *edt_sum_;
    QPushButton *btn_sum_;

    QLineEdit *edt_temp_;
    QPushButton *btn_temp_;

    QLineEdit *edt_maple_;
    QPushButton *btn_maple_;

    QRadioButton *btn_bgblack_;
    QRadioButton *btn_bgwhite_;

    QPushButton *btn_ok_;
    QPushButton *btn_reset_;
    QPushButton *btn_cancel_;

    QBoxLayout *mainLayout_;

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
