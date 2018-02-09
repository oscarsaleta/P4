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

#include "win_vf.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "p4application.h"
#include "win_find.h"

#include <QBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include <QSpinBox>

QVectorFieldDlg::QVectorFieldDlg(P4FindDlg *finddlg) : QWidget(finddlg)
{
    parent_ = finddlg;
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    QLabel *p4title = new QLabel("Specify the vector field:", this);
    p4title->setFont(*(g_p4app->titleFont_));

    edt_xprime_ = new QLineEdit(g_ThisVF->xdot_, this);
    QLabel *xlabel = new QLabel("&x' = ", this);
    xlabel->setFont(*(g_p4app->boldFont_));
    xlabel->setBuddy(edt_xprime_);

    edt_yprime_ = new QLineEdit(g_ThisVF->ydot_, this);
    QLabel *ylabel = new QLabel("&y' = ", this);
    ylabel->setFont(*(g_p4app->boldFont_));
    ylabel->setBuddy(edt_yprime_);

    edt_gcf_ = new QLineEdit(g_ThisVF->gcf_, this);
    QLabel *glabel = new QLabel("&Gcf: ", this);
    glabel->setFont(*(g_p4app->boldFont_));
    glabel->setBuddy(edt_gcf_);

    QLabel *plabel = new QLabel("Number of Parameters: ", this);
    plabel->setFont(*(g_p4app->boldFont_));

    spin_numparams_ = new QSpinBox(this);
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

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    mainLayout_->addWidget(p4title);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(xlabel, edt_xprime_);
    formLayout->addRow(ylabel, edt_yprime_);
    formLayout->addRow(glabel, edt_gcf_);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(plabel);
    layout3->addWidget(spin_numparams_);
    layout3->addStretch(0);

    paramLayout_ = new QHBoxLayout();

    mainLayout_->addLayout(formLayout);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(paramLayout_);

    setLayout(mainLayout_);

    sb_params_ = nullptr;
    params_ = nullptr;

    if (g_ThisVF->numparams_ != 0) {
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            sb_params_ = new QScrollBar(Qt::Vertical, this);
            sb_params_->setRange(0, g_ThisVF->numparams_ - MAXNUMPARAMSSHOWN);
            sb_params_->setSingleStep(1);
            sb_params_->setPageStep(MAXNUMPARAMSSHOWN);
        } else
            sb_params_ = nullptr;

        params_ = new QVFParams(this, sb_params_);
        paramLayout_->addWidget(params_);
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            paramLayout_->addWidget(sb_params_);
            connect(sb_params_, &QScrollBar::valueChanged, params_,
                    &QVFParams::paramsSliderChanged);
            connect(sb_params_, &QScrollBar::sliderMoved, params_,
                    &QVFParams::paramsSliderChanged);
        }
        params_->show();
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN)
            sb_params_->show();

        spin_numparams_->setValue(g_ThisVF->numparams_);
    }

    // connections

    connect(spin_numparams_,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
            &QVectorFieldDlg::numParamsChanged);
}

void QVectorFieldDlg::numParamsChanged(int val)
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

            params_ = new QVFParams(this, sb_params_);
            paramLayout_->addWidget(params_);
            if (val > MAXNUMPARAMSSHOWN) {
                paramLayout_->addWidget(sb_params_);
                connect(sb_params_, &QScrollBar::valueChanged, params_,
                        &QVFParams::paramsSliderChanged);
                connect(sb_params_, &QScrollBar::sliderMoved, params_,
                        &QVFParams::paramsSliderChanged);
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

QVectorFieldDlg::~QVectorFieldDlg()
{
    if (params_ != nullptr) {
        delete params_;
        params_ = nullptr;
    }
    getDataFromDlg();
}

void QVectorFieldDlg::getDataFromDlg(void)
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

void QVectorFieldDlg::updateDlgData(void)
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

        params_ = new QVFParams(this, sb_params_);
        paramLayout_->addWidget(params_);
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN) {
            paramLayout_->addWidget(sb_params_);
            connect(sb_params_, &QScrollBar::valueChanged, params_,
                    &QVFParams::paramsSliderChanged);
            connect(sb_params_, &QScrollBar::sliderMoved, params_,
                    &QVFParams::paramsSliderChanged);
        }
        params_->show();
        if (g_ThisVF->numparams_ > MAXNUMPARAMSSHOWN)
            sb_params_->show();
    }
}

QVFParams::QVFParams(QVectorFieldDlg *parent, QScrollBar *sb) : QWidget(parent)
{
    int i;

    QLabel *label0 = new QLabel("Enter values for all parameters");
    label0->setFont(*(g_p4app->titleFont_));

    dataInvalid_ = false;
    sb_params_ = sb;
    currentNumParams_ = g_ThisVF->numparams_;
    currentShownParams_ = (currentNumParams_ < MAXNUMPARAMSSHOWN) ?
                              currentNumParams_ :
                              MAXNUMPARAMSSHOWN;
    currentPageIndex_ = 0;

    for (i = 0; i < currentShownParams_; i++) {
        // parlabel_ and parvalue_ might (or not) be filled from the input file
        paramNames_[i] = new QLineEdit(g_ThisVF->parlabel_[i], this);
        paramLabels_[i] = new QLabel(" = ", this);
        paramValues_[i] = new QLineEdit(g_ThisVF->parvalue_[i], this);
#ifdef TOOLTIPS
        paramNames_[i]->setToolTip("Enter the name of the parameter");
        paramValues_[i]->setToolTip(
            "Enter the value of this parameter during this evaluation session");
#endif
    }

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout_->addWidget(label0);
    for (i = 0; i < currentShownParams_; i++) {
        paramLines_[i] = new QHBoxLayout();
        paramLines_[i]->addWidget(paramNames_[i]);
        paramLines_[i]->addWidget(paramLabels_[i]);
        paramLines_[i]->addWidget(paramValues_[i]);
        mainLayout_->addLayout(paramLines_[i]);
    }

    for (i = 0; i < currentNumParams_; i++) {
        sLabels_[i] = g_ThisVF->parlabel_[i];
        sValues_[i] = g_ThisVF->parvalue_[i];
    }
    setLayout(mainLayout_);
}

QVFParams::~QVFParams()
{
    if (!dataInvalid_)
        getDataFromDlg();
}

bool QVFParams::focusNextPrevChild(bool next)
{
    QWidget *p;
    p = focusWidget();
    if (p == paramNames_[0] && currentPageIndex_ != 0 && !next) {
        sb_params_->setValue(currentPageIndex_ - 1);
        return focusNextPrevChild(true);
    }
    if (p == paramValues_[currentShownParams_ - 1] &&
        currentPageIndex_ < currentNumParams_ - MAXNUMPARAMSSHOWN && next) {
        sb_params_->setValue(currentPageIndex_ + 1);
        return focusNextPrevChild(false);
    }
    return QWidget::focusNextPrevChild(next);
}

bool QVFParams::getDataFromDlg(void)
{
    int i;
    bool changed;
    QString temp;

    if (g_ThisVF == nullptr)
        return false;

    if (g_ThisVF->numparams_ != currentNumParams_)
        return false;

    currentNumParams_ = g_ThisVF->numparams_;

    changed = false;

    for (i = 0; i < currentShownParams_; i++) {
        sLabels_[i + currentPageIndex_] = paramNames_[i]->text();
        sValues_[i + currentPageIndex_] = paramValues_[i]->text();
    }
    for (i = 0; i < currentNumParams_; i++) {
        temp = sLabels_[i];
        temp = temp.trimmed();
        if (temp.compare(g_ThisVF->parlabel_[i])) {
            changed = true;
            g_ThisVF->parlabel_[i] = temp;
        }
        temp = sValues_[i];
        temp = temp.trimmed();
        if (temp.compare(g_ThisVF->parvalue_[i])) {
            changed = true;
            g_ThisVF->parvalue_[i] = temp;
        }
    }

    if (changed) {
        if (g_ThisVF->changed_ == false) {
            g_ThisVF->changed_ = true;
            g_p4app->signalChanged();
        }
    }
    return true;
}

// only called when vf is loaded, and only when numparams did not change.

bool QVFParams::updateDlgData(void)
{
    int i;

    if (g_ThisVF->numparams_ != currentNumParams_) {
        dataInvalid_ = true;
        return false;

        // when the number of parameters changed, signal it to the parent
        // window.  This parent window will close this window and re-open it.
        // In order to prevent data-overwriting when the window is closed,
        // set a flag to say that the current data in this object is no longer
        // valid.
    }

    for (i = 0; i < currentShownParams_; i++) {
        paramNames_[i]->setText(g_ThisVF->parlabel_[i + currentPageIndex_]);
        paramValues_[i]->setText(g_ThisVF->parvalue_[i + currentPageIndex_]);
    }

    return true;
}

void QVFParams::paramsSliderChanged(int value)
{
    int i;
    int newindex;

    newindex = value;
    if (newindex < 0)
        newindex = 0;
    if (newindex > currentNumParams_ - MAXNUMPARAMSSHOWN)
        newindex = currentNumParams_ - MAXNUMPARAMSSHOWN;

    if (newindex != currentPageIndex_) {
        for (i = 0; i < currentShownParams_; i++) {
            sLabels_[i + currentPageIndex_] = paramNames_[i]->text();
            sValues_[i + currentPageIndex_] = paramValues_[i]->text();
        }

        currentPageIndex_ = newindex;

        for (i = 0; i < currentShownParams_; i++) {
            paramNames_[i]->setText(sLabels_[i + currentPageIndex_]);
            paramValues_[i]->setText(sValues_[i + currentPageIndex_]);
        }
    }
}
