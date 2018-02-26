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

#include "P4VFParams.h"

#include "P4VectorFieldDlg.h"

#include <QLabel>
#include <QScrollBar>

P4VFParams::P4VFParams(P4VectorFieldDlg *parent, std::shared_ptr<QScrollBar> sb)
    : QWidget{parent}, dataInvalid_{false}, sb_params{sb},
      currentNumParams_{gThisVF->numparams_}, currentPageIndex_{0}
{
    int i;

    std::unique_ptr<QLabel> label0{
        std::make_unique<QLabel>("Enter values for all parameters")};
    label0->setFont(*(gP4app->titleFont_));

    currentShownParams_ = (currentNumParams_ < MAXNUMPARAMSSHOWN)
                              ? currentNumParams_
                              : MAXNUMPARAMSSHOWN;

    // strLabels_ = gThisVF->parlabel_;
    // strValues_ = gThisVF->parvalue_;

    for (i = 0; i < currentShownParams_; i++) {
        // parlabel_ and parvalue_ might (or not) be filled from the input file
        paramNames_.push_back(
            std::make_unique<QLineEdit>(gThisVF->parlabel_[i], this));
        paramEqual_.push_back(std::make_unique<QLabel>(" = ", this));
        paramValues_.push_back(std::make_unique<QLineEdit>("", this));
#ifdef TOOLTIPS
        paramNames_[i]->setToolTip("Enter the name of the parameter");
        paramValues_[i]->setToolTip(
            "Enter the value of this parameter during this evaluation session");
#endif
    }

    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom);
    mainLayout_->addWidget(label0);
    for (i = 0; i < currentShownParams_; i++) {
        paramLayouts_.push_back(std::make_unique<QHBoxLayout>{});
        paramLayouts_[i]->addWidget(paramNames_[i]);
        paramLayouts_[i]->addWidget(paramEqual_[i]);
        paramLayouts_[i]->addWidget(paramValues_[i]);
        mainLayout_->addLayout(paramLayouts_[i]);

        QObject::connect(paramNames_[i], &QLineEdit::editingFinished, this,
                         &P4VFParams::paramsEditingFinished);
        QObject::connect(paramValues_[i], &QLineEdit::editingFinished, this,
                         &P4VFParams::paramsEditingFinished);
    }

    setLayout(mainLayout_);
    updateDlgData();
}

bool P4VFParams::focusNextPrevChild(bool next)
{
    std::unique_ptr<QWidget> p{focusWidget()};

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

void P4VFParams::paramsEditingFinished()
{
    if (!gThisVF || gThisVF->numParams_ != currentNumParams_)
        return;

    bool changed{false};
    QString lbl;

    for (int i = 0; i < currentShownParams_; i++) {
        lbl = paramNames_[i]->text().trimmed();
        if (lbl.compare(gThisVF->parlabel_[i + currentPageIndex_])) {
            gThisVF->parlabel_[i + currentPageIndex_] = lbl;
            changed = true;
        }
        changed |=
            getLineEditCommonParValue(paramValues_[i].get(), i + currentPageIndex_);
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
    if (gThisVF->numparams_ != currentNumParams_)
        return false;

    for (int i = 0; i < currentShownParams_; i++) {
        paramNames_[i]->setText(gThisVF->parlabel_[i + currentPageIndex_]);
        setLineEditCommonParValue(paramValues_[i].get(), i + currentPageIndex_);
    }

    return true;
}

void P4VFParams::paramsSliderChanged(int value)
{
    int newindex{std::move(value)};

    if (newindex < 0)
        newindex = 0;
    if (newindex > currentNumParams_ - MAXNUMPARAMSSHOWN)
        newindex = currentNumParams_ - MAXNUMPARAMSSHOWN;

    if (newindex != currentPageIndex_) {
        currentPageIndex_ = newindex;

        for (int i = 0; i < currentShownParams_; i++) {
            paramNames_[i]->setText(gThisVF->parlabel_[i + currentPageIndex_]);
            setLineEditCommonParValue(paramValues_[i].get(), i + currentPageIndex_);
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

void P4VFParams::getLineEditCommonParValue(QLineEdit *le, int index)
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