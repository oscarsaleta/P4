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

#include "P4FindDlg.h"

#include "P4InputVF.h"
#include "P4ParentStudy.h"
#include "main.h"
#include "p4application.h"
#include "p4settings.h"
#include "P4StartDlg.h"
#include "P4ParamsDlg.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>

P4FindDlg::P4FindDlg(std::unique_ptr<P4StartDlg> startdlg)
    : QWidget(startdlg.get()), parent_{startdlg}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    std::unique_ptr<QLabel> p4title{
        new QLabel("Find and Examine Singular Points", this)};
    p4title->setFont(*(g_p4app->titleFont_));

    std::unique_ptr<QLabel> actlabel{new QLabel("File Action:  ", this)};
    actlabel->setFont(*(g_p4app->boldFont_));
    btn_actionrun_.reset(new QRadioButton("Run File", this));
    btn_actionprep_.reset(new QRadioButton("Prepare File", this));

    std::unique_ptr<QLabel> singpoints{
        new QLabel("Singular points:  \n\n", this)};
    singpoints->setFont(*(g_p4app->boldFont_));
    btn_all_.reset(new QRadioButton("All ", this));
    btn_fin_.reset(new QRadioButton("Finite ", this));
    btn_inf_.reset(new QRadioButton("Infinite ", this));
    btn_one_.reset(new QRadioButton("One ", this));

    std::unique_ptr<QLabel> saveall{new QLabel("Save all information: ", this)};
    saveall->setFont(*(g_p4app->boldFont_));
    btn_yes_.reset(new QRadioButton("Yes", this));
    btn_no_.reset(new QRadioButton("No", this));

    btn_load_.reset(new QPushButton("&Load", this));
    btn_save_.reset(new QPushButton("&Save", this));

    if (g_action_OnlyPrepareFile)
        btn_eval_.reset(new QPushButton("Pr&epare", this));
    else
        btn_eval_.reset(new QPushButton("&Evaluate", this));

#ifdef TOOLTIPS
    btn_actionrun_->setToolTip(
        "Make sure the symbolic manipulator processes the file");
    btn_actionprep_->setToolTip(
        "Do not process the file, only prepare it.\n"
        "Choose this if you want to process the file yourselves\n"
        "from within a terminal window.");
    btn_all_->setToolTip("Study all singularities, finite and infinite");
    btn_fin_->setToolTip("Study only singularities at the finite region");
    btn_inf_->setToolTip("Study only singularities at infinity");
    btn_one_->setToolTip("Study one specific singularity.\n"
                         "Do not forget to specify the coordinates.");
    btn_yes_->setToolTip(
        "The View/Finite and View/Infinite windows will display more details");
    btn_no_->setToolTip(
        "The View/Finite and View/Infinite windows will display no details");
    btn_load_->setToolTip("Load the vector field & parameters from disc");
    btn_save_->setToolTip("Save the vector field & parameters to disc");
    btn_eval_->setToolTip("Prepare a file for the symbolic manipulator, and "
                          "optionally process it");
#endif

    // layout
    superLayout_.reset(new QBoxLayout(QBoxLayout::LeftToRight, this));
    parLayout_.reset(new QBoxLayout(QBoxLayout::TopToBottom));
    mainLayout_.reset(new QBoxLayout(QBoxLayout::TopToBottom));

    mainLayout_->addSpacing(8);
    mainLayout_->addWidget(p4title);

    std::unique_ptr<QHBoxLayout> actLayout{new QHBoxLayout()};
    actLayout->addWidget(actlabel);
    actLayout->addWidget(btn_actionrun_);
    actLayout->addWidget(btn_actionprep_);
    actLayout->addStretch(0);
    mainLayout_->addLayout(actLayout);

    std::unique_ptr<QHBoxLayout> singlineLayout{new QHBoxLayout()};
    singlineLayout->addWidget(singpoints, 0, Qt::AlignBottom);

    std::unique_ptr<QGridLayout> singLayout{new QGridLayout()};
    singLayout->addWidget(btn_all_, 0, 0);
    singLayout->addWidget(btn_fin_, 0, 1);
    singLayout->addWidget(btn_inf_, 1, 0);
    singLayout->addWidget(btn_one_, 1, 1);
    singlineLayout->addLayout(singLayout);
    singlineLayout->addStretch(0);

    mainLayout_->addLayout(singlineLayout);

    std::unique_ptr<QHBoxLayout> layout0{new QHBoxLayout()};
    layout0->addWidget(saveall);
    layout0->addWidget(btn_yes_);
    layout0->addWidget(btn_no_);
    layout0->addStretch(0);

    std::unique_ptr<QGridLayout> layout1{new QGridLayout()};
    layout1->addWidget(btn_load_, 0, 0);
    layout1->addWidget(btn_save_, 0, 1);
    layout1->addWidget(btn_eval_, 1, 0, 1, 2);

    mainLayout_->addLayout(layout0);
    mainLayout_->addLayout(layout1);

    //   mainLayout_->setSizeConstraint(QLayout::SetFixedSize);

    mainLayout_->addStretch(0);
    superLayout_->addLayout(mainLayout_);
    setLayout(superLayout_);

    // connections
    std::unique_ptr<QButtonGroup> btngrp1{new QButtonGroup(this)};
    btngrp1->addButton(btn_all_);
    btngrp1->addButton(btn_fin_);
    btngrp1->addButton(btn_inf_);
    btngrp1->addButton(btn_one_);

    std::unique_ptr<QButtonGroup> btngrp2{new QButtonGroup(this)};
    btngrp2->addButton(btn_yes_);
    btngrp2->addButton(btn_no_);

    std::unique_ptr<QButtonGroup> btngrp4{new QButtonGroup(this)};
    btngrp4->addButton(btn_actionrun_);
    btngrp4->addButton(btn_actionprep_);

    if (g_action_OnlyPrepareFile)
        btn_actionprep_->toggle();
    else
        btn_actionrun_->toggle();

    switch (g_ThisVF->typeofstudy_) {
    case TYPEOFSTUDY_ALL:
        btn_all_->toggle();
        break;
    case TYPEOFSTUDY_FIN:
        btn_fin_->toggle();
        break;
    case TYPEOFSTUDY_INF:
        btn_inf_->toggle();
        break;
    case TYPEOFSTUDY_ONE:
        btn_one_->toggle();
        break;
    }

    if (g_action_SaveAll)
        btn_yes_->toggle();
    else
        btn_no_->toggle();

    /* set evaluate as text in the run button if evaluate option is selected */
    connect(btn_actionrun_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            g_action_OnlyPrepareFile = false;
            btn_eval_->setText("&Evaluate");
        }
    });

    /* set prepare as text in the run button if prepare option is selected */
    connect(btn_actionprep_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            g_action_OnlyPrepareFile = true;
            btn_eval_->setText("Pr&epare");
        }
    });

    connect(btn_all_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            if (g_ThisVF->typeofstudy_ != TYPEOFSTUDY_ALL) {
                g_ThisVF->typeofstudy_ = TYPEOFSTUDY_ALL;
                if (g_ThisVF->changed_ == false) {
                    g_ThisVF->changed_ = true;
                    g_p4app->signalChanged();
                }
                if (paramsWindow_) {
                    paramsWindow_->getDataFromDlg();
                    paramsWindow_->updateDlgData();
                }
            }
        }
    });
    connect(btn_one_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            if (g_ThisVF->typeofstudy_ != TYPEOFSTUDY_ONE) {
                g_ThisVF->typeofstudy_ = TYPEOFSTUDY_ONE;
                if (g_ThisVF->changed_ == false) {
                    g_ThisVF->changed_ = true;
                    g_p4app->signalChanged();
                }
                if (paramsWindow_) {
                    paramsWindow_->getDataFromDlg();
                    paramsWindow_->updateDlgData();
                }
            }
        }
    });
    connect(btn_fin_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            if (g_ThisVF->typeofstudy_ != TYPEOFSTUDY_FIN) {
                g_ThisVF->typeofstudy_ = TYPEOFSTUDY_FIN;
                if (g_ThisVF->changed_ == false) {
                    g_ThisVF->changed_ = true;
                    g_p4app->signalChanged();
                }
                if (paramsWindow_) {
                    paramsWindow_->getDataFromDlg();
                    paramsWindow_->updateDlgData();
                }
            }
        }
    });
    connect(btn_inf_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            if (g_ThisVF->typeofstudy_ != TYPEOFSTUDY_FIN) {
                g_ThisVF->typeofstudy_ = TYPEOFSTUDY_FIN;
                if (g_ThisVF->changed_ == false) {
                    g_ThisVF->changed_ = true;
                    g_p4app->signalChanged();
                }
                if (paramsWindow_) {
                    paramsWindow_->getDataFromDlg();
                    paramsWindow_->updateDlgData();
                }
            }
        }
    });
    connect(btn_yes_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            g_action_SaveAll = true;
        }
    });
    connect(btn_no_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            g_action_SaveAll = false;
        }
    });

    connect(btn_load_, &QPushButton::clicked, this, &P4FindDlg::onBtnLoad);
    connect(btn_save_, &QPushButton::clicked, this, &P4FindDlg::onBtnSave);
    connect(btn_eval_, &QPushButton::clicked, this, &P4FindDlg::onBtnEval);
    connect(g_ThisVF, &P4InputVF::saveSignal, this, &P4FindDlg::onSaveSignal);
    // TODO: implement onSaveSignal slot

    // finishing
    g_ThisVF->findDlg_.reset(this);

    // show vector field dialog
    if (!vfWindow_) {
        vfWindow_.reset(new QVectorFieldDlg(this));
        vfWindow_->show();
        mainLayout_->addWidget(vfWindow_);
    } else {
        vfWindow_.reset();
    }
    // show params dialog
    if (!paramsWindow_) {
        paramsWindow_.reset(new P4ParamsDlg(this));
        vfSelectWindow_.reset(new P4VFSelectDlg(this));
        paramsWindow_->show();
        vfSelectWindow_->show();
        superLayout_->addWidget(paramsWindow_, 0, Qt::AlignTop);
        superLayout_->addWidget(vfSelectWindow_, 0, Qt::AlignTop);
    } else {
        paramsWindow_.reset();
        vfSelectWindow_.reset();
    }

    if (g_ThisVF->evaluating_)
        btn_eval_->setEnabled(false);

    // readSettings();
}

void P4FindDlg::onSaveSignal()
{
    // QSettings settings(g_ThisVF->getbarefilename().append(".conf"),
    // QSettings::NativeFormat);
    // settings.setValue("P4FindDlg/state",saveState());
}

void P4FindDlg::onBtnLoad()
{
    if (g_ThisVF->load() == false) {
        QMessageBox::critical(this, "P4",
                              "Unable to find the input vector field.\n");
    } else {
        // we first signal that the vector field has changed to invalidate all
        // open windows.
        bool oldeval = g_ThisVF->evaluated_;

        if (vfSelectWindow_) {
            if (vfSelectWindow_->win_curves_) {
                g_VFResults.readTables(g_ThisVF->getbarefilename(), true, true);
            }
        }

        updateDlgData();
        if (g_ThisVF->changed_ == false) {
            g_ThisVF->changed_ = true;
            g_p4app->signalChanged();
        }

        // then, we signal that the vector field is unchanged because a save
        // vector field is not needed when quiting.
        g_ThisVF->changed_ = false;
        g_ThisVF->evaluated_ = oldeval;
        g_p4app->signalLoaded();
    }
}

void P4FindDlg::onBtnSave()
{
    getDataFromDlg();
    if (g_ThisVF->changed_)
        g_ThisVF->cleared_ = false;
    if (g_ThisVF->cleared_) {
        QMessageBox::critical(this, "P4", "No data has been entered.\n"
                                          "Please specify vector field.\n");
        return;
    }
    if (g_ThisVF->save() == false) {
        QMessageBox::critical(
            this, "P4", "Unable to save the input vector field.\n"
                        "Please check permissions on the write location.\n");
    }
}

void P4FindDlg::onBtnEval()
{
    if (g_ThisVF->evaluating_)
        return;

    getDataFromDlg();
    if (g_ThisVF->getfilename().length() == 0) {
        // error: need to specify filename first
        QMessageBox::critical(this, "P4", "Unable to evaluate.\n"
                                          "Please specify a valid filename.\n");
        return;
    }

    if (g_ThisVF->changed_)
        g_ThisVF->cleared_ = false;

    if (g_ThisVF->cleared_) {
        QMessageBox::critical(this, "P4", "No data has been entered.\n"
                                          "Please specify vector field.\n");
        return;
    }

    if (g_action_OnlyPrepareFile) {
        g_ThisVF->evaluated_ = false;
        g_ThisVF->evaluating_ = false;
        g_ThisVF->prepare();
    } else {
        g_ThisVF->evaluated_ = false;
        g_ThisVF->evaluating_ = true;
        g_ThisVF->evaluatingPiecewiseConfig_ = false;
        signalEvaluating();
        g_ThisVF->evaluate();
    }
}

void P4FindDlg::getDataFromDlg()
{
    if (vfWindow_) {
        vfWindow_->getDataFromDlg();
    }
    if (paramsWindow_) {
        paramsWindow_->getDataFromDlg();
    }
    if (vfSelectWindow_)
        vfSelectWindow_.getDataFromDlg();
}

void P4FindDlg::updateDlgData()
{
    switch (g_ThisVF->typeofstudy_) {
    case TYPEOFSTUDY_ALL:
        btn_all_->toggle();
        break;
    case TYPEOFSTUDY_FIN:
        btn_fin_->toggle();
        break;
    case TYPEOFSTUDY_INF:
        btn_inf_->toggle();
        break;
    case TYPEOFSTUDY_ONE:
        btn_one_->toggle();
        break;
    }

    if (vfWindow_) {
        vfWindow_->updateDlgData();
    }
    if (paramsWindow_) {
        paramsWindow_->updateDlgData();
    }
    if (vfSelectWindow_)
        vfSelectWindow_->updateDlgData();
}

void P4FindDlg::signalEvaluating() { btn_eval_->setEnabled(false); }

void P4FindDlg::signalEvaluated() { btn_eval_->setEnabled(true); }

void P4FindDlg::signalCurvesEvaluated()
{
    btn_eval_->setEnabled(true);
    if (vfSelectWindow_) {
        if (!vfSelectWindow_->win_curves.empty())
            vfSelectWindow_->win_curves->signalSeparatingCurvesEvaluated();
    }
}