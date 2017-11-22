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
#include "file_tab.h"
#include "main.h"
#include "math_orbits.h"
#include "win_plot.h"
#include "win_sphere.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

QOrbitsDlg::QOrbitsDlg(QPlotWnd *plt, QWinSphere *sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    mainSphere_ = sp;
    plotwnd_ = plt;

    edt_x0_ = new QLineEdit("", this);
    QLabel *lbl1 = new QLabel("&x0 = ", this);
    lbl1->setBuddy(edt_x0_);
    edt_y0_ = new QLineEdit("", this);
    QLabel *lbl2 = new QLabel("&y0 = ", this);
    lbl2->setBuddy(edt_y0_);

    btnSelect_ = new QPushButton("&Select", this);

    btnForwards_ = new QPushButton("&Forwards", this);
    btnContinue_ = new QPushButton("&Continue", this);
    btnBackwards_ = new QPushButton("&Backwards", this);
    btnDelLast_ = new QPushButton("&Delete Last Orbit", this);
    btnDelAll_ = new QPushButton("Delete &All Orbits", this);

#ifdef TOOLTIPS
    edt_x0_->setToolTip(
        "Start point of orbit.\n"
        "You can also click on the plot window to fill this field.");
    edt_y0_->setToolTip(
        "Start point of orbit.\n"
        "You can also click on the plot window to fill this field.");
    btnSelect_->setToolTip("Validate your choice of (x0,y0).\n"
                           "When using the mouse, this is not necessary.");
    btnForwards_->setToolTip("Start integrating the orbit in forward time");
    btnBackwards_->setToolTip("Start integrating the orbit in backward time");
    btnContinue_->setToolTip(
        "Continue integrating the orbit in the chosen time direction");
    btnDelLast_->setToolTip("Delete last orbit drawn");
    btnDelAll_->setToolTip("Delete all orbits (separatrices remain)");
#endif

    // layout

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QGridLayout *lay00 = new QGridLayout();
    lay00->addWidget(lbl1, 0, 0);
    lay00->addWidget(edt_x0_, 0, 1);
    lay00->addWidget(lbl2, 1, 0);
    lay00->addWidget(edt_y0_, 1, 1);
    lay00->addWidget(btnSelect_, 0, 2, 2, 1);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(btnForwards_);
    layout1->addWidget(btnContinue_);
    layout1->addWidget(btnBackwards_);
    layout1->addStretch(0);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(btnDelLast_);
    layout2->addWidget(btnDelAll_);
    layout2->addStretch(0);

    mainLayout_->addLayout(lay00);
    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections

    connect(btnSelect_, &QPushButton::clicked, this, &QOrbitsDlg::onBtnSelect);
    connect(btnForwards_, &QPushButton::clicked, this,
            &QOrbitsDlg::onBtnForwards);
    connect(btnBackwards_, &QPushButton::clicked, this,
            &QOrbitsDlg::onBtnBackwards);
    connect(btnContinue_, &QPushButton::clicked, this,
            &QOrbitsDlg::onBtnContinue);
    connect(btnDelAll_, &QPushButton::clicked, this, &QOrbitsDlg::onBtnDelAll);
    connect(btnDelLast_, &QPushButton::clicked, this,
            &QOrbitsDlg::onBtnDelLast);

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

    setP4WindowTitle(this, "Plot Orbits");
}

void QOrbitsDlg::setInitialPoint(double x, double y)
{
    QString bufx;
    QString bufy;

    plotwnd_->getDlgData();

    selected_x0_ = x;
    selected_y0_ = y;

    bufx.sprintf("%g", (float)x);
    bufy.sprintf("%g", (float)y);

    edt_x0_->setText(bufx);
    edt_y0_->setText(bufy);
    orbitStarted_ = false;
    orbitSelected_ = true;
    btnForwards_->setEnabled(true);
    btnBackwards_->setEnabled(true);
    btnContinue_->setEnabled(false);
}

void QOrbitsDlg::onBtnSelect(void)
{
    plotwnd_->getDlgData();

    QString bufx;
    QString bufy;

    bufx = edt_x0_->text();
    bufy = edt_y0_->text();

    selected_x0_ = bufx.toDouble();
    selected_y0_ = bufy.toDouble();
    orbitStarted_ = false;
    orbitSelected_ = true;

    btnForwards_->setEnabled(true);
    btnBackwards_->setEnabled(true);
    btnContinue_->setEnabled(false);
}

void QOrbitsDlg::onBtnBackwards(void)
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
        integrateOrbit(mainSphere_, -1);
        mainSphere_->finishDrawing();

        btnBackwards_->setEnabled(false);
        btnContinue_->setEnabled(true);
    }
}

void QOrbitsDlg::onBtnContinue(void)
{
    plotwnd_->getDlgData();

    if (orbitStarted_) {
        mainSphere_->prepareDrawing();
        integrateOrbit(mainSphere_, 0);
        mainSphere_->finishDrawing();
    }
}

void QOrbitsDlg::onBtnForwards(void)
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

void QOrbitsDlg::onBtnDelAll(void)
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

void QOrbitsDlg::onBtnDelLast(void)
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

void QOrbitsDlg::orbitEvent(int i)
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

void QOrbitsDlg::reset(void)
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
