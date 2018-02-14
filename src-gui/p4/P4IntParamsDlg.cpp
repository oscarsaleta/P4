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

#include "P4IntParamsDlg.h"

#include "custom.h"
#include "file_tab.h"
#include "file_vf.h"
#include "main.h"
#include "p4application.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

P4IntParamsDlg::~P4IntParamsDlg() { getDataFromDlg(); }

P4IntParamsDlg::P4IntParamsDlg()
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    std::unique_ptr<QLabel> kindlabel{new QLabel("Vector Field: ", this)};
    kindlabel->setFont(*(g_p4app->boldFont_));
    std::unique_ptr<QButtonGroup> btngrp1{new QButtonGroup(this)};
    btn_org_.reset(new QRadioButton("Original", this));
    btn_red_.reset(new QRadioButton("Reduced", this));
    btngrp1->addButton(btn_org_);
    btngrp1->addButton(btn_red_);

    std::unique_ptr<QLabel> typelabel{new QLabel("Appearance: ", this)};
    typelabel->setFont(*(g_p4app->boldFont_));
    std::unique_ptr<QButtonGroup> btngrp2{new QButtonGroup(this)};
    btn_dots_.reset(new QRadioButton("Dots", this));
    btn_dashes_.reset(new QRadioButton("Dashes", this));
    btngrp2->addButton(btn_dots_);
    btngrp2->addButton(btn_dashes_);

    lbl_stepsize_.reset(new QLabel("Step Size:", this));
    lbl_stepsize_->setFont(*(g_p4app->boldFont_));
    edt_stepsize_.reset(new QLineEdit("0.01", this));

    lbl0_curstep_.reset(new QLabel("Current Step Size:", this));
    lbl0_curstep_->setFont(*(g_p4app->boldFont_));
    lbl_curstep_.reset(new QLabel("0.01", this));

    lbl_maxstep_.reset(new QLabel("Max Step Size:", this));
    lbl_maxstep_->setFont(*(g_p4app->boldFont_));
    edt_maxstep_.reset(new QLineEdit("0.1", this));

    lbl_minstep_.reset(new QLabel("Min Step Size:", this));
    lbl_minstep_->setFont(*(g_p4app->boldFont_));
    edt_minstep_.reset(new QLineEdit("1e-06", this));

    lbl_branchminstep_.reset(new QLabel("Branching Step Size:", this));
    lbl_branchminstep_->setFont(*(g_p4app->boldFont_));
    edt_branchminstep_.reset(new QLineEdit("1e-03", this));

    lbl_tolerance_.reset(new QLabel("Tolerance:", this));
    lbl_tolerance_->setFont(*(g_p4app->boldFont_));
    edt_tolerance_.reset(new QLineEdit("1e-06", this));

    std::unique_ptr<QLabel> lbl_numpoints{new QLabel("# Points:", this)};
    lbl_numpoints->setFont(*(g_p4app->boldFont_));
    spin_numpoints_.reset(new QSpinBox(this));
    spin_numpoints_->setMinimum(MIN_INTPOINTS);
    spin_numpoints_->setMaximum(MAX_INTPOINTS);

    btn_reset_.reset(new QPushButton("&Reset", this));

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
    mainLayout_.reset(new QBoxLayout(QBoxLayout::TopToBottom, this));

    std::unique_ptr<QHBoxLayout> kindLayout{new QHBoxLayout()};
    kindLayout->addWidget(kindlabel);
    kindLayout->addWidget(btn_org_);
    kindLayout->addWidget(btn_red_);

    mainLayout_->addLayout(kindLayout);

    std::unique_ptr<QHBoxLayout> typeLayout{new QHBoxLayout()};
    typeLayout->addWidget(typelabel);
    typeLayout->addWidget(btn_dots_);
    typeLayout->addWidget(btn_dashes_);
    mainLayout_->addLayout(typeLayout);

    std::unique_ptr<QHBoxLayout> layout2{new QHBoxLayout()};
    layout2->addWidget(lbl_stepsize_);
    layout2->addWidget(edt_stepsize_);
    layout2->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout3{new QHBoxLayout()};
    layout3->addWidget(lbl0_curstep_);
    layout3->addWidget(lbl_curstep_);
    layout3->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout4{new QHBoxLayout()};
    layout4->addWidget(lbl_maxstep_);
    layout4->addWidget(edt_maxstep_);
    layout4->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout5{new QHBoxLayout()};
    layout5->addWidget(lbl_minstep_);
    layout5->addWidget(edt_minstep_);
    layout5->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout5b{new QHBoxLayout()};
    layout5b->addWidget(lbl_branchminstep_);
    layout5b->addWidget(edt_branchminstep_);
    layout5b->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout6{new QHBoxLayout()};
    layout6->addWidget(lbl_tolerance_);
    layout6->addWidget(edt_tolerance_);
    layout6->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout7{new QHBoxLayout()};
    layout7->addWidget(lbl_numpoints);
    layout7->addWidget(spin_numpoints_);
    layout7->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout8{new QHBoxLayout()};
    layout8->addStretch(1);
    layout8->addWidget(btn_reset_);
    layout8->addStretch(1);

    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);
    mainLayout_->addLayout(layout5);
    mainLayout_->addLayout(layout5b);
    mainLayout_->addLayout(layout6);
    mainLayout_->addLayout(layout7);
    mainLayout_->addLayout(layout8);
    mainLayout_->addStretch(0);

    setLayout(mainLayout_);

    // connections
    connect(btn_org_, &QRadioButton::toggled, this, [=]() { changed_ = true; });
    connect(btn_red_, &QRadioButton::toggled, this, [=]() { changed_ = true; });
    connect(btn_dots_, &QRadioButton::toggled, this,
            [=]() { changed_ = true; });
    connect(btn_dashes_, &QRadioButton::toggled, this,
            [=]() { changed_ = true; });

    connect(edt_stepsize_, &QLineEdit::textChanged, this,
            [=]() { changed_ = true; });
    connect(edt_maxstep_, &QLineEdit::textChanged, this,
            [=]() { changed_ = true; });
    connect(edt_minstep_, &QLineEdit::textChanged, this,
            [=]() { changed_ = true; });
    connect(edt_branchminstep_, &QLineEdit::textChanged, this,
            [=]() { changed_ = true; });
    connect(edt_tolerance_, &QLineEdit::textChanged, this,
            [=]() { changed_ = true; });
    connect(spin_numpoints_,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
            [=]() { changed_ = true; });

    connect(btn_reset_, &QPushButton::clicked, this,
            &P4IntParamsDlg::on_btn_reset);

    // finishing
    updateDlgData();

    setP4WindowTitle(this, "Integration Parameters");
}

bool P4IntParamsDlg::readFloatField(std::unique_ptr<QLineEdit> edt,
                                    double &presult, double defvalue,
                                    double minvalue, double maxvalue)
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

void P4IntParamsDlg::markBad(std::unique_ptr<QLineEdit> edt)
{
    QString t{edt->text()};
    int i;

    while ((i = t.indexOf('?')) >= 0)
        t.remove(i, 1);
    t = t.trimmed();
    t.append(" ???");

    edt->setText(t);
}

void P4IntParamsDlg::getDataFromDlg(void)
{
    if (!changed_)
        return;

    g_VFResults.config_kindvf_ =
        (btn_org_->isChecked()) ? INTCONFIG_ORIGINAL : INTCONFIG_REDUCED;
    g_VFResults.config_dashes_ = (btn_dashes_->isChecked()) ? true : false;

    changed_ = false;
    changed_ |= readFloatField(edt_tolerance_, g_VFResults.config_tolerance_,
                               DEFAULT_TOLERANCE, MIN_TOLERANCE, MAX_TOLERANCE);
    changed_ |= readFloatField(edt_minstep_, g_VFResults.config_hmi_,
                               DEFAULT_HMI, MIN_HMI, MAX_HMI);
    changed_ |= readFloatField(edt_maxstep_, g_VFResults.config_hma_,
                               DEFAULT_HMA, MIN_HMA, MAX_HMA);
    changed |= ReadFloatField(edt_branchminstep_, g_VFResults.config_branchhmi_,
                              DEFAULT_BRANCHHMI, MIN_BRANCHHMI, MAX_BRANCHHMI);
    changed_ |= readFloatField(edt_stepsize_, g_VFResults.config_step_,
                               DEFAULT_STEPSIZE, MIN_HMI, MAX_HMA);

    g_VFResults.config_intpoints_ = spin_numpoints_->value();

    if (!changed_) {
        if (g_VFResults.config_step_ < g_VFResults.config_hmi_ ||
            g_VFResults.config_step_ > g_VFResults.config_hma_) {
            // error
            g_VFResults.config_step_ = DEFAULT_STEPSIZE;
            if (g_VFResults.config_step_ < g_VFResults.config_hmi_ ||
                g_VFResults.config_step_ > g_VFResults.config_hma_) {
                g_VFResults.config_step_ = g_VFResults.config_hma_;
            }
            changed_ = true;
            markBad(edt_stepsize_);
        }
    }
}

void P4IntParamsDlg::updateDlgData(void)
{
    QString buf;

    changed_ = false;

    if (g_VFResults.config_kindvf_ == INTCONFIG_ORIGINAL)
        btn_org_->toggle();
    else
        btn_red_->toggle();

    if (g_VFResults.config_dashes_)
        btn_dashes_->toggle();
    else
        btn_dots_->toggle();

    buf.sprintf("%g", (float)(g_VFResults.config_step_));
    edt_stepsize_->setText(buf);

    buf.sprintf("%g", (float)(g_VFResults.config_currentstep_));
    lbl_curstep_->setText(buf);

    buf.sprintf("%g", (float)(g_VFResults.config_hmi_));
    edt_minstep_->setText(buf);

    buf.sprintf("%g", (float)(g_VFResults.config_branchhmi_));
    edt_branchminstep_->setText(buf);

    buf.sprintf("%g", (float)(g_VFResults.config_hma_));
    edt_maxstep_->setText(buf);

    buf.sprintf("%g", (float)(g_VFResults.config_tolerance_));
    edt_tolerance_->setText(buf);

    spin_numpoints_->setValue(g_VFResults.config_intpoints_);

    if (!g_VFResults.gcf_.empty() ||
        (g_VFResults.singinf_ && g_VFResults.typeofstudy_ != TYPEOFSTUDY_ONE)) {
        btn_org_->setEnabled(true);
        btn_red_->setEnabled(true);
    } else {
        btn_org_->setEnabled(false);
        btn_red_->setEnabled(false);
    }
}

void P4IntParamsDlg::setCurrentStep(double curstep)
{
    QString buf;
    buf.sprintf("%g", (float)curstep);
    lbl_curstep_->setText(buf);
}

void P4IntParamsDlg::on_btn_reset(void)
{
    // maximum step size
    g_VFResults.config_hma_ = DEFAULT_HMA;
    // minimum step size
    g_VFResults.config_hmi_ = DEFAULT_HMI;
    // step size
    g_VFResults.config_step_ = DEFAULT_STEPSIZE;
    // current step size (during integration)
    g_VFResults.config_currentstep_ = DEFAULT_STEPSIZE;
    // tolerance
    g_VFResults.config_tolerance_ = DEFAULT_TOLERANCE;
    // projection in the case of Poincare sphere
    g_VFResults.config_projection_ = DEFAULT_PROJECTION;
    // number of points to integrate
    g_VFResults.config_intpoints_ = DEFAULT_INTPOINTS;
    // line style (dashes or points)
    g_VFResults.config_dashes_ = DEFAULT_LINESTYLE;
    // original or reduced VF
    g_VFResults.config_kindvf_ = DEFAULT_INTCONFIG;

    updateDlgData();
}
