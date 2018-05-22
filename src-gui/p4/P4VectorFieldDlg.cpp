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

P4VectorFieldDlg::P4VectorFieldDlg(P4FindDlg *parent)
    : QWidget{parent}, parent_{parent}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    auto p4title = new QLabel{"Specify the vector field:", this};
    p4title->setFont(gP4app->getTitleFont());

    edt_xprime_ = new QLineEdit{"", this};
    auto xlabel = new QLabel{"&x' = ", this};
    xlabel->setFont(gP4app->getBoldFont());
    xlabel->setBuddy(edt_xprime_);

    edt_yprime_ = new QLineEdit{"", this};
    auto ylabel = new QLabel{"&y' = ", this};
    ylabel->setFont(gP4app->getBoldFont());
    ylabel->setBuddy(edt_yprime_);

    edt_gcf_ = new QLineEdit{"", this};
    auto glabel = new QLabel{"&Gcf: ", this};
    glabel->setFont(gP4app->getBoldFont());
    glabel->setBuddy(edt_gcf_);

    auto plabel = new QLabel{"Number of Parameters: ", this};
    plabel->setFont(gP4app->getBoldFont());

    spin_numparams_ = new QSpinBox{this};
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
    mainLayout_ = new QBoxLayout{QBoxLayout::TopToBottom, this};
    mainLayout_->addWidget(p4title);

    auto formLayout_ = new QFormLayout{};
    formLayout_->addRow(xlabel, edt_xprime_);
    formLayout_->addRow(ylabel, edt_yprime_);
    formLayout_->addRow(glabel, edt_gcf_);

    auto layout3_ = new QHBoxLayout{};
    layout3_->addWidget(plabel);
    layout3_->addWidget(spin_numparams_);
    layout3_->addStretch(0);

    paramLayout_ = new QHBoxLayout{};

    mainLayout_->addLayout(formLayout_);
    mainLayout_->addLayout(layout3_);
    mainLayout_->addLayout(paramLayout_);

    setLayout(mainLayout_);

    if (gThisVF->numParams_ != 0) {
        if (gThisVF->numParams_ > MAXNUMPARAMSSHOWN) {
            sb_params_ = new QScrollBar{Qt::Vertical, this};
            sb_params_->setRange(0, gThisVF->numParams_ - MAXNUMPARAMSSHOWN);
            sb_params_->setSingleStep(1);
            sb_params_->setPageStep(MAXNUMPARAMSSHOWN);
        }

        params_ = new P4VFParams{sb_params_, this};
        paramLayout_->addWidget(params_);
        if (gThisVF->numParams_ > MAXNUMPARAMSSHOWN) {
            paramLayout_->addWidget(sb_params_);
            QObject::connect(sb_params_, &QScrollBar::valueChanged, params_,
                             &P4VFParams::paramsSliderChanged);
            QObject::connect(sb_params_, &QScrollBar::sliderMoved, params_,
                             &P4VFParams::paramsSliderChanged);
        }
        params_->show();
        if (gThisVF->numParams_ > MAXNUMPARAMSSHOWN)
            sb_params_->show();

        spin_numparams_->setValue(gThisVF->numParams_);
    }

    // connections
    QObject::connect(edt_xprime_, &QLineEdit::editingFinished, this,
                     &P4VectorFieldDlg::editingFinished);
    QObject::connect(edt_yprime_, &QLineEdit::editingFinished, this,
                     &P4VectorFieldDlg::editingFinished);
    QObject::connect(edt_gcf_, &QLineEdit::editingFinished, this,
                     &P4VectorFieldDlg::editingFinished);
    QObject::connect(
        spin_numparams_,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &P4VectorFieldDlg::numParamsChanged);

    setP4WindowTitle(this, "Vector Field");
    updateDlgData();
}

void P4VectorFieldDlg::numParamsChanged(int val)
{
    if (val < 0 || val > MAXNUMPARAMS || val == gThisVF->numParams_)
        return;

    delete params_;
    params_ = nullptr;

    if (sb_params_ != nullptr) {
        delete sb_params_;
        sb_params_ = nullptr;
    }

    if (gThisVF->changed_ == false) {
        gThisVF->changed_ = true;
        gP4app->signalChanged();
    }
    gThisVF->numParams_ = val;

    if (val == 0)
        return;

    if (val > MAXNUMPARAMSSHOWN) {
        sb_params_ = new QScrollBar{Qt::Vertical, this};
        sb_params_->setRange(0, gThisVF->numParams_ - MAXNUMPARAMSSHOWN);
        sb_params_->setSingleStep(1);
        sb_params_->setPageStep(MAXNUMPARAMSSHOWN);
    }

    params_ = new P4VFParams{sb_params_, this};
    paramLayout_->addWidget(params_);
    if (val > MAXNUMPARAMSSHOWN) {
        paramLayout_->addWidget(sb_params_);
        QObject::connect(sb_params_, &QScrollBar::valueChanged, params_,
                         &P4VFParams::paramsSliderChanged);
        QObject::connect(sb_params_, &QScrollBar::sliderMoved, params_,
                         &P4VFParams::paramsSliderChanged);
    }
    params_->show();
    if (val > MAXNUMPARAMSSHOWN)
        sb_params_->show();
}

P4VectorFieldDlg::~P4VectorFieldDlg() { getDataFromDlg(); }

void P4VectorFieldDlg::getDataFromDlg()
{
    bool changed{false};

    changed |= getLineEditCommonValue(edt_xprime_, gThisVF->xdot_);
    changed |= getLineEditCommonValue(edt_yprime_, gThisVF->ydot_);
    changed |= getLineEditCommonValue(edt_gcf_, gThisVF->gcf_);

    if (changed) {
        if (!gThisVF->changed_) {
            gThisVF->changed_ = true;
            gP4app->signalChanged();
        }
    }
}

void P4VectorFieldDlg::updateDlgData()
{
    setLineEditCommonValue(edt_xprime_, gThisVF->xdot_);
    setLineEditCommonValue(edt_yprime_, gThisVF->ydot_);
    setLineEditCommonValue(edt_gcf_, gThisVF->gcf_);
    spin_numparams_->setValue(gThisVF->numParams_);

    if (params_ != nullptr) {
        if (!params_->updateDlgData()) {
            delete params_;
            params_ = nullptr;
        }
    }

    if (gThisVF->numParams_ == 0) {
        if (sb_params_ != nullptr) {
            delete sb_params_;
            sb_params_ = nullptr;
        }
    }

    if (gThisVF->numParams_ != 0 && !params_) {
        if (gThisVF->numParams_ > MAXNUMPARAMSSHOWN) {
            sb_params_ = new QScrollBar{Qt::Vertical, this};
            sb_params_->setRange(0, gThisVF->numParams_ - MAXNUMPARAMSSHOWN);
            sb_params_->setSingleStep(1);
            sb_params_->setPageStep(MAXNUMPARAMSSHOWN);
        }

        params_ = new P4VFParams{sb_params_, this};
        paramLayout_->addWidget(params_);
        if (gThisVF->numParams_ > MAXNUMPARAMSSHOWN) {
            paramLayout_->addWidget(sb_params_);
            QObject::connect(sb_params_, &QScrollBar::valueChanged, params_,
                             &P4VFParams::paramsSliderChanged);
            QObject::connect(sb_params_, &QScrollBar::sliderMoved, params_,
                             &P4VFParams::paramsSliderChanged);
        }
        params_->show();
        if (gThisVF->numParams_ > MAXNUMPARAMSSHOWN)
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

bool P4VectorFieldDlg::getLineEditCommonValue(QLineEdit *le,
                                              std::vector<QString> &newval)
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
        return false;

    gThisVF->setCommonString(newval, val);
    return true;
}

void P4VectorFieldDlg::editingFinished()
{
    getDataFromDlg();
    if (parent_->getPiecewiseConfigWindowPtr() != nullptr) {
        parent_->getPiecewiseConfigWindowPtr()->updateDlgData();
    }
}
