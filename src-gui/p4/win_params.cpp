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

#include "win_params.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "p4application.h"
#include "p4settings.h"

#include <QButtonGroup>

QParamsDlg::~QParamsDlg() { getDataFromDlg(); }

QParamsDlg::QParamsDlg(QFindDlg *finddlg)
#ifdef DOCK_PARAMSWINDOW
    : QWidget(finddlg)
#else
    : QWidget()
#endif
{
    parent_ = finddlg;
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    QLabel *p4title = new QLabel("Find Singular Points Parameters", this);
    p4title->setFont(*(g_p4app->titleFont_));

    QLabel *symlabel = new QLabel("Calculations: ", this);
    symlabel->setFont(*(g_p4app->boldFont_));
    btn_alg_ = new QRadioButton("Algebraic", this);
    btn_num_ = new QRadioButton("Numeric", this);

    QLabel *seplabel = new QLabel("Test Separatrices:  ", this);
    seplabel->setFont(*(g_p4app->boldFont_));
    btn_sepyes_ = new QRadioButton("Yes", this);
    btn_sepno_ = new QRadioButton("No", this);

    QLabel *lbl_precis = new QLabel("Accuracy:", this);
    lbl_precis->setFont(*(g_p4app->boldFont_));
    spin_precis_ = new QSpinBox(this);
    spin_precis_->setMinimum(MINPRECISION);
    spin_precis_->setMaximum(MAXPRECISION);

    QLabel *lbl_precis0 = new QLabel("Precision:", this);
    lbl_precis0->setFont(*(g_p4app->boldFont_));
    spin_precis0_ = new QSpinBox(this);
    spin_precis0_->setMinimum(MINPRECISION0);
    spin_precis0_->setMaximum(MAXPRECISION0);

    QLabel *lbl_epsilon = new QLabel("Epsilon:", this);
    lbl_epsilon->setFont(*(g_p4app->boldFont_));
    edt_epsilon_ = new QLineEdit(g_ThisVF->epsilon_, this);

    QLabel *lbl_level = new QLabel("Level of Approximation:", this);
    lbl_level->setFont(*(g_p4app->boldFont_));
    spin_level_ = new QSpinBox(this);
    spin_level_->setMinimum(MINLEVEL);
    spin_level_->setMaximum(MAXLEVEL);

    QLabel *lbl_numlevel = new QLabel("Numeric level:", this);
    lbl_numlevel->setFont(*(g_p4app->boldFont_));
    spin_numlevel_ = new QSpinBox(this);
    spin_numlevel_->setMinimum(MINNUMLEVEL);
    spin_numlevel_->setMaximum(MAXNUMLEVEL);

    QLabel *lbl_maxlevel = new QLabel("Maximum level:", this);
    lbl_maxlevel->setFont(*(g_p4app->boldFont_));
    spin_maxlevel_ = new QSpinBox(this);
    spin_maxlevel_->setMinimum(MINMAXLEVEL);
    spin_maxlevel_->setMaximum(MAXMAXLEVEL);

    QLabel *lbl_weakness = new QLabel("Maximum level of weakness:", this);
    lbl_weakness->setFont(*(g_p4app->boldFont_));
    spin_weakness_ = new QSpinBox(this);
    spin_weakness_->setMinimum(MINWEAKNESS);
    spin_weakness_->setMaximum(MAXWEAKNESS);

    lbl_p_ = new QLabel("p:", this);
    lbl_p_->setFont(*(g_p4app->boldFont_));
    spin_p_ = new QSpinBox(this);
    spin_p_->setMinimum(MINP);
    spin_p_->setMaximum(MAXP);

    lbl_q_ = new QLabel("q:", this);
    lbl_q_->setFont(*(g_p4app->boldFont_));
    spin_q_ = new QSpinBox(this);
    spin_q_->setMinimum(MINP);
    spin_q_->setMaximum(MAXP);

    lbl_x0_ = new QLabel("x0:", this);
    lbl_x0_->setFont(*(g_p4app->boldFont_));
    edt_x0_ = new QLineEdit("", this);

    lbl_y0_ = new QLabel("y0:", this);
    lbl_y0_->setFont(*(g_p4app->boldFont_));
    edt_y0_ = new QLineEdit("", this);

#ifdef TOOLTIPS
    btn_alg_->setToolTip(
        "Let the symbolic manipulator work in algebraic mode.\n"
        "The symbolic package will automatically switch over to\n"
        "numeric calculations whenever a symbolic study is impossible.");
    btn_num_->setToolTip("Let the symbolic manipulator work in numeric mode");
    btn_sepyes_->setToolTip("Enable numeric testing of the Taylor developments "
                            "for the separatrices");
    btn_sepno_->setToolTip("Disable separatrice testing");
    spin_precis_->setToolTip(
        "Number of digits required accuracy when checking for zero");
    spin_precis0_->setToolTip("Number of digits of numeric precision of all "
                              "calculations.  Put 0 for default double "
                              "precision");
    edt_epsilon_->setToolTip(
        "Default epsilon for the separatrices.\n"
        "This may be changed later when plotting individual separatrices");
    spin_level_->setToolTip("Order of Taylor approximation\nThis order is "
                            "increased as long as the separatrix test fails.");
    spin_numlevel_->setToolTip("Start working numerically when order of Taylor "
                               "approximation reaches this level");
    spin_maxlevel_->setToolTip("Maximum order of Taylor approximation.  Stop "
                               "testing separatrices at this order.");
    spin_weakness_->setToolTip(
        "Maximum order of Lyapunov constants to be calculated for weak foci");
    spin_p_->setToolTip("Poincare-Lyapunov weights");
    spin_q_->setToolTip("Poincare-Lyapunov weights");
    edt_x0_->setToolTip("Coordinates of the singular point in case of study of "
                        "ONE singularity");
    edt_y0_->setToolTip("Coordinates of the singular point in case of study of "
                        "ONE singularity");
#endif

    // layout

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    mainLayout_->addWidget(p4title);

    QHBoxLayout *symLayout = new QHBoxLayout();
    symLayout->addWidget(symlabel);
    symLayout->addWidget(btn_alg_);
    symLayout->addWidget(btn_num_);
    mainLayout_->addLayout(symLayout);

    QHBoxLayout *sepLayout = new QHBoxLayout();
    sepLayout->addWidget(seplabel);
    sepLayout->addWidget(btn_sepyes_);
    sepLayout->addWidget(btn_sepno_);
    mainLayout_->addLayout(sepLayout);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(lbl_precis);
    layout1->addWidget(spin_precis_);
    layout1->addStretch(0);
    layout1->addWidget(lbl_precis0);
    layout1->addWidget(spin_precis0_);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(lbl_epsilon);
    layout2->addWidget(edt_epsilon_);
    layout2->addStretch(0);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(lbl_level);
    layout3->addWidget(spin_level_);
    layout3->addStretch(0);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addWidget(lbl_numlevel);
    layout4->addWidget(spin_numlevel_);
    layout4->addStretch(0);

    QHBoxLayout *layout5 = new QHBoxLayout();
    layout5->addWidget(lbl_maxlevel);
    layout5->addWidget(spin_maxlevel_);
    layout5->addStretch(0);

    QHBoxLayout *layout6 = new QHBoxLayout();
    layout6->addWidget(lbl_weakness);
    layout6->addWidget(spin_weakness_);
    layout6->addStretch(0);

    QHBoxLayout *layout7 = new QHBoxLayout();
    layout7->addWidget(lbl_p_);
    layout7->addWidget(spin_p_);
    layout7->addWidget(lbl_q_);
    layout7->addWidget(spin_q_);
    layout7->addStretch(0);

    QHBoxLayout *layout8 = new QHBoxLayout();
    layout8->addWidget(lbl_x0_);
    layout8->addWidget(edt_x0_);
    layout8->addStretch(0);

    QHBoxLayout *layout9 = new QHBoxLayout();
    layout9->addWidget(lbl_y0_);
    layout9->addWidget(edt_y0_);
    layout9->addStretch(0);

    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);
    mainLayout_->addLayout(layout5);
    mainLayout_->addLayout(layout6);
    mainLayout_->addLayout(layout7);
    mainLayout_->addLayout(layout8);
    mainLayout_->addLayout(layout9);

    setLayout(mainLayout_);

    QButtonGroup *btngrp1 = new QButtonGroup(this);
    btngrp1->addButton(btn_alg_);
    btngrp1->addButton(btn_num_);

    QButtonGroup *btngrp2 = new QButtonGroup(this);
    btngrp2->addButton(btn_sepyes_);
    btngrp2->addButton(btn_sepno_);

    // connections

    QObject::connect(btn_alg_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_alg_toggled(bool)));
    QObject::connect(btn_num_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_num_toggled(bool)));
    QObject::connect(spin_level_, SIGNAL(valueChanged(int)), this,
                     SLOT(onLevelChange(int)));
    QObject::connect(btn_sepno_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_sepno_toggled(bool)));
    QObject::connect(btn_sepyes_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_sepyes_toggled(bool)));

    // finishing

    updateDlgData();
#ifndef DOCK_PARAMSWINDOW
    setP4WindowTitle(this, "Parameters");
#endif
}

void QParamsDlg::onLevelChange(int value)
{
    spin_maxlevel_->setMinimum(value);
    spin_numlevel_->setMinimum(value);

    if (spin_maxlevel_->value() < value)
        spin_maxlevel_->setValue(value);

    if (spin_numlevel_->value() < value)
        spin_numlevel_->setValue(value);
}

void QParamsDlg::btn_alg_toggled(bool on)
{
    if (on) {
        if (g_ThisVF->numeric_ != false) {
            g_ThisVF->numeric_ = false;
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
        }
    }
}

void QParamsDlg::btn_num_toggled(bool on)
{
    if (on) {
        if (g_ThisVF->numeric_ != true) {
            g_ThisVF->numeric_ = true;
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
        }
    }
}

void QParamsDlg::btn_sepyes_toggled(bool on)
{
    if (on) {
        if (g_ThisVF->testsep_ != true) {
            g_ThisVF->testsep_ = true;
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
        }
    }
}

void QParamsDlg::btn_sepno_toggled(bool on)
{
    if (on) {
        if (g_ThisVF->testsep_ != false) {
            g_ThisVF->testsep_ = false;
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
        }
    }
}

void QParamsDlg::getDataFromDlg(void)
{
    QString epsilon;
    QString x0;
    QString y0;

    if (spin_level_->value() != g_ThisVF->taylorlevel_ ||
        spin_numlevel_->value() != g_ThisVF->numericlevel_ ||
        spin_maxlevel_->value() != g_ThisVF->maxlevel_ ||
        spin_weakness_->value() != g_ThisVF->weakness_ ||
        spin_p_->value() != g_ThisVF->p_ || spin_q_->value() != g_ThisVF->q_ ||
        spin_precis_->value() != g_ThisVF->precision_ ||
        spin_precis0_->value() != g_ThisVF->precision0_) {
        g_ThisVF->taylorlevel_ = spin_level_->value();
        g_ThisVF->numericlevel_ = spin_numlevel_->value();
        g_ThisVF->maxlevel_ = spin_maxlevel_->value();
        g_ThisVF->weakness_ = spin_weakness_->value();
        g_ThisVF->p_ = spin_p_->value();
        g_ThisVF->q_ = spin_q_->value();
        g_ThisVF->precision_ = spin_precis_->value();
        g_ThisVF->precision0_ = spin_precis0_->value();

        if (g_ThisVF->changed_ == false) {
            g_ThisVF->changed_ = true;
            g_p4app->signalChanged();
        }
    }

    epsilon = edt_epsilon_->text();
    x0 = edt_x0_->text();
    y0 = edt_y0_->text();

    epsilon = epsilon.trimmed();
    x0 = x0.trimmed();
    y0 = y0.trimmed();

    if (epsilon.compare(g_ThisVF->epsilon_) != 0 ||
        x0.compare(g_ThisVF->x0_) != 0 || y0.compare(g_ThisVF->y0_) != 0) {
        g_ThisVF->epsilon_ = epsilon;
        g_ThisVF->x0_ = x0;
        g_ThisVF->y0_ = y0;

        if (g_ThisVF->changed_ == false) {
            g_ThisVF->changed_ = true;
            g_p4app->signalChanged();
        }
    }
}

void QParamsDlg::updateDlgData(void)
{
    if (g_ThisVF->numeric_)
        btn_num_->toggle();
    else
        btn_alg_->toggle();

    if (g_ThisVF->testsep_)
        btn_sepyes_->toggle();
    else
        btn_sepno_->toggle();

    spin_level_->setValue(g_ThisVF->taylorlevel_);
    spin_numlevel_->setValue(g_ThisVF->numericlevel_);
    spin_maxlevel_->setValue(g_ThisVF->maxlevel_);
    spin_weakness_->setValue(g_ThisVF->weakness_);
    edt_epsilon_->setText(g_ThisVF->epsilon_);
    spin_precis_->setValue(g_ThisVF->precision_);
    spin_precis0_->setValue(g_ThisVF->precision0_);

    if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
        edt_x0_->setText(g_ThisVF->x0_);
        edt_y0_->setText(g_ThisVF->y0_);
        spin_p_->setValue(g_ThisVF->p_);
        spin_q_->setValue(g_ThisVF->q_);

        spin_p_->setEnabled(false);
        spin_q_->setEnabled(false);
        edt_x0_->setEnabled(true);
        edt_y0_->setEnabled(true);

        lbl_x0_->setFont(*(g_p4app->boldFont_));
        lbl_y0_->setFont(*(g_p4app->boldFont_));
        lbl_p_->setFont(*(g_p4app->standardFont_));
        lbl_q_->setFont(*(g_p4app->standardFont_));
    } else {
        spin_p_->setValue(g_ThisVF->p_);
        spin_q_->setValue(g_ThisVF->q_);
        edt_x0_->setText(g_ThisVF->x0_);
        edt_y0_->setText(g_ThisVF->x0_);

        spin_p_->setEnabled(true);
        spin_q_->setEnabled(true);
        edt_x0_->setEnabled(false);
        edt_y0_->setEnabled(false);

        lbl_p_->setFont(*(g_p4app->boldFont_));
        lbl_q_->setFont(*(g_p4app->boldFont_));
        lbl_x0_->setFont(*(g_p4app->standardFont_));
        lbl_y0_->setFont(*(g_p4app->standardFont_));
    }
}
