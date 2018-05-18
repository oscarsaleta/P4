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

#include "P4FindDlg.hpp"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>

#include "P4Application.hpp"
#include "P4InputVF.hpp"
#include "P4ParamsDlg.hpp"
#include "P4ParentStudy.hpp"
#include "P4SeparatingCurvesDlg.hpp"
#include "P4StartDlg.hpp"
#include "P4VFSelectDlg.hpp"
#include "P4VectorFieldDlg.hpp"
#include "main.hpp"
#include "p4settings.hpp"

P4FindDlg::P4FindDlg(P4StartDlg *startdlg)
    : QWidget{startdlg}, parent_{startdlg}
{
    p4title_ = new QLabel{"Find and Examine Singular Points", this};
    p4title_->setFont(gP4app->getTitleFont());

    actlabel_ = new QLabel{"File Action:  ", this};
    actlabel_->setFont(gP4app->getBoldFont());
    btn_actionrun_ = new QRadioButton{"Run File", this};
    btn_actionprep_ = new QRadioButton{"Prepare File", this};

    singpoints_ = new QLabel{"Singular points:  \n\n", this};
    singpoints_->setFont(gP4app->getBoldFont());
    btn_all_ = new QRadioButton{"All ", this};
    btn_fin_ = new QRadioButton{"Finite ", this};
    btn_inf_ = new QRadioButton{"Infinite ", this};
    btn_one_ = new QRadioButton{"One ", this};

    saveall_ = new QLabel{"Save all information: ", this};
    saveall_->setFont(gP4app->getBoldFont());
    btn_yes_ = new QRadioButton{"Yes", this};
    btn_no_ = new QRadioButton{"No", this};

    btn_load_ = new QPushButton{"&Load", this};
    btn_save_ = new QPushButton{"&Save", this};

    if (gActionOnlyPrepareFile)
        btn_eval_ = new QPushButton{"Pr&epare", this};
    else
        btn_eval_ = new QPushButton{"&Evaluate", this};

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

    superLayout_ = new QGridLayout{this};

    // find and examine singular points layout
    mainLayout_ = new QBoxLayout{QBoxLayout::TopToBottom};

    mainLayout_->addSpacing(8);
    mainLayout_->addWidget(p4title_);

    actLayout_ = new QHBoxLayout{};
    actLayout_->addWidget(actlabel_);
    actLayout_->addWidget(btn_actionrun_);
    actLayout_->addWidget(btn_actionprep_);
    actLayout_->addStretch(0);
    mainLayout_->addLayout(actLayout_);

    singlineLayout_ = new QHBoxLayout{};
    singlineLayout_->addWidget(singpoints_, 0, Qt::AlignBottom);

    singLayout_ = new QGridLayout{};
    singLayout_->addWidget(btn_all_, 0, 0);
    singLayout_->addWidget(btn_fin_, 0, 1);
    singLayout_->addWidget(btn_inf_, 1, 0);
    singLayout_->addWidget(btn_one_, 1, 1);
    singlineLayout_->addLayout(singLayout_);
    singlineLayout_->addStretch(0);

    mainLayout_->addLayout(singlineLayout_);

    layout0_ = new QHBoxLayout{};
    layout0_->addWidget(saveall_);
    layout0_->addWidget(btn_yes_);
    layout0_->addWidget(btn_no_);
    layout0_->addStretch(0);
    mainLayout_->addLayout(layout0_);

    layout1_ = new QGridLayout{};
    layout1_->addWidget(btn_load_, 0, 0);
    layout1_->addWidget(btn_save_, 0, 1);
    layout1_->addWidget(btn_eval_, 1, 0, 1, 2);
    mainLayout_->addLayout(layout1_);

    mainLayout_->addStretch(0);
    superLayout_->addLayout(mainLayout_, 0, 0);

    // parameters layout
    parLayout_ = new QBoxLayout{QBoxLayout::TopToBottom};
    if (paramsWindow_ == nullptr) {
        paramsWindow_ = new P4ParamsDlg{this};
        paramsWindow_->show();
        parLayout_->addWidget(paramsWindow_, 0, Qt::AlignTop);
    }
    parLayout_->addStretch(0);
    superLayout_->addLayout(parLayout_, 0, 1);

    // vector field layout
    vfLayout_ = new QBoxLayout{QBoxLayout::TopToBottom};
    if (vfWindow_ == nullptr) {
        vfWindow_ = new P4VectorFieldDlg{this};
        vfWindow_->show();
        vfLayout_->addWidget(vfWindow_);
    }
    vfLayout_->addStretch(0);
    superLayout_->addLayout(vfLayout_, 1, 0);

    // vector field select layout
    vfSelectLayout_ = new QBoxLayout{QBoxLayout::TopToBottom};
    if (vfSelectWindow_ == nullptr) {
        vfSelectWindow_ = new P4VFSelectDlg{this};
        vfSelectWindow_->show();
        vfSelectLayout_->addWidget(vfSelectWindow_, 0, Qt::AlignTop);
    }
    vfSelectLayout_->addStretch(0);
    superLayout_->addLayout(vfSelectLayout_, 1, 1);

    setLayout(superLayout_);

    // connections
    btngrp1_ = new QButtonGroup{this};
    btngrp1_->addButton(btn_all_);
    btngrp1_->addButton(btn_fin_);
    btngrp1_->addButton(btn_inf_);
    btngrp1_->addButton(btn_one_);

    btngrp2_ = new QButtonGroup{this};
    btngrp2_->addButton(btn_yes_);
    btngrp2_->addButton(btn_no_);

    btngrp3_ = new QButtonGroup{this};
    btngrp3_->addButton(btn_actionrun_);
    btngrp3_->addButton(btn_actionprep_);

    if (gActionOnlyPrepareFile)
        btn_actionprep_->toggle();
    else
        btn_actionrun_->toggle();

    switch (gThisVF->typeofstudy_) {
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

    if (gActionSaveAll)
        btn_yes_->toggle();
    else
        btn_no_->toggle();

    /* set evaluate as text in the run button if evaluate option is selected */
    QObject::connect(btn_actionrun_, &QRadioButton::toggled, this,
                     [=](bool on) {
                         if (on) {
                             gActionOnlyPrepareFile = false;
                             btn_eval_->setText("&Evaluate");
                         }
                     });

    /* set prepare as text in the run button if prepare option is selected */
    QObject::connect(btn_actionprep_, &QRadioButton::toggled, this,
                     [=](bool on) {
                         if (on) {
                             gActionOnlyPrepareFile = true;
                             btn_eval_->setText("Pr&epare");
                         }
                     });

    QObject::connect(btn_all_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            if (gThisVF->typeofstudy_ != TYPEOFSTUDY_ALL) {
                gThisVF->typeofstudy_ = TYPEOFSTUDY_ALL;
                if (gThisVF->changed_ == false) {
                    gThisVF->changed_ = true;
                    gP4app->signalChanged();
                }
                if (paramsWindow_ != nullptr) {
                    paramsWindow_->getDataFromDlg();
                    paramsWindow_->updateDlgData();
                }
            }
        }
    });
    QObject::connect(btn_one_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            if (gThisVF->typeofstudy_ != TYPEOFSTUDY_ONE) {
                gThisVF->typeofstudy_ = TYPEOFSTUDY_ONE;
                if (gThisVF->changed_ == false) {
                    gThisVF->changed_ = true;
                    gP4app->signalChanged();
                }
                if (paramsWindow_ != nullptr) {
                    paramsWindow_->getDataFromDlg();
                    paramsWindow_->updateDlgData();
                }
            }
        }
    });
    QObject::connect(btn_fin_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            if (gThisVF->typeofstudy_ != TYPEOFSTUDY_FIN) {
                gThisVF->typeofstudy_ = TYPEOFSTUDY_FIN;
                if (gThisVF->changed_ == false) {
                    gThisVF->changed_ = true;
                    gP4app->signalChanged();
                }
                if (paramsWindow_ != nullptr) {
                    paramsWindow_->getDataFromDlg();
                    paramsWindow_->updateDlgData();
                }
            }
        }
    });
    QObject::connect(btn_inf_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            if (gThisVF->typeofstudy_ != TYPEOFSTUDY_FIN) {
                gThisVF->typeofstudy_ = TYPEOFSTUDY_FIN;
                if (gThisVF->changed_ == false) {
                    gThisVF->changed_ = true;
                    gP4app->signalChanged();
                }
                if (paramsWindow_ != nullptr) {
                    paramsWindow_->getDataFromDlg();
                    paramsWindow_->updateDlgData();
                }
            }
        }
    });
    QObject::connect(btn_yes_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            gActionSaveAll = true;
        }
    });
    QObject::connect(btn_no_, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            gActionSaveAll = false;
        }
    });

    QObject::connect(btn_load_, &QPushButton::clicked, this,
                     &P4FindDlg::onBtnLoad);
    QObject::connect(btn_save_, &QPushButton::clicked, this,
                     &P4FindDlg::onBtnSave);
    QObject::connect(btn_eval_, &QPushButton::clicked, this,
                     &P4FindDlg::onBtnEval);
    QObject::connect(gThisVF, &P4InputVF::saveSignal, this,
                     &P4FindDlg::onSaveSignal);
    // TODO: implement onSaveSignal slot

    // finishing
    gThisVF->setFindDlg(this);

    if (gThisVF->evaluating_)
        btn_eval_->setEnabled(false);

    // readSettings();
}

void P4FindDlg::onSaveSignal()
{
    // QSettings settings(gThisVF->getbarefilename().append(".conf"),
    // QSettings::NativeFormat);
    // settings.setValue("P4FindDlg/state",saveState());
}

void P4FindDlg::onBtnLoad()
{
    if (gThisVF->load() == false) {
        QMessageBox::critical(this, "P4",
                              "Unable to find the input vector field.\n");
    } else {
        // we first signal that the vector field has changed to invalidate all
        // open windows.
        bool oldeval = gThisVF->evaluated_;

        if (vfSelectWindow_ != nullptr) {
            if (piecewiseConfigWindow_) {
                gVFResults.readTables(gThisVF->getbarefilename(), true, true);
            }
        }

        updateDlgData();
        if (gThisVF->changed_ == false) {
            gThisVF->changed_ = true;
            gP4app->signalChanged();
        }

        // then, we signal that the vector field is unchanged because a save
        // vector field is not needed when quiting.
        gThisVF->changed_ = false;
        gThisVF->evaluated_ = oldeval;
        gP4app->signalLoaded();
    }
}

void P4FindDlg::onBtnSave()
{
    getDataFromDlg();
    if (gThisVF->changed_)
        gThisVF->cleared_ = false;
    if (gThisVF->cleared_) {
        QMessageBox::critical(this, "P4",
                              "No data has been entered.\n"
                              "Please specify vector field.\n");
        return;
    }
    if (gThisVF->save() == false) {
        QMessageBox::critical(
            this, "P4",
            "Unable to save the input vector field.\n"
            "Please check permissions on the write location.\n");
    }
}

void P4FindDlg::onBtnEval()
{
    if (gThisVF->evaluating_)
        return;

    getDataFromDlg();
    if (gThisVF->getfilename().length() == 0) {
        // error: need to specify filename first
        QMessageBox::critical(this, "P4",
                              "Unable to evaluate.\n"
                              "Please specify a valid filename.\n");
        return;
    }

    if (gThisVF->changed_)
        gThisVF->cleared_ = false;

    if (gThisVF->cleared_) {
        QMessageBox::critical(this, "P4",
                              "No data has been entered.\n"
                              "Please specify vector field.\n");
        return;
    }

    if (gActionOnlyPrepareFile) {
        gThisVF->evaluated_ = false;
        gThisVF->evaluating_ = false;
        gThisVF->prepare();
    } else {
        gThisVF->evaluated_ = false;
        gThisVF->evaluating_ = true;
        gThisVF->evaluatingPiecewiseConfig_ = false;
        signalEvaluating();
        gThisVF->evaluate();
    }
}

void P4FindDlg::getDataFromDlg()
{
    if (vfWindow_ != nullptr) {
        vfWindow_->getDataFromDlg();
    }
    if (paramsWindow_ != nullptr) {
        paramsWindow_->getDataFromDlg();
    }
}

void P4FindDlg::updateDlgData()
{
    switch (gThisVF->typeofstudy_) {
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

    if (vfWindow_ != nullptr) {
        vfWindow_->updateDlgData();
    }
    if (paramsWindow_ != nullptr) {
        paramsWindow_->updateDlgData();
    }
    if (vfSelectWindow_ != nullptr)
        vfSelectWindow_->updateDlgData();
}

void P4FindDlg::signalEvaluating() { btn_eval_->setEnabled(false); }

void P4FindDlg::signalEvaluated() { btn_eval_->setEnabled(true); }

void P4FindDlg::signalSeparatingCurvesEvaluated()
{
    btn_eval_->setEnabled(true);
    if (vfSelectWindow_ != nullptr) {
        if (piecewiseConfigWindow_ != nullptr)
            piecewiseConfigWindow_->signalSeparatingCurvesEvaluated();
    }
}

P4VFSelectDlg *P4FindDlg::getVfSelectWindowPtr() const
{
    return vfSelectWindow_;
}

P4StartDlg *P4FindDlg::getParentPtr() const { return parent_; }

P4SeparatingCurvesDlg *P4FindDlg::getPiecewiseConfigWindowPtr() const
{
    return piecewiseConfigWindow_;
}

void P4FindDlg::createPiecewiseConfigWindow()
{
    piecewiseConfigWindow_ = new P4SeparatingCurvesDlg{this};
}

void P4FindDlg::closePiecewiseConfigWindow()
{
    delete piecewiseConfigWindow_;
    piecewiseConfigWindow_ = nullptr;
}
