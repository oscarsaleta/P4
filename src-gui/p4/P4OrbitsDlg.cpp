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

#include "P4OrbitsDlg.h"

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

P4OrbitsDlg::P4OrbitsDlg(std::shared_ptr<QPlotWnd> plt,
                         std::shared_ptr<QWinSphere> sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint), plotWnd_{plt},
      mainSphere_{sp}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    edt_x0_.reset(new QLineEdit("", this));
    std::unique_ptr<QLabel> lbl1{new QLabel("&x0.reset(", this)});
    lbl1->setBuddy(edt_x0_);
    edt_y0_.reset(new QLineEdit("", this));
    std::unique_ptr<QLabel> lbl2{new QLabel("&y0.reset(", this)});
    lbl2->setBuddy(edt_y0_);

    btnSelect_.reset(new QPushButton("&Select", this));

    btnForwards_.reset(new QPushButton("&Forwards", this));
    btnContinue_.reset(new QPushButton("&Continue", this));
    btnBackwards_.reset(new QPushButton("&Backwards", this));
    btnDelLast_.reset(new QPushButton("&Delete Last Orbit", this));
    btnDelAll_.reset(new QPushButton("Delete &All Orbits", this));

#ifdef TOOLTIPS
    edt_x0_->setToolTip(
        "Start point of orbit.\n"
        "You can also click on the plot window to fill this field.");
    edt_y0_->setToolTip(
        "Start point of orbit.\n"
        "You can also click on the plot window to fill this field.");
    btnSelect_->setToolTip(
        "Validate your choice of (x0,y0).\n"
        "When using the mouse, this is not necessary.");
    btnForwards_->setToolTip("Start integrating the orbit in forward time");
    btnBackwards_->setToolTip("Start integrating the orbit in backward time");
    btnContinue_->setToolTip(
        "Continue integrating the orbit in the chosen time direction");
    btnDelLast_->setToolTip("Delete last orbit drawn");
    btnDelAll_->setToolTip("Delete all orbits (separatrices remain)");
#endif

    // layout

    mainLayout_.reset(new QBoxLayout(QBoxLayout::TopToBottom, this));

    std::unique_ptr<QGridLayout> lay00{new QGridLayout()};
    lay00->addWidget(lbl1, 0, 0);
    lay00->addWidget(edt_x0_, 0, 1);
    lay00->addWidget(lbl2, 1, 0);
    lay00->addWidget(edt_y0_, 1, 1);
    lay00->addWidget(btnSelect_, 0, 2, 2, 1);

    std::unique_ptr<QHBoxLayout> layout1{new QHBoxLayout()};
    layout1->addWidget(btnForwards_);
    layout1->addWidget(btnContinue_);
    layout1->addWidget(btnBackwards_);
    layout1->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout2{new QHBoxLayout()};
    layout2->addWidget(btnDelLast_);
    layout2->addWidget(btnDelAll_);
    layout2->addStretch(0);

    mainLayout_->addLayout(lay00);
    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections

    QObject::connect(btnSelect_, &QPushButton::clicked, this,
                     &P4OrbitsDlg::onBtnSelect);
    QObject::connect(btnForwards_, &QPushButton::clicked, this,
                     &P4OrbitsDlg::onBtnForwards);
    QObject::connect(btnBackwards_, &QPushButton::clicked, this,
                     &P4OrbitsDlg::onBtnBackwards);
    QObject::connect(btnContinue_, &QPushButton::clicked, this,
                     &P4OrbitsDlg::onBtnContinue);
    QObject::connect(btnDelAll_, &QPushButton::clicked, this,
                     &P4OrbitsDlg::onBtnDelAll);
    QObject::connect(btnDelLast_, &QPushButton::clicked, this,
                     &P4OrbitsDlg::onBtnDelLast);

    // finishing
    selected_x0_ = 0;
    selected_y0_ = 0;

    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);

    if (g_VFResults.orbits_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
    orbitStarted_ = false;
    orbitSelected_ = false;

    setP4WindowTitle(this, "Plot Orbits");
}

void P4OrbitsDlg::setInitialPoint(double x, double y)
{
    QString bufx;
    QString bufy;

    plotWnd_->getDlgData();

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

void P4OrbitsDlg::onBtnSelect()
{
    plotWnd_->getDlgData();

    QString bufx{edt_x0_->text()};
    QString bufy{edt_y0_->text()};

    selected_x0_ = bufx.toDouble();
    selected_y0_ = bufy.toDouble();
    orbitStarted_ = false;
    orbitSelected_ = true;

    btnForwards_->setEnabled(true);
    btnBackwards_->setEnabled(true);
    btnContinue_->setEnabled(false);
}

void P4OrbitsDlg::onBtnBackwards()
{
    plotWnd_->getDlgData();

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

void P4OrbitsDlg::onBtnContinue()
{
    plotWnd_->getDlgData();

    if (orbitStarted_) {
        mainSphere_->prepareDrawing();
        integrateOrbit(mainSphere_, 0);
        mainSphere_->finishDrawing();
    }
}

void P4OrbitsDlg::onBtnForwards()
{
    plotWnd_->getDlgData();

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

void P4OrbitsDlg::onBtnDelAll()
{
    plotWnd_->getDlgData();

    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);
    btnDelAll_->setEnabled(false);
    btnDelLast_->setEnabled(false);

    g_VFResults.orbits_.clear();

    mainSphere_->refresh();
}

void P4OrbitsDlg::onBtnDelLast()
{
    plotWnd_->getDlgData();

    mainSphere_->prepareDrawing();
    deleteLastOrbit(mainSphere_);
    mainSphere_->finishDrawing();

    orbitStarted_ = false;
    orbitSelected_ = false;
    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);

    if (g_VFResults.orbits_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
}

void P4OrbitsDlg::orbitEvent(int i)
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

void P4OrbitsDlg::reset()
{
    // finishing
    selected_x0_ = 0;
    selected_y0_ = 0;

    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);

    if (g_VFResults.orbits_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
    orbitStarted_ = false;
    orbitSelected_ = false;
}
