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

#ifndef WIN_PRINT_H
#define WIN_PRINT_H

#include <QBoxLayout>
#include <QCheckBox>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class QPrintDlg : public QDialog
{
    Q_OBJECT

  private:
    QPushButton *btn_default;
    QPushButton *btn_epsimage;
    QPushButton *btn_xfigimage;
    QPushButton *btn_cancel;
    QPushButton *btn_jpeg;
    QBoxLayout *mainLayout;
    QCheckBox *btn_blackwhite;
    QLineEdit *edt_resolution;
    QLineEdit *edt_linewidth;
    QLineEdit *edt_symbolsize;

  public:
    QPrintDlg(QWidget *parent, Qt::WindowFlags f);

    static bool LastBlackWhite;
    static double LastLineWidth;
    static double LastSymbolSize;
    static int LastResolution;

  public slots:
    void OnDefaultPrinter(void);
    void OnEpsImagePrinter(void);
    void OnXfigImagePrinter(void);
    void OnJpegImagePrinter(void);
    void OnCancel(void);

    bool ReadDialog(void);
    bool ReadFloatField(QLineEdit *edt, double *presult, double defvalue,
                        double minvalue, double maxvalue);
    void MarkBad(QLineEdit *edt);

    int GetChosenResolution(void);
    double GetChosenLineWidth(void);
    double GetChosenSymbolSize(void);
};

#define P4PRINT_NONE 0
#define P4PRINT_DEFAULT 1
#define P4PRINT_EPSIMAGE 2
#define P4PRINT_XFIGIMAGE 3
#define P4PRINT_JPEGIMAGE 4

#endif /* WIN_PRINT_H */
