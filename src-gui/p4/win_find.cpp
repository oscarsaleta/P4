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

#include "win_find.h"

#include "file_vf.h"

#include "main.h"
#include "p4application.h"
#include "p4settings.h"
#include "win_params.h"
#include "win_vf.h"

#include <QButtonGroup>
#include <QLabel>
#include <QMessageBox>

QFindDlg::~QFindDlg()
{
    GetDataFromDlg();

    if (Vf_Window != nullptr) {
        delete Vf_Window;
        Vf_Window = nullptr;
    }
    if (Params_Window != nullptr) {
        delete Params_Window;
        Params_Window = nullptr;
    }
}

QFindDlg::QFindDlg(QStartDlg *startdlg)
#ifdef DOCK_FINDWINDOW
    : QWidget(startdlg)
#else
    : QWidget()
#endif
{
    parent = startdlg;
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

#ifdef DOCK_FINDWINDOW
    QLabel *p4title = new QLabel("Find and Examine Singular Points", this);
    p4title->setFont(*(p4app->TitleFont));
#endif

    // QLabel *symlabel = new QLabel("Symbolic package: ", this);
    // symlabel->setFont(*(p4app->BoldFont));
    // btn_maple = new QRadioButton("Maple", this);
    // btn_reduce = new QRadioButton("Reduce", this);

    //#ifdef Q_OS_WIN
    // btn_reduce->setEnabled(false); // reduce is not available under windows
    //#endif

    QLabel *actlabel = new QLabel("File Action:  ", this);
    actlabel->setFont(*(p4app->BoldFont));
    btn_actionrun = new QRadioButton("Run File", this);
    btn_actionprep = new QRadioButton("Prepare File", this);

    QLabel *singpoints = new QLabel("Singular points:  \n\n", this);
    singpoints->setFont(*(p4app->BoldFont));
    btn_all = new QRadioButton("All ", this);
    btn_fin = new QRadioButton("Finite ", this);
    btn_inf = new QRadioButton("Infinite ", this);
    btn_one = new QRadioButton("One ", this);

    QLabel *saveall = new QLabel("Save all information: ", this);
    saveall->setFont(*(p4app->BoldFont));
    btn_yes = new QRadioButton("Yes", this);
    btn_no = new QRadioButton("No", this);

    btn_params = new QPushButton("P&arameters", this);
    btn_vf = new QPushButton("&Vector Field", this);

    btn_load = new QPushButton("&Load", this);
    btn_save = new QPushButton("&Save", this);

    if (action_OnlyPrepareFile)
        btn_eval = new QPushButton("Pr&epare", this);
    else
        btn_eval = new QPushButton("&Evaluate", this);

#ifdef TOOLTIPS
    // btn_maple->setToolTip("Select Maple as the symbolic manipulator");
    // btn_reduce->setToolTip("Select Reduce as the symbolic manipulator.\n"
    //                       "This is only available in the Unix version.");
    btn_actionrun->setToolTip(
        "Make sure the symbolic manipulator processes the file");
    btn_actionprep->setToolTip(
        "Do not process the file, only prepare it.\n"
        "Choose this if you want to process the file yourselves\n"
        "from within a terminal window.");
    btn_all->setToolTip("Study all singularities, finite and infinite");
    btn_fin->setToolTip("Study only singularities at the finite region");
    btn_inf->setToolTip("Study only singularities at infinity");
    btn_one->setToolTip("Study one specific singularity.\n"
                        "Do not forget to specify the coordinates.");
    btn_yes->setToolTip(
        "The View/Finite and View/Infinite windows will display more details");
    btn_no->setToolTip(
        "The View/Finite and View/Infinite windows will display no details");
    btn_params->setToolTip("Opens/closes the parameters window");
    btn_vf->setToolTip("Opens/closes the vector field window");
    btn_load->setToolTip("Load the vector field & parameters from disc");
    btn_save->setToolTip("Save the vector field & parameters to disc");
    btn_eval->setToolTip("Prepare a file for the symbolic manipulator, and "
                         "optionally process it");
#endif

// layout

#ifdef DOCK_PARAMSWINDOW
    superLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
#else
    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
#endif

#ifdef DOCK_FINDWINDOW
    mainLayout->addSpacing(8);
    mainLayout->addWidget(p4title);
#endif

    QHBoxLayout *symLayout = new QHBoxLayout();
    // symLayout->addWidget(symlabel);
    // symLayout->addWidget(btn_maple);
    // symLayout->addWidget(btn_reduce);
    symLayout->addStretch(0);
    mainLayout->addLayout(symLayout);

    QHBoxLayout *actLayout = new QHBoxLayout();
    actLayout->addWidget(actlabel);
    actLayout->addWidget(btn_actionrun);
    actLayout->addWidget(btn_actionprep);
    actLayout->addStretch(0);
    mainLayout->addLayout(actLayout);

    QHBoxLayout *singlineLayout = new QHBoxLayout();
    singlineLayout->addWidget(singpoints, 0, Qt::AlignBottom);

    QGridLayout *singLayout = new QGridLayout();
    singLayout->addWidget(btn_all, 0, 0);
    singLayout->addWidget(btn_fin, 0, 1);
    singLayout->addWidget(btn_inf, 1, 0);
    singLayout->addWidget(btn_one, 1, 1);
    singlineLayout->addLayout(singLayout);
    singlineLayout->addStretch(0);

    mainLayout->addLayout(singlineLayout);

    QHBoxLayout *layout0 = new QHBoxLayout();
    layout0->addWidget(saveall);
    layout0->addWidget(btn_yes);
    layout0->addWidget(btn_no);
    layout0->addStretch(0);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addStretch(0);
    layout1->addWidget(btn_params);
    layout1->addWidget(btn_vf);
    layout1->addStretch(0);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addStretch(0);
    layout2->addWidget(btn_load);
    layout2->addWidget(btn_save);
    layout2->addStretch(0);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addStretch(0);
    layout3->addWidget(btn_eval);
    layout3->addStretch(0);

    mainLayout->addLayout(layout0);
    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);
    mainLayout->addLayout(layout3);

//   mainLayout->setSizeConstraint(QLayout::SetFixedSize);

#ifdef DOCK_PARAMSWINDOW
    mainLayout->addStretch(0);
    superLayout->addLayout(mainLayout);
    setLayout(superLayout);
#else
    setLayout(mainLayout);
#endif

    // connections

    QButtonGroup *btngrp1 = new QButtonGroup(this);
    btngrp1->addButton(btn_all);
    btngrp1->addButton(btn_fin);
    btngrp1->addButton(btn_inf);
    btngrp1->addButton(btn_one);

    QButtonGroup *btngrp2 = new QButtonGroup(this);
    btngrp2->addButton(btn_yes);
    btngrp2->addButton(btn_no);

    // QButtonGroup *btngrp3 = new QButtonGroup(this);
    // btngrp3->addButton(btn_maple);
    // btngrp3->addButton(btn_reduce);

    QButtonGroup *btngrp4 = new QButtonGroup(this);
    btngrp4->addButton(btn_actionrun);
    btngrp4->addButton(btn_actionprep);

    // if (ThisVF->symbolicpackage == PACKAGE_MAPLE)
    //    btn_maple->toggle();
    // else
    //    btn_reduce->toggle();

    if (action_OnlyPrepareFile)
        btn_actionprep->toggle();
    else
        btn_actionrun->toggle();

    switch (ThisVF->typeofstudy) {
    case TYPEOFSTUDY_ALL:
        btn_all->toggle();
        break;
    case TYPEOFSTUDY_FIN:
        btn_fin->toggle();
        break;
    case TYPEOFSTUDY_INF:
        btn_inf->toggle();
        break;
    case TYPEOFSTUDY_ONE:
        btn_one->toggle();
        break;
    }

    if (action_SaveAll)
        btn_yes->toggle();
    else
        btn_no->toggle();

    // QObject::connect(btn_maple, SIGNAL(toggled(bool)), this,
    //                 SLOT(btn_maple_toggled(bool)));
    // QObject::connect(btn_reduce, SIGNAL(toggled(bool)), this,
    //                 SLOT(btn_reduce_toggled(bool)));
    QObject::connect(btn_actionrun, SIGNAL(toggled(bool)), this,
                     SLOT(btn_actionrun_toggled(bool)));
    QObject::connect(btn_actionprep, SIGNAL(toggled(bool)), this,
                     SLOT(btn_actionprep_toggled(bool)));
    QObject::connect(btn_all, SIGNAL(toggled(bool)), this,
                     SLOT(btn_all_toggled(bool)));
    QObject::connect(btn_one, SIGNAL(toggled(bool)), this,
                     SLOT(btn_one_toggled(bool)));
    QObject::connect(btn_fin, SIGNAL(toggled(bool)), this,
                     SLOT(btn_fin_toggled(bool)));
    QObject::connect(btn_inf, SIGNAL(toggled(bool)), this,
                     SLOT(btn_inf_toggled(bool)));
    QObject::connect(btn_yes, SIGNAL(toggled(bool)), this,
                     SLOT(btn_yes_toggled(bool)));
    QObject::connect(btn_no, SIGNAL(toggled(bool)), this,
                     SLOT(btn_no_toggled(bool)));

    QObject::connect(btn_params, SIGNAL(clicked()), this, SLOT(OnBtnParams()));
    QObject::connect(btn_vf, SIGNAL(clicked()), this, SLOT(OnBtnVf()));
    QObject::connect(btn_load, SIGNAL(clicked()), this, SLOT(OnBtnLoad()));
    QObject::connect(btn_save, SIGNAL(clicked()), this, SLOT(OnBtnSave()));
    QObject::connect(btn_eval, SIGNAL(clicked()), this, SLOT(OnBtnEval()));

    // finishing

    Vf_Window = nullptr;
    Params_Window = nullptr;

#ifdef AUTO_OPEN_VFWINDOW
    OnBtnVf();
#endif

#ifdef AUTO_OPEN_PARAMSWINDOW
    OnBtnParams();
#endif

    if (ThisVF->evaluating)
        btn_eval->setEnabled(false);

#ifndef DOCK_FINDWINDOW
    SetP4WindowTitle(this, "Find");
#endif
}

void QFindDlg::btn_yes_toggled(bool on)
{
    if (on) {
        action_SaveAll = true;
    }
}

void QFindDlg::btn_no_toggled(bool on)
{
    if (on) {
        action_SaveAll = false;
    }
}

void QFindDlg::btn_maple_toggled(bool on)
{
    if (on) {
        if (ThisVF->symbolicpackage != PACKAGE_MAPLE) {
            ThisVF->symbolicpackage = PACKAGE_MAPLE;
            setMathPackage(PACKAGE_MAPLE);
        }
    }
}

void QFindDlg::btn_reduce_toggled(bool on)
{
    if (on) {
        if (ThisVF->symbolicpackage != PACKAGE_REDUCE) {
            ThisVF->symbolicpackage = PACKAGE_REDUCE;
            setMathPackage(PACKAGE_REDUCE);
        }
    }
}

void QFindDlg::btn_actionrun_toggled(bool on)
{
    if (on) {
        action_OnlyPrepareFile = false;
        btn_eval->setText("&Evaluate");
    }
}

void QFindDlg::btn_actionprep_toggled(bool on)
{
    if (on) {
        action_OnlyPrepareFile = true;
        btn_eval->setText("Pr&epare");
    }
}

void QFindDlg::btn_all_toggled(bool on)
{
    if (on) {
        if (ThisVF->typeofstudy != TYPEOFSTUDY_ALL) {
            ThisVF->typeofstudy = TYPEOFSTUDY_ALL;
            if (ThisVF->changed == false) {
                ThisVF->changed = true;
                p4app->Signal_Changed();
            }
            if (Params_Window != nullptr) {
                Params_Window->GetDataFromDlg();
                Params_Window->UpdateDlgData();
            }
        }
    }
}

void QFindDlg::btn_fin_toggled(bool on)
{
    if (on) {
        if (ThisVF->typeofstudy != TYPEOFSTUDY_FIN) {
            ThisVF->typeofstudy = TYPEOFSTUDY_FIN;
            if (ThisVF->changed == false) {
                ThisVF->changed = true;
                p4app->Signal_Changed();
            }
            if (Params_Window != nullptr) {
                Params_Window->GetDataFromDlg();
                Params_Window->UpdateDlgData();
            }
        }
    }
}

void QFindDlg::btn_inf_toggled(bool on)
{
    if (on) {
        if (ThisVF->typeofstudy != TYPEOFSTUDY_INF) {
            ThisVF->typeofstudy = TYPEOFSTUDY_INF;
            if (ThisVF->changed == false) {
                ThisVF->changed = true;
                p4app->Signal_Changed();
            }
            if (Params_Window != nullptr) {
                Params_Window->GetDataFromDlg();
                Params_Window->UpdateDlgData();
            }
        }
    }
}

void QFindDlg::btn_one_toggled(bool on)
{
    if (on) {
        if (ThisVF->typeofstudy != TYPEOFSTUDY_ONE) {
            ThisVF->typeofstudy = TYPEOFSTUDY_ONE;
            if (ThisVF->changed == false) {
                ThisVF->changed = true;
                p4app->Signal_Changed();
            }
            if (Params_Window != nullptr) {
                Params_Window->GetDataFromDlg();
                Params_Window->UpdateDlgData();
            }
        }
    }
}

void QFindDlg::ExclusiveToggle(bool on, QRadioButton *first, ...)
{
    first->setChecked(on);
}

void QFindDlg::OnBtnParams(void)
{
    if (Params_Window == nullptr) {
        Params_Window = new QParamsDlg(this);
        Params_Window->show();
#ifdef DOCK_PARAMSWINDOW
        superLayout->addWidget(Params_Window, 0, Qt::AlignTop);
#else
        Params_Window->raise();
#endif
    } else {
#ifdef DOCK_PARAMSWINDOW
        delete Params_Window;
        Params_Window = nullptr;
#else
        Params_Window->show();
        Params_Window->raise();
#endif
    }
}

void QFindDlg::OnBtnVf(void)
{
    // show find dialog

    if (Vf_Window == nullptr) {
        Vf_Window = new QVectorFieldDlg(this);
#ifdef DOCK_VFWINDOW
        Vf_Window->show();
        mainLayout->addWidget(Vf_Window);
#else
        Vf_Window->raise();
#endif
    } else {
#ifdef DOCK_VFWINDOW
        delete Vf_Window;
        Vf_Window = nullptr;
#else
        Vf_Window->show();
        Vf_Window->raise();
#endif
    }
}

void QFindDlg::OnBtnLoad(void)
{
    if (ThisVF->load() == false) {
        QMessageBox::critical(this, "P4",
                              "Unable to find the input vector field.\n");
    } else {
        // we first signal that the vector field has changed
        // to invalidate all open windows.

        bool oldeval = ThisVF->evaluated;

        UpdateDlgData();
        if (ThisVF->changed == false) {
            ThisVF->changed = true;
            p4app->Signal_Changed();
        }

        // then, we signal that the vector field is unchanged
        // because a save vector field is not needed when quiting.

        ThisVF->changed = false;
        ThisVF->evaluated = oldeval;
        p4app->Signal_Loaded();
    }
}

void QFindDlg::OnBtnSave(void)
{
    GetDataFromDlg();
    if (ThisVF->changed)
        ThisVF->cleared = false;
    if (ThisVF->cleared) {
        QMessageBox::critical(this, "P4", "No data has been entered.\n"
                                          "Please specify vector field.\n");
        return;
    }
    if (ThisVF->save() == false) {
        QMessageBox::critical(
            this, "P4", "Unable to save the input vector field.\n"
                        "Please check permissions on the write location.\n");
    }
}

void QFindDlg::OnBtnEval(void)
{
    //  int result;

    GetDataFromDlg();
    if (ThisVF->getfilename().length() == 0) {
        // error: need to specify filename first

        QMessageBox::critical(this, "P4", "Unable to evaluate.\n"
                                          "Please specify a valid filename.\n");
        return;
    }

    if (ThisVF->changed)
        ThisVF->cleared = false;

    if (ThisVF->cleared) {
        QMessageBox::critical(this, "P4", "No data has been entered.\n"
                                          "Please specify vector field.\n");
        return;
    }

    /*if (ThisVF->changed) {
      ThisVF->evaluated = false;
        result = QMessageBox::information( this, "P4",
                    "The vector field has been changed since "
                    "the last save.\nSave before evaluation?",
                    "&Yes", "&No", "&Cancel",
                    0, 2 );
        if( result == 2 )
        {
            return;
        }
        if( result == 0 )
        {
            if( ThisVF->save() == false )
            {
                QMessageBox::critical( this, "P4",
                            "Unable to save the input vector field.\n"
                            "Please check permissions on the write location.\n"
                                  );
                return;
            }
        }
    }*/

    if (action_OnlyPrepareFile) {
        ThisVF->evaluated = false;
        ThisVF->evaluating = false;

        ThisVF->prepare();
    } else {
        ThisVF->evaluated = false;
        ThisVF->evaluating = true;

        Signal_Evaluating();
        ThisVF->evaluate();
        //      RunTask( &EvalVF );
    }
}

void QFindDlg::GetDataFromDlg(void)
{
    if (Vf_Window != nullptr) {
        Vf_Window->GetDataFromDlg();
    }
    if (Params_Window != nullptr) {
        Params_Window->GetDataFromDlg();
    }
}

void QFindDlg::UpdateDlgData(void)
{
    switch (ThisVF->typeofstudy) {
    case TYPEOFSTUDY_ALL:
        ExclusiveToggle(true, btn_all);
        break;
    case TYPEOFSTUDY_FIN:
        ExclusiveToggle(true, btn_fin);
        break;
    case TYPEOFSTUDY_INF:
        ExclusiveToggle(true, btn_inf);
        break;
    case TYPEOFSTUDY_ONE:
        ExclusiveToggle(true, btn_one);
        break;
    }

    if (Vf_Window != nullptr) {
        Vf_Window->UpdateDlgData();
    }
    if (Params_Window != nullptr) {
        Params_Window->UpdateDlgData();
    }
}

void QFindDlg::Signal_Evaluating(void) { btn_eval->setEnabled(false); }

void QFindDlg::signalEvaluated(void) { btn_eval->setEnabled(true); }
