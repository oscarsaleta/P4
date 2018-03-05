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

#include "P4ArbitraryCurveDlg.hpp"

#include "P4InputVF.hpp"
#include "P4PlotWnd.hpp"
#include "P4WinSphere.hpp"
#include "custom.hpp"
#include "main.hpp"
#include "math_arbitrarycurve.hpp"
#include "math_polynom.hpp"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

P4ArbitraryCurveDlg::P4ArbitraryCurveDlg(P4PlotWnd *plt, P4WinSphere *sp)
    : QWidget{nullptr, Qt::Tool | Qt::WindowStaysOnTopHint},
      mainSphere_{sp}, plotwnd_{plt}
{
    edt_curve_ = std::make_unique<QLineEdit>("", this);
    auto lbl0 = std::make_unique<QLabel>("Curve: ", this);

    auto btngrp = std::make_unique<QButtonGroup>(this);
    btn_dots_ = std::make_unique<QRadioButton>("Dots", this);
    btn_dashes_ = std::make_unique<QRadioButton>("Dashes", this);
    btngrp->addButton(btn_dots_.get());
    btngrp->addButton(btn_dashes_.get());
    auto lbl1 = std::make_unique<QLabel>("Appearance: ", this);

    edt_points_ = std::make_unique<QLineEdit>("", this);
    auto lbl2 = std::make_unique<QLabel>("Num. Points: ", this);

    edt_precis_ = std::make_unique<QLineEdit>("", this);
    auto lbl3 = std::make_unique<QLabel>("Precision: ", this);

    edt_memory_ = std::make_unique<QLineEdit>("", this);
    auto lbl4 = std::make_unique<QLabel>("Max. Memory: ", this);

    btnEvaluate_ = std::make_unique<QPushButton>("Evaluate", this);
    btnPlot_ = std::make_unique<QPushButton>("&Plot", this);
    btnDelLast_ = std::make_unique<QPushButton>("&Delete Last Curve", this);
    btnDelAll_ = std::make_unique<QPushButton>("Delete &All Curves", this);

#ifdef TOOLTIPS
    btn_dots_->setToolTip("Plot individual points of the curve");
    btn_dashes_->setToolTip(
        "Connect points of the curve with small line segments");
    edt_points_->setToolTip("Number of points");
    btnEvaluate_->setToolTip("Evaluate singular points of plynomial curve");
    btnPlot_->setToolTip("Plot curve (using symbolic manipulator)");
    btnDelAll_->setToolTip("Delete all curves");
    QString ttip;
    ttip.sprintf("Number of points. Must be between %d and %d.",
                 MIN_CURVEPOINTS, MAX_CURVEPOINTS);
    edt_points_->setToolTip(ttip);
    ttip.sprintf("Required precision. Must be between %d and %d.",
                 MIN_CURVEPRECIS, MAX_CURVEPRECIS);
    edt_precis_->setToolTip(ttip);
    ttip.sprintf(
        "Maximum amount of memory (in kilobytes) spent on plotting GCF.\n"
        "Must be between %d and %d.",
        MIN_CURVEMEMORY, MAX_CURVEMEMORY);
    edt_memory_->setToolTip(ttip);
#endif

    // layout
    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    auto layout0 = std::make_unique<QHBoxLayout>();
    layout0->addWidget(lbl1.get());
    layout0->addWidget(btn_dots_.get());
    layout0->addWidget(btn_dashes_.get());

    auto layout1 = std::make_unique<QGridLayout>();
    layout1->addWidget(lbl0.get(), 0, 0);
    layout1->addWidget(edt_curve_.get(), 0, 1);
    layout1->addWidget(lbl2.get(), 1, 0);
    layout1->addWidget(edt_points_.get(), 1, 1);
    layout1->addWidget(lbl3.get(), 2, 0);
    layout1->addWidget(edt_precis_.get(), 2, 1);
    layout1->addWidget(lbl4.get(), 3, 0);
    layout1->addWidget(edt_memory_.get(), 3, 1);

    auto layout2 = std::make_unique<QHBoxLayout>();
    layout2->addWidget(btnEvaluate_.get());
    layout2->addStretch(0);
    layout2->addWidget(btnDelLast_.get());

    auto layout3 = std::make_unique<QHBoxLayout>();
    layout3->addWidget(btnPlot_.get());
    layout3->addStretch(0);
    layout3->addWidget(btnDelAll_.get());

    mainLayout_->addLayout(layout0.get());
    mainLayout_->addLayout(layout1.get());
    mainLayout_->addLayout(layout2.get());
    mainLayout_->addLayout(layout3.get());

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_.get());

    // connections

    QObject::connect(btnEvaluate_.get(), &QPushButton::clicked, this,
                     &P4ArbitraryCurveDlg::onBtnEvaluate);
    QObject::connect(btnPlot_.get(), &QPushButton::clicked, this,
                     &P4ArbitraryCurveDlg::onBtnPlot);
    QObject::connect(btnDelLast_.get(), &QPushButton::clicked, this,
                     &P4ArbitraryCurveDlg::onBtnDelLast);
    QObject::connect(btnDelAll_.get(), &QPushButton::clicked, this,
                     &P4ArbitraryCurveDlg::onBtnDelAll);

    // finishing

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (gVFResults.arbitraryCurveVector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }

    setP4WindowTitle(this, "Curve plot");
}

void P4ArbitraryCurveDlg::reset()
{
    edt_curve_->setText("");
    edt_points_->setText(QString::number(DEFAULT_CURVEPOINTS));
    edt_memory_->setText(QString::number(DEFAULT_CURVEMEMORY));
    edt_precis_->setText(QString::number(DEFAULT_CURVEPRECIS));

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (!gVFResults.arbitraryCurveVector_.empty()) {
        btnDelAll_->setEnabled(true);
        btnDelLast_->setEnabled(true);
    }

    if (gVFResults.config_dashes_)
        btn_dashes_->toggle();
    else
        btn_dots_->toggle();
}

void P4ArbitraryCurveDlg::onBtnEvaluate()
{
    if (edt_curve_->text().isNull() || edt_curve_->text().isEmpty()) {
        QMessageBox::information(this, "P4",
                                 "The curve field has to be filled\n"
                                 "with the equation of a curve.\n");
        return;
    }
    gThisVF.arbitraryCurve_ = edt_curve_->text().trimmed();

    // FIRST: create filename_veccurve.tab for transforming the curve QString to
    // a list of p4polynom::term2
    gThisVF.setArbitraryCurveDlg(this);
    gThisVF.evaluateCurveTable();
    btnPlot_->setEnabled(true);
}

void P4ArbitraryCurveDlg::onBtnPlot()
{
    int points, precis, memory;
    bool convertok;
    bool dashes{btn_dashes_->isChecked()};
    bool ok{true};

    QString buf{edt_points_->text()};
    points = buf.toInt(&convertok);
    if (!convertok || points < MIN_CURVEPOINTS || points > MAX_CURVEPOINTS) {
        buf += " ???";
        edt_points_->setText(buf);
        ok = false;
    }

    buf = edt_precis_->text();
    precis = buf.toInt(&convertok);
    if (!convertok || precis < MIN_CURVEPRECIS || precis > MAX_CURVEPRECIS) {
        buf += " ???";
        edt_precis_->setText(buf);
        ok = false;
    }

    buf = edt_memory_->text();
    memory = buf.toInt(&convertok);
    if (!convertok || memory < MIN_CURVEMEMORY || memory > MAX_CURVEMEMORY) {
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
    if (!gVFResults.readArbitraryCurve(gThisVF.getbarefilename())) {
        QMessageBox::critical(this, "P4",
                              "Cannot read curve.\n"
                              "Please check the input field!\n");
        return;
    }

    // THIRD: evaluate curve with given parameters {dashes, points, memory}.

    evaluating_points_ = points;
    evaluating_memory_ = memory;
    evaluating_precision_ = precis;

    btnPlot_->setEnabled(false);

    gThisVF.setArbitraryCurveDlg(this);
    if (!evalArbitraryCurveStart(mainSphere_, dashes, precis, points);) {
        btnPlot_->setEnabled(true);
        QMessageBox::critical(this, "P4",
                              "An error occured while plotting the "
                              "curve.\nThe singular locus may not "
                              "be visible, or may be partially "
                              "visible.");
        return;
    }

    btnDelAll_->setEnabled(true);
    btnDelLast_->setEnabled(true);
}

void P4ArbitraryCurveDlg::onBtnDelAll()
{
    plotwnd_->getDlgData();

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);
    btnDelAll_->setEnabled(false);
    btnDelLast_->setEnabled(false);

    gVFResults.arbitraryCurveVector_.clear();

    mainSphere_->refresh();
}

void P4ArbitraryCurveDlg::onBtnDelLast()
{
    plotwnd_->getDlgData();

    deleteLastArbitraryCurve(mainSphere_);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (gVFResults.arbitraryCurveVector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
}

void P4ArbitraryCurveDlg::finishCurveEvaluation()
{
    if (btnPlot_->isEnabled() == true)
        return;  // not busy??

    if (evalCurveContinue(evaluating_precision_, evaluating_points_)) {
        btnPlot_->setEnabled(false);
        if (!evalCurveFinish()) {
            QMessageBox::critical(this, "P4",
                                  "An error occured while plotting "
                                  "the curve.\nThe singular locus "
                                  "may not be visible, or may "
                                  "be partially visible.");
        }
    }
}
