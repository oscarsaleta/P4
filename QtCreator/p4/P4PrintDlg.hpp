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
class QCheckBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QWidget;

class P4PrintDlg : public QDialog
{
    Q_OBJECT

  public:
    P4PrintDlg(QWidget *parent, Qt::WindowFlags f);

    static bool sM_lastBlackWhite;
    static double sM_lastLineWidth;
    static double sM_lastSymbolSize;
    static int sM_lastResolution;

  private:
    QPushButton *btn_default_{nullptr};
    QPushButton *btn_epsimage_;
    QPushButton *btn_xfigimage_;
    QPushButton *btn_cancel_;
    QPushButton *btn_jpeg_;
    QBoxLayout *mainLayout_;
    QCheckBox *btn_blackwhite_;

    QRadioButton *btn_whitebg_;
    QRadioButton *btn_blackbg_;

    QLineEdit *edt_resolution_;
    QLineEdit *edt_linewidth_;
    QLineEdit *edt_symbolsize_;

  public slots:
    void onDefaultPrinter();
    void onEpsImagePrinter();
    void onXfigImagePrinter();
    void onJpegImagePrinter();
    void onCancel();

    bool readDialog();
    bool readFloatField(QLineEdit *edt, double &presult, double defvalue,
                        double minvalue, double maxvalue);
    void markBad(QLineEdit *edt);

    int getChosenResolution();
    double getChosenLineWidth();
    double getChosenSymbolSize();
};

#define P4PRINT_NONE 0
#define P4PRINT_DEFAULT 1
#define P4PRINT_EPSIMAGE 2
#define P4PRINT_XFIGIMAGE 3
#define P4PRINT_JPEGIMAGE 4
