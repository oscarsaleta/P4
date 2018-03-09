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

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include "P4ParentStudy.hpp"
#include "P4PlotWnd.hpp"
#include "P4WinSphere.hpp"
#include "main.hpp"
#include "math_isoclines.hpp"

P4IsoclinesDlg::P4IsoclinesDlg(P4PlotWnd *plt, P4WinSphere *sp)
    : QWidget{nullptr, Qt::Tool | Qt::WindowStaysOnTopHint},
      mainSphere_{sp}, plotwnd_{plt}
{
    edt_value_ = std::make_unique<QLineEdit>("", this);
    auto lbl0 = std::make_unique<QLabel>("&Value = ", this);
    lbl0->setBuddy(edt_value_.get());

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

    btnEvaluate_ = std::make_unique<QPushButton>("&Evaluate", this);
    btnPlot_ = std::make_unique<QPushButton>("&Plot", this);
    btnDelLast_ = std::make_unique<QPushButton>("&Delete Last Isocline", this);
    btnDelAll_ = std::make_unique<QPushButton>("Delete &All Isoclines", this);

#ifdef TOOLTIPS
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
    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    auto layout0 = std::make_unique<QHBoxLayout>();
    layout0->addWidget(lbl1.get());
    layout0->addWidget(btn_dots_.get());
    layout0->addWidget(btn_dashes_.get());

    auto layout1 = std::make_unique<QGridLayout>();
    layout1->addWidget(lbl0.get(), 0, 0);
    layout1->addWidget(edt_value_.get(), 0, 1);
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
                     &P4IsoclinesDlg::onBtnEvaluate);
    QObject::connect(btnPlot_.get(), &QPushButton::clicked, this,
                     &P4IsoclinesDlg::onBtnPlot);
    QObject::connect(btnDelAll_.get(), &QPushButton::clicked, this,
                     &P4IsoclinesDlg::onBtnDelAll);
    QObject::connect(btnDelLast_.get(), &QPushButton::clicked, this,
                     &P4IsoclinesDlg::onBtnDelLast);

    // finishing

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    // FIXME: quin mirar? algun? primer? fer un flag q digui si n'hi ha?
    if (!gVFResults.vf_[0]->isocline_vector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }

    setP4WindowTitle(this, "Plot Isoclines");
}

void P4IsoclinesDlg::onBtnEvaluate()
{
    bool ok;
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

    // FIXME: això també mira només un dels camps... Passo d'aquest check? O el
    // reescric? O miro només el primer? Si n'hi ha més d'un vol dir que estan
    // bé?
    if ((gThisVF->xdot_[0] == "0" || gThisVF->xdot_[0].isEmpty()) &&
        (gThisVF->ydot_[0] == "0" || gThisVF->ydot_[0].isEmpty())) {
        QMessageBox::information(this, "P4",
                                 "Check that the vector field is "
                                 "correctly introduced.\nIf you "
                                 "used an input file, make sure "
                                 "you pressed\nthe Load button.");
        return;
    }
    if (edt_value_->text().trimmed() == "0") {
        gThisVF->isoclines_ = gThisVF->ydot_;
    } else if (edt_value_->text().trimmed().toLower() == "inf") {
        gThisVF->isoclines_ = gThisVF->xdot_;
    } else {
        std::vector<QString> tmp;
        auto &vfx = std::begin(gThisVF->xdot_);
        auto &vfy = std::begin(gThisVF->ydot_);
        for (; vfx != std::end(gThisVF->xdot_), vfy != std::end(gThisVF->ydot_);
             ++vfx, ++vfy) {
            tmp.emplace_back("(" + (*vfy) + +")-(" +
                             edt_value_->text().trimmed() + ")*(" + (*vfx) +
                             ")");
        }
        gThisVF->isoclines_ = std::move(tmp);
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
    // FIXME: aqui hem de trobar una forma de guardar les isoclines per cada VF
    // diferent. Posar-les a diferents fitxers de nom
    // gThisVF->getbarefilename()+#vf ?
    for (auto &vf : gVFResults.vf_) {
        if (!vf->readIsoclines(gThisVF->getbarefilename()))
            QMessageBox::critical(this, "P4",
                                  "Cannot read isoclines.\n"
                                  "Plese check the input field.\n");
    }

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

    deleteLastIsocline(mainSphere_);

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