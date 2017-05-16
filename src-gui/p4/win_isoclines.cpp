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

#include "win_orbits.h"

#include "custom.h"
#include "math_orbits.h"

QIsoclinesDlg::QIsoclinesDlg(QPlotWnd *plt, QWinSphere *sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    mainSphere_ = sp;
    plotwnd_ = plt;

    edt_value_ = new QLineEdit("", this);
    QLabel *lbl1 = new QLabel("&Value = ", this);
    lbl1->setBuddy(edt_value_);

    btnEvaluate_ = new QPushButton("&Evaluate", this);
    btnPlot_ = new QPushButton("&Plot", this);
    btnDelLast_ = new QPushButton("&Delete Last Orbit", this);
    btnDelAll_ = new QPushButton("Delete &All Orbits", this);

    btnSelect_ = new QPushButton("&Select", this);

#ifdef TOOLTIPS
    edt_value_->setToolTip("Value of isoclines to plot.");
    btnSelect_->setToolTip("Validate your choice of isoclines value.");
    btnEvaluate_->setToolTip("Evaluate isoclines at the selected value.");
    btnPlot_->setToolTip("Plot isocline.");
    btnDelLast_->setToolTip("Delete last isocline drawn");
    btnDelAll_->setToolTip("Delete all isoclines (separatrices remain)");
#endif

    // layout
    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *layout0 = new QHBoxLayout();
    layout0->addWidget(lbl1);
    layout0->addWidget(edt_value_);
    layout0->addWidget(btn_select_);
    layout0->addStretch(0);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(btnEvaluate_);
    layout1->addWidget(btnPlot_);
    layout1->addStretch(0);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(btnDelLast_);
    layout2->addWidget(btnDelAll_);
    layout2->addStretch(0);

    mainLayout_->addLayout(layout0);
    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections
    QObject::connect(btnSelect_, SIGNAL(clicked()), this, SLOT(onBtnSelect()));
    QObject::connect(btnEvaluate_, SIGNAL(clicked()), this,
                     SLOT(onBtnEvaluate()));
    QObject::connect(btnPlot_, SIGNAL(clicked()), this, SLOT(onBtnPlot()));
    QObject::connect(btnDelAll_, SIGNAL(clicked()), this, SLOT(onBtnDelAll()));
    QObject::connect(btnDelLast_, SIGNAL(clicked()), this,
                     SLOT(onBtnDelLast()));

    // finishing
    selected_value_ = 0;

    btnEvaluate_->setEnabled(false);
    btnPlot_->setEnabled(false);

    if (g_VFResults.first_isocline_ == nullptr) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }

    setP4WindowTitle(this, "Plot Isoclines");
}

void QIsoclinesDlg::setValue(double v)
{
    QString buf;

    plotwnd_->getDlgData();

    selected_value_ = v;

    buf.sprintf("%g", v);

    edt_value_->setText(buf);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);
}

void QIsoclinesDlg::onBtnSelect(void)
{
    plotwnd_->getDlgData();
    QString buf(edt_value_->text());
    setValue(buf.toDouble());
}

void QIsoclinesDlg::onBtnEvaluate(void)
{
    /*if (edt_value_->text().isNull() || edt_value_->text().isEmpty()) {
        QMessageBox::information(
            this, "P4", "The value field has to be filled with a valid value.");
        return;
    }
    g_ThisVF->curve_ = edt_curve_->text().trimmed();

    // FIRST: create filename_veccurve.tab for transforming the curve QString to
    // a list of P4POLYNOM2
    g_ThisVF->evaluateCurveTable();
    btn_plot_->setEnabled(true);
    plotwnd_->getDlgData();*/

    if (!orbitStarted_) {
        if (!orbitSelected_)
            return;

        mainSphere_->prepareDrawing();
        orbitStarted_ =
            startOrbit(mainSphere_, selected_x0_, selected_y0_, true);
        mainSphere_->finishDrawing();

        if (orbitStarted_) {
            btnDelAll_->setEnabled(true);
            btnDelLast_->setEnabled(true);
        }
    }

    if (orbitStarted_) {
        mainSphere_->prepareDrawing();
        integrateOrbit(mainSphere_, -1);
        mainSphere_->finishDrawing();

        btnBackwards_->setEnabled(false);
        btnContinue_->setEnabled(true);
    }
}

void QIsoclinesDlg::onBtnContinue(void)
{
    plotwnd_->getDlgData();

    if (orbitStarted_) {
        mainSphere_->prepareDrawing();
        integrateOrbit(mainSphere_, 0);
        mainSphere_->finishDrawing();
    }
}

void QIsoclinesDlg::onBtnForwards(void)
{
    plotwnd_->getDlgData();

    if (!orbitStarted_) {
        if (!orbitSelected_)
            return;

        mainSphere_->prepareDrawing();
        orbitStarted_ =
            startOrbit(mainSphere_, selected_x0_, selected_y0_, true);
        mainSphere_->finishDrawing();

        if (orbitStarted_) {
            btnDelAll_->setEnabled(true);
            btnDelLast_->setEnabled(true);
        }
    }

    if (orbitStarted_) {
        mainSphere_->prepareDrawing();
        integrateOrbit(mainSphere_, 1);
        mainSphere_->finishDrawing();

        btnForwards_->setEnabled(false);
        btnContinue_->setEnabled(true);
    }
}

void QIsoclinesDlg::onBtnDelAll(void)
{
    plotwnd_->getDlgData();

    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);
    btnDelAll_->setEnabled(false);
    btnDelLast_->setEnabled(false);

    g_VFResults.deleteOrbit(g_VFResults.first_orbit_);
    g_VFResults.first_orbit_ = nullptr;
    g_VFResults.current_orbit_ = nullptr;

    mainSphere_->refresh();
}

void QIsoclinesDlg::onBtnDelLast(void)
{
    plotwnd_->getDlgData();

    mainSphere_->prepareDrawing();
    deleteLastOrbit(mainSphere_);
    mainSphere_->finishDrawing();

    orbitStarted_ = false;
    orbitSelected_ = false;
    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);

    if (g_VFResults.first_orbit_ == nullptr) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
}

void QIsoclinesDlg::orbitEvent(int i)
{
    switch (i) {
    case -1:
        onBtnBackwards();
        break;
    case 1:
        onBtnForwards();
        break;
    case 0:
        onBtnContinue();
        break;
    case 2:
        onBtnDelLast();
        break;
    case 3:
        onBtnDelAll();
        break;
    }
}

void QIsoclinesDlg::reset(void)
{
    // finishing

    selected_x0_ = 0;
    selected_y0_ = 0;

    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);

    if (g_VFResults.first_orbit_ == nullptr) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
    orbitStarted_ = false;
    orbitSelected_ = false;
}
