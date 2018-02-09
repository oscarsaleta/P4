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

#ifndef WIN_VF_H
#define WIN_VF_H

#include "custom.h"

#include <QString>
#include <QWidget>

class P4FindDlg;
class QVFParams; // declare them first because both classes defined in
                 // this file need each other

class QBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QScrollBar;
class QSpinBox;

class QVectorFieldDlg : public QWidget
{
    Q_OBJECT

  public:
    QVectorFieldDlg(P4FindDlg *startwindow);
    ~QVectorFieldDlg();
    void getDataFromDlg(void);
    void updateDlgData(void);

  private:
    QBoxLayout *mainLayout_;
    P4FindDlg *parent_;
    QLineEdit *edt_xprime_;
    QLineEdit *edt_yprime_;
    QLineEdit *edt_gcf_;
    QSpinBox *spin_numparams_;
    QHBoxLayout *paramLayout_;
    QScrollBar *sb_params_;
    QVFParams *params_;

  public slots:
    void numParamsChanged(int);
};

class QVFParams : public QWidget
{
    Q_OBJECT

  public:
    QVFParams(QVectorFieldDlg *parent, QScrollBar *sb);
    ~QVFParams();
    bool getDataFromDlg(void);
    bool updateDlgData(void);

  private:
    bool dataInvalid_;
    int currentNumParams_;
    int currentShownParams_;
    int currentPageIndex_;

    QScrollBar *sb_params_;
    QBoxLayout *mainLayout_;
    QBoxLayout *superLayout_;
    QHBoxLayout *paramLines_[MAXNUMPARAMSSHOWN];
    QLineEdit *paramNames_[MAXNUMPARAMSSHOWN];
    QLabel *paramLabels_[MAXNUMPARAMSSHOWN];
    QLineEdit *paramValues_[MAXNUMPARAMSSHOWN];

    QString sLabels_[MAXNUMPARAMS];
    QString sValues_[MAXNUMPARAMS];

  protected:
    bool focusNextPrevChild(bool next);

  public slots:
    void paramsSliderChanged(int);
};

#endif /* WIN_VF_H */
