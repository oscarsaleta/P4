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

#include "P4VFSelectDlg.h"

#include <QBoxLayout>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

#include "P4InputVF.h"
#include "P4ParentStudy.h"
#include "main.h"

P4VFSelectDlg::P4VFSelectDlg(std::unique_ptr<P4FindDlg> finddlg)
    : QWidget(finddlg), parent_{finddlg}
{
    std::unique_ptr<QLabel> p4title{new QLabel("Vector Fields:", this)};
    p4title->setFont(*(g_p4app->titleFont_));

    cbb_vfselect_.reset(new QComboBox(this));
    btn_add_.reset(new QPushButton("Add", this));
    btn_del_.reset(new QPushButton("Delete", this));
    btn_prev_.reset(new QPushButton("<", this));
    btn_next_.reset(new QPushButton(">", this));
    btn_p5config_.reset(new QPushButton("Piecewise &Configuration...", this));

#ifdef TOOLTIPS
    btn_add_->setToolTip("Adds a new vector field");
    btn_del_->setToolTip("Deletes the current vector field");
    btn_prev_->setToolTip("Selects previous vector field");
    btn_next_->setToolTip("Selects next vector field");
    btn_p5config_->setToolTip("Opens the piecewise-configuration window");
#endif

    // layout
    mainLayout_.reset(new QBoxLayout(QBoxLayout::TopToBottom, this));

    std::unique_ptr<QHBoxLayout> layout1{new QHBoxLayout()};
    layout1->addWidget(p4title);
    layout1->addWidget(cbb_vfselect_);
    mainLayout_->addLayout(layout1);

    std::unique_ptr<QHBoxLayout> layout2{new QHBoxLayout()};
    layout2->addWidget(btn_prev_);
    layout2->addWidget(btn_next_);
    mainLayout_->addLayout(layout2);

    std::unique_ptr<QHBoxLayout> layout3{new QHBoxLayout()};
    layout3->addWidget(btn_add_);
    layout3->addWidget(btn_del_);
    mainLayout_->addLayout(layout3);

    mainLayout_->addWidget(btn_p5config_);

    // connections
    QObject::connect(btn_prev_, &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnPrev);
    QObject::connect(btn_next_, &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnNext);
    QObject::connect(btn_add_, &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnAdd);
    QObject::connect(btn_del_, &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnDel);
    QObject::connect(btn_p5config_, &QPushButton::clicked, this,
                     &P4VFSelectDlg::onBtnP5Config);
    QObject::connect(cbb_vfselect_, static_cast<void (QComboBox::*)(int)>(
                                        &QComboBox::currentIndexChanged),
                     this, &P4VFSelectDlg::onVFSelectionChanged);

    setP4WindowTitle(this, "P4 Vector Field Selection");

    updateDlgData();
}

void P4VFSelectDlg::getDataFromDlg()
{
    if (win_curves_)
        win_curves_->getDataFromDlg();
}

void P4VFSelectDlg::updateDlgData()
{
    QString s;

    for (int k = 0; k < g_ThisVF->numVF_; k++) {
        s.sprintf("%d", k + 1);
        if (cbb_vfselect_->count() > k)
            cbb_vfselect_->setItemText(k, s);
        else
            cbb_vfselect_->addItem(s);
    }
    if (g_ThisVF->numVF_ > 1) {
        s = "Select All";
        if (cbb_vfselect_->count() > k)
            cbb_vfselect_->setItemText(k, s);
        else
            cbb_vfselect_->addItem(s);
        while (cbb_vfselect_->count() > g_ThisVF->numVF_ + 1)
            cbb_vfselect_->removeItem(g_ThisVF->numVF_ + 1);
    } else {
        while (cbb_vfselect_->count() > g_ThisVF->numVF_ + 1)
            cbb_vfselect_->removeItem(g_ThisVF->numVF_ + 1);
    }

    if (g_ThisVF->numSelected_ == 1)
        cbb_vfselect_->setCurrentIndex(g_ThisVF->selected_[0]);
    else
        cbb_vfselect_->setCurrentIndex(g_ThisVF->numSelected_);

    if (g_ThisVF->numVF_ > 1) {
        if (g_ThisVF->selected_[0] == 0 && g_ThisVF->numSelected_ == 1)
            btn_prev_->setEnabled(false);
        else
            btn_prev_->setEnabled(true);

        if (g_ThisVF->selected_[0] =
                g_ThisVF->numVF_ - 1 && g_ThisVF->numSelected_ == 1)
            btn_next_->setEnabled(false);
        else
            btn_next_->setEnabled(true);
    } else {
        btn_prev_->setEnabled(false);
        btn_next_->setEnabled(false);
    }

    if (win_curves_)
        win_curves_->updateDlgData();
}

void P4VFSelectDlg::onBtnAdd()
{
    parent_->getDataFromDlg();
    if (!checkPlotWindowClosed())
        return;

    g_ThisVF->addVectorField();
    if (g_ThisVF->numSelected_ > 1) {
        g_ThisVF->selected_.clear();
        g_ThisVF->numSelected_ = 1;
    }
    g_ThisVF->selected_.push_back(g_ThisVF->numVF_ - 1);

    parent_->updateDlgData();
    if (g_ThisVF->changed_ == false) {
        g_ThisVF->changed = true;
        g_p4app->signalChanged();
    }
}

void P4VFSelectDlg::onBtnDel()
{
    parent_->getDataFromDlg();
    if (!checkPlotWindowClosed())
        return;
    while (g_ThisVF->numSelected_ > 1)
        g_ThisVF->deleteVectorField(
            g_ThisVF->selected_[g_ThisVF->numSelected_ - 1]);
    g_ThisVF->deleteVectorField(g_ThisVF->selected_[0]);
    parent_->updateDlgData();
    if (g_ThisVF->changed_ == false) {
        g_ThisVF->changed_ = true;
        g_p4app->signalChanged();
    }
}

void P4VFSelectDlg::onBtnPrev()
{
    parent_->getDataFromDlg();
    int j{g_ThisVF->selected_[0]};
    if (g_ThisVF->numSelected_ > 1) {
        g_ThisVF->selected_.clear();
        g_ThisVF->numSelected_ = 1;
    }
    if (--j < 0)
        j = 0;
    g_ThisVF->selected_.push_back(j);
    parent_->updateDlgData();
}

void P4VFSelectDlg::onBtnNext()
{
    parent_->getDataFromDlg();
    int j{g_ThisVF->selected_[0]};
    if (g_ThisVF->numSelected_ > 1) {
        g_ThisVF->selected_.clear();
        g_ThisVF->numSelected_ = 1;
    }
    if (++j >= g_ThisVF->numVF_)
        j = g_ThisVF->numVF_ - 1;
    g_ThisVF->selected_.push_back(j);
    parent_->updateDlgData();
}

void P4VFSelectDlg::onVFSelectionChanged(int index)
{
    if (index < g_ThisVF->numVF_) {
        if (index != g_ThisVF->selected_[0] || g_ThisVF->numSelected_ > 1) {
            parent_->getDataFromDlg();
            g_ThisVF->selected_.clear();
            g_ThisVF->selected_.push_back(index);
            g_ThisVF->numSelected_ = 1;
            parent_->getDataFromDlg();
        }
    } else if (index >= g_ThisVF->numVF_) {
        // selected all
        if (g_ThisVF->numSelected_ != g_ThisVF->numVF_) {
            parent_->getDataFromDlg();
            g_ThisVF->selected_.clear();
            for (int k = 0; k < g_ThisVF->numVF_; k++)
                g_ThisVF->selected_.push_back(k);
            g_ThisVF->numSelected_ = g_ThisVF->numVF_;
            parent_->updateDlgData();
        }
    }
}

bool P4VFSelectDlg::checkPlotWindowClosed()
{
    if (!(parent_->parent_->plotWindow_))
        return true;
    if (!(parent_->parent_->plotWindow_->isVisible())) {
        parent_->parent_->closePlotWindow();
        return true;
    }
    QMessageBox::critical(this, "P4", "Please close the plot window before "
                                      "changing the piecewise "
                                      "configuration!\n");
    return false;
}

void P4VFSelectDlg::onBtnP5Config()
{
    if (!checkPlotWindowClosed())
        return;

    if (!win_curves_) {
        g_VFResults.readTables(g_ThisVF->getbarefilename(), true, true);
        win_curves_.reset(new P4SeparatingCurvesDlg(parent_));
        win_curves_->show();
    } else {
        win_curves_->show();
        win_curves_->raise();
    }
}

void P4VFSelectDlg::closeConfigWindow()
{
    if (win_curves_) {
        win_curves_.reset();
    }
}