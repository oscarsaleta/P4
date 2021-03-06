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

#include <algorithm>
#include <iterator>
#include <memory>

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include "P4ParentStudy.hpp"
#include "P4PlotWnd.hpp"
#include "P4Sphere.hpp"
#include "main.hpp"
#include "math_isoclines.hpp"

P4IsoclinesDlg::P4IsoclinesDlg(P4PlotWnd *plt, P4Sphere *sp)
    : QWidget{nullptr, Qt::Tool | Qt::WindowStaysOnTopHint},
      mainSphere_{sp}, plotwnd_{plt}
{
    edt_vfselect_ = new QLineEdit{"", this};
    auto lblvf = new QLabel{"Vector field:"};
    lblvf->setBuddy(edt_vfselect_);

    edt_value_ = new QLineEdit{"", this};
    auto lbl0 = new QLabel{"&Value = ", this};
    lbl0->setBuddy(edt_value_);

    auto btngrp = new QButtonGroup{this};
    btn_dots_ = new QRadioButton{"Dots", this};
    btn_dashes_ = new QRadioButton{"Dashes", this};
    btngrp->addButton(btn_dots_);
    btngrp->addButton(btn_dashes_);
    auto lbl1 = new QLabel{"Appearance: ", this};

    edt_points_ = new QLineEdit{"", this};
    auto lbl2 = new QLabel{"Num. Points: ", this};
    lbl2->setBuddy(edt_points_);

    edt_precis_ = new QLineEdit{"", this};
    auto lbl3 = new QLabel{"Precision: ", this};
    lbl3->setBuddy(edt_precis_);

    edt_memory_ = new QLineEdit{"", this};
    auto lbl4 = new QLabel{"Max. Memory: ", this};
    lbl4->setBuddy(edt_memory_);

    btnEvaluate_ = new QPushButton{"&Evaluate", this};
    btnPlot_ = new QPushButton{"&Plot", this};
    btnDelLast_ = new QPushButton{"&Delete Last Isocline", this};
    btnDelAll_ = new QPushButton{"Delete &All Isoclines", this};

#ifdef TOOLTIPS
    edt_vfselect_->setToolTip("Vector field selector. If there is only one "
                              "vector field this value is ignored.");
    edt_value_->setToolTip("Value of isoclines to plot.\nCan be 0 for 0-slope "
                           "isoclines, and \"inf\"\n(without quotes, case "
                           "insensitive) for\ninfinite-slope isoclines.");
    btnEvaluate_->setToolTip("Evaluate isoclines at the selected value.");
    btnPlot_->setToolTip("Plot isocline.");
    btnDelLast_->setToolTip("Delete last isocline drawn");
    btnDelAll_->setToolTip("Delete all isoclines (separatrices remain)");
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
    layout1->addWidget(lblvf, 0, 0);
    layout1->addWidget(edt_vfselect_, 0, 1);
    layout1->addWidget(lbl0, 1, 0);
    layout1->addWidget(edt_value_, 1, 1);
    layout1->addWidget(lbl2, 2, 0);
    layout1->addWidget(edt_points_, 2, 1);
    layout1->addWidget(lbl3, 3, 0);
    layout1->addWidget(edt_precis_, 3, 1);
    layout1->addWidget(lbl4, 4, 0);
    layout1->addWidget(edt_memory_, 4, 1);

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
                     &P4IsoclinesDlg::onBtnEvaluate);
    QObject::connect(btnPlot_, &QPushButton::clicked, this,
                     &P4IsoclinesDlg::onBtnPlot);
    QObject::connect(btnDelAll_, &QPushButton::clicked, this,
                     &P4IsoclinesDlg::onBtnDelAll);
    QObject::connect(btnDelLast_, &QPushButton::clicked, this,
                     &P4IsoclinesDlg::onBtnDelLast);

    // finishing

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (gThisVF->numVF_ == 1 && !gVFResults.vf_[0]->isocline_vector_.empty()) {
        // Enable delete buttons if there's only one VF and it has isoclines
        btnDelAll_->setEnabled(true);
        btnDelAll_->setEnabled(true);
    } else {
        // Buttons for deleting will be disabled until a VF is selected
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }

    setP4WindowTitle(this, "Plot Isoclines");
}

void P4IsoclinesDlg::onBtnEvaluate()
{
    bool ok;
    // check if VF is valid
    if (gThisVF->numVF_ != 1) {
        auto vf = edt_vfselect_->text().toInt(&ok);
        if (!ok || vf < 0 || static_cast<unsigned int>(vf) >= gThisVF->numVF_) {
            QMessageBox::information(
                this, "P4",
                "The VF selection is invalid. Select a value between\n0 and "
                "the number of vector fields in your\nconfiguration. (minus "
                "one, since we start counting at\nzero!)");
            return;
        }
        gThisVF->isoclinesVF_ = vf;
    } else {
        gThisVF->isoclinesVF_ = 0;
    }
    // check if there's a value
    if (edt_value_->text().isNull() && edt_value_->text().isEmpty()) {
        QMessageBox::information(
            this, "P4",
            "The value field has to be filled with a valid number.");
        return;
    } else {
        QString val = edt_value_->text();
        gThisVF->getDataFromDlg();
        val = gThisVF->convertMapleUserParametersLabelsToValues(val);
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

    // check if any of the VFs is not correctly written
    if (std::any_of(std::cbegin(gThisVF->xdot_), std::cend(gThisVF->xdot_),
                    [](QString s) { return (s == "0" || s.isEmpty()); }) ||
        std::any_of(std::cbegin(gThisVF->ydot_), std::cend(gThisVF->ydot_),
                    [](QString s) { return (s == "0" || s.isEmpty()); })) {
        QMessageBox::information(
            this, "P4",
            "Check that the vector field is correctly introduced.\nIf you used "
            "an input file, make sure you pressed\nthe Load button.");
        return;
    }

    // setup the isocline computations
    if (edt_value_->text().trimmed() == "0") {
        gThisVF->isoclines_ = gThisVF->ydot_[gThisVF->isoclinesVF_];
    } else if (edt_value_->text().trimmed().toLower() == "inf") {
        gThisVF->isoclines_ = gThisVF->xdot_[gThisVF->isoclinesVF_];
    } else {
        gThisVF->isoclines_ = "(" + gThisVF->ydot_[gThisVF->isoclinesVF_] +
                              +")-(" + edt_value_->text().trimmed() + ")*(" +
                              gThisVF->xdot_[gThisVF->isoclinesVF_] + ")";
    }

    // FIRST: create filename_vecisoclines.tab for transforming the isoclines
    // QString to a list of P4POLYNOM2
    gThisVF->setIsoclinesDlg(this);
    gThisVF->evaluateIsoclinesTable();
    btnPlot_->setEnabled(true);
    plotwnd_->getDlgData();
}

void P4IsoclinesDlg::onBtnPlot()
{
    bool dashes{btn_dashes_->isChecked()}, result, ok{true}, convertok;
    int points, precis, memory;
    QString buf;

    buf = edt_points_->text();
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
    if (!gVFResults.vf_[gThisVF->isoclinesVF_]->readIsoclines(
            gThisVF->getbarefilename()))
        QMessageBox::critical(this, "P4",
                              "Cannot read isoclines.\n"
                              "Plese check the input field.\n");

    /*if (!gVFResults.readIsoclines(gThisVF->getbarefilename())) {
        QMessageBox::critical(this, "P4",
                              "Cannot read isoclines.\n"
                              "Please check the input field!\n");
        return;
    }*/
    // THIRD: evaluate isoclines with given parameters {dashes, points, memory}.

    evaluating_points_ = points;
    evaluating_memory_ = memory;
    evaluating_precision_ = precis;

    btnPlot_->setEnabled(false);

    gThisVF->setIsoclinesDlg(this);

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

void P4IsoclinesDlg::onBtnDelAll()
{
    plotwnd_->getDlgData();

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);
    btnDelAll_->setEnabled(false);
    btnDelLast_->setEnabled(false);

    for (auto &vf : gVFResults.vf_) {
        vf->isocline_vector_.clear();
    }

    mainSphere_->refresh();
}

void P4IsoclinesDlg::onBtnDelLast()
{
    plotwnd_->getDlgData();

    int vf;
    if (edt_vfselect_->text().isEmpty()) {
        vf = 0;
    } else {
        bool ok;
        vf = edt_vfselect_->text().toInt(&ok);
        if (!ok || vf < 0 || static_cast<unsigned int>(vf) >= gThisVF->numVF_) {
            QMessageBox::information(
                this, "P4",
                "The VF selection is invalid. Select a value between\n0 and "
                "the number of vector fields in your\nconfiguration. (minus "
                "one, since we start counting at\nzero!)");
            return;
        }
    }

    deleteLastIsocline(mainSphere_, vf);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    // check if all the gVFResults.vf_->isoclines_vector_ are empty, and if
    // true, disable the delete buttons
    if (std::all_of(std::begin(gVFResults.vf_), std::end(gVFResults.vf_),
                    [](const std::unique_ptr<P4VFStudy> &vf) {
                        return vf->isocline_vector_.empty();
                    })) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }

    /*
    if (gVFResults.isocline_vector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
    */
}

void P4IsoclinesDlg::reset()
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

    // check if any of the gVFResults.vf_->isoclines_vector_ are not empty, and
    // if true, enable the delete buttons
    if (!std::any_of(std::begin(gVFResults.vf_), std::end(gVFResults.vf_),
                     [](const std::unique_ptr<P4VFStudy> &vf) {
                         return vf->isocline_vector_.empty();
                     })) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }

    /*
    if (!gVFResults.isocline_vector_.empty()) {
        btnDelLast_->setEnabled(true);
        btnDelAll_->setEnabled(true);
    }
    */
    if (gVFResults.config_dashes_)
        btn_dashes_->toggle();
    else
        btn_dots_->toggle();
}

void P4IsoclinesDlg::finishIsoclinesEvaluation()
{
    bool result;

    if (btnPlot_->isEnabled() == true)
        return; // not busy??

    result = evalIsoclinesContinue(evaluating_precision_, evaluating_points_);

    if (result) {
        btnPlot_->setEnabled(false);
        result = evalIsoclinesFinish(); // return false in case an error occured
        if (!result) {
            QMessageBox::critical(this, "P4",
                                  "An error occured while plotting "
                                  "the isoclines.\nThe singular locus "
                                  "may not be visible, or may "
                                  "be partially visible.");
        }
    }
}
