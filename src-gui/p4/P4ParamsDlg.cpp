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

#include "P4ParamsDlg.hpp"

#include "P4Application.hpp"
#include "P4FindDlg.hpp"
#include "P4InputVF.hpp"
#include "custom.hpp"
#include "main.hpp"
#include "p4settings.hpp"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>

P4ParamsDlg::~P4ParamsDlg() { getDataFromDlg(); }

P4ParamsDlg::P4ParamsDlg(P4FindDlg *finddlg)
    : QWidget{finddlg}, parent_{finddlg}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    auto p4title =
        std::make_unique<QLabel>("Find Singular Points Parameters", this);
    p4title->setFont(*(gP4app->titleFont_));

    auto symlabel = std::make_unique<QLabel>("Calculations: ", this);
    symlabel->setFont(*(gP4app->boldFont_));
    btn_alg_ = std::make_unique<QRadioButton>("Algebraic", this);
    btn_num_ = std::make_unique<QRadioButton>("Numeric", this);

    auto seplabel = std::make_unique<QLabel>("Test Separatrices:  ", this);
    seplabel->setFont(*(gP4app->boldFont_));
    btn_sepyes_ = std::make_unique<QRadioButton>("Yes", this);
    btn_sepno_ = std::make_unique<QRadioButton>("No", this);

    auto lbl_precis = std::make_unique<QLabel>("Accuracy:", this);
    lbl_precis->setFont(*(gP4app->boldFont_));
    spin_precis_ = std::make_unique<QSpinBox>(this);
    spin_precis_->setMinimum(MINPRECISION);
    spin_precis_->setMaximum(MAXPRECISION);

    auto lbl_precis0 = std::make_unique<QLabel>("Precision:", this);
    lbl_precis0->setFont(*(gP4app->boldFont_));
    spin_precis0_ = std::make_unique<QSpinBox>(this);
    spin_precis0_->setMinimum(MINPRECISION0);
    spin_precis0_->setMaximum(MAXPRECISION0);

    auto lbl_epsilon = std::make_unique<QLabel>("Epsilon:", this);
    lbl_epsilon->setFont(*(gP4app->boldFont_));
    edt_epsilon_ = std::make_unique<QLineEdit>(gThisVF->epsilon_, this);

    auto lbl_level = std::make_unique<QLabel>("Level of Approximation:", this);
    lbl_level->setFont(*(gP4app->boldFont_));
    spin_level_ = std::make_unique<QSpinBox>(this);
    spin_level_->setMinimum(MINLEVEL);
    spin_level_->setMaximum(MAXLEVEL);

    auto lbl_numlevel = std::make_unique<QLabel>("Numeric level:", this);
    lbl_numlevel->setFont(*(gP4app->boldFont_));
    spin_numlevel_ = std::make_unique<QSpinBox>(this);
    spin_numlevel_->setMinimum(MINNUMLEVEL);
    spin_numlevel_->setMaximum(MAXNUMLEVEL);

    auto lbl_maxlevel = std::make_unique<QLabel>("Maximum level:", this);
    lbl_maxlevel->setFont(*(gP4app->boldFont_));
    spin_maxlevel_ = std::make_unique<QSpinBox>(this);
    spin_maxlevel_->setMinimum(MINMAXLEVEL);
    spin_maxlevel_->setMaximum(MAXMAXLEVEL);

    auto lbl_weakness =
        std::make_unique<QLabel>("Maximum level of weakness:", this);
    lbl_weakness->setFont(*(gP4app->boldFont_));
    spin_weakness_ = std::make_unique<QSpinBox>(this);
    spin_weakness_->setMinimum(MINWEAKNESS);
    spin_weakness_->setMaximum(MAXWEAKNESS);

    lbl_p_ = std::make_unique<QLabel>("p:", this);
    lbl_p_->setFont(*(gP4app->boldFont_));
    spin_p_ = std::make_unique<QSpinBox>(this);
    spin_p_->setMinimum(MINP);
    spin_p_->setMaximum(MAXP);

    lbl_q_ = std::make_unique<QLabel>("q:", this);
    lbl_q_->setFont(*(gP4app->boldFont_));
    spin_q_ = std::make_unique<QSpinBox>(this);
    spin_q_->setMinimum(MINP);
    spin_q_->setMaximum(MAXP);

    lbl_x0_ = std::make_unique<QLabel>("x0:", this);
    lbl_x0_->setFont(*(gP4app->boldFont_));
    edt_x0_ = std::make_unique<QLineEdit>("", this);

    lbl_y0_ = std::make_unique<QLabel>("y0:", this);
    lbl_y0_->setFont(*(gP4app->boldFont_));
    edt_y0_ = std::make_unique<QLineEdit>("", this);

#ifdef TOOLTIPS
    btn_alg_->setToolTip(
        "Let the symbolic manipulator work in algebraic mode.\n"
        "The symbolic package will automatically switch over to\n"
        "numeric calculations whenever a symbolic study is impossible.");
    btn_num_->setToolTip("Let the symbolic manipulator work in numeric mode");
    btn_sepyes_->setToolTip(
        "Enable numeric testing of the Taylor developments "
        "for the separatrices");
    btn_sepno_->setToolTip("Disable separatrice testing");
    spin_precis_->setToolTip(
        "Number of digits required accuracy when checking for zero");
    spin_precis0_->setToolTip(
        "Number of digits of numeric precision of all "
        "calculations.  Put 0 for default double "
        "precision");
    edt_epsilon_->setToolTip(
        "Default epsilon for the separatrices.\n"
        "This may be changed later when plotting individual separatrices");
    spin_level_->setToolTip(
        "Order of Taylor approximation\nThis order is "
        "increased as long as the separatrix test fails.");
    spin_numlevel_->setToolTip(
        "Start working numerically when order of Taylor "
        "approximation reaches this level");
    spin_maxlevel_->setToolTip(
        "Maximum order of Taylor approximation.  Stop "
        "testing separatrices at this order.");
    spin_weakness_->setToolTip(
        "Maximum order of Lyapunov constants to be calculated for weak foci");
    spin_p_->setToolTip("Poincare-Lyapunov weights");
    spin_q_->setToolTip("Poincare-Lyapunov weights");
    edt_x0_->setToolTip(
        "Coordinates of the singular point in case of study of "
        "ONE singularity");
    edt_y0_->setToolTip(
        "Coordinates of the singular point in case of study of "
        "ONE singularity");
#endif

    // layout

    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    mainLayout_->addWidget(p4title.get());

    auto symLayout = std::make_unique<QHBoxLayout>();
    symLayout->addWidget(symlabel.get());
    symLayout->addWidget(btn_alg_.get());
    symLayout->addWidget(btn_num_.get());
    mainLayout_->addLayout(symLayout.get());

    auto sepLayout = std::make_unique<QHBoxLayout>();
    sepLayout->addWidget(seplabel.get());
    sepLayout->addWidget(btn_sepyes_.get());
    sepLayout->addWidget(btn_sepno_.get());
    mainLayout_->addLayout(sepLayout.get());

    auto layout1 = std::make_unique<QHBoxLayout>();
    layout1->addWidget(lbl_precis.get());
    layout1->addWidget(spin_precis_.get());
    layout1->addStretch(0);
    layout1->addWidget(lbl_precis0.get());
    layout1->addWidget(spin_precis0_.get());

    auto layout2 = std::make_unique<QHBoxLayout>();
    layout2->addWidget(lbl_epsilon.get());
    layout2->addWidget(edt_epsilon_.get());
    layout2->addStretch(0);

    auto layout3 = std::make_unique<QHBoxLayout>();
    layout3->addWidget(lbl_level.get());
    layout3->addWidget(spin_level_.get());
    layout3->addStretch(0);

    auto layout4 = std::make_unique<QHBoxLayout>();
    layout4->addWidget(lbl_numlevel.get());
    layout4->addWidget(spin_numlevel_.get());
    layout4->addStretch(0);

    auto layout5 = std::make_unique<QHBoxLayout>();
    layout5->addWidget(lbl_maxlevel.get());
    layout5->addWidget(spin_maxlevel_.get());
    layout5->addStretch(0);

    auto layout6 = std::make_unique<QHBoxLayout>();
    layout6->addWidget(lbl_weakness.get());
    layout6->addWidget(spin_weakness_.get());
    layout6->addStretch(0);

    auto layout7 = std::make_unique<QHBoxLayout>();
    layout7->addWidget(lbl_p_.get());
    layout7->addWidget(spin_p_.get());
    layout7->addWidget(lbl_q_.get());
    layout7->addWidget(spin_q_.get());
    layout7->addStretch(0);

    auto layout8 = std::make_unique<QHBoxLayout>();
    layout8->addWidget(lbl_x0_.get());
    layout8->addWidget(edt_x0_.get());
    layout8->addStretch(0);

    auto layout9 = std::make_unique<QHBoxLayout>();
    layout9->addWidget(lbl_y0_.get());
    layout9->addWidget(edt_y0_.get());
    layout9->addStretch(0);

    mainLayout_->addLayout(layout1.get());
    mainLayout_->addLayout(layout2.get());
    mainLayout_->addLayout(layout3.get());
    mainLayout_->addLayout(layout4.get());
    mainLayout_->addLayout(layout5.get());
    mainLayout_->addLayout(layout6.get());
    mainLayout_->addLayout(layout7.get());
    mainLayout_->addLayout(layout8.get());
    mainLayout_->addLayout(layout9.get());

    setLayout(mainLayout_.get());

    btngrp_numeric_ = std::make_unique<QButtonGroup>(this);
    btngrp_numeric_->addButton(btn_alg_.get());
    btngrp_numeric_->addButton(btn_num_.get());

    btngrp_testsep_ = std::make_unique<QButtonGroup>(this);
    btngrp_testsep_->addButton(btn_sepyes_.get());
    btngrp_testsep_->addButton(btn_sepno_.get());

    // connections.
    QObject::connect(btn_alg_.get(), &QRadioButton::toggled, this,
                     &P4ParamsDlg::btn_alg_toggled);
    QObject::connect(btn_num_.get(), &QRadioButton::toggled, this,
                     &P4ParamsDlg::btn_num_toggled);
    QObject::connect(.get() spin_level_,
                     static_cast<void (QSpinBox::*)(int)>(
                         &QSpinBox::valueChanged),
                     this, &P4ParamsDlg::onLevelChange);
    QObject::connect(btn_sepno_.get(), &QRadioButton::toggled, this,
                     &P4ParamsDlg::btn_sepno_toggled);
    QObject::connect(btn_sepyes_.get(), &QRadioButton::toggled, this,
                     &P4ParamsDlg::btn_sepyes_toggled);

    // finishing
    updateDlgData();
}

void P4ParamsDlg::onLevelChange(int value)
{
    /*
    spin_maxlevel_->setMinimum(value);
    spin_numlevel_->setMinimum(value);

    if (spin_maxlevel_->value() < value)
        spin_maxlevel_->setValue(value);

    if (spin_numlevel_->value() < value)
        spin_numlevel_->setValue(value);
    */
}

void P4ParamsDlg::btn_alg_toggled(bool on)
{
    if (on) {
        if (!gThisVF->hasCommonBool(gThisVF->numeric_) ||
            gThisVF->commonBool(gThisVF->numeric_) != false) {
            gThisVF->setCommonBool(gThisVF->numeric_, false);
            if (gThisVF->changed_ == false) {
                gThisVF->changed_ = true;
                gP4app->signalChanged();
            }
        }
    }
}

void P4ParamsDlg::btn_num_toggled(bool on)
{
    if (on) {
        if (!gThisVF->hasCommonBool(gThisVF->numeric_) ||
            gThisVF->commonBool(gThisVF->numeric_) != true) {
            gThisVF->setCommonBool(gThisVF->numeric_, true);
            if (gThisVF->changed_ == false) {
                gThisVF->changed_ = true;
                gP4app->signalChanged();
            }
        }
    }
}

void P4ParamsDlg::btn_sepyes_toggled(bool on)
{
    if (on) {
        if (!gThisVF->hasCommonBool(gThisVF->testsep_) ||
            gThisVF->commonBool(gThisVF->testsep_) != true) {
            gThisVF->setCommonBool(gThisVF->testsep_, true);
            if (gThisVF->changed_ == false) {
                gThisVF->changed_ = true;
                gP4app->signalChanged();
            }
        }
    }
}

void P4ParamsDlg::btn_sepno_toggled(bool on)
{
    if (on) {
        if (!gThisVF->hasCommonBool(gThisVF->testsep_) ||
            gThisVF->commonBool(gThisVF->testsep_) != false) {
            gThisVF->setCommonBool(gThisVF->testsep_, false);
            if (gThisVF->changed_ == false) {
                gThisVF->changed_ = true;
                gP4app->signalChanged();
            }
        }
    }
}

void P4ParamsDlg::setSpinBoxCommonValue(QSpinBox &sb,
                                        const std::vector<int> &val, int minval)
{
    if (gThisVF->hasCommonInt(val)) {
        sb.setSpecialValueText("");
        sb.setMinimum(minval);
        sb.setValue(gThisVF->commonInt(val));
    } else {
        sb.setMinimal(minval - 1);
        sb.setSpecialValueText("##");
        sb.setValue(minval - 1);
    }
}

bool P4ParamsDlg::getSpinBoxCommonValue(const QSpinBox &sb,
                                        std::vector<int> &newval)
{
    int val{sb.value()};
    if (val == sb.minimum()) {
        if (!sb.specialValueText().isEmpty()) {
            // special value: ## -> ignore change
            return false;
        }
    }

    if (gThisVF->hasCommonInt(newval)) {
        if (val == gThisVF->commonInt(newval))
            return false;
    }

    gThisVF->setCommonInt(newval, val);
    return true;
}

void P4ParamsDlg::setLineEditCommonValue(QLineEdit &le,
                                         const std::vector<QString> &val)
{
    if (gThisVF->hasCommonString(val))
        le.setText(gThisVF->commonString(val));
    else
        le.setText("##");
}

bool P4ParamsDlg::getLineEditCommonValue(const QLineEdit &le,
                                         std::vector<QString> &newval)
{
    QString val{le.text().trimmed()};

    if (!gThisVF->hasCommonString(newval)) {
        for (int i = val.length() - 1; i >= 0; i--) {
            if (val[i] != '#')
                break;
            if (i == 0)
                return false;
        }
        if (!val.compare(gThisVF->commonString(newval)))
            return false;
    }

    if (!val.compare(gThisVF->commonString(newval)))
        return false;

    gThisVF->setCommonString(newval, val);
    return true;
}

void P4ParamsDlg::getDataFromDlg()
{
    bool changed{false};

    if (spin_p_->value() != gThisVF->p_ || spin_q_->value() != gThisVF->q_) {
        gThisVF->p_ = spin_p_->value();
        gThisVF->q_ = spin_q_->value();
        changed = true;
    }

    QString x0 {edt_x0_->text().trimmed()};
    QString y0 {edt_y0_->text().trimmed()};

    if (x0.compare(gThisVF->x0_) != 0 || y0.compare(gThisVF->y0_) != 0) {
        gThisVF->x0_ = x0;
        gThisVF->y0_ = y0;
        changed = true;
    }

    changed |= getSpinBoxCommonValue(*spin_level_, gThisVF->taylorlevel_);
    changed |= getSpinBoxCommonValue(*spin_numlevel_, gThisVF->numericlevel_);
    changed |= getSpinBoxCommonValue(*spin_maxlevel_, gThisVF->maxlevel_);
    changed |= getSpinBoxCommonValue(*spin_weakness_, gThisVF->weakness_);
    changed |= getSpinBoxCommonValue(*spin_precis_, gThisVF->precision_);
    changed |= getSpinBoxCommonValue(*spin_precis0_, gThisVF->precision0_);
    changed |= getLineEditCommonValue(*edt_epsilon_, gThisVF->epsilon_);

    if (changed) {
        if (gThisVF->changed == false) {
            gThisVF->changed = true;
            gP4app->signalChanged();
        }
    }
}

void P4ParamsDlg::updateDlgData()
{
    if (gThisVF->hasCommonBool(gThisVF->numeric_)) {
        if (gThisVF->commonBool(gThisVF->numeric_))
            btn_num_->toggle();
        else
            btn_alg_->toggle();
    } else {
        btngrp_numeric_->setExclusive(false);
        btn_num_->setChecked(false);
        btn_alg_->setChecked(false);
        btngrp_numeric_->setExclusive(true);
    }

    if (gThisVF->hasCommonBool(gThisVF->testsep_)) {
        if (gThisVF->commonBool(gThisVF->testsep_))
            btn_sepyes_->toggle();
        else
            btn_sepno_->toggle();
    } else {
        btngrp_testsep_->setExclusive(false);
        btn_sepyes_->setChecked(false);
        btn_sepno_->setChecked(false);
        btngrp_testsep_->setExclusive(true);
    }

    setSpinBoxCommonValue(*spin_level_, gThisVF->taylorlevel_, MINLEVEL);
    setSpinBoxCommonValue(*spin_numlevel_, gThisVF->numericlevel_, MINNUMLEVEL);
    setSpinBoxCommonValue(*spin_maxlevel_, gThisVF->maxlevel_, MINMAXLEVEL);
    setSpinBoxCommonValue(*spin_weakness_, gThisVF->weakness_, MINWEAKNESS);
    setLineEditCommonValue(*edt_epsilon_, gThisVF->epsilon_);
    setSpinBoxCommonValue(*spin_precis_, gThisVF->precision_, MINPRECISION);
    setSpinBoxCommonValue(*spin_precis0_, gThisVF->precision0_, MINPRECISION0);

    if (gThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
        edt_x0_->setText(gThisVF->x0_);
        edt_y0_->setText(gThisVF->y0_);
        spin_p_->setValue(gThisVF->p_);
        spin_q_->setValue(gThisVF->q_);

        spin_p_->setEnabled(false);
        spin_q_->setEnabled(false);
        edt_x0_->setEnabled(true);
        edt_y0_->setEnabled(true);

        lbl_x0_->setFont(*(gP4app->boldFont_));
        lbl_y0_->setFont(*(gP4app->boldFont_));
        lbl_p_->setFont(*(gP4app->standardFont_));
        lbl_q_->setFont(*(gP4app->standardFont_));
    } else {
        spin_p_->setValue(gThisVF->p_);
        spin_q_->setValue(gThisVF->q_);
        edt_x0_->setText(gThisVF->x0_);
        edt_y0_->setText(gThisVF->y0_);

        spin_p_->setEnabled(true);
        spin_q_->setEnabled(true);
        edt_x0_->setEnabled(false);
        edt_y0_->setEnabled(false);

        lbl_p_->setFont(*(gP4app->boldFont_));
        lbl_q_->setFont(*(gP4app->boldFont_));
        lbl_x0_->setFont(*(gP4app->standardFont_));
        lbl_y0_->setFont(*(gP4app->standardFont_));
    }
}
