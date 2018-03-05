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

#include "P4IsoclinesDlg.hpp"

#include "P4InputVF.hpp"
#include "P4PlotWnd.hpp"
#include "P4WinSphere.hpp"
#include "custom.hpp"
#include "file_tab.hpp"
#include "main.hpp"
#include "math_gcf.hpp"
#include "math_isoclines.hpp"
#include "math_polynom.hpp"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

P4IsoclinesDlg::P4IsoclinesDlg(std::shared_ptr<P4PlotWnd> plt,
                               std::shared_ptr<P4WinSphere> sp)
    : QWidget{nullptr, Qt::Tool | Qt::WindowStaysOnTopHint},
      mainSphere_{std::move(sp)}, plotwnd_{std::move(plt)}
{
    edt_value_ = std::make_unique<QLineEdit>("", this);
    auto lbl0 = std::make_unique<QLabel>("&Value = ", this);
    lbl0->setBuddy(edt_value_);

    auto btngrp = std::make_unique<QButtonGroup>(this);
    btn_dots_ = std::make_unique<QRadioButton>("Dots", this);
    btn_dashes_ = std::make_unique<QRadioButton>("Dashes", this);
    btngrp->addButton(btn_dots_);
    btngrp->addButton(btn_dashes_);
    auto lbl1 = std::make_unique<QLabel>("Appearance: ", this);

    edt_points_ = std::make_unique<QLineEdit>("", this);
    auto lbl2 = std::make_unique<QLabel>("Num. Points: ", this);

    edt_precis_ = std::make_unique<QLineEdit>("", this);
    auto lbl3 = std::make_unique<QLabel>("Precision: ", this);

    edt_memory_ = std::make_unique<QLineEdit>("", this);
    auto lbl4 = std::make_unique<QLabel>("Max. Memory: ", this);

    btnEvaluate_ = std::make_unique<QPushButton>("&Evaluate", this);
    btnPlot_ = std::make_unique<QPushButton>("&Plot", this);
    btnDelLast_ = std::make_unique<QPushButton>("&Delete Last Isocline", this);
    btnDelAll_ = std::make_unique<QPushButton>("Delete &All Isoclines", this);

#ifdef TOOLTIPS
    edt_value_->setToolTip(
        "Value of isoclines to plot.\nCan be 0 for 0-slope "
        "isoclines, and \"inf\"\n(without quotes, case "
        "insensitive) for\ninfinite-slope isoclines.");
    btnEvaluate_->setToolTip("Evaluate isoclines at the selected value.");
    btnPlot_->setToolTip("Plot isocline.");
    btnDelLast_->setToolTip("Delete last isocline drawn");
    btnDelAll_->setToolTip("Delete all isoclines (separatrices remain)");
    QString ttip;
    ttip = QString::fromStdString("Number of points. Must be between " +
                                  std::to_string(MIN_CURVEPOINTS) + " and " +
                                  std::to_string(MAX_CURVEPOINTS));
    edt_points_->setToolTip(ttip);
    ttip = QString::fromStdString("Required precision. Must be between " +
                                  std::to_string(MIN_CURVEPRECIS) + " and " +
                                  std::to_string(MAX_CURVEPRECIS));
    edt_precis_->setToolTip(ttip);
    ttip = QString::fromStdString(
        "Maximum amount of memory (in kilobytes) "
        "spent on plotting GCF.\nMust be between " +
        std::to_string(MIN_CURVEMEMORY) + " and " +
        std::to_string(MAX_CURVEMEMORY));
    edt_memory_->setToolTip(ttip);
#endif

    // layout
    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    std::unique_ptr<QHBoxLayout> layout0 = std::make_unique<QHBoxLayout>();
    layout0->addWidget(lbl1);
    layout0->addWidget(btn_dots_);
    layout0->addWidget(btn_dashes_);

    std::unique_ptr<QGridLayout> layout1{std::make_unique<QGridLayout>()};
    layout1->addWidget(lbl0, 0, 0);
    layout1->addWidget(edt_value_, 0, 1);
    layout1->addWidget(lbl2, 1, 0);
    layout1->addWidget(edt_points_, 1, 1);
    layout1->addWidget(lbl3, 2, 0);
    layout1->addWidget(edt_precis_, 2, 1);
    layout1->addWidget(lbl4, 3, 0);
    layout1->addWidget(edt_memory_, 3, 1);

    std::unique_ptr<QHBoxLayout> layout2{std::make_unique<QHBoxLayout>()};
    layout2->addWidget(btnEvaluate_);
    layout2->addStretch(0);
    layout2->addWidget(btnDelLast_);

    std::unique_ptr<QHBoxLayout> layout3{std::make_unique<QHBoxLayout>()};
    layout3->addWidget(btnPlot_);
    layout3->addStretch(0);
    layout3->addWidget(btnDelAll_);

    mainLayout_->addLayout(layout0);
    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections
    connect(btnEvaluate_, &QPushButton::clicked, this,
            &P4IsoclinesDlg::onBtnEvaluate);
    connect(btnPlot_, &QPushButton::clicked, this, &P4IsoclinesDlg::onBtnPlot);
    connect(btnDelAll_, &QPushButton::clicked, this,
            &P4IsoclinesDlg::onBtnDelAll);
    connect(btnDelLast_, &QPushButton::clicked, this,
            &P4IsoclinesDlg::onBtnDelLast);

    // finishing

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (!gVFResults.isocline_vector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }

    setP4WindowTitle(this, "Plot Isoclines");
}

void P4IsoclinesDlg::onBtnEvaluate(void)
{
    bool ok;
    if (edt_value_->text().isNull() && edt_value_->text().isEmpty()) {
        QMessageBox::information(
            this, "P4",
            "The value field has to be filled with a valid number.");
        return;
    } else {
        QString val = edt_value_->text();
        gThisVF.getDataFromDlg();
        val = gThisVF.convertMapleUserParametersLabelsToValues(val);
        val.toDouble(&ok);
        if (!ok) {
            QMessageBox::information(
                this, "P4",
                "The value field has to be filled with a valid number.\n(P4 "
                "does not parse mathematical expressions, so\nmake sure to "
                "enter a value and not a string of\noperations)");
            return;
        }
    }
    if ((gThisVF.xdot_ == "0" || gThisVF.xdot_.isEmpty()) &&
        (gThisVF.ydot_ == "0" || gThisVF.ydot_.isEmpty())) {
        QMessageBox::information(this, "P4",
                                 "Check that the vector field is "
                                 "correctly introduced.\nIf you "
                                 "used an input file, make sure "
                                 "you pressed\nthe Load button.");
        return;
    }
    if (edt_value_->text().trimmed() == "0") {
        gThisVF.isoclines_ = gThisVF.ydot_;
    } else if (edt_value_->text().trimmed().toLower() == "inf") {
        gThisVF.isoclines_ = gThisVF.xdot_;
    } else {
        gThisVF.isoclines_ = "(" + gThisVF.ydot_ + ")-(" +
                              edt_value_->text().trimmed() + ")*(" +
                              gThisVF.xdot_ + ")";
    }

    // FIRST: create filename_vecisoclines.tab for transforming the isoclines
    // QString to a list of P4POLYNOM2
    gThisVF.setIsoclinesDlg(this);
    gThisVF.evaluateIsoclinesTable();
    btnPlot_->setEnabled(true);
    plotwnd_->getDlgData();
}

void P4IsoclinesDlg::onBtnPlot(void)
{
    bool dashes, result;
    int points, precis, memory;

    bool ok;
    QString buf;

    dashes = btn_dashes_->isChecked();

    ok = true;

    buf = edt_points_->text();
    points = buf.toInt();
    if (points < MIN_CURVEPOINTS || points > MAX_CURVEPOINTS) {
        buf += " ???";
        edt_points_->setText(buf);
        ok = false;
    }

    buf = edt_precis_->text();
    precis = buf.toInt();
    if (precis < MIN_CURVEPRECIS || precis > MAX_CURVEPRECIS) {
        buf += " ???";
        edt_precis_->setText(buf);
        ok = false;
    }

    buf = edt_memory_->text();
    memory = buf.toInt();
    if (memory < MIN_CURVEMEMORY || memory > MAX_CURVEMEMORY) {
        buf += " ???";
        edt_memory_->setText(buf);
        ok = false;
    }

    if (!ok) {
        QMessageBox::information(
            this, "P4",
            "One of the fields has a value that is out of bounds.\n"
            "Please correct before continuing.\n");
        return;
    }

    // SECOND: read the resulting file and store the list
    if (!gVFResults.readIsoclines(gThisVF.getbarefilename())) {
        QMessageBox::critical(this, "P4",
                              "Cannot read isoclines.\n"
                              "Please check the input field!\n");
        return;
    }
    // THIRD: evaluate isoclines with given parameters {dashes, points, memory}.

    evaluating_points_ = points;
    evaluating_memory_ = memory;
    evaluating_precision_ = precis;

    btnPlot_->setEnabled(false);

    gThisVF.setIsoclinesDlg(this);

    result = evalIsoclinesStart(mainSphere_, dashes, precis, points);
    if (!result) {
        btnPlot_->setEnabled(true);
        QMessageBox::critical(this, "P4",
                              "An error occured while plotting the "
                              "isoclines.\nThe singular locus may not "
                              "be visible, or may be partially "
                              "visible.");
        return;
    }

    btnDelAll_->setEnabled(true);
    btnDelLast_->setEnabled(true);
}

void P4IsoclinesDlg::onBtnDelAll(void)
{
    plotwnd_->getDlgData();

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);
    btnDelAll_->setEnabled(false);
    btnDelLast_->setEnabled(false);

    gVFResults.isocline_vector_.clear();

    mainSphere_->refresh();
}

void P4IsoclinesDlg::onBtnDelLast(void)
{
    plotwnd_->getDlgData();

    deleteLastIsocline(mainSphere_);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (gVFResults.isocline_vector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
}

void P4IsoclinesDlg::reset(void)
{
    QString buf;

    edt_value_->setText("");

    buf.sprintf("%d", DEFAULT_CURVEPOINTS);
    edt_points_->setText(buf);

    buf.sprintf("%d", DEFAULT_CURVEMEMORY);
    edt_memory_->setText(buf);

    buf.sprintf("%d", DEFAULT_CURVEPRECIS);
    edt_precis_->setText(buf);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    // if (gVFResults.first_isoclines_ != nullptr) {
    if (!gVFResults.isocline_vector_.empty()) {
        btnDelLast_->setEnabled(true);
        btnDelAll_->setEnabled(true);
    }

    if (gVFResults.config_dashes_)
        btn_dashes_->toggle();
    else
        btn_dots_->toggle();
}

void P4IsoclinesDlg::finishIsoclinesEvaluation()
{
    bool result;

    if (btnPlot_->isEnabled() == true)
        return;  // not busy??

    result = evalIsoclinesContinue(evaluating_precision_, evaluating_points_);

    if (result) {
        btnPlot_->setEnabled(false);
        result =
            evalIsoclinesFinish();  // return false in case an error occured
        if (!result) {
            QMessageBox::critical(this, "P4",
                                  "An error occured while plotting "
                                  "the isoclines.\nThe singular locus "
                                  "may not be visible, or may "
                                  "be partially visible.");
        }
    }
}