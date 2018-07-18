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
#include <QRadioButton>

class QSettingsDlg : public QDialog
{
    Q_OBJECT

  private:
    QLabel *lbl_base_;
    QLineEdit *edt_base_;
    QPushButton *btn_base_;

    QLabel *lbl_sum_;
    QLineEdit *edt_sum_;
    QPushButton *btn_sum_;

    QLabel *lbl_temp_;
    QLineEdit *edt_temp_;
    QPushButton *btn_temp_;

    QLabel *lbl_maple_;
    QLineEdit *edt_maple_;
    QPushButton *btn_maple_;
    
    // QLabel *lbl_red;
    // QLineEdit *edt_red;
    // QPushButton *btn_red;

    QLabel *lbl_bgcolor_;
    QRadioButton *btn_bgblack_;
    QRadioButton *btn_bgwhite_;

    QPushButton *btn_ok_;
    QPushButton *btn_reset_;
    QPushButton *btn_cancel_;
    
    
    QBoxLayout *mainLayout_;

  public:
    QSettingsDlg(QWidget *, Qt::WindowFlags);
    void browseForExistingPathOrFile(QLineEdit *, QString, bool);

  public slots:
    void onOk(void);
    void onCancel(void);
    void onReset(void);
    void onBrowseBase(void);
    void onBrowseSum(void);
    void onBrowseTemp(void);
    void onBrowseMaple(void);
    // void OnBrowseReduce(void);
};

#endif /* WIN_SETTINGS_H */
