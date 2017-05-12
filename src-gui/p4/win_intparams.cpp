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
    kindlabel->setFont(*(p4app->boldFont_));
    btn_org = new QRadioButton("Original", this);
    btn_red = new QRadioButton("Reduced", this);

    QLabel *typelabel = new QLabel("Appearance: ", this);
    typelabel->setFont(*(p4app->boldFont_));
    btn_dots = new QRadioButton("Dots", this);
    btn_dashes = new QRadioButton("Dashes", this);

    lbl_stepsize = new QLabel("Step Size:", this);
    lbl_stepsize->setFont(*(p4app->boldFont_));
    edt_stepsize = new QLineEdit("0.01", this);

    lbl0_curstep = new QLabel("Current Step Size:", this);
    lbl0_curstep->setFont(*(p4app->boldFont_));
    lbl_curstep = new QLabel("0.01", this);

    lbl_maxstep = new QLabel("Max Step Size:", this);
    lbl_maxstep->setFont(*(p4app->boldFont_));
    edt_maxstep = new QLineEdit("0.1", this);

    lbl_minstep = new QLabel("Min Step Size:", this);
    lbl_minstep->setFont(*(p4app->boldFont_));
    edt_minstep = new QLineEdit("1e-06", this);

    lbl_tolerance = new QLabel("Tolerance:", this);
    lbl_tolerance->setFont(*(p4app->boldFont_));
    edt_tolerance = new QLineEdit("1e-06", this);

    QLabel *lbl_numpoints = new QLabel("# Points:", this);
    lbl_numpoints->setFont(*(p4app->boldFont_));
    spin_numpoints = new QSpinBox(this);
    spin_numpoints->setMinimum(MIN_INTPOINTS);
    spin_numpoints->setMaximum(MAX_INTPOINTS);

    btn_reset = new QPushButton("&Reset", this);

#ifdef TOOLTIPS
    btn_org->setToolTip("Integrate orbits w.r.t. original vector field");
    btn_red->setToolTip("Integrate orbits w.r.t. reduced vector field,\n"
                        "removing common factors.\n"
                        "This is disabled if there are no common factors.");
    btn_dots->setToolTip("Plot individual points during orbit-integration");
    btn_dashes->setToolTip(
        "Connect orbit-integration points with small line segments");
    edt_stepsize->setToolTip(
        "Runge-Kutta 7/8 initial step size when starting integration");
    lbl_curstep->setToolTip(
        "Runge-Kutta 7/8 initial step size when continuing integration");
    edt_maxstep->setToolTip("Runge-Kutta 7/8 maximum step size");
    edt_minstep->setToolTip("Runge-Kutta 7/8 minimum step size");
    edt_tolerance->setToolTip("Runge-Kutta Tolerance");
    spin_numpoints->setToolTip("Number of points to integrate each time");
    btn_reset->setToolTip("Reset the integration parameters to default values");
#endif

    // layout

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *kindLayout = new QHBoxLayout();
    kindLayout->addWidget(kindlabel);
    kindLayout->addWidget(btn_org);
    kindLayout->addWidget(btn_red);

    mainLayout_->addLayout(kindLayout);

    QHBoxLayout *typeLayout = new QHBoxLayout();
    typeLayout->addWidget(typelabel);
    typeLayout->addWidget(btn_dots);
    typeLayout->addWidget(btn_dashes);
    mainLayout_->addLayout(typeLayout);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(lbl_stepsize);
    layout2->addWidget(edt_stepsize);
    layout2->addStretch(0);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(lbl0_curstep);
    layout3->addWidget(lbl_curstep);
    layout3->addStretch(0);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addWidget(lbl_maxstep);
    layout4->addWidget(edt_maxstep);
    layout4->addStretch(0);

    QHBoxLayout *layout5 = new QHBoxLayout();
    layout5->addWidget(lbl_minstep);
    layout5->addWidget(edt_minstep);
    layout5->addStretch(0);

    QHBoxLayout *layout6 = new QHBoxLayout();
    layout6->addWidget(lbl_tolerance);
    layout6->addWidget(edt_tolerance);
    layout6->addStretch(0);

    QHBoxLayout *layout7 = new QHBoxLayout();
    layout7->addWidget(lbl_numpoints);
    layout7->addWidget(spin_numpoints);
    layout7->addStretch(0);

    QHBoxLayout *layout8 = new QHBoxLayout();
    layout8->addStretch(1);
    layout8->addWidget(btn_reset);
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

    QButtonGroup *btngrp1 = new QButtonGroup(this);
    btngrp1->addButton(btn_org);
    btngrp1->addButton(btn_red);

    QButtonGroup *btngrp2 = new QButtonGroup(this);
    btngrp2->addButton(btn_dots);
    btngrp2->addButton(btn_dashes);

    // connections

    QObject::connect(btn_org, SIGNAL(toggled(bool)), this,
                     SLOT(btn_org_toggled(bool)));
    QObject::connect(btn_red, SIGNAL(toggled(bool)), this,
                     SLOT(btn_red_toggled(bool)));
    QObject::connect(btn_dots, SIGNAL(toggled(bool)), this,
                     SLOT(btn_dots_toggled(bool)));
    QObject::connect(btn_dashes, SIGNAL(toggled(bool)), this,
                     SLOT(btn_dashes_toggled(bool)));
    QObject::connect(edt_stepsize, SIGNAL(textChanged(const QString &)), this,
                     SLOT(OnFieldChange(const QString &)));
    // QObject::connect( edt_curstep,        SIGNAL(textChanged(const QString
    // &)), this, SLOT(OnFieldChange(const QString &)) );
    QObject::connect(edt_maxstep, SIGNAL(textChanged(const QString &)), this,
                     SLOT(OnFieldChange(const QString &)));
    QObject::connect(edt_minstep, SIGNAL(textChanged(const QString &)), this,
                     SLOT(OnFieldChange(const QString &)));
    QObject::connect(edt_tolerance, SIGNAL(textChanged(const QString &)), this,
                     SLOT(OnFieldChange(const QString &)));
    QObject::connect(spin_numpoints, SIGNAL(valueChanged(int)), this,
                     SLOT(OnFieldChange(int)));
    QObject::connect(btn_reset, SIGNAL(clicked()), this, SLOT(on_btn_reset()));

    // finishing

    updateDlgData();

    setP4WindowTitle(this, "Integration Parameters");
}

void QIntParamsDlg::OnFieldChange(const QString &dummy)
{
    UNUSED(dummy);
    changed = true;
}

void QIntParamsDlg::OnFieldChange(int dummy)
{
    UNUSED(dummy);
    changed = true;
}

void QIntParamsDlg::btn_org_toggled(bool dummy)
{
    UNUSED(dummy);
    changed = true;
}

void QIntParamsDlg::btn_red_toggled(bool dummy)
{
    UNUSED(dummy);
    changed = true;
}

void QIntParamsDlg::btn_dots_toggled(bool dummy)
{
    UNUSED(dummy);
    changed = true;
}

void QIntParamsDlg::btn_dashes_toggled(bool dummy)
{
    UNUSED(dummy);
    changed = true;
}

void QIntParamsDlg::exclusiveToggle(bool on, QRadioButton *first, ...)
{
    first->setChecked(on);
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
        MarkBad(edt);
        *presult = defvalue;
        return true;
    }

    x.sprintf("%g", (float)(*presult));
    if (x != t)
        edt->setText(x);

    return false;
}

void QIntParamsDlg::MarkBad(QLineEdit *edt)
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
    if (!changed) {
        return;
    }

    changed = false;
    changed |= readFloatField(edt_tolerance, &(VFResults.config_tolerance_),
                              DEFAULT_TOLERANCE, MIN_TOLERANCE, MAX_TOLERANCE);
    changed |= readFloatField(edt_minstep, &(VFResults.config_hmi_), DEFAULT_HMI,
                              MIN_HMI, MAX_HMI);
    changed |= readFloatField(edt_maxstep, &(VFResults.config_hma_), DEFAULT_HMA,
                              MIN_HMA, MAX_HMA);
    changed |= readFloatField(edt_stepsize, &(VFResults.config_step_),
                              DEFAULT_STEPSIZE, MIN_HMI, MAX_HMA);
    //  changed |= ReadFloatField( edt_curstep, &(VFResults.config_currentstep_),
    //                      DEFAULT_STEPSIZE, MIN_HMI, MAX_HMA );

    VFResults.config_intpoints_ = spin_numpoints->value();

    VFResults.config_kindvf_ =
        (btn_org->isChecked()) ? INTCONFIG_ORIGINAL : INTCONFIG_REDUCED;
    VFResults.config_dashes_ = (btn_dashes->isChecked()) ? true : false;

    if (!changed) {
        if (VFResults.config_step_ < VFResults.config_hmi_ ||
            VFResults.config_step_ > VFResults.config_hma_) {
            // error

            VFResults.config_step_ = DEFAULT_STEPSIZE;
            if (VFResults.config_step_ < VFResults.config_hmi_ ||
                VFResults.config_step_ > VFResults.config_hma_) {
                VFResults.config_step_ = VFResults.config_hma_;
            }
            changed = true;
            MarkBad(edt_stepsize);
        }
        /*
                if( VFResults.config_currentstep_ < VFResults.config_hmi_ ||
           VFResults.config_currentstep_ > VFResults.config_hma_ )
                {
                    // error

                    VFResults.config_currentstep_ = DEFAULT_STEPSIZE;
                    if( VFResults.config_currentstep_ < VFResults.config_hmi_ ||
           VFResults.config_currentstep_ > VFResults.config_hma_ )
                    {
                        VFResults.config_currentstep_ = VFResults.config_hma_;
                    }
                    edt_curstep->setBackgroundColor( QColor(255,192,192) );
                    changed = true;
                    MarkBad( edt_curstep );
                }
        */
    }
}

void QIntParamsDlg::updateDlgData(void)
{
    QString buf;

    changed = false;

    if (VFResults.config_kindvf_ == INTCONFIG_ORIGINAL)
        exclusiveToggle(true, btn_org, btn_red, nullptr);
    else
        exclusiveToggle(true, btn_red, btn_org, nullptr);

    if (VFResults.config_dashes_)
        exclusiveToggle(true, btn_dashes, btn_dots, nullptr);
    else
        exclusiveToggle(true, btn_dots, btn_dashes, nullptr);

    buf.sprintf("%g", (float)(VFResults.config_step_));
    edt_stepsize->setText(buf);

    buf.sprintf("%g", (float)(VFResults.config_currentstep_));
    lbl_curstep->setText(buf);

    buf.sprintf("%g", (float)(VFResults.config_hmi_));
    edt_minstep->setText(buf);

    buf.sprintf("%g", (float)(VFResults.config_hma_));
    edt_maxstep->setText(buf);

    buf.sprintf("%g", (float)(VFResults.config_tolerance_));
    edt_tolerance->setText(buf);

    spin_numpoints->setValue(VFResults.config_intpoints_);

    if (VFResults.gcf_ != nullptr ||
        (VFResults.singinf_ && VFResults.typeofstudy_ != TYPEOFSTUDY_ONE)) {
        btn_org->setEnabled(true);
        btn_red->setEnabled(true);
    } else {
        btn_org->setEnabled(false);
        btn_red->setEnabled(false);
    }
}

void QIntParamsDlg::setCurrentStep(double curstep)
{
    QString buf;
    buf.sprintf("%g", (float)curstep);
    lbl_curstep->setText(buf);
}

void QIntParamsDlg::on_btn_reset(void)
{
    VFResults.config_hma_ = DEFAULT_HMA;       // maximum step size
    VFResults.config_hmi_ = DEFAULT_HMI;       // minimum step size
    VFResults.config_step_ = DEFAULT_STEPSIZE; // step size
    VFResults.config_currentstep_ =
        DEFAULT_STEPSIZE; // current step size (during integration)
    VFResults.config_tolerance_ = DEFAULT_TOLERANCE; // tolerance
    VFResults.config_projection_ =
        DEFAULT_PROJECTION; // projection in the case of Poincare sphere
    VFResults.config_intpoints_ =
        DEFAULT_INTPOINTS; // number of points to integrate
    VFResults.config_dashes_ =
        DEFAULT_LINESTYLE; // line style (dashes or points)
    VFResults.config_kindvf_ = DEFAULT_INTCONFIG;

    updateDlgData();
}
