/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
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
#include "math_curve.h"
#include "math_polynom.h"

#include <QButtonGroup>
#include <QMessageBox>

QCurveDlg::QCurveDlg(QPlotWnd *plt, QWinSphere *sp)
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

    btn_evaluate_ = new QPushButton("Evaluate", this);
    btn_plot_ = new QPushButton("Plot", this);
    btn_plot_->setEnabled(false);
    btn_delete_ = new QPushButton("Delete", this);
    btn_delete_->setEnabled(false);

#ifdef TOOLTIPS
    btn_dots_->setToolTip(
        "Plot individual points of the curve of singularities");
    btn_dashes_->setToolTip("Connect points of the curve of singularities with "
                            "small line segments");
    edt_points_->setToolTip("Number of points");
    edt_precis_->setToolTip("Required precision");
    edt_memory_->setToolTip(
        "Maximum amount of memory (in kilobytes) spent on plotting the curve");
    btn_evaluate_->setToolTip("Evaluate singular points of plynomial curve");
    btn_plot_->setToolTip("Plot curve (using symbolic manipulator)");
    btn_delete_->setToolTip("Delete curve");
#endif

    // layout

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(lbl1);
    layout1->addWidget(btn_dots_);
    layout1->addWidget(btn_dashes_);

    QGridLayout *lay00 = new QGridLayout();
    lay00->addWidget(lbl0, 0, 0);
    lay00->addWidget(edt_curve_, 0, 1);
    lay00->addWidget(lbl2, 1, 0);
    lay00->addWidget(edt_points_, 1, 1);
    lay00->addWidget(lbl3, 2, 0);
    lay00->addWidget(edt_precis_, 2, 1);
    lay00->addWidget(lbl4, 3, 0);
    lay00->addWidget(edt_memory_, 3, 1);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addStretch(0);
    layout2->addWidget(btn_evaluate_);
    layout2->addWidget(btn_plot_);
    layout2->addWidget(btn_delete_);
    layout2->addStretch(0);

    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(lay00);
    mainLayout_->addLayout(layout2);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections

    QObject::connect(btn_evaluate_, SIGNAL(clicked()), this,
                     SLOT(onbtn_evaluate()));
    QObject::connect(btn_plot_, SIGNAL(clicked()), this, SLOT(onbtn_plot()));
    QObject::connect(btn_delete_, SIGNAL(clicked()), this,
                     SLOT(onbtn_delete()));

    // finishing

    setP4WindowTitle(this, "Curve plot");
}

void QCurveDlg::reset(void)
{
    QString buf;

    edt_curve_->setText("");

    buf.sprintf("%d", DEFAULT_CURVEPOINTS);
    edt_points_->setText(buf);

    buf.sprintf("%d", DEFAULT_CURVEMEMORY);
    edt_memory_->setText(buf);

    buf.sprintf("%d", DEFAULT_CURVEPRECIS);
    edt_precis_->setText(buf);

    btn_evaluate_->setEnabled(true);
    btn_plot_->setEnabled(false);
    if (g_VFResults.gcf_ != nullptr)
        btn_delete_->setEnabled(false);
    else
        btn_delete_->setEnabled(true);

    if (g_VFResults.config_dashes_)
        btn_dashes_->toggle();
    else
        btn_dots_->toggle();
}

void QCurveDlg::onbtn_evaluate(void)
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
    g_ThisVF->evaluateCurveTable();
    btn_plot_->setEnabled(true);
}

void QCurveDlg::onbtn_plot(void)
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

    btn_plot_->setEnabled(false);

    g_ThisVF->curveDlg_ = this;
    result = evalCurveStart(mainSphere_, dashes, precis, points);
    if (!result) {
        btn_plot_->setEnabled(true);
        QMessageBox::critical(this, "P4", "An error occured while plotting the "
                                          "curve.\nThe singular locus may not "
                                          "be visible, or may be partially "
                                          "visible.");
        return;
    }

    btn_delete_->setEnabled(true);
}

void QCurveDlg::onbtn_delete(void)
{
    delete_term2(g_VFResults.curve_);
    delete_term2(g_VFResults.curve_U1_);
    delete_term2(g_VFResults.curve_U2_);
    delete_term2(g_VFResults.curve_V1_);
    delete_term2(g_VFResults.curve_V2_);
    delete_term3(g_VFResults.curve_C_);
    g_VFResults.deleteOrbitPoint(g_VFResults.curve_points_);

    g_VFResults.curve_ = nullptr;
    g_VFResults.curve_U1_ = nullptr;
    g_VFResults.curve_U2_ = nullptr;
    g_VFResults.curve_V1_ = nullptr;
    g_VFResults.curve_V2_ = nullptr;
    g_VFResults.curve_C_ = nullptr;
    g_VFResults.curve_points_ = nullptr;

    mainSphere_->refresh();
    btn_delete_->setEnabled(false);
    btn_plot_->setEnabled(false);
}

void QCurveDlg::finishCurveEvaluation(void)
{
    bool result;

    if (btn_plot_->isEnabled() == true)
        return; // not busy??

    result = evalCurveContinue(evaluating_precision_, evaluating_points_);

    if (result) {
        btn_plot_->setEnabled(false);
        result = evalCurveFinish(); // return false in case an error occured
        if (!result) {
            QMessageBox::critical(this, "P4", "An error occured while plotting "
                                              "the curve.\nThe singular locus "
                                              "may not be visible, or may "
                                              "be partially visible.");
        }
    }
}
