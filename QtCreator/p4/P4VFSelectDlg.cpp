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

#include "P4VFSelectDlg.hpp"

#include <QMessageBox>

#include "P4Application.hpp"
#include "P4FindDlg.hpp"
#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4PlotWnd.hpp"
#include "P4SeparatingCurvesDlg.hpp"
#include "P4StartDlg.hpp"
#include "main.hpp"

P4VFSelectDlg::P4VFSelectDlg(P4FindDlg *finddlg)
    : QWidget{finddlg}, parent_{finddlg}
{
    p4title_ = std::make_unique<QLabel>("Vector Fields:", this);
    p4title_->setFont(gP4app->getTitleFont());

    cbb_vfselect_ = std::make_unique<QComboBox>(this);
    btn_add_ = std::make_unique<QPushButton>("Add", this);
    btn_del_ = std::make_unique<QPushButton>("Delete", this);
    btn_prev_ = std::make_unique<QPushButton>("<", this);
    btn_next_ = std::make_unique<QPushButton>(">", this);
    btn_p4config_ =
        std::make_unique<QPushButton>("Piecewise &Configuration...", this);

#ifdef TOOLTIPS
    btn_add_->setToolTip("Adds a new vector field");
    btn_del_->setToolTip("Deletes the current vector field");
    btn_prev_->setToolTip("Selects previous vector field");
    btn_next_->setToolTip("Selects next vector field");
    btn_p4config_->setToolTip("Opens the piecewise-configuration window");
#endif

    // layout
    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    layout1_ = std::make_unique<QHBoxLayout>();
    layout1_->addWidget(p4title_.get());
    layout1_->addWidget(cbb_vfselect_.get());
    mainLayout_->addLayout(layout1_.get());

    layout2_ = std::make_unique<QHBoxLayout>();
    layout2_->addWidget(btn_prev_.get());
    layout2_->addWidget(btn_next_.get());
    mainLayout_->addLayout(layout2_.get());

    layout3_ = std::make_unique<QHBoxLayout>();
    layout3_->addWidget(btn_add_.get());
    layout3_->addWidget(btn_del_.get());
    mainLayout_->addLayout(layout3_.get());

    mainLayout_->addWidget(btn_p4config_.get());

    // connections
    QObject::connect(btn_prev_.get(), &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnPrev);
    QObject::connect(btn_next_.get(), &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnNext);
    QObject::connect(btn_add_.get(), &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnAdd);
    QObject::connect(btn_del_.get(), &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnDel);
    QObject::connect(btn_p4config_.get(), &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnP4Config);
    QObject::connect(
        cbb_vfselect_.get(),
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &P4VFSelectDlg::onVfSelectionChanged);

    setP4WindowTitle(this, "P4 Vector Field Selection");

    updateDlgData();
}

void P4VFSelectDlg::updateDlgData()
{
    QString s;
    int k;

    for (k = 0; k < gThisVF->numVF_; k++) {
        s.sprintf("%d", k + 1);
        if (cbb_vfselect_->count() > k)
            cbb_vfselect_->setItemText(k, s);
        else
            cbb_vfselect_->addItem(s);
    }
    if (gThisVF->numVF_ > 1) {
        s = "Select All";
        if (cbb_vfselect_->count() > k)
            cbb_vfselect_->setItemText(k, s);
        else
            cbb_vfselect_->addItem(s);
        while (cbb_vfselect_->count() > gThisVF->numVF_ + 1)
            cbb_vfselect_->removeItem(gThisVF->numVF_ + 1);
    } else {
        while (cbb_vfselect_->count() > gThisVF->numVF_ + 1)
            cbb_vfselect_->removeItem(gThisVF->numVF_ + 1);
    }

    if (gThisVF->numSelected_ == 1)
        cbb_vfselect_->setCurrentIndex(gThisVF->selected_[0]);
    else
        cbb_vfselect_->setCurrentIndex(gThisVF->numSelected_);

    if (gThisVF->numVF_ > 1) {
        if (gThisVF->selected_[0] == 0 && gThisVF->numSelected_ == 1)
            btn_prev_->setEnabled(false);
        else
            btn_prev_->setEnabled(true);

        if ((gThisVF->selected_[0] == gThisVF->numVF_ - 1 &&
             gThisVF->numSelected_) == 1)
            btn_next_->setEnabled(false);
        else
            btn_next_->setEnabled(true);
    } else {
        btn_prev_->setEnabled(false);
        btn_next_->setEnabled(false);
    }

    if (win_curves_ != nullptr)
        win_curves_->updateDlgData();
}

void P4VFSelectDlg::onBtnAdd()
{
    parent_->getDataFromDlg();
    if (!checkPlotWindowClosed())
        return;

    gThisVF->addVectorField();
    if (gThisVF->numSelected_ > 1) {
        gThisVF->selected_.clear();
        gThisVF->numSelected_ = 1;
    }
    gThisVF->selected_.push_back(gThisVF->numVF_ - 1);

    parent_->updateDlgData();
    if (gThisVF->changed_ == false) {
        gThisVF->changed_ = true;
        gP4app->signalChanged();
    }
}

void P4VFSelectDlg::onBtnDel()
{
    parent_->getDataFromDlg();
    if (!checkPlotWindowClosed())
        return;

    while (gThisVF->numSelected_ > 1)
        gThisVF->deleteVectorField(
            gThisVF->selected_[gThisVF->numSelected_ - 1]);

    gThisVF->deleteVectorField(gThisVF->selected_[0]);
    parent_->updateDlgData();
    if (gThisVF->changed_ == false) {
        gThisVF->changed_ = true;
        gP4app->signalChanged();
    }
}

void P4VFSelectDlg::onBtnPrev()
{
    parent_->getDataFromDlg();
    int j{gThisVF->selected_[0]};
    gThisVF->selected_.clear();
    if (gThisVF->numSelected_ > 1)
        gThisVF->numSelected_ = 1;
    if (--j < 0)
        j = 0;
    gThisVF->selected_.push_back(j);
    parent_->updateDlgData();
}

void P4VFSelectDlg::onBtnNext()
{
    parent_->getDataFromDlg();
    int j{gThisVF->selected_[0]};
    gThisVF->selected_.clear();
    if (gThisVF->numSelected_ > 1)
        gThisVF->numSelected_ = 1;
    if (++j >= gThisVF->numVF_)
        j = gThisVF->numVF_ - 1;
    gThisVF->selected_.push_back(j);
    parent_->updateDlgData();
}

void P4VFSelectDlg::onVfSelectionChanged(int index)
{
    if (index < gThisVF->numVF_) {
        if (index != gThisVF->selected_[0] || gThisVF->numSelected_ > 1) {
            parent_->getDataFromDlg();
            gThisVF->selected_.clear();
            gThisVF->selected_.push_back(index);
            gThisVF->numSelected_ = 1;
            parent_->updateDlgData();
        }
    } else if (index >= gThisVF->numVF_) {
        // selected all
        if (gThisVF->numSelected_ != gThisVF->numVF_) {
            parent_->getDataFromDlg();
            gThisVF->selected_.clear();
            for (int k = 0; k < gThisVF->numVF_; k++)
                gThisVF->selected_.push_back(k);
            gThisVF->numSelected_ = gThisVF->numVF_;
            parent_->updateDlgData();
        }
    }
}

bool P4VFSelectDlg::checkPlotWindowClosed()
{
    if (parent_->getParentPtr()->getPlotWindowPtr() == nullptr)
        return true;
    if (!(parent_->getParentPtr()->getPlotWindowPtr()->isVisible())) {
        parent_->getParentPtr()->closePlotWindow();
        return true;
    }
    QMessageBox::critical(this, "P4",
                          "Please close the plot window before "
                          "changing the piecewise "
                          "configuration!\n");
    return false;
}

void P4VFSelectDlg::onBtnP4Config()
{
    if (!checkPlotWindowClosed())
        return;

    win_curves_ = parent_->getPiecewiseConfigWindowPtr();

    if (win_curves_ == nullptr) {
        gVFResults.readTables(gThisVF->getbarefilename(), true, true);
        parent_->createPiecewiseConfigWindow();
        win_curves_ = parent_->getPiecewiseConfigWindowPtr();
        win_curves_->show();
    } else {
        win_curves_->show();
        win_curves_->raise();
    }
}

void P4VFSelectDlg::closeConfigWindow()
{
    if (win_curves_ != nullptr) {
        parent_->closePiecewiseConfigWindow();
    }
}
