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

#include "P4VFParams.hpp"

#include <QBoxLayout>
#include <QScrollBar>

#include "P4Application.hpp"
#include "P4InputVF.hpp"
#include "P4VectorFieldDlg.hpp"

P4VFParams::P4VFParams(QScrollBar *sb, P4VectorFieldDlg *parent)
    : QWidget{parent}, currentNumParams_{gThisVF->numParams_}, sb_params_{sb}
{
    int i;

    auto label0 = new QLabel{"Enter values for all parameters"};
    label0->setFont(gP4app->getTitleFont());

    currentShownParams_ = (currentNumParams_ < MAXNUMPARAMSSHOWN)
                              ? currentNumParams_
                              : MAXNUMPARAMSSHOWN;

    // strLabels_ = gThisVF->parlabel_;
    // strValues_ = gThisVF->parvalue_;

    for (i = 0; i < currentShownParams_; i++) {
        // parlabel_ and parvalue_ might (or not) be filled from the input file
        paramNames_.emplace_back(
            std::make_unique<QLineEdit>(gThisVF->parlabel_[i], this));
        paramEqual_.emplace_back(std::make_unique<QLabel>(" = ", this));
        paramValues_.emplace_back(std::make_unique<QLineEdit>("", this));
#ifdef TOOLTIPS
        paramNames_[i]->setToolTip("Enter the name of the parameter");
        paramValues_[i]->setToolTip(
            "Enter the value of this parameter during this evaluation session");
#endif
    }

    mainLayout_ = new QBoxLayout{QBoxLayout::TopToBottom, this};
    mainLayout_->addWidget(label0);
    for (i = 0; i < currentShownParams_; i++) {
        paramLayouts_.emplace_back(std::make_unique<QHBoxLayout>());
        paramLayouts_[i]->addWidget(paramNames_[i].get());
        paramLayouts_[i]->addWidget(paramEqual_[i].get());
        paramLayouts_[i]->addWidget(paramValues_[i].get());
        mainLayout_->addLayout(paramLayouts_[i].get());

        QObject::connect(paramNames_[i].get(), &QLineEdit::editingFinished,
                         this, &P4VFParams::paramsEditingFinished);
        QObject::connect(paramValues_[i].get(), &QLineEdit::editingFinished,
                         this, &P4VFParams::paramsEditingFinished);
    }

    setLayout(mainLayout_);
    updateDlgData();
}

bool P4VFParams::focusNextPrevChild(bool next)
{
    auto p = focusWidget();

    if (p == paramNames_[0].get() && currentPageIndex_ != 0 && !next) {
        sb_params_->setValue(currentPageIndex_ - 1);
        return focusNextPrevChild(true);
    }
    if (p == paramValues_[currentShownParams_ - 1].get() &&
        currentPageIndex_ < currentNumParams_ - MAXNUMPARAMSSHOWN && next) {
        sb_params_->setValue(currentPageIndex_ + 1);
        return focusNextPrevChild(false);
    }
    return QWidget::focusNextPrevChild(next);
}

void P4VFParams::paramsEditingFinished()
{
    if (gThisVF == nullptr || gThisVF->numParams_ != currentNumParams_)
        return;

    bool changed{false};
    QString lbl;

    for (int i = 0; i < currentShownParams_; i++) {
        lbl = paramNames_[i]->text().trimmed();
        if (lbl.compare(gThisVF->parlabel_[i + currentPageIndex_])) {
            gThisVF->parlabel_[i + currentPageIndex_] = lbl;
            changed = true;
        }
        changed |= getLineEditCommonParValue(paramValues_[i].get(),
                                             i + currentPageIndex_);
    }

    if (changed) {
        if (!gThisVF->changed_) {
            gThisVF->changed_ = true;
            gP4app->signalChanged();
        }
    }
}

// only called when vf is loaded, and only when numparams did not change.
bool P4VFParams::updateDlgData()
{
    if (gThisVF->numParams_ != currentNumParams_)
        return false;

    for (int i = 0; i < currentShownParams_; i++) {
        paramNames_[i]->setText(gThisVF->parlabel_[i + currentPageIndex_]);
        setLineEditCommonParValue(paramValues_[i].get(), i + currentPageIndex_);
    }

    return true;
}

void P4VFParams::paramsSliderChanged(int value)
{
    if (value < 0)
        value = 0;
    if (value > currentNumParams_ - MAXNUMPARAMSSHOWN)
        value = currentNumParams_ - MAXNUMPARAMSSHOWN;

    if (value != currentPageIndex_) {
        currentPageIndex_ = value;

        for (int i = 0; i < currentShownParams_; i++) {
            paramNames_[i]->setText(gThisVF->parlabel_[i + currentPageIndex_]);
            setLineEditCommonParValue(paramValues_[i].get(),
                                      i + currentPageIndex_);
        }
    }
}

void P4VFParams::setLineEditCommonParValue(QLineEdit *le, int index)
{
    if (gThisVF->hasCommonParValue(index))
        le->setText(gThisVF->commonParValue(index));
    else
        le->setText("########");
}

bool P4VFParams::getLineEditCommonParValue(QLineEdit *le, int index)
{
    int i;
    QString val{le->text().trimmed()};
    if (!gThisVF->hasCommonParValue(index)) {
        for (i = val.length() - 1; i >= 0; i--) {
            if (val[i] != '#')
                break;
        }
        if (i < 0 || !val.compare(gThisVF->commonParValue(index)))
            return false;
    }
    if (!val.compare(gThisVF->commonParValue(index)))
        return false;

    gThisVF->setCommonParValue(index, val);
    return true;
}
