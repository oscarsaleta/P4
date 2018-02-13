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
    mainLayout_.reset(new QBoxLayout(QBoxLayout::TopToBottom,this));

    std::unique_ptr<QHBoxLayout> layout1{new QHBoxLayout()};
    layout1->addWidget(p4title);
    layout1->addWidget(cbb_vfselect_);
    mainLayout_->addLayout(layout1);

    std::unique_ptr<QHBoxLayout> layout2{new QHBoxLayout()};
    layout2->addWidget(btn_add_);
    layout2->addWidget(btn_del_);
    mainLa
}