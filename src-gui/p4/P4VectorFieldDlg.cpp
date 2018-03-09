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

#include "P4VectorFieldDlg.hpp"

#include <QBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include <QSpinBox>

#include "P4Application.hpp"
#include "P4FindDlg.hpp"
#include "P4InputVF.hpp"
#include "P4SeparatingCurvesDlg.hpp"
#include "P4VFParams.hpp"
#include "P4VFSelectDlg.hpp"
#include "custom.hpp"
#include "main.hpp"

P4VectorFieldDlg::P4VectorFieldDlg(P4FindDlg *finddlg)
    : QWidget{finddlg}, parent_{finddlg}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    auto p4title = std::make_unique<QLabel>("Specify the vector field:", this);
    p4title->setFont(gP4app->getTitleFont());

    edt_xprime_ = std::make_unique<QLineEdit>(gThisVF->xdot_, this);
    auto xlabel = std::make_unique<QLabel>("&x' = ", this);
    xlabel->setFont(gP4app->getBoldFont());
    xlabel->setBuddy(edt_xprime_.get());

    edt_yprime_ = std::make_unique<QLineEdit>(gThisVF->ydot_, this);
    auto ylabel = std::make_unique<QLabel>("&y' = ", this);
    ylabel->setFont(gP4app->getBoldFont());
    ylabel->setBuddy(edt_yprime_.get());

    edt_gcf_ = std::make_unique<QLineEdit>(gThisVF->gcf_, this);
    auto glabel = std::make_unique<QLabel>("&Gcf: ", this);
    glabel->setFont(gP4app->getBoldFont());
    glabel->setBuddy(edt_gcf_.get());

    auto plabel = std::make_unique<QLabel>("Number of Parameters: ", this);
    plabel->setFont(gP4app->getBoldFont());

    spin_numparams_ = std::make_unique<QSpinBox>(this);
    spin_numparams_->setMinimum(0);
    spin_numparams_->setMaximum(MAXNUMPARAMS);

#ifdef TOOLTIPS
    edt_xprime_->setToolTip("Enter your differential equations here.\n"
                            "Use syntax conform to the symbolic manipulator.");
    edt_yprime_->setToolTip("Enter your differential equations here.\n"
                            "Use syntax conform to the symbolic manipulator.");
    edt_gcf_->setToolTip("Enter greatest common factor here.\n"
                         "Enter \"1\" if you are sure there is no GCF\n"
                         "Enter \"0\" if you want P4 to search for a GCF");
    spin_numparams_->setToolTip("If your vector field contains parameters,\n"
                                "specify the number of parameters here.");
#endif

    // layout
    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);
    mainLayout_->addWidget(p4title.get());

    auto formLayout = std::make_unique<QFormLayout>();
    formLayout->addRow(xlabel.get(), edt_xprime_.get());
    formLayout->addRow(ylabel.get(), edt_yprime_.get());
    formLayout->addRow(glabel.get(), edt_gcf_.get());

    auto layout3 = std::make_unique<QHBoxLayout>();
    layout3->addWidget(plabel.get());
    layout3->addWidget(spin_numparams_.get());
    layout3->addStretch(0);

    paramLayout_ = std::make_unique<QHBoxLayout>();

    mainLayout_->addLayout(formLayout.get());
    mainLayout_->addLayout(layout3.get());
    mainLayout_->addLayout(paramLayout_.get());

    setLayout(mainLayout_.get());

    if (gThisVF->numparams_ != 0) {
        if (gThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            sb_params_ = std::make_shared<QScrollBar>(Qt::Vertical, this);
            sb_params_->setRange(0, gThisVF->numparams_ - MAXNUMPARAMSSHOWN);
            sb_params_->setSingleStep(1);
            sb_params_->setPageStep(MAXNUMPARAMSSHOWN);
        }

        params_ = std::make_unique<P4VFParams>(this, sb_params_);
        paramLayout_->addWidget(params_.get());
        if (gThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            paramLayout_->addWidget(sb_params_.get());
            QObject::connect(sb_params_.get(), &QScrollBar::valueChanged,
                             params_.get(), &P4VFParams::paramsSliderChanged);
            QObject::connect(sb_params_.get(), &QScrollBar::sliderMoved,
                             params_.get(), &P4VFParams::paramsSliderChanged);
        }
        params_->show();
        if (gThisVF->numparams_ > MAXNUMPARAMSSHOWN)
            sb_params_->show();

        spin_numparams_->setValue(gThisVF->numparams_);
    }

    // connections
    QObject::connect(edt_xprime_.get(), &QLineEdit::editingFinished, this,
                     &P4VectorFieldDlg::editingFinished);
    QObject::connect(edt_yprime_.get(), &QLineEdit::editingFinished, this,
                     &P4VectorFieldDlg::editingFinished);
    QObject::connect(edt_gcf_.get(), &QLineEdit::editingFinished, this,
                     &P4VectorFieldDlg::editingFinished);
    QObject::connect(
        spin_numparams_.get(),
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &P4VectorFieldDlg::numParamsChanged);

    setP4WindowTitle(this, "Vector Field");
    updateDlgData();
}

void P4VectorFieldDlg::numParamsChanged(int val)
{
    if (val < 0 || val > MAXNUMPARAMS || val == gThisVF->numparams_)
        return;

    params_.reset();

    if (sb_params_)
        sb_params_.reset();

    if (gThisVF->changed_ == false) {
        gThisVF->changed_ = true;
        gP4app->signalChanged();
    }
    gThisVF->numparams_ = val;

    if (val == 0)
        return;

    if (val > MAXNUMPARAMSSHOWN) {
        sb_params_ = std::make_shared<QScrollBar>(Qt::Vertical, this);
        sb_params_->setRange(0, gThisVF->numparams_ - MAXNUMPARAMSSHOWN);
        sb_params_->setSingleStep(1);
        sb_params_->setPageStep(MAXNUMPARAMSSHOWN);
    }

    params_ = std::make_unique<P4VFParams>(this, sb_params_);
    paramLayout_->addWidget(params_.get());
    if (val > MAXNUMPARAMSSHOWN) {
        paramLayout_->addWidget(sb_params_.get());
        QObject::connect(sb_params_.get(), &QScrollBar::valueChanged,
                         params_.get(), &P4VFParams::paramsSliderChanged);
        QObject::connect(sb_params_.get(), &QScrollBar::sliderMoved,
                         params_.get(), &P4VFParams::paramsSliderChanged);
    }
    params_->show();
    if (val > MAXNUMPARAMSSHOWN)
        sb_params_->show();
}

P4VectorFieldDlg::~P4VectorFieldDlg() { getDataFromDlg(); }

void P4VectorFieldDlg::getDataFromDlg()
{
    bool changed{false};

    changed |= getLineEditCommonValue(edt_xprime_.get(), gThisVF->xdot_);
    changed |= getLineEditCommonValue(edt_yprime_.get(), gThisVF->ydot_);
    changed |= getLineEditCommonValue(edt_gcf_.get(), gThisVF->gcf_);

    if (changed) {
        if (!gThisVF->changed_) {
            gThisVF->changed_ = true;
            gP4app->signalChanged();
        }
    }
}

void P4VectorFieldDlg::updateDlgData()
{
    setLineEditCommonValue(edt_xprime_.get(), gThisVF->xdot_);
    setLineEditCommonValue(edt_yprime_.get(), gThisVF->ydot_);
    setLineEditCommonValue(edt_gcf_.get(), gThisVF->gcf_);
    spin_numparams_->setValue(gThisVF->numparams_);

    if (params_)
        if (!params_->updateDlgData())
            params_.reset();

    if (gThisVF->numparams_ == 0)
        if (sb_params_)
            sb_params_.reset();

    if (gThisVF->numparams_ != 0 && !params_) {
        if (gThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            sb_params_ = std::make_shared<QScrollBar>(Qt::Vertical, this);
            sb_params_->setRange(0, gThisVF->numparams_ - MAXNUMPARAMSSHOWN);
            sb_params_->setSingleStep(1);
            sb_params_->setPageStep(MAXNUMPARAMSSHOWN);
        }

        params_ = std::make_unique<P4VFParams>(this, sb_params_);
        paramLayout_->addWidget(params_.get());
        if (gThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            paramLayout_->addWidget(sb_params_.get());
            QObject::connect(sb_params_.get(), &QScrollBar::valueChanged,
                             params_.get(), &P4VFParams::paramsSliderChanged);
            QObject::connect(sb_params_.get(), &QScrollBar::sliderMoved,
                             params_.get(), &P4VFParams::paramsSliderChanged);
        }
        params_->show();
        if (gThisVF->numparams_ > MAXNUMPARAMSSHOWN)
            sb_params_->show();
    }
}

void P4VectorFieldDlg::setLineEditCommonValue(QLineEdit *le,
                                              const std::vector<QString> &val)
{
    if (gThisVF->hasCommonString(val))
        le->setText(gThisVF->commonString(val));
    else
        le->setText("#########");
}

bool P4VectorFieldDlg::getLineEditCommonValue(
    QLineEdit *le, const std::vector<QString> &newval)
{
    QString val{le->text().trimmed()};
    int i;

    if (!gThisVF->hasCommonString(newval)) {
        for (i = val.length() - 1; i >= 0; i--) {
            if (val[i] != '#')
                break;
        }
        if (i < 0)
            return false;
        if (!val.compare(gThisVF->commonString(newval)))
            return false;
    }
    if (!val.compare(gThisVF->commonString(newval)))
        return true;
}

void P4VectorFieldDlg::editingFinished()
{
    getDataFromDlg();
    auto *p = parent_->getVfSelectWindowPtr();
    if (p != nullptr) {
        auto *q = p->getWinCurvesPtr();
        if (q != nullptr)
            q->updateDlgData();
    }
}
