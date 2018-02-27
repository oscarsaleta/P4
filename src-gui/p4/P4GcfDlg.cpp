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

#include "P4GcfDlg.h"

#include "custom.hpp"
#include "file_tab.hpp"
#include "P4InputVF.h"
#include "main.hpp"
#include "math_gcf.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

P4GcfDlg::P4GcfDlg(std::unique_ptr<P4PlotWnd> plt,
                   std::unique_ptr<P4WinSphere> sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint),
      plotwnd_{plt},
      mainSphere_{sp}
{
    std::unique_ptr<QButtonGroup> btngrp{new QButtonGroup(this)};
    btn_dots_.reset(QRadioButton("Dots", this));
    btn_dashes_.reset(QRadioButton("Dashes", this));
    btngrp->addButton(btn_dots_);
    btngrp->addButton(btn_dashes_);

    std::unique_ptr<QLabel> lbl1{new QLabel("Appearance: ", this)};

    edt_points_.reset(QLineEdit("", this));
    std::unique_ptr<QLabel> lbl2{new QLabel("#Points: ", this)};

    edt_precis_.reset(QLineEdit("", this));
    std::unique_ptr<QLabel> lbl3{new QLabel("Precision: ", this)};

    edt_memory_.reset(QLineEdit("", this));
    std::unique_ptr<QLabel> lbl4{new QLabel("Max. Memory: ", this)};

    btn_evaluate_.reset(QPushButton("&Evaluate", this));

#ifdef TOOLTIPS
    btn_dots_->setToolTip(
        "Plot individual points of the curve of singularities");
    btn_dashes_->setToolTip("Connect points of the curve of singularities with "
                            "small line segments");
    btn_evaluate_->setToolTip("Start evaluation (using symbolic manipulator)");
    QString ttip{QString::fromStdString(
        "Number of points. Must be between " + std::to_string(MIN_GCFPOINTS) +
        " and " + std::to_string(MAX_GCFPOINTS))};
    edt_points_->setToolTip(ttip);
    ttip = QString::fromStdString("Required precision. Must be between " +
                                  std::to_string(MIN_GCFPRECIS) + " and " +
                                  std::to_string(MAX_GCFPRECIS));
    edt_precis_->setToolTip(ttip);
    ttip = QString::fromStdString("Maximum amount of memory (in kilobytes) "
                                  "spent on plotting GCF.\nMust be between " +
                                  std::to_string(MIN_GCFMEMORY) + " and " +
                                  std::to_string(MAX_GCFMEMORY));
    edt_memory_->setToolTip(ttip);
#endif

    // layout
    mainLayout_.reset(new QBoxLayout(QBoxLayout::TopToBottom, this));

    std::unique_ptr<QHBoxLayout> layout1{new QHBoxLayout()};
    layout1->addWidget(lbl1);
    layout1->addWidget(btn_dots_);
    layout1->addWidget(btn_dashes_);

    std::unique_ptr<QGridLayout> lay00{new QGridLayout()};
    lay00->addWidget(lbl2, 0, 0);
    lay00->addWidget(edt_points_, 0, 1);
    lay00->addWidget(lbl3, 1, 0);
    lay00->addWidget(edt_precis_, 1, 1);
    lay00->addWidget(lbl4, 2, 0);
    lay00->addWidget(edt_memory_, 2, 1);

    std::unique_ptr<QHBoxLayout> layout2{new QHBoxLayout()};
    layout2->addStretch(0);
    layout2->addWidget(btn_evaluate_);
    layout2->addStretch(0);

    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(lay00);
    mainLayout_->addLayout(layout2);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections
    connect(btn_evaluate_, &QPushButton::clicked, this,
            &P4GcfDlg::onbtn_evaluate);

    // finishing
    setP4WindowTitle(this, "GCF Plot");
}

void P4GcfDlg::reset()
{
    QString buf;

    buf.sprintf("%d", DEFAULT_GCFPOINTS);
    edt_points_->setText(buf);

    buf.sprintf("%d", DEFAULT_GCFPRECIS);
    edt_precis_->setText(buf);

    buf.sprintf("%d", DEFAULT_GCFMEMORY);
    edt_memory_->setText(buf);

    if (gVFResults.config_dashes_)
        btn_dashes_->toggle();
    else
        btn_dots_->toggle();
}

void P4GcfDlg::onbtn_evaluate()
{
    bool dashes{btn_dashes_->isChecked()}, result;
    int points, precis, memory;
    bool ok{true};

    QString buf{edt_points_->text()};
    points = buf.toInt();
    if (points < MIN_GCFPOINTS || points > MAX_GCFPOINTS) {
        buf += " ???";
        edt_points_->setText(buf);
        ok = false;
    }

    buf = edt_precis_->text();
    precis = buf.toInt();
    if (precis < MIN_GCFPRECIS || precis > MAX_GCFPRECIS) {
        buf += " ???";
        edt_precis_->setText(buf);
        ok = false;
    }

    buf = edt_memory_->text();
    memory = buf.toInt();
    if (memory < MIN_GCFMEMORY || memory > MAX_GCFMEMORY) {
        buf += " ???";
        edt_memory_->setText(buf);
        ok = false;
    }

    if (!ok) {
        QMessageBox::information(
            this, "P4", "One of the fields has a value that is out of bounds.\n"
                        "Please correct before continuing.\n");
        return;
    }

    // Evaluate GCF with given parameters {dashes, points, precis, memory}.
    evaluating_points_ = points;
    evaluating_memory_ = memory;
    evaluating_precision_ = precis;

    btn_evaluate_->setEnabled(false);

    gThisVF->gcfDlg_ = this;
    result = evalGcfStart(mainSphere_, dashes, points, precis);
    if (!result) {
        btn_evaluate_->setEnabled(true);
        QMessageBox::critical(this, "P4", "An error occured while plotting the "
                                          "GCF.\nThe singular locus may not be "
                                          "visible, or may be partially "
                                          "visible.");
    }
}

void P4GcfDlg::finishGcfEvaluation()
{
    if (btn_evaluate_->isEnabled() == true)
        return; // not busy??

    bool result{evalGcfContinue(evaluating_points_, evaluating_precision_)};

    if (result) {
        btn_evaluate_->setEnabled(true);
        // return false in case an error occured
        if (!evalGcfFinish()) {
            QMessageBox::critical(this, "P4", "An error occured while plotting "
                                              "the GCF.\nThe singular locus "
                                              "may not be visible, or may "
                                              "be partially visible.");
        }
    }
}
