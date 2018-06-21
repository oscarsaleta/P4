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

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4PlotWnd.hpp"
#include "P4Sphere.hpp"
#include "custom.hpp"
#include "main.hpp"
#include "math_arbitrarycurve.hpp"
#include "math_polynom.hpp"

P4ArbitraryCurveDlg::P4ArbitraryCurveDlg(P4PlotWnd *plt, P4Sphere *sp)
    : QWidget{nullptr}, mainSphere_{sp}, plotwnd_{plt}
{
    edt_curve_ = new QLineEdit{"", this};
    auto lbl0 = new QLabel{"Curve: ", this};

    auto btngrp = new QButtonGroup{this};
    btn_dots_ = new QRadioButton{"Dots", this};
    btn_dashes_ = new QRadioButton{"Dashes", this};
    btngrp->addButton(btn_dots_);
    btngrp->addButton(btn_dashes_);
    auto lbl1 = new QLabel{"Appearance: ", this};

    edt_points_ = new QSpinBox{this};
    edt_points_->setRange(MIN_CURVEPOINTS, MAX_CURVEPOINTS);
    auto lbl2 = new QLabel{"Num. Points: ", this};

    edt_precis_ = new QSpinBox{this};
    edt_precis_->setRange(MIN_CURVEPRECIS, MAX_CURVEPRECIS);
    auto lbl3 = new QLabel{"Precision: ", this};

    edt_memory_ = new QSpinBox{this};
    edt_memory_->setRange(MIN_CURVEMEMORY, MAX_CURVEMEMORY);
    auto lbl4 = new QLabel{"Max. Memory: ", this};

    btnEvaluate_ = new QPushButton{"Evaluate", this};
    btnPlot_ = new QPushButton{"&Plot", this};
    btnDelLast_ = new QPushButton{"&Delete Last Curve", this};
    btnDelAll_ = new QPushButton{"Delete &All Curves", this};

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
    mainLayout_ = new QBoxLayout{QBoxLayout::TopToBottom, this};

    auto layout0 = new QHBoxLayout{};
    layout0->addWidget(lbl1);
    layout0->addWidget(btn_dots_);
    layout0->addWidget(btn_dashes_);

    auto layout1 = new QGridLayout{};
    layout1->addWidget(lbl0, 0, 0);
    layout1->addWidget(edt_curve_, 0, 1);
    layout1->addWidget(lbl2, 1, 0);
    layout1->addWidget(edt_points_, 1, 1);
    layout1->addWidget(lbl3, 2, 0);
    layout1->addWidget(edt_precis_, 2, 1);
    layout1->addWidget(lbl4, 3, 0);
    layout1->addWidget(edt_memory_, 3, 1);

    auto layout2 = new QHBoxLayout{};
    layout2->addWidget(btnEvaluate_);
    layout2->addStretch(0);
    layout2->addWidget(btnDelLast_);

    auto layout3 = new QHBoxLayout{};
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

    QObject::connect(btnEvaluate_, &QPushButton::clicked, this,
                     &P4ArbitraryCurveDlg::onBtnEvaluate);
    QObject::connect(btnPlot_, &QPushButton::clicked, this,
                     &P4ArbitraryCurveDlg::onBtnPlot);
    QObject::connect(btnDelLast_, &QPushButton::clicked, this,
                     &P4ArbitraryCurveDlg::onBtnDelLast);
    QObject::connect(btnDelAll_, &QPushButton::clicked, this,
                     &P4ArbitraryCurveDlg::onBtnDelAll);

    // finishing

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (gVFResults.arbitraryCurves_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }

    setP4WindowTitle(this, "Curve plot");
    reset();
}

void P4ArbitraryCurveDlg::reset()
{
    edt_curve_->setText("");
    edt_points_->setValue(DEFAULT_CURVEPOINTS);
    edt_memory_->setValue(DEFAULT_CURVEMEMORY);
    edt_precis_->setValue(DEFAULT_CURVEPRECIS);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (!gVFResults.arbitraryCurves_.empty()) {
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
    gThisVF->arbitraryCurve_ = edt_curve_->text().trimmed();

    // FIRST: create filename_veccurve.tab for transforming the curve QString to
    // a list of p4polynom::term2
    gThisVF->setArbitraryCurveDlg(this);
    gThisVF->evaluateArbitraryCurveTable();
    btnPlot_->setEnabled(true);
}

void P4ArbitraryCurveDlg::onBtnPlot()
{
    int points, precis, memory;
    bool dashes{btn_dashes_->isChecked()};

    points = edt_points_->value();
    precis = edt_precis_->value();
    memory = edt_memory_->value();

    // SECOND: read the resulting file and store the list
    if (!gVFResults.readArbitraryCurve(gThisVF->getbarefilename())) {
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

    gThisVF->setArbitraryCurveDlg(this);
    if (!evalArbitraryCurveStart(mainSphere_, dashes, precis, points)) {
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

    gVFResults.arbitraryCurves_.clear();

    mainSphere_->refresh();
}

void P4ArbitraryCurveDlg::onBtnDelLast()
{
    plotwnd_->getDlgData();

    deleteLastArbitraryCurve(mainSphere_);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (gVFResults.arbitraryCurves_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
}

void P4ArbitraryCurveDlg::finishArbitraryCurveEvaluation()
{
    if (btnPlot_->isEnabled() == true)
        return; // not busy??

    if (evalArbitraryCurveContinue(evaluating_precision_, evaluating_points_)) {
        btnPlot_->setEnabled(false);
        if (!evalArbitraryCurveFinish()) {
            QMessageBox::critical(this, "P4",
                                  "An error occured while plotting "
                                  "the curve.\nThe singular locus "
                                  "may not be visible, or may "
                                  "be partially visible.");
        }
    }
}
