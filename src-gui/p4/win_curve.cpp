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

#include "win_curve.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "math_curve.h"
#include "math_polynom.h"
#include "P4PlotWnd.h"
#include "win_sphere.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

QCurveDlg::QCurveDlg(P4PlotWnd *plt, QWinSphere *sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint), mainSphere_(sp),
      plotwnd_(plt)
{
    edt_curve_ = new QLineEdit("", this);
    QLabel *lbl0 = new QLabel("Curve: ", this);

    QButtonGroup *btngrp = new QButtonGroup(this);
    btn_dots_ = new QRadioButton("Dots", this);
    btn_dashes_ = new QRadioButton("Dashes", this);
    btngrp->addButton(btn_dots_);
    btngrp->addButton(btn_dashes_);
    QLabel *lbl1 = new QLabel("Appearance: ", this);

    edt_points_ = new QLineEdit("", this);
    QLabel *lbl2 = new QLabel("Num. Points: ", this);

    edt_precis_ = new QLineEdit("", this);
    QLabel *lbl3 = new QLabel("Precision: ", this);

    edt_memory_ = new QLineEdit("", this);
    QLabel *lbl4 = new QLabel("Max. Memory: ", this);

    btnEvaluate_ = new QPushButton("Evaluate", this);
    btnPlot_ = new QPushButton("&Plot", this);
    btnDelLast_ = new QPushButton("&Delete Last Curve", this);
    btnDelAll_ = new QPushButton("Delete &All Curves", this);

#ifdef TOOLTIPS
    btn_dots_->setToolTip("Plot individual points of the curve");
    btn_dashes_->setToolTip(
        "Connect points of the curve with small line segments");
    edt_points_->setToolTip("Number of points");
    btnEvaluate_->setToolTip("Evaluate singular points of plynomial curve");
    btnPlot_->setToolTip("Plot curve (using symbolic manipulator)");
    btnDelAll_->setToolTip("Delete all curves");
    QString ttip;
    ttip = QString::fromStdString("Number of points. Must be between " +
                                  std::to_string(MIN_CURVEPOINTS) + " and " +
                                  std::to_string(MAX_CURVEPOINTS));
    edt_points_->setToolTip(ttip);
    ttip = QString::fromStdString("Required precision. Must be between " +
                                  std::to_string(MIN_CURVEPRECIS) + " and " +
                                  std::to_string(MAX_CURVEPRECIS));
    edt_precis_->setToolTip(ttip);
    ttip = QString::fromStdString("Maximum amount of memory (in kilobytes) "
                                  "spent on plotting GCF.\nMust be between " +
                                  std::to_string(MIN_CURVEMEMORY) + " and " +
                                  std::to_string(MAX_CURVEMEMORY));
    edt_memory_->setToolTip(ttip);
#endif

    // layout
    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *layout0 = new QHBoxLayout();
    layout0->addWidget(lbl1);
    layout0->addWidget(btn_dots_);
    layout0->addWidget(btn_dashes_);

    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(lbl0, 0, 0);
    layout1->addWidget(edt_curve_, 0, 1);
    layout1->addWidget(lbl2, 1, 0);
    layout1->addWidget(edt_points_, 1, 1);
    layout1->addWidget(lbl3, 2, 0);
    layout1->addWidget(edt_precis_, 2, 1);
    layout1->addWidget(lbl4, 3, 0);
    layout1->addWidget(edt_memory_, 3, 1);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(btnEvaluate_);
    layout2->addStretch(0);
    layout2->addWidget(btnDelLast_);

    QHBoxLayout *layout3 = new QHBoxLayout();
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
            &QCurveDlg::onBtnEvaluate);
    connect(btnPlot_, &QPushButton::clicked, this, &QCurveDlg::onBtnPlot);
    connect(btnDelLast_, &QPushButton::clicked, this, &QCurveDlg::onBtnDelLast);
    connect(btnDelAll_, &QPushButton::clicked, this, &QCurveDlg::onBtnDelAll);

    // finishing

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (g_VFResults.curve_vector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }

    setP4WindowTitle(this, "Curve plot");
}

void QCurveDlg::reset()
{
    QString buf;

    edt_curve_->setText("");

    buf.sprintf("%d", DEFAULT_CURVEPOINTS);
    edt_points_->setText(buf);

    buf.sprintf("%d", DEFAULT_CURVEMEMORY);
    edt_memory_->setText(buf);

    buf.sprintf("%d", DEFAULT_CURVEPRECIS);
    edt_precis_->setText(buf);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (!g_VFResults.curve_vector_.empty()) {
        btnDelAll_->setEnabled(true);
        btnDelLast_->setEnabled(true);
    }

    if (g_VFResults.config_dashes_)
        btn_dashes_->toggle();
    else
        btn_dots_->toggle();
}

void QCurveDlg::onBtnEvaluate()
{
    if (edt_curve_->text().isNull() || edt_curve_->text().isEmpty()) {
        QMessageBox::information(this, "P4",
                                 "The curve field has to be filled\n"
                                 "with the equation of a curve.\n");
        return;
    }
    g_ThisVF->curve_ = edt_curve_->text().trimmed();

    // FIRST: create filename_veccurve.tab for transforming the curve QString to
    // a list of P4POLYNOM2
    g_ThisVF->curveDlg_ = this;
    g_ThisVF->evaluateCurveTable();
    btnPlot_->setEnabled(true);
}

void QCurveDlg::onBtnPlot()
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
            this, "P4", "One of the fields has a value that is out of bounds.\n"
                        "Please correct before continuing.\n");
        return;
    }

    // SECOND: read the resulting file and store the list
    if (!g_VFResults.readCurve(g_ThisVF->getbarefilename())) {
        QMessageBox::critical(this, "P4", "Cannot read curve.\n"
                                          "Please check the input field!\n");
        return;
    }

    // THIRD: evaluate curve with given parameters {dashes, points, memory}.

    evaluating_points_ = points;
    evaluating_memory_ = memory;
    evaluating_precision_ = precis;

    btnPlot_->setEnabled(false);

    g_ThisVF->curveDlg_ = this;
    result = evalCurveStart(mainSphere_, dashes, precis, points);
    if (!result) {
        btnPlot_->setEnabled(true);
        QMessageBox::critical(this, "P4", "An error occured while plotting the "
                                          "curve.\nThe singular locus may not "
                                          "be visible, or may be partially "
                                          "visible.");
        return;
    }

    btnDelAll_->setEnabled(true);
    btnDelLast_->setEnabled(true);
}

void QCurveDlg::onBtnDelAll()
{
    plotwnd_->getDlgData();

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);
    btnDelAll_->setEnabled(false);
    btnDelLast_->setEnabled(false);

    g_VFResults.curve_vector_.clear();

    mainSphere_->refresh();
}

void QCurveDlg::onBtnDelLast()
{
    plotwnd_->getDlgData();

    deleteLastCurve(mainSphere_);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (g_VFResults.curve_vector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
}

void QCurveDlg::finishCurveEvaluation()
{
    bool result;

    if (btnPlot_->isEnabled() == true)
        return; // not busy??

    result = evalCurveContinue(evaluating_precision_, evaluating_points_);

    if (result) {
        btnPlot_->setEnabled(false);
        result = evalCurveFinish(); // return false in case an error occured
        if (!result) {
            QMessageBox::critical(this, "P4", "An error occured while plotting "
                                              "the curve.\nThe singular locus "
                                              "may not be visible, or may "
                                              "be partially visible.");
        }
    }
}
