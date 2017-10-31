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

#include "win_intparams.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "p4application.h"

#include <QButtonGroup>

QIntParamsDlg::~QIntParamsDlg() { getDataFromDlg(); }

QIntParamsDlg::QIntParamsDlg()
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    QLabel *kindlabel = new QLabel("Vector Field: ", this);
    kindlabel->setFont(*(g_p4app->boldFont_));
    QButtonGroup *btngrp1 = new QButtonGroup(this);
    btn_org_ = new QRadioButton("Original", this);
    btn_red_ = new QRadioButton("Reduced", this);
    btngrp1->addButton(btn_org_);
    btngrp1->addButton(btn_red_);

    QLabel *typelabel = new QLabel("Appearance: ", this);
    typelabel->setFont(*(g_p4app->boldFont_));
    QButtonGroup *btngrp2 = new QButtonGroup(this);
    btn_dots_ = new QRadioButton("Dots", this);
    btn_dashes_ = new QRadioButton("Dashes", this);
    btngrp2->addButton(btn_dots_);
    btngrp2->addButton(btn_dashes_);

    lbl_stepsize_ = new QLabel("Step Size:", this);
    lbl_stepsize_->setFont(*(g_p4app->boldFont_));
    edt_stepsize_ = new QLineEdit("0.01", this);

    lbl0_curstep_ = new QLabel("Current Step Size:", this);
    lbl0_curstep_->setFont(*(g_p4app->boldFont_));
    lbl_curstep_ = new QLabel("0.01", this);

    lbl_maxstep_ = new QLabel("Max Step Size:", this);
    lbl_maxstep_->setFont(*(g_p4app->boldFont_));
    edt_maxstep_ = new QLineEdit("0.1", this);

    lbl_minstep_ = new QLabel("Min Step Size:", this);
    lbl_minstep_->setFont(*(g_p4app->boldFont_));
    edt_minstep_ = new QLineEdit("1e-06", this);

    lbl_tolerance_ = new QLabel("Tolerance:", this);
    lbl_tolerance_->setFont(*(g_p4app->boldFont_));
    edt_tolerance_ = new QLineEdit("1e-06", this);

    QLabel *lbl_numpoints = new QLabel("# Points:", this);
    lbl_numpoints->setFont(*(g_p4app->boldFont_));
    spin_numpoints_ = new QSpinBox(this);
    spin_numpoints_->setMinimum(MIN_INTPOINTS);
    spin_numpoints_->setMaximum(MAX_INTPOINTS);

    btn_reset_ = new QPushButton("&Reset", this);

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

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *kindLayout = new QHBoxLayout();
    kindLayout->addWidget(kindlabel);
    kindLayout->addWidget(btn_org_);
    kindLayout->addWidget(btn_red_);

    mainLayout_->addLayout(kindLayout);

    QHBoxLayout *typeLayout = new QHBoxLayout();
    typeLayout->addWidget(typelabel);
    typeLayout->addWidget(btn_dots_);
    typeLayout->addWidget(btn_dashes_);
    mainLayout_->addLayout(typeLayout);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(lbl_stepsize_);
    layout2->addWidget(edt_stepsize_);
    layout2->addStretch(0);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(lbl0_curstep_);
    layout3->addWidget(lbl_curstep_);
    layout3->addStretch(0);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addWidget(lbl_maxstep_);
    layout4->addWidget(edt_maxstep_);
    layout4->addStretch(0);

    QHBoxLayout *layout5 = new QHBoxLayout();
    layout5->addWidget(lbl_minstep_);
    layout5->addWidget(edt_minstep_);
    layout5->addStretch(0);

    QHBoxLayout *layout6 = new QHBoxLayout();
    layout6->addWidget(lbl_tolerance_);
    layout6->addWidget(edt_tolerance_);
    layout6->addStretch(0);

    QHBoxLayout *layout7 = new QHBoxLayout();
    layout7->addWidget(lbl_numpoints);
    layout7->addWidget(spin_numpoints_);
    layout7->addStretch(0);

    QHBoxLayout *layout8 = new QHBoxLayout();
    layout8->addStretch(1);
    layout8->addWidget(btn_reset_);
    layout8->addStretch(1);

    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);
    mainLayout_->addLayout(layout5);
    mainLayout_->addLayout(layout6);
    mainLayout_->addLayout(layout7);
    mainLayout_->addLayout(layout8);
    mainLayout_->addStretch(0);

    //   mainLayout_->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(mainLayout_);

    /*QButtonGroup *btngrp1 = new QButtonGroup(this);
    btngrp1->addButton(btn_org_);
    btngrp1->addButton(btn_red_);

    QButtonGroup *btngrp2 = new QButtonGroup(this);
    btngrp2->addButton(btn_dots_);
    btngrp2->addButton(btn_dashes_);*/

    // connections

    QObject::connect(btn_org_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_org_toggled(bool)));
    QObject::connect(btn_red_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_red_toggled(bool)));
    QObject::connect(btn_dots_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_dots_toggled(bool)));
    QObject::connect(btn_dashes_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_dashes_toggled(bool)));
    QObject::connect(edt_stepsize_, SIGNAL(textChanged(const QString &)), this,
                     SLOT(onFieldChange(const QString &)));
    // QObject::connect( edt_curstep,        SIGNAL(textChanged(const QString
    // &)), this, SLOT(onFieldChange(const QString &)) );
    QObject::connect(edt_maxstep_, SIGNAL(textChanged(const QString &)), this,
                     SLOT(onFieldChange(const QString &)));
    QObject::connect(edt_minstep_, SIGNAL(textChanged(const QString &)), this,
                     SLOT(onFieldChange(const QString &)));
    QObject::connect(edt_tolerance_, SIGNAL(textChanged(const QString &)), this,
                     SLOT(onFieldChange(const QString &)));
    QObject::connect(spin_numpoints_, SIGNAL(valueChanged(int)), this,
                     SLOT(onFieldChange(int)));
    QObject::connect(btn_reset_, SIGNAL(clicked()), this, SLOT(on_btn_reset()));

    // finishing

    updateDlgData();

    setP4WindowTitle(this, "Integration Parameters");
}

void QIntParamsDlg::onFieldChange(const QString &dummy)
{
    UNUSED(dummy);
    changed_ = true;
}

void QIntParamsDlg::onFieldChange(int dummy)
{
    UNUSED(dummy);
    changed_ = true;
}

void QIntParamsDlg::btn_org_toggled(bool dummy)
{
    UNUSED(dummy);
    changed_ = true;
}

void QIntParamsDlg::btn_red_toggled(bool dummy)
{
    UNUSED(dummy);
    changed_ = true;
}

void QIntParamsDlg::btn_dots_toggled(bool dummy)
{
    UNUSED(dummy);
    changed_ = true;
}

void QIntParamsDlg::btn_dashes_toggled(bool dummy)
{
    UNUSED(dummy);
    changed_ = true;
}

bool QIntParamsDlg::readFloatField(QLineEdit *edt, double *presult,
                                   double defvalue, double minvalue,
                                   double maxvalue)
{
    // returns true in case of error

    QString x, t;
    bool ok;

    t = edt->text();
    *presult = t.toDouble(&ok);
    if (!ok || *presult < minvalue || *presult > maxvalue) {
        markBad(edt);
        *presult = defvalue;
        return true;
    }

    x.sprintf("%g", (float)(*presult));
    if (x != t)
        edt->setText(x);

    return false;
}

void QIntParamsDlg::markBad(QLineEdit *edt)
{
    QString t;
    int i;

    t = edt->text();
    while ((i = t.indexOf('?')) >= 0)
        t.remove(i, 1);
    t = t.trimmed();
    t.append(" ???");

    edt->setText(t);
}

void QIntParamsDlg::getDataFromDlg(void)
{
    if (!changed_) {
        return;
    }

    changed_ = false;
    changed_ |= readFloatField(edt_tolerance_, &(g_VFResults.config_tolerance_),
                               DEFAULT_TOLERANCE, MIN_TOLERANCE, MAX_TOLERANCE);
    changed_ |= readFloatField(edt_minstep_, &(g_VFResults.config_hmi_),
                               DEFAULT_HMI, MIN_HMI, MAX_HMI);
    changed_ |= readFloatField(edt_maxstep_, &(g_VFResults.config_hma_),
                               DEFAULT_HMA, MIN_HMA, MAX_HMA);
    changed_ |= readFloatField(edt_stepsize_, &(g_VFResults.config_step_),
                               DEFAULT_STEPSIZE, MIN_HMI, MAX_HMA);
    //  changed_ |= readFloatField( edt_curstep,
    //  &(g_VFResults.config_currentstep_),
    //                      DEFAULT_STEPSIZE, MIN_HMI, MAX_HMA );

    g_VFResults.config_intpoints_ = spin_numpoints_->value();

    g_VFResults.config_kindvf_ =
        (btn_org_->isChecked()) ? INTCONFIG_ORIGINAL : INTCONFIG_REDUCED;
    g_VFResults.config_dashes_ = (btn_dashes_->isChecked()) ? true : false;

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
        /*
                if( g_VFResults.config_currentstep_ < g_VFResults.config_hmi_ ||
           g_VFResults.config_currentstep_ > g_VFResults.config_hma_ )
                {
                    // error

                    g_VFResults.config_currentstep_ = DEFAULT_STEPSIZE;
                    if( g_VFResults.config_currentstep_ <
           g_VFResults.config_hmi_ ||
           g_VFResults.config_currentstep_ > g_VFResults.config_hma_ )
                    {
                        g_VFResults.config_currentstep_ =
           g_VFResults.config_hma_;
                    }
                    edt_curstep->setBackgroundColor( QColor(255,192,192) );
                    changed_ = true;
                    markBad( edt_curstep );
                }
        */
    }
}

void QIntParamsDlg::updateDlgData(void)
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

    buf.sprintf("%g", (float)(g_VFResults.config_hma_));
    edt_maxstep_->setText(buf);

    buf.sprintf("%g", (float)(g_VFResults.config_tolerance_));
    edt_tolerance_->setText(buf);

    spin_numpoints_->setValue(g_VFResults.config_intpoints_);

    if (g_VFResults.gcf_ != nullptr ||
        (g_VFResults.singinf_ && g_VFResults.typeofstudy_ != TYPEOFSTUDY_ONE)) {
        btn_org_->setEnabled(true);
        btn_red_->setEnabled(true);
    } else {
        btn_org_->setEnabled(false);
        btn_red_->setEnabled(false);
    }
}

void QIntParamsDlg::setCurrentStep(double curstep)
{
    QString buf;
    buf.sprintf("%g", (float)curstep);
    lbl_curstep_->setText(buf);
}

void QIntParamsDlg::on_btn_reset(void)
{
    g_VFResults.config_hma_ = DEFAULT_HMA;       // maximum step size
    g_VFResults.config_hmi_ = DEFAULT_HMI;       // minimum step size
    g_VFResults.config_step_ = DEFAULT_STEPSIZE; // step size
    g_VFResults.config_currentstep_ =
        DEFAULT_STEPSIZE; // current step size (during integration)
    g_VFResults.config_tolerance_ = DEFAULT_TOLERANCE; // tolerance
    g_VFResults.config_projection_ =
        DEFAULT_PROJECTION; // projection in the case of Poincare sphere
    g_VFResults.config_intpoints_ =
        DEFAULT_INTPOINTS; // number of points to integrate
    g_VFResults.config_dashes_ =
        DEFAULT_LINESTYLE; // line style (dashes or points)
    g_VFResults.config_kindvf_ = DEFAULT_INTCONFIG;

    updateDlgData();
}
