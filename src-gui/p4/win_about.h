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

#ifndef WIN_ABOUT_H
#define WIN_ABOUT_H

#include <QDialog>
#include <QPixmap>

class QBoxLayout;
class QPushButton;
class QWidget;

class QP4AboutDlg : public QDialog
{
    Q_OBJECT

  private:
    QPushButton *btn_ok_;
    QPushButton *btn_settings_;

    QBoxLayout *mainLayout_;
    QPixmap p4image_;

  public:
    QP4AboutDlg(QWidget *parent = 0, Qt::WindowFlags f = 0);

  public slots:
    void onOk(void);
    void onSettings(void);
};

#endif /* WIN_ABOUT_H */
