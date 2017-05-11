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

    QObject::connect(btnSelect_, SIGNAL(clicked()), this, SLOT(onbtnSelect_()));
    QObject::connect(btnForwards_, SIGNAL(clicked()), this,
                     SLOT(onbtnForwards_()));
    QObject::connect(btnBackwards_, SIGNAL(clicked()), this,
                     SLOT(onbtnBackwards_()));
    QObject::connect(btnContinue_, SIGNAL(clicked()), this,
                     SLOT(onbtnContinue_()));
    QObject::connect(btnDelAll_, SIGNAL(clicked()), this, SLOT(onbtnDelAll_()));
    QObject::connect(btnDelLast_, SIGNAL(clicked()), this,
                     SLOT(onbtnDelLast_()));

    // finishing

    selected_x0 = 0;
    selected_y0 = 0;

    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);

    if (VFResults.first_orbit == nullptr) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
    orbit_started = false;
    orbit_selected = false;

    setP4WindowTitle(this, "Plot Orbits");
}

void QOrbitsDlg::setInitialPoint(double x, double y)
{
    QString bufx;
    QString bufy;

    plotwnd_->getDlgData();

    selected_x0 = x;
    selected_y0 = y;

    bufx.sprintf("%g", (float)x);
    bufy.sprintf("%g", (float)y);

    edt_x0_->setText(bufx);
    edt_y0_->setText(bufy);
    orbit_started = false;
    orbit_selected = true;
    btnForwards_->setEnabled(true);
    btnBackwards_->setEnabled(true);
    btnContinue_->setEnabled(false);
}

void QOrbitsDlg::onbtnSelect_(void)
{
    plotwnd_->getDlgData();

    QString bufx;
    QString bufy;

    bufx = edt_x0_->text();
    bufy = edt_y0_->text();

    selected_x0 = bufx.toDouble();
    selected_y0 = bufy.toDouble();
    orbit_started = false;
    orbit_selected = true;

    btnForwards_->setEnabled(true);
    btnBackwards_->setEnabled(true);
    btnContinue_->setEnabled(false);
}

void QOrbitsDlg::onbtnBackwards_(void)
{
    plotwnd_->getDlgData();

    if (!orbit_started) {
        if (!orbit_selected)
            return;

        mainSphere_->prepareDrawing();
        orbit_started = startOrbit(mainSphere_, selected_x0, selected_y0, true);
        mainSphere_->finishDrawing();

        if (orbit_started) {
            btnDelAll_->setEnabled(true);
            btnDelLast_->setEnabled(true);
        }
    }

    if (orbit_started) {
        mainSphere_->prepareDrawing();
        integrateOrbit(mainSphere_, -1);
        mainSphere_->finishDrawing();

        btnBackwards_->setEnabled(false);
        btnContinue_->setEnabled(true);
    }
}

void QOrbitsDlg::onbtnContinue_(void)
{
    plotwnd_->getDlgData();

    if (orbit_started) {
        mainSphere_->prepareDrawing();
        integrateOrbit(mainSphere_, 0);
        mainSphere_->finishDrawing();
    }
}

void QOrbitsDlg::onbtnForwards_(void)
{
    plotwnd_->getDlgData();

    if (!orbit_started) {
        if (!orbit_selected)
            return;

        mainSphere_->prepareDrawing();
        orbit_started = startOrbit(mainSphere_, selected_x0, selected_y0, true);
        mainSphere_->finishDrawing();

        if (orbit_started) {
            btnDelAll_->setEnabled(true);
            btnDelLast_->setEnabled(true);
        }
    }

    if (orbit_started) {
        mainSphere_->prepareDrawing();
        integrateOrbit(mainSphere_, 1);
        mainSphere_->finishDrawing();

        btnForwards_->setEnabled(false);
        btnContinue_->setEnabled(true);
    }
}

void QOrbitsDlg::onbtnDelAll_(void)
{
    plotwnd_->getDlgData();

    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);
    btnDelAll_->setEnabled(false);
    btnDelLast_->setEnabled(false);

    VFResults.deleteOrbit(VFResults.first_orbit);
    VFResults.first_orbit = nullptr;
    VFResults.current_orbit = nullptr;

    mainSphere_->refresh();
}

void QOrbitsDlg::onbtnDelLast_(void)
{
    plotwnd_->getDlgData();

    mainSphere_->prepareDrawing();
    deleteLastOrbit(mainSphere_);
    mainSphere_->finishDrawing();

    orbit_started = false;
    orbit_selected = false;
    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);

    if (VFResults.first_orbit == nullptr) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
}

void QOrbitsDlg::OrbitEvent(int i)
{
    switch (i) {
    case -1:
        onbtnBackwards_();
        break;
    case 1:
        onbtnForwards_();
        break;
    case 0:
        onbtnContinue_();
        break;
    case 2:
        onbtnDelLast_();
        break;
    case 3:
        onbtnDelAll_();
        break;
    }
}

void QOrbitsDlg::reset(void)
{
    // finishing

    selected_x0 = 0;
    selected_y0 = 0;

    btnForwards_->setEnabled(false);
    btnBackwards_->setEnabled(false);
    btnContinue_->setEnabled(false);

    if (VFResults.first_orbit == nullptr) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
    orbit_started = false;
    orbit_selected = false;
}
