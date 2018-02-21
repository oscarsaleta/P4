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

#include "P4VectorFieldDlg.h"

#include "P4FindDlg.h"
#include "P4InputVF.h"
#include "P4VFParams.h"
#include "custom.h"
#include "main.h"
#include "p4application.h"

#include <QBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include <QSpinBox>

P4VectorFieldDlg::P4VectorFieldDlg(std::shared_ptr<P4FindDlg> finddlg)
    : QWidget{finddlg.get()}, parent_{finddlg}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    std::unique_ptr<QLabel> p4title{
        std::make_unique<QLabel>("Specify the vector field:", this)};
    p4title->setFont(*(g_p4app->titleFont_));

    edt_xprime_ = std::make_unique<QLineEdit>(g_ThisVF->xdot_, this);
    std::unique_ptr<QLabel> xlabel{std::make_unique<QLabel>("&x' = ", this)};
    xlabel->setFont(*(g_p4app->boldFont_));
    xlabel->setBuddy(edt_xprime_);

    edt_yprime_ = std::make_unique<QLineEdit>(g_ThisVF->ydot_, this);
    std::unique_ptr<QLabel> ylabel{std::make_unique<QLabel>("&y' = ", this)};
    ylabel->setFont(*(g_p4app->boldFont_));
    ylabel->setBuddy(edt_yprime_);

    edt_gcf_ = std::make_unique<QLineEdit>(g_ThisVF->gcf_, this);
    std::unique_ptr<QLabel> glabel{std::make_unique<QLabel>("&Gcf: ", this)};
    glabel->setFont(*(g_p4app->boldFont_));
    glabel->setBuddy(edt_gcf_);

    std::unique_ptr<QLabel> plabel{
        std::make_unique<QLabel>("Number of Parameters: ", this)};
    plabel->setFont(*(g_p4app->boldFont_));

    spin_numparams_ = std::make_unique<QSpinBox>(this);
    spin_numparams_->setMinimum(0);
    spin_numparams_->setMaximum(MAXNUMPARAMS);

#ifdef TOOLTIPS
    edt_xprime_->setToolTip(
        "Enter your differential equations here.\n"
        "Use syntax conform to the symbolic manipulator.");
    edt_yprime_->setToolTip(
        "Enter your differential equations here.\n"
        "Use syntax conform to the symbolic manipulator.");
    edt_gcf_->setToolTip(
        "Enter greatest common factor here.\n"
        "Enter \"1\" if you are sure there is no GCF\n"
        "Enter \"0\" if you want P4 to search for a GCF");
    spin_numparams_->setToolTip(
        "If your vector field contains parameters,\n"
        "specify the number of parameters here.");
#endif
    // layout

    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    mainLayout_->addWidget(p4title);

    std::unique_ptr<QFormLayout> formLayout{std::make_unique<QFormLayout>()};
    formLayout->addRow(xlabel, edt_xprime_);
    formLayout->addRow(ylabel, edt_yprime_);
    formLayout->addRow(glabel, edt_gcf_);

    std::unique_ptr<QHBoxLayout> layout3{std::make_unique<QHBoxLayout>()};
    layout3->addWidget(plabel);
    layout3->addWidget(spin_numparams_);
    layout3->addStretch(0);

    paramLayout_ = std::make_unique<QHBoxLayout>();

    mainLayout_->addLayout(formLayout);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(paramLayout_);

    setLayout(mainLayout_);

    if (g_ThisVF->numparams_ != 0) {
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            sb_params_ = std::make_shared<QScrollBar>(Qt::Vertical, this);
            sb_params_->setRange(0, g_ThisVF->numparams_ - MAXNUMPARAMSSHOWN);
            sb_params_->setSingleStep(1);
            sb_params_->setPageStep(MAXNUMPARAMSSHOWN);
        }

        params_ = std::make_unique<P4VFParams>(this, sb_params_);
        paramLayout_->addWidget(params_);
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            paramLayout_->addWidget(sb_params_);
            connect(sb_params_, &QScrollBar::valueChanged, params_,
                    &P4VFParams::paramsSliderChanged);
            connect(sb_params_, &QScrollBar::sliderMoved, params_,
                    &P4VFParams::paramsSliderChanged);
        }
        params_->show();
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN)
            sb_params_->show();

        spin_numparams_->setValue(g_ThisVF->numparams_);
    }

    // connections

    connect(spin_numparams_,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
            &P4VectorFieldDlg::numParamsChanged);
}

void P4VectorFieldDlg::numParamsChanged(int val)
{
    if (val >= 0 && val <= MAXNUMPARAMS && val != g_ThisVF->numparams_) {
        delete params_;
        params_ = nullptr;

        if (sb_params_ != nullptr) {
            delete sb_params_;
            sb_params_ = nullptr;
        }

        if (val != 0) {
            g_ThisVF->numparams_ = val;
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
            if (val > MAXNUMPARAMSSHOWN) {
                sb_params_ = new QScrollBar(Qt::Vertical, this);
                sb_params_->setRange(0,
                                     g_ThisVF->numparams_ - MAXNUMPARAMSSHOWN);
                sb_params_->setSingleStep(1);
                sb_params_->setPageStep(MAXNUMPARAMSSHOWN);
            } else
                sb_params_ = nullptr;

            params_ = new P4VFParams(this, sb_params_);
            paramLayout_->addWidget(params_);
            if (val > MAXNUMPARAMSSHOWN) {
                paramLayout_->addWidget(sb_params_);
                connect(sb_params_, &QScrollBar::valueChanged, params_,
                        &P4VFParams::paramsSliderChanged);
                connect(sb_params_, &QScrollBar::sliderMoved, params_,
                        &P4VFParams::paramsSliderChanged);
            }
            params_->show();
            if (val > MAXNUMPARAMSSHOWN)
                sb_params_->show();
        } else {
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
            g_ThisVF->numparams_ = val;
        }
    }
}

P4VectorFieldDlg::~P4VectorFieldDlg()
{
    if (params_ != nullptr) {
        delete params_;
        params_ = nullptr;
    }
    getDataFromDlg();
}

void P4VectorFieldDlg::getDataFromDlg(void)
{
    QString xdot;
    QString ydot;
    QString gcf;

    xdot = edt_xprime_->text();
    ydot = edt_yprime_->text();
    gcf = edt_gcf_->text();

    xdot = xdot.trimmed();
    ydot = ydot.trimmed();
    gcf = gcf.trimmed();

    if (xdot.compare(g_ThisVF->xdot_) || ydot.compare(g_ThisVF->ydot_) ||
        gcf.compare(g_ThisVF->gcf_)) {
        g_ThisVF->xdot_ = xdot;
        g_ThisVF->ydot_ = ydot;
        g_ThisVF->gcf_ = gcf;
        if (g_ThisVF->changed_ == false) {
            g_ThisVF->changed_ = true;
            g_p4app->signalChanged();
        }
    }
    if (params_ != nullptr) {
        params_->getDataFromDlg();
    }
}

void P4VectorFieldDlg::updateDlgData(void)
{
    edt_xprime_->setText(g_ThisVF->xdot_);
    edt_yprime_->setText(g_ThisVF->ydot_);
    edt_gcf_->setText(g_ThisVF->gcf_);
    spin_numparams_->setValue(g_ThisVF->numparams_);

    if (params_ != nullptr) {
        if (!params_->updateDlgData()) {
            delete params_;
            params_ = nullptr;
        }
    }

    if (g_ThisVF->numparams_ != 0 && params_ == nullptr) {
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            sb_params_ = new QScrollBar(Qt::Vertical, this);
            sb_params_->setRange(0, g_ThisVF->numparams_ - MAXNUMPARAMSSHOWN);
            sb_params_->setSingleStep(1);
            sb_params_->setPageStep(MAXNUMPARAMSSHOWN);
        } else
            sb_params_ = nullptr;

        params_ = new P4VFParams(this, sb_params_);
        paramLayout_->addWidget(params_);
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            paramLayout_->addWidget(sb_params_);
            connect(sb_params_, &QScrollBar::valueChanged, params_,
                    &P4VFParams::paramsSliderChanged);
            connect(sb_params_, &QScrollBar::sliderMoved, params_,
                    &P4VFParams::paramsSliderChanged);
        }
        params_->show();
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN)
            sb_params_->show();
    }
}

