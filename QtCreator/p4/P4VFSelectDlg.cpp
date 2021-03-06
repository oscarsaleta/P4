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

#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

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
    auto p4title = new QLabel{"Vector Fields:", this};
    p4title->setFont(gP4app->getTitleFont());

    cbb_vfselect_ = new QComboBox{this};
    btn_add_ = new QPushButton{"Add", this};
    btn_del_ = new QPushButton{"Delete", this};
    btn_prev_ = new QPushButton{"<", this};
    btn_next_ = new QPushButton{">", this};
    btn_p4config_ = new QPushButton{"Piecewise &Configuration...", this};

#ifdef TOOLTIPS
    btn_add_->setToolTip("Adds a new vector field");
    btn_del_->setToolTip("Deletes the current vector field");
    btn_prev_->setToolTip("Selects previous vector field");
    btn_next_->setToolTip("Selects next vector field");
    btn_p4config_->setToolTip("Opens the piecewise-configuration window");
#endif

    // layout
    mainLayout_ = new QBoxLayout{QBoxLayout::TopToBottom, this};

    auto layout1_ = new QHBoxLayout{};
    layout1_->addWidget(p4title);
    layout1_->addWidget(cbb_vfselect_);
    mainLayout_->addLayout(layout1_);

    auto layout2_ = new QHBoxLayout{};
    layout2_->addWidget(btn_prev_);
    layout2_->addWidget(btn_next_);
    mainLayout_->addLayout(layout2_);

    auto layout3_ = new QHBoxLayout{};
    layout3_->addWidget(btn_add_);
    layout3_->addWidget(btn_del_);
    mainLayout_->addLayout(layout3_);

    mainLayout_->addWidget(btn_p4config_);

    // connections
    QObject::connect(btn_prev_, &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnPrev);
    QObject::connect(btn_next_, &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnNext);
    QObject::connect(btn_add_, &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnAdd);
    QObject::connect(btn_del_, &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnDel);
    QObject::connect(btn_p4config_, &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnP4Config);
    QObject::connect(
        cbb_vfselect_,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &P4VFSelectDlg::onVfSelectionChanged);

    setP4WindowTitle(this, "P4 Vector Field Selection");

    updateDlgData();
}

void P4VFSelectDlg::updateDlgData()
{
    QString s;

    // Clear combobox
    // We need to disconnect the currentIndexChanged signal first because it
    // would call onVfSelectionChanged on the empty QComboBox, causing a
    // segmentation fault
    QObject::disconnect(
        cbb_vfselect_,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &P4VFSelectDlg::onVfSelectionChanged);

    cbb_vfselect_->clear();

    // Add number for each VF
    for (unsigned int i = 0; i < gThisVF->numVF_; i++) {
        s.sprintf("%d", i + 1);
        cbb_vfselect_->addItem(s);
    }

    // Add select all
    if (gThisVF->numVF_ > 1) {
        s = "Select All";
        cbb_vfselect_->addItem(s);
    }

    // Set current index to selected VF (or last one if several)
    if (gThisVF->numSelected_ == 1)
        cbb_vfselect_->setCurrentIndex(gThisVF->selected_[0]);
    else
        cbb_vfselect_->setCurrentIndex(gThisVF->numSelected_ - 1);

    // Set prev and next buttons depending on VF count and selection
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

    // Update separating curves window data
    if (win_curves_ != nullptr)
        win_curves_->updateDlgData();

    QObject::connect(
        cbb_vfselect_,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &P4VFSelectDlg::onVfSelectionChanged);
}

void P4VFSelectDlg::onBtnAdd()
{
    parent_->getDataFromDlg();
    if (!checkPlotWindowClosed())
        return;

    gThisVF->addVectorField();
    gThisVF->numSelected_ = 1;
    gThisVF->selected_.clear();
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
    unsigned int j{gThisVF->selected_[0]};
    gThisVF->selected_.clear();
    if (gThisVF->numSelected_ > 1)
        gThisVF->numSelected_ = 1;
    if (j > 0)
        j--;
    else
        j = 0;
    gThisVF->selected_.push_back(j);
    parent_->updateDlgData();
}

void P4VFSelectDlg::onBtnNext()
{
    parent_->getDataFromDlg();
    unsigned int j{gThisVF->selected_[0]};
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
    if (index < static_cast<int>(gThisVF->numVF_)) {
        if (index != static_cast<int>(gThisVF->selected_[0]) ||
            gThisVF->numSelected_ > 1) {
            parent_->getDataFromDlg();
            gThisVF->selected_.clear();
            gThisVF->selected_.push_back(index);
            gThisVF->numSelected_ = 1;
            parent_->updateDlgData();
        }
    } else {
        // selected all
        if (gThisVF->numSelected_ != gThisVF->numVF_) {
            parent_->getDataFromDlg();
            gThisVF->selected_.clear();
            for (unsigned int k = 0; k < gThisVF->numVF_; k++)
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

    if (win_curves_ == nullptr) {
        gVFResults.readTables(gThisVF->getbarefilename(), true, true);
        win_curves_ = new P4SeparatingCurvesDlg{parent_};
        win_curves_->show();
    } else {
        win_curves_->show();
        win_curves_->raise();
    }
}

void P4VFSelectDlg::closeConfigWindow()
{
    if (win_curves_ != nullptr) {
        delete win_curves_;
        win_curves_ = nullptr;
    }
}

P4SeparatingCurvesDlg *P4VFSelectDlg::getConfigWindowPtr() const
{
    return win_curves_;
}
