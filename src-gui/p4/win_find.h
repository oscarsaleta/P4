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

#ifndef WIN_FIND_H
#define WIN_FIND_H

#include "win_main.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>

/* Forward-declaration to solve cross-include problems */
class QParamsDlg;      // in win_params.h
class QVectorFieldDlg; // in win_vf.h

class QFindDlg : public QWidget
{
    Q_OBJECT

  public:
    QFindDlg(QStartDlg *startwindow);
    ~QFindDlg();
    void getDataFromDlg();

  private:
    QStartDlg *parent_;

    QBoxLayout *mainLayout_;
    QBoxLayout *superLayout_;

    // QRadioButton *btn_maple_;
    // QRadioButton *btn_reduce_;
    QRadioButton *btn_actionrun_;
    QRadioButton *btn_actionprep_;
    QRadioButton *btn_all_;
    QRadioButton *btn_fin_;
    QRadioButton *btn_inf_;
    QRadioButton *btn_one_;
    QRadioButton *btn_yes_;
    QRadioButton *btn_no_;
    QPushButton *btn_load_;
    QPushButton *btn_save_;
    QPushButton *btn_eval_;

    QVectorFieldDlg *vfWindow_;
    QParamsDlg *paramsWindow_;

    //void saveSettings();
    //void readSettings();

  public slots:
    // void btn_maple_toggled(bool);
    // void btn_reduce_toggled(bool);
    void updateDlgData();

    void signalEvaluating();
    void signalEvaluated();

    void onBtnLoad();
    void onBtnSave();
    void onBtnEval();

    void onSaveSignal();
};

#endif /* WIN_FIND_H */
