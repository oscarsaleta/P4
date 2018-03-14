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

#include "P4IntParamsDlg.hpp"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

#include "P4Application.hpp"
#include "P4ParentStudy.hpp"
#include "main.hpp"

P4IntParamsDlg::~P4IntParamsDlg() { getDataFromDlg(); }

P4IntParamsDlg::P4IntParamsDlg()
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    auto kindlabel = std::make_unique<QLabel>("Vector Field: ", this);
    kindlabel->setFont(gP4app->getBoldFont());
    auto btngrp1 = std::make_unique<QButtonGroup>(this);
    btn_org_ = std::make_unique<QRadioButton>("Original", this);
    btn_red_ = std::make_unique<QRadioButton>("Reduced", this);
    btngrp1->addButton(btn_org_.get());
    btngrp1->addButton(btn_red_.get());

    auto typelabel = std::make_unique<QLabel>("Appearance: ", this);
    typelabel->setFont(gP4app->getBoldFont());
    auto btngrp2 = std::make_unique<QButtonGroup>(this);
    btn_dots_ = std::make_unique<QRadioButton>("Dots", this);
    btn_dashes_ = std::make_unique<QRadioButton>("Dashes", this);
    btngrp2->addButton(btn_dots_.get());
    btngrp2->addButton(btn_dashes_.get());

    lbl_stepsize_ = std::make_unique<QLabel>("Step Size:", this);
    lbl_stepsize_->setFont(gP4app->getBoldFont());
    edt_stepsize_ = std::make_unique<QLineEdit>("0.01", this);

    lbl0_curstep_ = std::make_unique<QLabel>("Current Step Size:", this);
    lbl0_curstep_->setFont(gP4app->getBoldFont());
    lbl_curstep_ = std::make_unique<QLabel>("0.01", this);

    lbl_maxstep_ = std::make_unique<QLabel>("Max Step Size:", this);
    lbl_maxstep_->setFont(gP4app->getBoldFont());
    edt_maxstep_ = std::make_unique<QLineEdit>("0.1", this);

    lbl_minstep_ = std::make_unique<QLabel>("Min Step Size:", this);
    lbl_minstep_->setFont(gP4app->getBoldFont());
    edt_minstep_ = std::make_unique<QLineEdit>("1e-06", this);

    lbl_branchminstep_ = std::make_unique<QLabel>("Branching Step Size:", this);
    lbl_branchminstep_->setFont(gP4app->getBoldFont());
    edt_branchminstep_ = std::make_unique<QLineEdit>("1e-03", this);

    lbl_tolerance_ = std::make_unique<QLabel>("Tolerance:", this);
    lbl_tolerance_->setFont(gP4app->getBoldFont());
    edt_tolerance_ = std::make_unique<QLineEdit>("1e-06", this);

    auto lbl_numpoints = std::make_unique<QLabel>("# Points:", this);
    lbl_numpoints->setFont(gP4app->getBoldFont());
    spin_numpoints_ = std::make_unique<QSpinBox>(this);
    spin_numpoints_->setMinimum(MIN_INTPOINTS);
    spin_numpoints_->setMaximum(MAX_INTPOINTS);

    btn_reset_ = std::make_unique<QPushButton>("&Reset", this);

#ifdef TOOLTIPS
    btn_org_->setToolTip("Integrate orbits w.r.t. original vector field");
    btn_red_->setToolTip("Integrate orbits w.r.t. reduced vector field,\n"
                         "removing common factors.\n"
                         "This is disabled if there are no common factors.");
    btn_dots_->setToolTip("Plot individual points during orbit-integration");
    btn_dashes_->setToolTip(
        "Connect orbit-integration points with small line segments");
    edt_stepsize_->setToolTip(
        "Runge-Kutta 7/8 initial step size when starting integration");
    lbl_curstep_->setToolTip(
        "Runge-Kutta 7/8 initial step size when continuing integration");
    edt_maxstep_->setToolTip("Runge-Kutta 7/8 maximum step size");
    edt_minstep_->setToolTip("Runge-Kutta 7/8 minimum step size");
    edt_tolerance_->setToolTip("Runge-Kutta Tolerance");
    spin_numpoints_->setToolTip("Number of points to integrate each time");
    btn_reset_->setToolTip(
        "Reset the integration parameters to default values");
#endif

    // layout
    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    auto kindLayout = std::make_unique<QHBoxLayout>();
    kindLayout->addWidget(kindlabel.get());
    kindLayout->addWidget(btn_org_.get());
    kindLayout->addWidget(btn_red_.get());

    mainLayout_->addLayout(kindLayout.get());

    auto typeLayout = std::make_unique<QHBoxLayout>();
    typeLayout->addWidget(typelabel.get());
    typeLayout->addWidget(btn_dots_.get());
    typeLayout->addWidget(btn_dashes_.get());
    mainLayout_->addLayout(typeLayout.get());

    auto layout2 = std::make_unique<QHBoxLayout>();
    layout2->addWidget(lbl_stepsize_.get());
    layout2->addWidget(edt_stepsize_.get());
    layout2->addStretch(0);

    auto layout3 = std::make_unique<QHBoxLayout>();
    layout3->addWidget(lbl0_curstep_.get());
    layout3->addWidget(lbl_curstep_.get());
    layout3->addStretch(0);

    auto layout4 = std::make_unique<QHBoxLayout>();
    layout4->addWidget(lbl_maxstep_.get());
    layout4->addWidget(edt_maxstep_.get());
    layout4->addStretch(0);

    auto layout5 = std::make_unique<QHBoxLayout>();
    layout5->addWidget(lbl_minstep_.get());
    layout5->addWidget(edt_minstep_.get());
    layout5->addStretch(0);

    auto layout5b = std::make_unique<QHBoxLayout>();
    layout5b->addWidget(lbl_branchminstep_.get());
    layout5b->addWidget(edt_branchminstep_.get());
    layout5b->addStretch(0);

    auto layout6 = std::make_unique<QHBoxLayout>();
    layout6->addWidget(lbl_tolerance_.get());
    layout6->addWidget(edt_tolerance_.get());
    layout6->addStretch(0);

    auto layout7 = std::make_unique<QHBoxLayout>();
    layout7->addWidget(lbl_numpoints.get());
    layout7->addWidget(spin_numpoints_.get());
    layout7->addStretch(0);

    auto layout8 = std::make_unique<QHBoxLayout>();
    layout8->addStretch(1);
    layout8->addWidget(btn_reset_.get());
    layout8->addStretch(1);

    mainLayout_->addLayout(layout2.get());
    mainLayout_->addLayout(layout3.get());
    mainLayout_->addLayout(layout4.get());
    mainLayout_->addLayout(layout5.get());
    mainLayout_->addLayout(layout5b.get());
    mainLayout_->addLayout(layout6.get());
    mainLayout_->addLayout(layout7.get());
    mainLayout_->addLayout(layout8.get());
    mainLayout_->addStretch(0);

    setLayout(mainLayout_.get());

    // connections
    QObject::connect(btn_org_.get(), &QRadioButton::toggled, this,
                     [=]() { changed_ = true; });
    QObject::connect(btn_red_.get(), &QRadioButton::toggled, this,
                     [=]() { changed_ = true; });
    QObject::connect(btn_dots_.get(), &QRadioButton::toggled, this,
                     [=]() { changed_ = true; });
    QObject::connect(btn_dashes_.get(), &QRadioButton::toggled, this,
                     [=]() { changed_ = true; });

    QObject::connect(edt_stepsize_.get(), &QLineEdit::textChanged, this,
                     [=]() { changed_ = true; });
    QObject::connect(edt_maxstep_.get(), &QLineEdit::textChanged, this,
                     [=]() { changed_ = true; });
    QObject::connect(edt_minstep_.get(), &QLineEdit::textChanged, this,
                     [=]() { changed_ = true; });
    QObject::connect(edt_branchminstep_.get(), &QLineEdit::textChanged, this,
                     [=]() { changed_ = true; });
    QObject::connect(edt_tolerance_.get(), &QLineEdit::textChanged, this,
                     [=]() { changed_ = true; });
    QObject::connect(
        spin_numpoints_.get(),
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        [=]() { changed_ = true; });

    QObject::connect(btn_reset_.get(), &QPushButton::clicked, this,
                     &P4IntParamsDlg::on_btn_reset);

    // finishing
    updateDlgData();

    setP4WindowTitle(this, "Integration Parameters");
}

bool P4IntParamsDlg::readFloatField(QLineEdit *edt, double &presult,
                                    double defvalue, double minvalue,
                                    double maxvalue)
{
    // returns true in case of error
    QString x;
    QString t{edt->text()};
    bool ok;

    presult = t.toDouble(&ok);
    if (!ok || presult < minvalue || presult > maxvalue) {
        markBad(edt);
        presult = defvalue;
        return true;
    }

    x.sprintf("%g", presult);
    if (x != t)
        edt->setText(x);
    return false;
}

void P4IntParamsDlg::markBad(QLineEdit *edt)
{
    QString t{edt->text()};
    int i;

    while ((i = t.indexOf('?')) >= 0)
        t.remove(i, 1);
    t = t.trimmed();
    t.append(" ???");

    edt->setText(t);
}

void P4IntParamsDlg::getDataFromDlg()
{
    if (!changed_)
        return;

    gVFResults.config_kindvf_ =
        (btn_org_->isChecked()) ? INTCONFIG_ORIGINAL : INTCONFIG_REDUCED;
    gVFResults.config_dashes_ = (btn_dashes_->isChecked()) ? true : false;

    changed_ = false;
    changed_ |=
        readFloatField(edt_tolerance_.get(), gVFResults.config_tolerance_,
                       DEFAULT_TOLERANCE, MIN_TOLERANCE, MAX_TOLERANCE);
    changed_ |= readFloatField(edt_minstep_.get(), gVFResults.config_hmi_,
                               DEFAULT_HMI, MIN_HMI, MAX_HMI);
    changed_ |= readFloatField(edt_maxstep_.get(), gVFResults.config_hma_,
                               DEFAULT_HMA, MIN_HMA, MAX_HMA);
    changed_ |=
        readFloatField(edt_branchminstep_.get(), gVFResults.config_branchhmi_,
                       DEFAULT_BRANCHHMI, MIN_BRANCHHMI, MAX_BRANCHHMI);
    changed_ |= readFloatField(edt_stepsize_.get(), gVFResults.config_step_,
                               DEFAULT_STEPSIZE, MIN_HMI, MAX_HMA);

    gVFResults.config_intpoints_ = spin_numpoints_->value();

    if (!changed_) {
        if (gVFResults.config_step_ < gVFResults.config_hmi_ ||
            gVFResults.config_step_ > gVFResults.config_hma_) {
            // error
            gVFResults.config_step_ = DEFAULT_STEPSIZE;
            if (gVFResults.config_step_ < gVFResults.config_hmi_ ||
                gVFResults.config_step_ > gVFResults.config_hma_) {
                gVFResults.config_step_ = gVFResults.config_hma_;
            }
            changed_ = true;
            markBad(edt_stepsize_.get());
        }
    }
}

void P4IntParamsDlg::updateDlgData()
{
    QString buf;

    changed_ = false;

    if (gVFResults.config_kindvf_ == INTCONFIG_ORIGINAL)
        btn_org_->toggle();
    else
        btn_red_->toggle();

    if (gVFResults.config_dashes_)
        btn_dashes_->toggle();
    else
        btn_dots_->toggle();

    buf.sprintf("%g", gVFResults.config_step_);
    edt_stepsize_->setText(buf);

    buf.sprintf("%g", gVFResults.config_currentstep_);
    lbl_curstep_->setText(buf);

    buf.sprintf("%g", gVFResults.config_hmi_);
    edt_minstep_->setText(buf);

    buf.sprintf("%g", gVFResults.config_branchhmi_);
    edt_branchminstep_->setText(buf);

    buf.sprintf("%g", gVFResults.config_hma_);
    edt_maxstep_->setText(buf);

    buf.sprintf("%g", gVFResults.config_tolerance_);
    edt_tolerance_->setText(buf);

    spin_numpoints_->setValue(gVFResults.config_intpoints_);

    btn_org_->setEnabled(true);
    btn_red_->setEnabled(true);
}

void P4IntParamsDlg::setCurrentStep(double curstep)
{
    QString buf;
    buf.sprintf("%g", curstep);
    lbl_curstep_->setText(buf);
}

void P4IntParamsDlg::on_btn_reset()
{
    // maximum step size
    gVFResults.config_hma_ = DEFAULT_HMA;
    // minimum step size
    gVFResults.config_hmi_ = DEFAULT_HMI;
    // step size
    gVFResults.config_step_ = DEFAULT_STEPSIZE;
    // current step size (during integration)
    gVFResults.config_currentstep_ = DEFAULT_STEPSIZE;
    // tolerance
    gVFResults.config_tolerance_ = DEFAULT_TOLERANCE;
    // projection in the case of Poincare sphere
    gVFResults.config_projection_ = DEFAULT_PROJECTION;
    // number of points to integrate
    gVFResults.config_intpoints_ = DEFAULT_INTPOINTS;
    // line style (dashes or points)
    gVFResults.config_dashes_ = DEFAULT_LINESTYLE;
    // original or reduced VF
    gVFResults.config_kindvf_ = DEFAULT_INTCONFIG;

    updateDlgData();
}
