/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
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

#ifndef WIN_SETTINGS_H
#define WIN_SETTINGS_H

#include <QBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QWidget>

class QSettingsDlg : public QDialog
{
    Q_OBJECT

  private:
    QPushButton *btn_base;
    QPushButton *btn_sum;
    QPushButton *btn_temp;
    QPushButton *btn_maple;
    QPushButton *btn_red;
    QPushButton *btn_ok_;
    QPushButton *btn_reset;
    QPushButton *btn_cancel;
    QLineEdit *edt_base;
    QLineEdit *edt_sum;
    QLineEdit *edt_temp;
    QLineEdit *edt_maple;
    QLineEdit *edt_red;
    QLabel *lbl_base;
    QLabel *lbl_sum;
    QLabel *lbl_temp;
    QLabel *lbl_maple;
    QLabel *lbl_red;
    QBoxLayout *mainLayout_;

  public:
    QSettingsDlg(QWidget *, Qt::WindowFlags);
    void BrowseForExistingPathOrFile(QLineEdit *, QString, bool);

  public slots:
    void onOk(void);
    void OnCancel(void);
    void OnReset(void);
    void OnBrowseBase(void);
    void OnBrowseSum(void);
    void OnBrowseTemp(void);
    void OnBrowseMaple(void);
    void OnBrowseReduce(void);
};

#endif /* WIN_SETTINGS_H */
